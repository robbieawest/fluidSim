#include "fluid.h"
#include "functions.h"

Fluid::Fluid(float deltatime, float diffusion, float viscocity, int n) {

	N = n;
	diff = diffusion;
	visc = viscocity;

	oldDensity = std::vector<float>(N * N, 0);
	density = std::vector<float>(N * N, 0);


	velocX = std::vector<float>(N * N, 0);
	velocY = std::vector<float>(N * N, 0);

	velocX0 = std::vector<float>(N * N, 0);
	velocY0 = std::vector<float>(N * N, 0);
}

void Fluid::fullStep(float dt, std::vector<grid> &g) {
	int iter = 16;


	Diffusion(g, 1, velocX0, velocX, visc, dt, N, iter); //Diffuse x velocities
    Diffusion(g, 2, velocY0, velocY, visc, dt, N, iter); //Diffuse y velocities

    ClearDivergence(g, velocX0, velocY0, velocX, velocY, N, iter); //Make incompressible

	Advection(g, 1, velocX, velocX0, velocX0, velocY0, dt, N); //Advect X velocties
	Advection(g, 2, velocY, velocY0, velocX0, velocY0, dt, N); //Advect Y velocties

	ClearDivergence(g, velocX, velocY, velocX0, velocY0, N, iter); //Make incompressible

	Diffusion(g, 0, oldDensity, density, diff, dt, N, iter); //Diffuse densities

	Advection(g, 0, density, oldDensity, velocX, velocY, dt, N); //Advect densities

	//Note that the advection must always come after clearing the vector field of divergence, as the advection relys on the fluid being incompressable
}


void Fluid::addDensity(int x, int y, float amount) {
	density[IX(x, y)] += amount;
}

void Fluid::addVelocity(sf::Vector2f vplus, int x, int y) {

	velocX[IX(x, y)] += vplus.x;
	velocY[IX(x, y)] += vplus.y;
}
