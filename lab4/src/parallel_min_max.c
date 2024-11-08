#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <getopt.h>
#include "utils.h" // Подключаем ваш заголовочный файл

#define MAX_PROCESSES 10 // Максимальное количество процессов

pid_t child_pids[MAX_PROCESSES]; // Массив для хранения PID дочерних процессов
int pnum = -1; // Число процессов
int timeout = -1; // Таймаут

// Обработчик сигнала для таймаута
void handle_alarm(int sig) {
    (void)sig; // Убирает предупреждение о неиспользуемом параметре
    for (int i = 0; i < pnum; i++) {
        if (child_pids[i] > 0) {
            kill(child_pids[i], SIGKILL); // Убиваем процесс по его PID
        }
    }
}

int main(int argc, char **argv) {
    int seed = -1;
    int array_size = -1;
    bool with_files = false;

    // Обработка аргументов командной строки
    while (true) {
        static struct option options[] = {
            {"seed", required_argument, 0, 0},
            {"array_size", required_argument, 0, 0},
            {"pnum", required_argument, 0, 0},
            {"by_files", no_argument, 0, 'f'},
            {"timeout", required_argument, 0, 0},
            {0, 0, 0, 0}
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "f", options, &option_index);
        if (c == -1) break;

        switch (c) {
            case 0:
                switch (option_index) {
                    case 0:
                        seed = atoi(optarg);
                        if (seed < 0) {
                            fprintf(stderr, "Seed must be a non-negative integer.\n");
                            return 1;
                        }
                        break;
                    case 1:
                        array_size = atoi(optarg);
                        if (array_size <= 0) {
                            fprintf(stderr, "Array size must be a positive integer.\n");
                            return 1;
                        }
                        break;
                    case 2:
                        pnum = atoi(optarg);
                        if (pnum <= 0) {
                            fprintf(stderr, "Number of processes must be a positive integer.\n");
                            return 1;
                        }
                        break;
                    case 3:
                        with_files = true;
                        break;
                    case 4:
                        timeout = atoi(optarg);
                        if (timeout <= 0) {
                            fprintf(stderr, "Timeout must be a positive integer.\n");
                            return 1;
                        }
                        break;
                }
                break;
            case 'f':
                with_files = true;
                break;
            case '?':
                break;
            default:
                printf("Unknown option\n");
                break;
        }
    }

    if (optind < argc) {
        printf("Has at least one non-option argument\n");
        return 1;
    }

    if (seed == -1 || array_size == -1 || pnum == -1) {
        printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" [--timeout \"num\"]\n", argv[0]);
        return 1;
    }

    // Выделение памяти для массива
    int *array = malloc(sizeof(int) * array_size);
    if (array == NULL) {
        fprintf(stderr, "Failed to allocate memory for the array.\n");
        return 1;
    }

    // Генерация массива
    GenerateArray(array, array_size, seed); 

    int active_child_processes = 0;

    // Установка таймера, если установлен таймаут
    if (timeout > 0) {
        signal(SIGALRM, handle_alarm); // Установка обработчика сигнала
        alarm(timeout); // Установка таймера
    }

    // Создание дочерних процессов
    for (int i = 0; i < pnum; i++) {
        child_pids[i] = fork(); // Сохраняем PID дочернего процесса
        if (child_pids[i] >= 0) {
            active_child_processes += 1;
            if (child_pids[i] == 0) { // Код дочернего процесса
                int local_min = INT_MAX;
                int local_max = INT_MIN;

                int chunk_size = array_size / pnum;
                int start_index = i * chunk_size;
                int end_index = (i == pnum - 1) ? array_size : start_index + chunk_size;

                for (int j = start_index; j < end_index; j++) {
                    if (array[j] < local_min) local_min = array[j];
                    if (array[j] > local_max) local_max = array[j];
                }

                // Запись результатов в файл или другие действия
                if (with_files) {
                    FILE *file = fopen("min_max.txt", "a");
                    fprintf(file, "%d %d\n", local_min, local_max);
                    fclose(file);
                } else {
                    printf("Process %d: Local Min: %d, Local Max: %d\n", i, local_min, local_max);
                }

                return 0; // Завершение дочернего процесса
            } 
        } else {
            perror("Fork failed");
            exit(1);
        }
    }

    // Ожидание завершения дочерних процессов
    while (active_child_processes > 0) {
        waitpid(-1, NULL, WNOHANG); // Неблокирующее ожидание завершения дочерних процессов
        active_child_processes--;
    }

    free(array); // Освобождение памяти
    printf("Calculation completed.\n"); // Вывод завершения
    return 0;
}