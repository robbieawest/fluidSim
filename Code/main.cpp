#include <SFML/Graphics.hpp>
#include <iostream>
#include "fluid.h"
#include "functions.h"
#include "grid.h"
#include <sstream>

std::string to_str (float dt){

	//Convert dt to string
	std::ostringstream os;
	os << dt;
	return os.str();
}

void clickCheck(std::vector<grid> g, sf::Vector2i mp, int N) {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {

			if (g[IX(i, j)].self.getGlobalBounds().intersects(sf::FloatRect(mp.x, mp.y, 1, 1))) {
				std::cout << i << " " << j << std::endl;
				std::cout << g[IX(i, j)].self.getPosition().x << " " << g[IX(i, j)].self.getPosition().y << std::endl;
				std::cout << IX(i, j) << " " << IX(i, j) << std::endl;

				//^ click check for testing grid positions, notice that breaks are not used as some errors may result in multiple squares being stacked on top of eachover
			}
		}
	}
}

int main() {


	sf::RenderWindow window(sf::VideoMode(900, 900), "Fluid", sf::Style::Default);

	//Display
	int N = 35;

	//Iterations for gauss seidel relaxion
	int iter = 16;
	//

	//Arrow Texture
	sf::Texture arrowTexture;
	if (!arrowTexture.loadFromFile("Arrow.png")) {
		std::cout << "Arrow did not load" << std::endl;
	}
	//

	//Creating grid and fluid object
	Fluid fluid = Fluid(0.2, 0.00000001, 0.00004, N);

	std::vector<grid> Grid;

	//Populating grid
	for (int j = 0; j < N; j++) {
		for (int i = 0; i < N; i++) {

			grid tempGridBox = grid(N, &arrowTexture, 900, i, j);
			Grid.push_back(tempGridBox);
		}
	}
	//
	//

	//Display type int
	int displayType = 1;
	bool projectingArrows = true;


	//Continous Adding
	sf::Clock addingClock;
	sf::Clock increClock;
	float addingTime = 0.8f;
	float increTime = 6.0f;
	int addingCount = 1;
	//

	//Delta
	float deltaTime;
	sf::Clock deltaClock;
	//

	//FPS counter
	sf::Font calibri;
	if (!calibri.loadFromFile("calibri.ttf")) {
		std::cout << "Could not load font file" << std::endl;
	}

	sf::Text fps;
	fps.setCharacterSize(25);
	fps.setFillColor(sf::Color::Red);
	fps.setFont(calibri);
	fps.setString("0");
	//

	//Mouse adding
	bool mouseAdding = false;

	//Options and such
	std::cout << "'d': Display Mode Change\nDisplayModes are White/Black with arrows and Yellow-Blue velocity indicated." << std::endl;
	std::cout << "\nHolding mouse click will allow the user to interact with the fluid." << std::endl;
	std::cout << "\nArrows can be turned off by pressing 'a' on the window." << std::endl;
	std::cout << "Arrows represent the direction of the velocity, and the colour represents the magnitude of the velocity for each grid position." << std::endl;
	std::cout << "\nAn FPS counter is in the top left" << std::endl;

	while (window.isOpen()) {
		sf::Event evnt;

		while (window.pollEvent(evnt)) {
			switch (evnt.type) {
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::MouseButtonPressed:
				//clickCheck(Grid, sf::Mouse::getPosition(window), N);
				mouseAdding = true;
				break;
			case sf::Event::MouseButtonReleased:
				mouseAdding = false;
				break;


			case sf::Event::TextEntered:
				if (evnt.text.unicode == 'a') {
					//Activate or deactivate arrow drawing
					projectingArrows = projectingArrows ? false : true; //better than if statement, if false then true if true then false
				}
				else if (evnt.text.unicode == 'd') {
					displayType = displayType == 2 ? 1 : displayType + 1;
				}
			}
		}

		if (mouseAdding) {
			mouseVInteract(Grid, fluid, N, sf::Mouse::getPosition(window), window);
		}

		//Get Delta
		deltaTime = deltaClock.getElapsedTime().asSeconds();
		deltaClock.restart();
		//

		//Deal with continous adding
		if (addingClock.getElapsedTime().asSeconds() > addingTime) {
			continuousAdding90(Grid, fluid, N, addingCount);
			addingClock.restart();
		}
		
		if (increClock.getElapsedTime().asSeconds() > increTime) {
			addingCount = addingCount == 4 ? 1 : addingCount + 1;
			increClock.restart();
		}
		//

		//Refresh FPS counter
		fps.setString(to_str(1.0f / deltaTime));
		
		//Simulation
		fluid.fullStep(deltaTime, Grid);
		//


		//Display
		//Refresh grid(arrows and grid square opacities)
		refresh(Grid, fluid.density, fluid.velocX, fluid.velocY, displayType, N, projectingArrows);

		window.clear();


		//Draw Objects
		for (int i = 0; i < Grid.size(); i++) {

			window.draw(Grid[i].self);
			if (displayType == 1 && projectingArrows == true) window.draw(Grid[i].arrow);
			
		}
		window.draw(fps);
		//

		window.display();
		//

	}

	return 0;
}
