#pragma once

#include "Board.hpp"
#include "Snake.hpp"
#include "raylib.h"

void	drawGrid(const Config& cfg, int cellSize, const Board& board);
void	drawLobby(Config& cfg, int cellSize, const Font& customFont, float& speed, int i, int len, long long int lenSum, float r, const std::vector<std::vector<float>>& qtable);
