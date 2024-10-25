#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  bool with_files = false;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            // your code here
            if (seed < 0) {
              fprintf(stderr, "Seed must be a non-negative integer.\n");
              return 1;  
            }
            // error handling
            break;
          case 1:
            array_size = atoi(optarg);
            // your code here
            if (array_size <= 0) {
              fprintf(stderr, "Array size must be a positive integer.\n");
              return 1;
            }
            // error handling
            break;
          case 2:
            pnum = atoi(optarg);
            // your code here
            if (pnum <= 0) {
              fprintf(stderr, "Number of processes must be a positive integer.\n");
              return 1;
            }
            // error handling
            break;
          case 3:
            with_files = true;
            break;

          defalut:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0;

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  int pipes[pnum][2];

  for (int i = 0; i < pnum; i++) {
    if (!with_files) {
      pipe(pipes[i]); 
    }
    pid_t child_pid = fork();
    if (child_pid >= 0) {
      // successful fork
      active_child_processes += 1;
      if (child_pid == 0) {

        int local_min = INT_MAX;
        int local_max = INT_MIN;

        // Определение части массива для обработки этим процессом
        int chunk_size = array_size / pnum;
        int start_index = i * chunk_size;
        int end_index = (i == pnum - 1) ? array_size : start_index + chunk_size;

        for (int j = start_index; j < end_index; j++) {
          if (array[j] < local_min) local_min = array[j];
          if (array[j] > local_max) local_max = array[j];
        }

        if (with_files) {
          // use files here
          FILE *file = fopen("min_max.txt", "a");
          fprintf(file, "%d %d\n", local_min, local_max);
          fclose(file);
        } else {
          // use pipe here
          write(pipes[i][1], &local_min, sizeof(local_min));
          write(pipes[i][1], &local_max, sizeof(local_max));
        }
        return 0;
      }

    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }

  while (active_child_processes > 0) {
    // your code here
    wait(NULL);
    active_child_processes -= 1;
  }

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  for (int i = 0; i < pnum; i++) {
    int min = INT_MAX;
    int max = INT_MIN;

    if (with_files) {
      // read from files
      FILE *file = fopen("min_max.txt", "r");
      int file_min, file_max;
      while (fscanf(file, "%d %d", &file_min, &file_max) == 2) {
        if (file_min < min_max.min) min_max.min = file_min;
        if (file_max > min_max.max) min_max.max = file_max;
      }
      fclose(file);
    } else {
      // read from pipes
      int local_min, local_max;
      read(pipes[i][0], &local_min, sizeof(local_min));
      read(pipes[i][0], &local_max, sizeof(local_max));
      if (local_min < min_max.min) min_max.min = local_min;
      if (local_max > min_max.max) min_max.max = local_max;
    }
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);

  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  return 0;
}
