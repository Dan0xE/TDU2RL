#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <TlHelp32.h>
#include <stdbool.h>
#include <process.h>
#include <direct.h>
#include <io.h>

#define CONFIG_PATH "config.ini"
#define BUFFER_SIZE 1024
#define MAX_PATH 260
#define EXE_NAME "TestDrive2.exe"

void show_message(const char *title, const char *message)
{
	MessageBox(NULL, message, title, MB_ICONEXCLAMATION | MB_OK);
}

HANDLE create_mutex(const char *name)
{
	HANDLE mutex = CreateMutex(NULL, FALSE, name);

	if (mutex == NULL)
	{
		fprintf(stderr, "Failed to create mutex: %d\n", GetLastError());
		exit(EXIT_FAILURE);
	}
	return mutex;
}

bool is_process_running(const char *process_name)
{
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	BOOL process_found = FALSE;

	if (Process32First(snapshot, &entry))
	{
		do
		{
			if (_stricmp(entry.szExeFile, process_name) == 0)
			{
				process_found = TRUE;
				break;
			}

		} while (Process32Next(snapshot, &entry));
	}
	CloseHandle(snapshot);
	return process_found;
}

void run_game(const char *path)
{
	char exe_path[BUFFER_SIZE];
	char steam_api_path[BUFFER_SIZE];
	char ul_steam_api_path[BUFFER_SIZE];
	char exe_path_quoted[BUFFER_SIZE];

	snprintf(exe_path, BUFFER_SIZE, "%s/TestDrive2.exe", path);
	snprintf(steam_api_path, BUFFER_SIZE, "%s/steam_api.dll", path);
	snprintf(ul_steam_api_path, BUFFER_SIZE, "%s/UL_steam_api.dll", path);
	snprintf(exe_path_quoted, BUFFER_SIZE, "\"%s\"", exe_path);

	if (_access(exe_path, 0) != -1)
	{
		if (_access(steam_api_path, 0) != -1 && rename(steam_api_path, ul_steam_api_path) != 0)
		{
			fprintf(stderr, "Failed to rename %s to %s\n", steam_api_path, ul_steam_api_path);
			exit(EXIT_FAILURE);
		}

		if (_chdir(path) != 0)
		{
			printf("path: %s", path);
			perror("Failed to change dir");
			exit(EXIT_FAILURE);
		}

		system(exe_path_quoted);

		if (_access(ul_steam_api_path, 0) != -1 && rename(ul_steam_api_path, steam_api_path) != 0)
		{
			fprintf(stderr, "Failed to rename %s to %s\n", ul_steam_api_path, steam_api_path);
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		fprintf(stderr, "The provided path %s does not exist!\n", path);
	}
}

void run_game_with_mutex(const char *hash, const char *path)
{
	if (is_process_running(EXE_NAME))
	{
		fprintf(stderr, "Game is already running!\n");
		return;
	}

	HANDLE mutex = create_mutex(hash);
	run_game(path);

	if (WaitForSingleObject(mutex, INFINITE) != WAIT_OBJECT_0)
	{
		fprintf(stderr, "Failed to wait for mutex: %d\n", GetLastError());
		exit(EXIT_FAILURE);
	}

	if (!ReleaseMutex(mutex))
	{
		fprintf(stderr, "Failed to relase the mutex: %d\n", GetLastError());
		exit(EXIT_FAILURE);
	}

	if (!CloseHandle(mutex))
	{
		fprintf(stderr, "Failed to close mutex handle: %d\n", GetLastError());
		exit(EXIT_FAILURE);
	}
	exit(0);
}

void read_config(char *path, char *mutex_name)
{
	FILE *file = fopen(CONFIG_PATH, "r");

	if (file == NULL)
	{
		perror("Could not open " CONFIG_PATH);
		exit(EXIT_FAILURE);
	}

	char line[BUFFER_SIZE];
	while (fgets(line, sizeof(line), file))
	{
		if (line[0] == '#')
		{
			continue;
		}
		if (strncmp(line, "TDU2_PATH=", 10) == 0)
		{
			strncpy(path, line + 10, BUFFER_SIZE - 1);
			path[strcspn(path, "\n")] = 0;
			printf("path: %s\n", path);
		}
		else if (strncmp(line, "MUTEX_NAME=", 11) == 0)
		{
			strncpy(mutex_name, line + 11, BUFFER_SIZE - 1);
			mutex_name[strcspn(mutex_name, "\n")] = 0;
			printf("mutex_name: %s\n", mutex_name);
		}
	}
	if (fclose(file) != 0)
	{
		perror("Could not Close" CONFIG_PATH);
		exit(EXIT_FAILURE);
	}
}