#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>

#define PI 3.14159265

class grid {
public:

	sf::RectangleShape self;
	sf::RectangleShape arrow;

	grid(int N, sf::Texture* arrowT, float screenwidth, int i, int j);
	void frameRefresh(float dens, float vX, float vY, int displayT, bool arrows, int N);
};
