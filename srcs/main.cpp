#include "../includes/Game.hpp"
#include <chrono>

int main(int ac, char **av)
{
	auto start = std::chrono::high_resolution_clock::now();
	Config cfg;
	if (!parseArgs(ac, av, cfg))
	{
		printUsage();
		return 0;
	}

	const int cellSize = 60;
	const int screenWidth = GRID_W * cellSize + 330;
	const int screenHeight = GRID_H * cellSize;
	float speed = 15;
	if (cfg.visual)
		InitWindow(screenWidth, screenHeight, "Learn2Slither by Charlotte");
	Font customFont = LoadFont("font/Bold.ttf");
	std::vector<std::vector<float>> qtable(STATECOUNT, std::vector<float>(ACTIONCOUNT));
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
			if (roll(rng) < rand) a = ra(rng);
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
				DrawRectangle(600, 0, 400, 600, Color{40, 40, 40, 255});
				DrawTextEx(customFont, "Learn2Slither", Vector2{620, 20}, 40, 2, GOLD);
				DrawLine(610, 65, 920, 60, DARKGRAY);

				DrawTextEx(customFont, TextFormat("Session: %d", i), Vector2{620, 80}, 20, 1, RAYWHITE);
				DrawTextEx(customFont, TextFormat("Max Length: %d", len), Vector2{620, 105}, 20, 1, LIME);
				DrawTextEx(customFont, TextFormat("Avg Length: %d", (i > 0 ? lenSum / i : 0)), Vector2{620, 130}, 20, 1, SKYBLUE);
				DrawTextEx(customFont, TextFormat("Speed: %i FPS", static_cast<int>(speed)), Vector2{620, 155}, 20, 1, RAYWHITE);

				Rectangle sliderBar = { 620, 185, 160, 10 };
				DrawRectangleRec(sliderBar, BLACK);

				if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
				{
					Vector2 mouse = GetMousePosition();
					if (CheckCollisionPointRec(mouse, {620, 175, 160, 30}))
					{
						speed = 1.0f + (mouse.x - 620) / 160.0f * 200.0f;
						if (speed < 1) speed = 1;
						if (speed > 120) speed = 120;
					}
				}
				DrawRectangle(620 + (int)((speed - 1) / 119.0f * 150), 180, 10, 20, RAYWHITE);
				SetTargetFPS(speed);
				EndDrawing();
			}

			float r = 0.0;
			if (e == StepEvent::None) r = cfg.rewardIdle;
			else if (e == StepEvent::GreenApple) r = cfg.rewardGreen;
			else if (e == StepEvent::Closer) r = cfg.rewardCloser;
			else if (e == StepEvent::RedApple) r = cfg.rewardRed;
			else r = cfg.rewardDie;

			int s2 = snake.getState(board);
			if (cfg.learn) update(state, a, r, s2, qtable);
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
	auto end = std::chrono::high_resolution_clock::now();

	double duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	std::cout << "Execution Time: " << duration / 1e6 << " seconds\n";
	if (cfg.visual) CloseWindow();
	return 0;
}
