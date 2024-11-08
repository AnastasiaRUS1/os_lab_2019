#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <getopt.h>

struct SumArgs {
    int *array;
    int begin;
    int end;
};

int Sum(const struct SumArgs *args) {
    int sum = 0;
    for (int i = args->begin; i < args->end; i++) {
        sum += args->array[i];
    }
    return sum;
}

void *ThreadSum(void *args) {
    struct SumArgs *sum_args = (struct SumArgs *)args; // Приведение типов
    return (void *)(size_t)Sum(sum_args); // Приведение результата к void*
}

void GenerateArray(int *array, uint32_t size, uint32_t seed) {
    srand(seed);
    for (uint32_t i = 0; i < size; i++) {
        array[i] = rand() % 100; 
    }
}

int main(int argc, char **argv) {
    uint32_t threads_num = 0;
    uint32_t array_size = 0;
    uint32_t seed = 0;
    
    // Обработка аргументов командной строки
    int opt;
    while ((opt = getopt(argc, argv, "t:s:a:")) != -1) {
        switch (opt) {
            case 't':
                threads_num = atoi(optarg);
                break;
            case 's':
                seed = atoi(optarg);
                break;
            case 'a':
                array_size = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s --threads_num <num> --seed <num> --array_size <num>\n", argv[0]);
                return 1;
        }
    }

    if (threads_num <= 0 || array_size <= 0) {
        fprintf(stderr, "Invalid input. Please provide positive integers for threads and array size.\n");
        return 1;
    }

   
    int *array = malloc(sizeof(int) * array_size);
    GenerateArray(array, array_size, seed);

    // Распределение работы между потоками
    pthread_t threads[threads_num];
    struct SumArgs args[threads_num];
    
    uint32_t chunk_size = array_size / threads_num;
    for (uint32_t i = 0; i < threads_num; i++) {
        args[i].array = array;
        args[i].begin = i * chunk_size;
        args[i].end = (i == threads_num - 1) ? array_size : (i + 1) * chunk_size; // Обработка последнего потока
        if (pthread_create(&threads[i], NULL, ThreadSum, (void *)&args[i])) {
            printf("Error: pthread_create failed!\n");
            free(array);
            return 1;
        }
    }

    int total_sum = 0;
    for (uint32_t i = 0; i < threads_num; i++) {
        int sum = 0;
        pthread_join(threads[i], (void **)&sum);
        total_sum += sum;
    }

    free(array);
    printf("Total: %d\n", total_sum);
    return 0;
}