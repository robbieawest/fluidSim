#include "functions.h"
#include <iostream>
#include <SFML/Graphics.hpp>


float convertToKey(sf::Vector2i pos);

void Diffusion(std::vector<grid> &g, int b, std::vector<float> &x, std::vector<float> x0, float diff, float dt, int N, int iter) {

	float a = dt * diff * (N - 2)  * (N - 2);
    solve(g, b, x, x0, a, 1 + 6 * a, N, iter);
}

void solve(std::vector<grid> &g, int boundarySide, std::vector<float> &target, std::vector<float> oldTarget, float a, float c, int N, int iter) {


    //Use gauss seidel relaxion to approximate values, turns out the diffusion calculation and the divergence calculation are the same, so they can be neatly put into the same 
    //function with the same method.

    //Gauss seidel method overcomes the problem that the diffusion and divergence calculations need the values from diffusion and divergence calculations to be accurate,
    //the gauss seidel method fixes this as it is essentially just repeating it "iter" amount of times until the target value is accurate

    for (int k = 0; k < iter; k++) {

        for (int j = 1; j < N - 1; j++) {

            for (int i = 1; i < N - 1; i++) {
                target[IX(i, j)] =
                    (oldTarget[IX(i, j)]
                        + a * (target[IX((i + 1), j)]
                            + target[IX((i - 1), j)]
                            + target[IX(i, (j + 1))]
                            + target[IX(i, (j - 1))]
                            )) / c;
            }

        }
        setBoundaries(boundarySide, target, N);
    }
}

void ClearDivergence(std::vector<grid> &g, std::vector<float>& velocX, std::vector<float>& velocY, std::vector<float>& p, std::vector<float>& div, int N, int iter) {

    //Helmholtz Theorem states that any vector field can be represented as the sum of its curl-free component and its divergence-free component
    //The Navier Stokes Equations state that for an incompressable fluid, (del)div * u = 0, in other words, the divergence of every point in the fluid must be zero
    //The divergence of a point can be expressed by how much fluid in going into a certain area in regards to how much is going out of that same area.
    //This code calculates that:
    for (int i = 1; i < N - 1; i++) {
        for (int j = 1; j < N - 1; j++) {
            div[IX(i, j)] = -0.5f * (velocX[IX((i + 1), j)] - velocX[IX((i - 1), j)] + velocY[IX(i, (j + 1))] - velocY[IX(i, (j - 1))]) / float(N);
            p[IX(i, j)] = 0.0f;
        }
    }
    //

    //Set corner boundaries
    setBoundaries(0, div, N);
    setBoundaries(0, p, N);

    //calculate p values
    solve(g, 0, p, div, 1, 6, N, iter);


    //find gradient vector field of this scalar p field, vector calculus states that the curl of this vector field is zero
    //then subtract this from the original vector field and the output is the divergence-free vector field(as stated before)
    for (int i = 1; i < N - 1; i++) {
        for (int j = 1; j < N - 1; j++) {
          
            velocX[IX(i, j)] -= 0.5f * (p[IX((i + 1), j)] - p[IX((i - 1), j)]) * N;
            velocY[IX(i, j)] -= 0.5f * (p[IX(i, (j + 1))] - p[IX(i, (j - 1))]) * N;
           
        }
    }

    //Set boundaries
    setBoundaries(1, velocX, N);
    setBoundaries(2, velocY, N);
}

float lerp(float a, float b, float k) {
    return a + k * (b - a);
}

void Advection(std::vector<grid> &g, int boundarySide, std::vector<float>& target, std::vector<float> oldTarget, std::vector<float> vX, std::vector<float> vY, float deltaTime, int N) {

    //Here the values are dealt with in sf::Vector2f for simplicity, however they are seperate in the class as these functions do not use sf::Vector2fs.
    //For advection, this is because you need the old and new values of each axis as the target and oldTarget, it would be too inefficient to add another 2 vectors into the class for this.

    sf::Vector2f floorPart, floorPartN, fractPart, fractPartN, backInTime, dt;

    dt = sf::Vector2f(deltaTime * (N - 2), deltaTime * (N - 2));

    for (int i = 1; i < N - 1; i++) {
        for (int j = 1; j < N - 1; j++) {


            /*

            Advection goes like this:
            Backtrace velocity for each square, linearly interpolate between surrounding square centers using floor and fract components

            */

            backInTime = sf::Vector2f(float(i) - dt.x * vX[IX(i, j)], float(j) - dt.y * vY[IX(i, j)]); //use deltatime for smoothness between frames(advection does not rely on framerate, relies on clock)

            if (backInTime.x < 0.5f)backInTime.x = 0.5f;
            if (backInTime.x > N - 1.5)backInTime.x = N - 1.5;

            if (backInTime.y < 0.5f)backInTime.y = 0.5f;
            if (backInTime.y > N - 1.5)backInTime.y = N - 1.5;

            floorPart = sf::Vector2f(floor(backInTime.x), floor(backInTime.y));
            floorPartN = sf::Vector2f(floorPart.x + 1.0f, floorPart.y + 1.0f);

            fractPart = backInTime - floorPart;
            fractPartN = sf::Vector2f(1.0f - fractPart.x, 1.0f - fractPart.y);

            sf::Vector2i intFloor = sf::Vector2i(int(floorPart.x), int(floorPart.y));
            sf::Vector2i intFloorN = sf::Vector2i(int(floorPartN.x), int(floorPartN.y));



            //Interpolations
            float lerp1 = lerp(oldTarget[IX(intFloor.x, intFloor.y)], oldTarget[IX(intFloorN.x, intFloor.y)], fractPart.x);
            float lerp2 = lerp(oldTarget[IX(intFloor.x, intFloorN.y)], oldTarget[IX(intFloorN.x, intFloorN.y)], fractPart.x);

            //Final lerp
            target[IX(i, j)] = lerp(lerp1, lerp2, fractPart.y);
        }
    }

    //Set boundaries
    setBoundaries(boundarySide, target, N);
    
}

void setBoundaries(int boundarySide, std::vector<float> &target, int N) {

    

    for (int i = 1; i < N - 1; i++) {

        target[IX(i, 0)] = boundarySide == 2 ? -target[IX(i, 1)] : target[IX(i, 1)];
        target[IX(i, (N - 1))] = boundarySide == 2 ? -target[IX(i, (N - 2))] : target[IX(i, (N - 2))];
    }

    for (int j = 1; j < N - 1; j++) {
        target[IX(0, j)] = boundarySide == 1 ? -target[IX(1, j)] : target[IX(1, j)];
        target[IX((N - 1), j)] = boundarySide == 1 ? -target[IX((N - 2), j)] : target[IX((N - 2), j)];
    }

    //Note that if boundarySide = 0, then it will only evaluate corners

    //Evaluate corners
    //Make each corner the average of the values next to it

    //top left
    target[IX(0, 0)] = (target[IX(1, 0)] + target[IX(0, 1)]) / 2;
    //bottom left
    target[IX(0, (N - 1))] = (target[IX(1, (N - 1))] + target[IX(0, (N - 2))]) / 2;
    //top right
    target[IX((N - 1), 0)] = (target[IX((N - 2), 0)] + target[IX((N - 1), 1)]) / 2;
    //bottom right
    target[IX((N - 1), (N - 1))] = (target[IX((N - 2), (N - 1))] + target[IX((N - 1), (N - 2))]) / 2;

    //Wierd note but IX does not work as: IX(N + 1, j), instead it works with IX((N + 1), j), it ended up using just N, not N + 1
} 



void refresh(std::vector<grid> &g, std::vector<float> d, std::vector<float> vX, std::vector<float> vY, int displayT, int N, bool arrows) {
 
    for (int i = 1; i < N - 1; i++) {
        for (int j = 1; j < N - 1; j++) {

            g[IX(i, j)].frameRefresh(d[IX(i, j)], vX[IX(i, j)], vY[IX(i, j)], displayT, arrows, N);
        }
    }

}

sf::Vector2i returnIndexFromMouse(std::vector<grid> g, sf::Vector2i mousePos, int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {

            if (g[IX(i, j)].self.getGlobalBounds().intersects(sf::FloatRect(mousePos.x, mousePos.y, 1, 1))) { //create floatrect with size 1,1 at mouse position, check if it inter-
                return sf::Vector2i(i, j);                                                                    //sects current grid position. This is easier than making an intersect-
            }                                                                                                 //ion function
        }
    }

    return sf::Vector2i(-1, -1);
}

void continuousAdding90(std::vector<grid> &g, Fluid& F, int N, int side) {
    //send velocites out from center of grid, rotating by 90deg everytime
    //side represents which way to point v, 1 = up, 2 = right, 3 = down, 4 = left
    //densities are placed in a clover like fashion:
    /*
    /////////////////////
    ///////00////////
    //////0000///////
    //////0000///////
    ///////00/////////
    ////////////////
    */
    sf::Vector2f v;

    switch (side) {
    case 1:
        v = sf::Vector2f(0.0f, 2.0f);
        break;
    case 2:
        v = sf::Vector2f(2.0f, 0.0f);
        break;
    case 3:
        v = sf::Vector2f(0.0f, -2.0f);
        break;
    case 4:
        v = sf::Vector2f(-2.0f, 0.0f);
        break;
    }


    sf::Vector2i c = sf::Vector2i((N - 1) / 2, (N - 1) / 2);
    std::unordered_map<float, sf::Vector2i> map;
    float key = 0;

    for (int i = -1; i < 3; i++) {
        sf::Vector2i newV = sf::Vector2i(c.x + i - 1, c.y - 1);
        key = float(newV.x) / float(newV.y) + newV.x + newV.y; //generates unique key for each position
        //the keys make it so that repeat placement does not occurr

        if (map.find(key) == map.end()) { //check for unique key, if not inside then add
            map.insert(std::make_pair(key , newV));
        }

        newV = sf::Vector2i(c.x - i, c.y);
        key = float(newV.x) / float(newV.y) + newV.x + newV.y;

        if (map.find(key) == map.end()) {
            map.insert(std::make_pair(key , newV));
        }
    }

    for (int i = -1; i < 3; i++) {

        sf::Vector2i newV = sf::Vector2i(c.x - 1, c.y + i - 1);

        key = float(newV.x) / float(newV.y) + newV.x + newV.y;

        if (map.find(key) == map.end()) {
            map.insert(std::make_pair(key , newV));
        }

        newV = sf::Vector2i(c.x, c.y - i);

        key = float(newV.x) / float(newV.y) + newV.x + newV.y;

       if (map.find(key) == map.end()) {
            map.insert(std::make_pair(key, newV));
        }
    }

    for (auto iter = map.begin(); iter != map.end(); iter++) { //iterate through the hashmap
        F.addDensity(iter->second.x, iter->second.y, 255);
        F.addVelocity(v, iter->second.x, iter->second.y);
    }

}

void mouseVInteract(std::vector<grid> g, Fluid& F, int N, sf::Vector2i pastMousePos, const sf::RenderWindow &window) {
    sf::Vector2i curr = returnIndexFromMouse(g, pastMousePos, N);
    
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

    sf::Vector2f newV = sf::Vector2f(float(mousePos.x -= pastMousePos.x), float(mousePos.y -= pastMousePos.y));

    if(curr.x > 0 && curr.x < N - 1 && curr.y > 0 && curr.y + N - 1)
       F.addVelocity(sf::Vector2f(newV.x / 2, -1 * newV.y / 2), curr.x, curr.y);
}

float convertToKey(sf::Vector2i pos) {
    return float(pos.x) / float(pos.y) + pos.x + pos.y;
}
