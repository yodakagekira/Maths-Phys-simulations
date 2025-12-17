#include "sand.hpp"
#include <utility> // std::swap

CellType grid[GRID_WIDTH][GRID_HEIGHT];
int fireLife[GRID_WIDTH][GRID_HEIGHT] = {0};


// Update the simulation state
void UpdateSimulation() {
    for (int y = GRID_HEIGHT - 2; y >= 0; y--) {
        int startX = (y % 2 == 0) ? 0 : GRID_WIDTH - 1;
        int endX   = (y % 2 == 0) ? GRID_WIDTH : -1;
        int stepX  = (y % 2 == 0) ? 1 : -1;
        // Randomize left-right scan each row for less bias
        for (int x = startX; x != endX; x += stepX) {
            CellType c = grid[x][y];
            if (c == AIR) continue;

            if (c == SAND || c == OIL) {
                if (grid[x][y + 1] == AIR) {
                    grid[x][y + 1] = c;
                    grid[x][y] = AIR;
                    continue;
                }

                int dir = GetRandomValue(0, 1) ? -1 : 1;
                int nx = x + dir;

                if (nx >= 0 && nx < GRID_WIDTH && grid[nx][y + 1] == AIR) {
                    grid[nx][y + 1] = c;
                    grid[x][y] = AIR;
                } else {
                    nx = x - dir;
                    if (nx >= 0 && nx < GRID_WIDTH && grid[nx][y + 1] == AIR) {
                        grid[nx][y + 1] = c;
                        grid[x][y] = AIR;
                    }
                }
            }
            else if (c == WATER) {
                if (grid[x][y + 1] == AIR || grid[x][y + 1] == OIL) {
                    std::swap(grid[x][y + 1], grid[x][y]);
                    continue;
                }

                int dir = GetRandomValue(0, 1) ? -1 : 1;
                int nx = x + dir;

                if (nx >= 0 && nx < GRID_WIDTH &&
                    (grid[nx][y + 1] == AIR || grid[nx][y + 1] == OIL)) {
                    std::swap(grid[nx][y + 1], grid[x][y]);
                } else {
                    nx = x - dir;
                    if (nx >= 0 && nx < GRID_WIDTH &&
                        (grid[nx][y + 1] == AIR || grid[nx][y + 1] == OIL)) {
                        std::swap(grid[nx][y + 1], grid[x][y]);
                    } else {
                        if (x + dir >= 0 && x + dir < GRID_WIDTH &&
                            grid[x + dir][y] == AIR) {
                            std::swap(grid[x + dir][y], grid[x][y]);
                        }
                    }
                }
            }
            else if (c == FIRE) {
                fireLife[x][y]--;
                if (fireLife[x][y] <= 0) {
                    grid[x][y] = AIR;
                    continue;
                }
                // Spread fire to nearby flammables (oil, sand)
                for (int dx = -1; dx <= 1; dx++) {
                    for (int dy = -1; dy <= 1; dy++) {
                        if (dx == 0 && dy == 0) continue;
                        int nx = x + dx, ny = y + dy;
                        if (nx >= 0 && nx < GRID_WIDTH &&
                            ny >= 0 && ny < GRID_HEIGHT) {
                            if ((grid[nx][ny] == OIL || grid[nx][ny] == SAND) &&
                                GetRandomValue(0, 10) < 4) {
                                grid[nx][ny] = FIRE;
                                fireLife[nx][ny] = 20 + GetRandomValue(0, 20);
                            }
                        }
                    }
                }
                    // Fire rises
                if (y > 0 && grid[x][y - 1] == AIR &&
                    GetRandomValue(0, 2) == 0) {
                    grid[x][y - 1] = FIRE;
                    fireLife[x][y - 1] = fireLife[x][y] - 5;
                    grid[x][y] = AIR;
                }
            }
        }
    }
}

// Draw the grid to the screen image
void DrawGrid(Image& screenImage) {
    for (int x = 0; x < GRID_WIDTH; x++) {
        for (int y = 0; y < GRID_HEIGHT; y++) {
            Color col;
            switch (grid[x][y]) {
                case AIR:   col = {50, 100, 150, 255}; break;
                case SAND:  col = {220, 190, 100, 255}; break;
                case WATER: col = {50, 100, 220, 255}; break;
                case STONE: col = {100, 100, 100, 255}; break;
                case OIL:   col = {80, 50, 20, 255}; break;
                case FIRE:
                    col = {255, (unsigned char)(150 + GetRandomValue(-20,20)), 0, 255};
                    break;
                default: col = WHITE;
            }

            for (int dx = 0; dx < CELL_SIZE; dx++) {
                for (int dy = 0; dy < CELL_SIZE; dy++) {
                    ImageDrawPixel(&screenImage,
                        x * CELL_SIZE + dx,
                        y * CELL_SIZE + dy,
                        col);
                }
            }
        }
    }
}
