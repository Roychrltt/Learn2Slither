#pragma once
#include "Utils.hpp"

class Snake
{
	private:
		std::deque<std::pair<int, int>> body;
		Direction 						dir;

	public:

		Snake(std::mt19937& rng, const Config& cfg)
		{
			std::uniform_int_distribution<int> dist(0, 3);
			int d = dist(rng);
			dir = static_cast<Direction>(d);
			std::pair<int, int> initDir{};

			if (d == 0) initDir = {-1, 0};
			else if (d == 1) initDir = {1, 0};
			else if (d == 2) initDir = {0, -1};
			else initDir = {0, 1};

			int xl = std::max(0, 2 * initDir.first);
			int xr = std::min(cfg.GRID_H - 1, cfg.GRID_H - 1 + 2 * initDir.first);
			int yl = std::max(0, 2 * initDir.second);
			int yr = std::min(cfg.GRID_W - 1, cfg.GRID_W - 1 + 2 * initDir.second);

			std::uniform_int_distribution<int> dx(xl, xr);
			std::uniform_int_distribution<int> dy(yl, yr);
			int x = dx(rng), y = dy(rng);

			body.push_back({x, y});
			body.push_back({x - initDir.first, y - initDir.second});
			body.push_back({x - initDir.first * 2, y - initDir.second * 2});
		
		};
		std::pair<int, int>				head(void) const noexcept { return body.front(); };
		std::pair<int, int>				tail(void) const noexcept { return body.back(); };
		Direction						direction(void) const noexcept { return dir; };
		int								length(void) const noexcept { return body.size(); };
		std::deque<std::pair<int, int>> getBody(void) const noexcept { return body; };
		StepEvent						takeAction(Board& board, Action a, std::mt19937& rng, const Config& cfg)
		{
			dir = applyAction(dir, a);
			std::pair<int, int> next = {body.front().first + dirToPar(dir).first, body.front().second + dirToPar(dir).second};
			if (!inBounds(next, cfg.GRID_H, cfg.GRID_W) || board.get(next) == Cell::SnakeBody || board.get(next) == Cell::Stone) return StepEvent::Died;
			body.push_front(next);
			std::pair<int, int> h = head();
			std::pair<int, int> t = tail();
			if (board.get(next) == Cell::GreenApple)
			{
				board.set(h, Cell::SnakeBody);
				board.placeApple(rng, Cell::GreenApple);
				return StepEvent::GreenApple;
			}
			else if (board.get(next) == Cell::RedApple)
			{
				board.set(t, Cell::Empty);
				body.pop_back();
				if (body.size() < 2) return StepEvent::Died;
				t = tail();
				board.set(t, Cell::Empty);
				body.pop_back();
				board.set(h, Cell::SnakeBody);
				board.placeApple(rng, Cell::RedApple);
				return StepEvent::RedApple;
			}
			else if (board.get(next) == Cell::Empty && board.look(h, dir) == Cell::GreenApple)
			{
				board.set(h, Cell::SnakeBody);
				board.set(t, Cell::Empty);
				body.pop_back();
				return StepEvent::Closer;
			}
			else
			{
				board.set(h, Cell::SnakeBody);
				board.set(t, Cell::Empty);
				body.pop_back();
				return StepEvent::None;
			}
		};
		unsigned int					getState(const Board& board)
		{
			const std::pair<int, int> h = body.front();

			const auto s = static_cast<int>(board.look(h, dir));
			const auto l = static_cast<int>(board.look(h, turnLeft(dir)));
			const auto r = static_cast<int>(board.look(h, turnRight(dir)));

			return s + l * 4 + r * 16;
		}
};
