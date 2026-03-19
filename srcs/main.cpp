#include "../includes/Game.hpp"

int main(int ac, char **av)
{
	Config cfg;
	if (!parseArgs(ac, av, cfg))
	{
		printUsage();
		return 0;
	}

	std::vector<std::vector<float>> qtable(STATECOUNT, std::vector<float>(ACTIONCOUNT));
	if (cfg.loadPath != "")
	{
		if (loadModel(cfg.loadPath, qtable, cfg))
			std::cout << "Successfully loaded model from " << cfg.loadPath << std::endl;
		else std::cerr << "Failed to load model!" << std::endl;
	}

	const int cellSize = 60;
	const int screenWidth = cfg.GRID_W * cellSize + 340;
	const int screenHeight = cfg.GRID_H * cellSize;
	float speed = 15;
	Font customFont = GetFontDefault();
	if (cfg.visual)
	{
		InitWindow(screenWidth, screenHeight, "Learn2Slither by Charlotte");
		customFont = LoadFont("font/Bold.ttf");
	}

	int len = 0;
	int i = 0;
	float rand = 1.0;
	std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> roll(0, 1);
	std::uniform_int_distribution<int> ra(0, 2);
	long long int lenSum = 0;
	while (++i <= cfg.sessions)
	{
		int curLen = 0;
		Snake snake(rng, cfg);
		Board board(cfg);
		board.init(rng, snake.getBody(), cfg.obstacle);
		int stepCnt = 0;
		float r = 0.0;
		while (++stepCnt < MAXSTEP)
		{
			if (cfg.visual && cfg.stepbystep)
			{
				if (WindowShouldClose())
				{
					cfg.visual = false;
					UnloadFont(customFont);
					CloseWindow();
					continue;
				}
				while (!WindowShouldClose())
				{
					BeginDrawing();
					ClearBackground(BLACK);
					drawGrid(cfg, cellSize, board);
					drawLobby(cfg, cellSize, customFont, speed, i, len, lenSum, r, qtable);

					const float GW = cfg.GRID_W * cellSize;
					Rectangle stepBtn = { GW + 20, 330, 160, 40 };
					Rectangle nextBtn = { GW + 190, 330, 60, 40 };
					bool nextClicked = (CheckCollisionPointRec(GetMousePosition(), nextBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
					bool toggleOff = (CheckCollisionPointRec(GetMousePosition(), stepBtn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON));

					EndDrawing();

					if (nextClicked) break;
					if (toggleOff) { cfg.stepbystep = false; break; }
				}
			}
			int state = snake.getState(board);
			int a = 0;
			if (qtable[state][1] > qtable[state][a]) a = 1;
			if (qtable[state][2] > qtable[state][a]) a = 2;
			if (cfg.learn && roll(rng) < rand) a = ra(rng);
			auto e = snake.takeAction(board, static_cast<Action>(a), rng, cfg);

			if (e == StepEvent::None) r = cfg.rewardIdle;
			else if (e == StepEvent::GreenApple) r = cfg.rewardGreen;
			else if (e == StepEvent::Closer) r = cfg.rewardCloser;
			else if (e == StepEvent::RedApple) r = cfg.rewardRed;
			else r = cfg.rewardDie;

			if (cfg.visual)
			{
				if (WindowShouldClose())
				{
					cfg.visual = false;
					UnloadFont(customFont);
					CloseWindow();
					continue;
				}

				BeginDrawing();
				ClearBackground(BLACK);
				drawGrid(cfg, cellSize, board);
				drawLobby(cfg, cellSize, customFont, speed, i, len, lenSum, r, qtable);
				EndDrawing();
			}

			int s2 = snake.getState(board);
			if (cfg.learn) update(state, a, r, s2, qtable);
			curLen = std::max(curLen, snake.length());
			if (e == StepEvent::Died) break;
		}
		len = std::max(len, curLen);
		std::cout << "Training loop: " << i << ". Snake length: " << curLen << std::endl;
		rand *= DECAY;
		lenSum += curLen;
	}
	if (cfg.learn) exportModel(qtable, cfg, cfg.sessions);
	std::cout << cfg.sessions << " training loops finished. Best length: " << len << std::endl;
	std::cout << "Average length: " << lenSum / cfg.sessions << std::endl;
	if (cfg.visual)
	{
		UnloadFont(customFont);
		CloseWindow();
	}
	return 0;
}
