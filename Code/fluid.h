#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include "grid.h"
#include <unordered_map>

class Fluid {
public:

	int N;

	float diff;
	float visc;
	std::vector<float> oldDensity;
	std::vector<float> density;

	std::vector<float> velocX;
	std::vector<float> velocY;

	std::vector<float> velocX0;
	std::vector<float> velocY0;
	

	Fluid(float deltatime, float diffusion, float viscocity, int n);

	void fullStep(float dt, std::vector<grid> &g);

	void addDensity(int x, int y, float amount);

	void addVelocity(sf::Vector2f v, int x, int y);
};

