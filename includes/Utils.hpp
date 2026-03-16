#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <deque>
#include <iostream>
#include <map>
#include <fstream>
#include <iomanip>
#include <optional>
#include <random>
#include <ranges>
#include <span>
#include <stdexcept>
#include <utility>
#include <vector>
#include "Types.hpp"

struct Config
{
	int  sessions = 10000;
	bool learn = true;
	bool visual = true;

	std::string loadFile = "";

	float rewardIdle = -0.1f;
	float rewardGreen = 100.0f;
	float rewardCloser = 0.01f;
	float rewardRed = -25.0f;
	float rewardDie = -50.0f;
};

constexpr Direction turnLeft(Direction d) noexcept
{
	constexpr std::array<Direction, 4> lut = {
		Direction::Left, Direction::Right, Direction::Down, Direction::Up
	};
	return lut[static_cast<uint8_t>(d)];
}

constexpr Direction turnRight(Direction d) noexcept
{
	constexpr std::array<Direction, 4> lut = {
		Direction::Right, Direction::Left, Direction::Up, Direction::Down
	};
	return lut[static_cast<uint8_t>(d)];
}

constexpr Direction applyAction(Direction current, Action a) noexcept
{
	switch (a)
	{
		case Action::TurnLeft:  return turnLeft(current);
		case Action::TurnRight: return turnRight(current);
		default:                return current;
	}
}

constexpr bool inBounds(const std::pair<int, int>& p) noexcept
{
	return p.first >= 0 && p.first < GRID_H && p.second >= 0 && p.second < GRID_W;
}

constexpr std::pair<int, int> dirToPar(Direction d) noexcept
{
	switch (d)
	{
		case Direction::Up:    return {-1, 0};
		case Direction::Down:  return {1, 0};
		case Direction::Left:  return {0, -1};
		case Direction::Right: return {0,  1};
		default:			   return {-1, 0};	
	}
}
void	printUsage(void);
bool	parseArgs(int ac, char** av, Config& cfg);
void	update(uint8_t s, uint8_t a, float r, uint8_t s2, std::vector<std::vector<float>>& qtable);
void	exportModel(const std::vector<std::vector<float>>& qtable, const Config& cfg, int sessionID);
 
