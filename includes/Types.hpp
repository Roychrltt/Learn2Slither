#pragma once
#include <cstdint>

constexpr int GRID_W = 10;
constexpr int GRID_H = 10;
constexpr int MAXSTEP = 1000;
constexpr int ACTIONCOUNT = 3;
constexpr int STATECOUNT = 64;
constexpr float DECAY = 0.925;

enum class Cell : uint8_t { Empty, SnakeBody, GreenApple, RedApple, Stone };

enum class Direction : uint8_t { Up, Down, Left, Right };

enum class Action : uint8_t { Straight, TurnLeft, TurnRight };

enum class StepEvent : uint8_t { None, GreenApple, RedApple, Died, Closer };
