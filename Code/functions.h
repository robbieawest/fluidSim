#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include "grid.h"
#include "fluid.h"
#include <unordered_map>

#define IX(i,j) (i + N * j)



void Diffusion(std::vector<grid> &g, int b, std::vector<float> &x, std::vector<float> x0, float diff, float dt, int N, int iter);

void solve(std::vector<grid> &g, int b, std::vector<float> &x, std::vector<float> x0, float a, float c, int N, int iter);

void ClearDivergence(std::vector<grid> &g, std::vector<float> &velocX, std::vector<float> &velocY, std::vector<float> &p, std::vector<float> &div, int N, int iter);

void Advection(std::vector<grid> &g, int boundarySide, std::vector<float>& target, std::vector<float> oldTarget, std::vector<float> vX, std::vector<float> vY, float deltaTime, int N);

void setBoundaries(int b, std::vector<float> &x, int N);

void refresh(std::vector<grid> &g, std::vector<float> d, std::vector<float> vX, std::vector<float> vY, int displayT, int N, bool arrows);

sf::Vector2i returnIndexFromMouse(std::vector<grid> g, sf::Vector2i mousePos, int N);

void continuousAdding90(std::vector<grid> &g, Fluid&F, int N, int side);

void mouseVInteract(std::vector<grid> g, Fluid& F, int N, sf::Vector2i pastMousePos, const sf::RenderWindow &window);
