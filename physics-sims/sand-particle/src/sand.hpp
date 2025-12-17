#pragma once
#include <raylib.h>
#include <raymath.h>

const int SCREEN_WIDTH  = 1200;
const int SCREEN_HEIGHT = 600;

const int CELL_SIZE = 4;
const int GRID_WIDTH  = SCREEN_WIDTH / CELL_SIZE;
const int GRID_HEIGHT = SCREEN_HEIGHT / CELL_SIZE;

enum CellType { AIR = 0, SAND = 1, WATER = 2, STONE = 3, OIL = 4, FIRE = 5};

extern CellType grid[GRID_WIDTH][GRID_HEIGHT];
extern int fireLife[GRID_WIDTH][GRID_HEIGHT];

void UpdateSimulation();
void DrawGrid(Image& screenImage);
