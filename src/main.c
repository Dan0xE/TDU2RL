#include <stdbool.h>
#include <stdio.h>
#include "utils/game_helper.h"
#include "../raylib/src/raylib.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define BUTTON_WIDTH 200
#define BUTTON_HEIGHT 60
#define INDICATOR_RADIUS 10
#define INDICATOR_OFFSET 30
#define EXE_NAME "TestDrive2.exe"

bool gameRunning = false;

void UpdateGameStatus()
{
	gameRunning = is_process_running(EXE_NAME);
}

int main()
{
	char path[BUFFER_SIZE];
	char mutex_name[BUFFER_SIZE];

	read_config(path, mutex_name);

	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Dan0xe Launcher");
	Rectangle buttonRec = {(WINDOW_WIDTH - BUTTON_WIDTH) / 2, WINDOW_HEIGHT / 2, BUTTON_WIDTH, BUTTON_HEIGHT};

	while (!WindowShouldClose())
	{
		UpdateGameStatus();

		if (CheckCollisionPointRec(GetMousePosition(), buttonRec) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			if (gameRunning)
			{
				show_message("Alert", "Game is already running!");
			}
			else
			{
				CloseWindow();
				run_game_with_mutex(mutex_name, path);
				return 0;
			}
		}

		BeginDrawing();
		ClearBackground(WHITE);

		DrawText("TDU2 Dan0xE Launcher", (WINDOW_WIDTH - MeasureText("TDU2 Dan0xE Launcher", 30)) / 2, 50, 30, DARKGRAY);

		// indicator for game status
		char *statusText = gameRunning ? "Game is running" : "Game is not running";
		int textWidth = MeasureText(statusText, 20);
		int totalWidth = textWidth + INDICATOR_RADIUS + 5;

		Color indicatorColor = gameRunning ? RED : GREEN;
		DrawCircle((WINDOW_WIDTH / 2) - totalWidth / 2, 100 + INDICATOR_OFFSET, INDICATOR_RADIUS, indicatorColor);

		DrawText(statusText, (WINDOW_WIDTH / 2) - totalWidth / 2 + INDICATOR_RADIUS + 5, 95 + INDICATOR_OFFSET, 20, DARKGRAY);

		DrawRectangleRec(buttonRec, LIGHTGRAY);
		if (CheckCollisionPointRec(GetMousePosition(), buttonRec))
		{
			DrawRectangleRec(buttonRec, SKYBLUE);
		}

		textWidth = MeasureText("Start Game", 20);
		DrawText("Start Game", buttonRec.x + (BUTTON_WIDTH - textWidth) / 2, buttonRec.y + (BUTTON_HEIGHT - 20) / 2, 20, DARKGRAY);

		EndDrawing();
	}
	CloseWindow();
	return 0;
}