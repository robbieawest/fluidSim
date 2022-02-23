#include "grid.h"
#include <iostream>


grid::grid(int N, sf::Texture* arrowT, float screenwidth, int i, int j) {
		

	//Box
	self.setPosition(i * (900 / N), (N - j - 1) * (900 / N)); 


	self.setSize(sf::Vector2f(900 / N, 900 / N));
	self.setOutlineColor(sf::Color(255, 255, 255, 50));
	self.setOutlineThickness(0);
		
	if (i == 0 || i == N - 1 || j == 0 || j == N - 1) {
		//If on outside
		self.setFillColor(sf::Color(40, 90, 120, 40));
		arrow.setSize(sf::Vector2f(0, 0));
	}
	else {
		self.setFillColor(sf::Color(0, 0, 0));
		arrow.setSize(sf::Vector2f(900 / N / 3 * 2, 900 / N * 3 / 5 * 2)); //Just played with numbers and found some that worked
	}

	//Arrow

	arrow.setTexture(arrowT);
	arrow.setOrigin(sf::Vector2f(arrow.getSize().x / 2, arrow.getSize().y / 2));
	arrow.setPosition(sf::Vector2f(sf::Vector2f(self.getPosition().x + 900 / N / 2, self.getPosition().y + 900 / N / 2)));
}

void grid::frameRefresh(float dens, float vX, float vY, int displayT, bool arrows, int N) {
	


	if (dens > 255 * 2) dens = 255 * 2;


	switch (displayT) {
	case 1:

		self.setFillColor(sf::Color(255, 255, 255, dens / 2)); //change colour of box to match density

		if (arrows) {

			float angle = atan(vY / vX) * 180.0 / PI;
			if (angle < 0)angle *= -1;

			if (vX < 0 && vY < 0) {
				angle = 90 - angle + 180;
			}
			else if (vX > 0 && vY < 0) {
				angle = angle + 90;
			}
			else if (vX < 0 && vY > 0) {
				angle += 270;
			}
			else if (vX > 0 && vY > 0) {
				angle = 90 - angle;
			}

			else if (vX == 0 && vY > 0) {
				angle = 0;
			}
			else if (vX == 0 && vY < 0) {
				angle = 180;
			}
			else if (vX < 0 && vY == 0) {
				angle = 270;
			}
			else if (vX > 0 && vY == 0) {
				angle = 90;
			}


			arrow.setRotation(angle - 90);


			//calculate |v|
			float magnitude = sqrt(pow(vX, 2) + pow(vY, 2));

			//confine |v| within 0:1 range
			if (magnitude >= 0.1f) {
				magnitude = 1.0f;
			}
			else {
				magnitude /= 0.1f;
			}

			arrow.setFillColor(sf::Color(255 * magnitude, 255 * (1 - magnitude), 0)); //apply range to r and g values, more red and less green for larger magnitudes and vice versa
		}
		break;
	case 2:

		//Handle the colour velocites

		float magnitude = sqrt(pow(vX, 2) + pow(vY, 2));

		//confine |v| within 0:1 range
		if (magnitude >= 0.1f) {
			magnitude = 1.0f;
		}
		else {
			magnitude /= 0.1f;
		}

		self.setFillColor(sf::Color(255 * magnitude, 255, 255 * (1 - magnitude), dens / 2));


		break;
	}
}
