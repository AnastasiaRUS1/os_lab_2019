#include <stdio.h>              
#include <stdlib.h>            
#include <pthread.h>           

int k;   // Число, факториал которого вычисляется
int pnum;  //  количество потоков
int mod;     // для хранения значения модуля
long long result = 1;  // для хранения итогового результата
pthread_mutex_t mut;         

// Функция, выполняемая в каждом потоке
void* factorial_mod(void* arg) {
    int thread_num = (int)(long)arg; // Получаем номер потока из аргументов
    long long product = 1;  // для хранения промежуточного результата факториала в потоке


    int start = thread_num * (k / pnum) + 1; // Вычисляем начало диапазона, в котором будет работать поток

    // вычисляем конец диапазона
    int end;
    if (thread_num == pnum - 1) {
        end = k; // Если это последний поток, обрабатываем все оставшиеся числа
    } else {
        end = (thread_num + 1) * (k / pnum); // "индекс следующего потока" * "количество элементов, обрабатываемых каждым потоком"
    }

    // Вычисление произведения в диапазоне
    for (int i = start; i <= end; i++) { // Цикл по всем числам в заданном диапазоне
        product = (product * i) % mod; // Вычисляем произведение с применением модуля
    }

    pthread_mutex_lock(&mut);
    result = (result * product) % mod; // Обновляем общий результат
    pthread_mutex_unlock(&mut);

    return NULL; 
}

int main(int argc, char* argv[]) {
    if (argc != 7) { // Проверяем количество аргументов командной строки.
        printf("Usage: %s -k <number> --pnum <threads> --mod <modulus>\n", argv[0]);
        return 1; 
    }

    k = atoi(argv[2]);       // Преобразовываем строку в число и сохраняем как k.
    pnum = atoi(argv[4]);    // Преобразовываем строку в число и сохраняем как количество потоков.
    mod = atoi(argv[6]);     // Преобразовываем строку в число и сохраняем как модуль.

    pthread_t threads[pnum]; // Массив для хранения идентификаторов потоков.
    pthread_mutex_init(&mut, NULL); // Инициализация мьютекса.


    for (int i = 0; i < pnum; i++) { 
        pthread_create(&threads[i], NULL, factorial_mod, (void*)(long)i); // Создаем поток, который выполняет функцию factorial_mod.
    }

    for (int i = 0; i < pnum; i++) { 
        pthread_join(threads[i], NULL); // Ожидаем завершения каждого потока.
    }


    printf("Factorial of %d mod %d = %lld\n", k, mod, result); 

    pthread_mutex_destroy(&mut); // Удаляем мьютекс после завершения работы
    return 0; 
}