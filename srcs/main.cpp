#include "../includes/Game.hpp"

void	printUsage(void)
{
    std::cout << "Usage: ./snake [OPTIONS]\n\n";
    std::cout << "Description:\n";
    std::cout << "  A Reinforcement Learning Snake agent using Q-Learning.\n\n";
    
    std::cout << "Options:\n";
    std::cout << "  --session <n>    Number of training sessions/episodes to run.\n";
    std::cout << "  --load <file>    Load a pre-trained Q-Table/Model from a JSON/txt file.\n";
    std::cout << "  --nolearn        Disable Q-Table updates (useful for testing a loaded model).\n";
    std::cout << "  --novisual       Train the model without the graphical interface (faster training).\n";
    std::cout << "  --help           Display this help message and exit.\n\n";

    std::cout << "Examples:\n";
    std::cout << "./snake --session 50000\n";
    std::cout << "./snake --load Session01.json --nolearn\n";
    std::cout << "./snake --novisual --session 100000\n\n";
    
    std::cout << "Lobby Controls (Visual Mode Only):\n";
    std::cout << "  - Use the slider to adjust game speed (FPS).\n";
    std::cout << "  - Click 'Export' to save the current Q-Table as SessionXX.json.\n";
    std::cout << "  - Toggle Auto/Step-by-step mode to debug the agent's decisions.\n";
}

bool parseArgs(int ac, char** av, Config& cfg)
{
	for (int i = 1; i < ac; ++i)
	{
		std::string arg = av[i];

		if (arg == "--help") return false;
		else if (arg == "--nolearn") cfg.learn = false;
		else if (arg == "--novisual") cfg.visual = false;
		else if (arg == "--session")
		{
			if (i + 1 < ac) cfg.sessions = std::stoi(av[++i]);
			else
			{
				std::cerr << "Error: --session requires a value.\n";
				return false;
			}
		}
		else if (arg == "--load")
		{
			if (i + 1 < ac) cfg.loadFile = av[++i];
			else
			{
				std::cerr << "Error: --load requires a file path.\n";
				return false;
			}
		}
		else
		{
			std::cerr << "Error: Unknown argument '" << arg << "'\n";
			return false;
		}
	}
	return true;
}

void	update(uint8_t s, uint8_t a, float r, uint8_t s2, std::vector<std::vector<float>>& qtable)
{
	float alpha = 0.1, gamma = 0.25;
	float maxNext = std::max({qtable[s2][0], qtable[s2][1], qtable[s2][2]});
	qtable[s][a] += alpha * (r + gamma * maxNext - qtable[s][a]);
}

int main(int ac, char **av)
{
	Config cfg;
	if (!parseArgs(ac, av, cfg))
	{
		printUsage();
		return 0;
	}

	const int cellSize = 60;
	const int screenWidth = GRID_W * cellSize;
	const int screenHeight = GRID_H * cellSize;
	if (cfg.visual)
	{
		InitWindow(screenWidth, screenHeight, "Learn2Slither by Charlotte");
		SetTargetFPS(15);
	}
	std::vector<std::vector<float>> qtable(STATECOUNT, std::vector<float>(ACTIONCOUNT));
	int len = 0;
	int i = 0;
	float rand = 10.0;
	std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> roll(0, 1);
	std::uniform_int_distribution<int> ra(0, 2);
	long long int lenSum = 0;
	while (++i <= cfg.sessions)
	{
		int curLen = 0;
		Snake snake(rng);
		Board board;
		board.init(rng, snake.getBody());
		int stepCnt = 0;
		while (++stepCnt < MAXSTEP)
		{
			int state = snake.getState(board);
			int a = 0;
			if (qtable[state][1] > qtable[state][a]) a = 1;
			if (qtable[state][2] > qtable[state][a]) a = 2;
			if (i < 4000 && roll(rng) < rand) a = ra(rng);
			auto e = snake.takeAction(board, static_cast<Action>(a), rng);
			if (cfg.visual)
			{
				BeginDrawing();
				ClearBackground(BLACK);
				for (int y = 0; y < GRID_H; y++)
				{
					for (int x = 0; x < GRID_W; x++)
					{
						Rectangle rect = { (float)x * cellSize, (float)y * cellSize, (float)cellSize - 2, (float)cellSize - 2 };
						Cell c = board.get({y, x});

						if (c == Cell::SnakeBody)
							DrawRectangleRounded(rect, 0.3, 6, GREEN);
						else if (c == Cell::GreenApple)
							DrawCircle(x * cellSize + cellSize/2, y * cellSize + cellSize/2, cellSize/3, GREEN);
						else if (c == Cell::RedApple)
							DrawCircle(x * cellSize + cellSize/2, y * cellSize + cellSize/2, cellSize/3, RED);
						else if (c == Cell::Stone)
							DrawCircle(x * cellSize + cellSize/2, y * cellSize + cellSize/2, cellSize/3, DARKBROWN);
						else
							DrawRectangleLinesEx(rect, 1, DARKGRAY);
					}
				}
				EndDrawing();
			}

			float r = 0.0;
			if (e == StepEvent::None) r = cfg.rewardIdle;
			else if (e == StepEvent::GreenApple) r = cfg.rewardGreen;
			else if (e == StepEvent::Closer) r = cfg.rewardCloser;
			else if (e == StepEvent::RedApple) r = cfg.rewardRed;
			else r = cfg.rewardDie;

			int s2 = snake.getState(board);
			update(state, a, r, s2, qtable);
			curLen = std::max(curLen, snake.length());
			if (e == StepEvent::Died) break;
		}
		len = std::max(len, curLen);
		std::cout << "Training loop: " << i << ". Snake length: " << curLen << std::endl;
		std::cout << "Step count: " << stepCnt << std::endl;
		rand *= DECAY;
		lenSum += curLen;
	}
	std::ofstream output;
	output.open("Q-Table.txt");
	for (auto& v : qtable)
		output << v[0] << " " << v[1] << " " << v[2] << std::endl;
	output.close();
	std::cout << cfg.sessions << " training loops finished. Best length: " << len << std::endl;
	std::cout << "Average length: " << lenSum / cfg.sessions << std::endl;
	if (cfg.visual) CloseWindow();
	return 0;
}
