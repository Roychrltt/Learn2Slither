#include "../includes/Game.hpp"

void	drawGrid(const Config& cfg, int cellSize, const Board& board)
{
	for (int y = 0; y < cfg.GRID_H; y++)
	{
		for (int x = 0; x < cfg.GRID_W; x++)
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
}

void	drawLobby(Config& cfg, int cellSize, const Font& customFont, float& speed, int i, int len, long long int lenSum, float r, const std::vector<std::vector<float>>& qtable)
{
	const float GW = cfg.GRID_W * cellSize;
	DrawRectangle(GW, 0, 400, GW, Color{40, 40, 40, 255});
	DrawTextEx(customFont, " Learn2Slither", Vector2{GW + 20, 20}, 40, 2, GOLD);
	DrawLine(GW + 10, 65, 920, 65, DARKGRAY);

	DrawTextEx(customFont, TextFormat("Session: %d", i), Vector2{GW + 20, 80}, 30, 1, RAYWHITE);
	DrawTextEx(customFont, TextFormat("Max Length: %d", len), Vector2{GW + 20, 115}, 30, 1, LIME);
	DrawTextEx(customFont, TextFormat("Avg Length: %d", (i > 0 ? lenSum / i : 0)), Vector2{GW + 20, 150}, 30, 1, SKYBLUE);
	DrawTextEx(customFont, TextFormat("Current reward: %.2f", r), Vector2{GW + 20, 185}, 30, 1, RAYWHITE);
	DrawTextEx(customFont, TextFormat("Speed: %i FPS", static_cast<int>(speed)), Vector2{GW + 20, 220}, 30, 1, RAYWHITE);

	Rectangle sliderBar = { GW + 20, 260, 160, 10 };
	DrawRectangleRec(sliderBar, BLACK);

	if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
	{
		Vector2 mouse = GetMousePosition();
		if (CheckCollisionPointRec(mouse, {GW + 15, 250, 170, 30}))
		{
			speed = (mouse.x - GW - 20) / 160.0f * 200.0f;
			if (speed < 1.0f) speed = 1.0f;
			if (speed > 200.0f) speed = 200.0f;
		}
	}
	int handleX = GW + 20 + static_cast<int>((speed / 200.0f) * 160.0f);
	DrawRectangle(handleX, 255, 10, 20, RAYWHITE);
	SetTargetFPS(speed);

	Rectangle exportBtn = { GW + 20, 280, 160, 40 };
	bool hovering = CheckCollisionPointRec(GetMousePosition(), exportBtn);
	DrawRectangleRec(exportBtn, hovering ? LIGHTGRAY : GRAY);
	DrawText("EXPORT", GW + 45, 290, 25, BLACK);

	if (hovering && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
		exportModel(qtable, cfg, i);

	Rectangle stepBtn = { GW + 20, 330, 160, 40 };
	bool stepHover = CheckCollisionPointRec(GetMousePosition(), stepBtn);
	DrawRectangleRec(stepBtn, cfg.stepbystep ? LIME : (stepHover ? LIGHTGRAY : GRAY));
	DrawText("STEP MODE", GW + 35, 340, 20, BLACK);
	if (stepHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) cfg.stepbystep = !cfg.stepbystep;
	Rectangle nextBtn = { GW + 190, 330, 60, 40 };
	bool nextHover = CheckCollisionPointRec(GetMousePosition(), nextBtn);
	DrawRectangleRec(nextBtn, nextHover ? LIGHTGRAY : GRAY);
	DrawText("->", GW + 210, 340, 25, BLACK);
}
