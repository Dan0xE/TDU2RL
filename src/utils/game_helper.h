#include <stdbool.h>

#ifndef GAME_HELPER_H
#define GAME_HELPER_H

#define CONFIG_PATH "config.ini"
#define BUFFER_SIZE 1024
#define EXE_NAME "TestDrive2.exe"

void show_message(const char *title, const char *message);
void read_config(char *path, char *mutex_name);
void run_game_with_mutex(const char *hash, const char *path);
bool is_process_running(const char *process_name);

#endif // GAME_HELPER_H