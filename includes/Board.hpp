#pragma once
#include "Types.hpp"
#include "Utils.hpp"

class Board
{
	private:
		int GRID_W;
		int GRID_H;
		std::vector<Cell> grid;

	public:

		Board(const Config& cfg)
		{
			GRID_W = cfg.GRID_W;
			GRID_H = cfg.GRID_H;
			grid.resize(GRID_W * GRID_H);
		};
		void				init(std::mt19937& rng, std::deque<std::pair<int, int>> body, bool ob = false)
		{
			std::fill(grid.begin(), grid.end(), Cell::Empty);
			for (const auto& segment : body)
				set(segment, Cell::SnakeBody);

			placeApple(rng, Cell::GreenApple);
			placeApple(rng, Cell::GreenApple);
			placeApple(rng, Cell::RedApple);
			if (ob)
			{
				placeApple(rng, Cell::Stone);
				placeApple(rng, Cell::Stone);
				placeApple(rng, Cell::Stone);
			}
		}
		[[nodiscard]] Cell	get(std::pair<int, int> p) const noexcept { return grid[p.first * GRID_W + p.second]; }
		void 				set(std::pair<int, int> p, Cell c) noexcept { grid[p.first * GRID_W + p.second] = c; }
		void				placeApple(std::mt19937& rng, Cell type)
		{
			std::vector<std::pair<int, int>> emptyCells;
			for (int i = 0; i < GRID_W * GRID_H; ++i)
				if (grid[i] == Cell::Empty) emptyCells.push_back({i / GRID_W, i % GRID_W});

			if (emptyCells.empty()) return;
			std::uniform_int_distribution<int> dist(0, emptyCells.size() - 1);
			std::pair<int, int> chosen = emptyCells[dist(rng)];
			set(chosen, type);
		}
		Cell look(std::pair<int, int> h, Direction d) const
		{
			auto [dx, dy] = dirToPar(d);
			std::pair<int, int> next = {h.first + dx, h.second + dy};
			if (!inBounds(next, GRID_H, GRID_W)) return Cell::SnakeBody;
			unsigned int idx = next.first * GRID_W + next.second;
			if (grid[idx] == Cell::SnakeBody || grid[idx] == Cell::Stone) return Cell::SnakeBody;
			if (grid[idx] == Cell::RedApple) return Cell::RedApple;
			while (inBounds(next, GRID_H, GRID_W))
			{
				idx = next.first * GRID_W + next.second;
				if (grid[idx] != Cell::Empty)
					return grid[idx] == Cell::GreenApple? Cell::GreenApple : Cell::Empty;
				next.first += dx;
				next.second += dy;
			}
			return Cell::Empty;
		}
};
