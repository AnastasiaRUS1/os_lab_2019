#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
void GenerateArray(int *array, unsigned int array_size, unsigned int seed) {
    srand(seed); // Инициализация генератора случайных чисел
    for (unsigned int i = 0; i < array_size; i++) { // Изменено на unsigned int
        array[i] = rand(); // Заполнение массива случайными числами
    }
}
