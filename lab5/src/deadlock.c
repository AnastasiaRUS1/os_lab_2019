#include <stdio.h>              
#include <pthread.h>           
#include <unistd.h>          

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER; 
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER; 

// Функция, запускаемая в потоке 1
void* thread1(void* arg) {
    pthread_mutex_lock(&mutex1); // Захватываем mutex1
    printf("Thread 1: Захватил mutex1\n");\
    sleep(1);
    pthread_mutex_lock(&mutex2); // Пытаемся захватить mutex2
    printf("Thread 1: Захватил mutex2\n"); 
    pthread_mutex_unlock(&mutex2); // Освобождаем mutex2
    pthread_mutex_unlock(&mutex1); // Освобождаем mutex1
    return NULL; 
}


void* thread2(void* arg) {
    pthread_mutex_lock(&mutex2); 
    printf("Thread 2: Захватил mutex2\n"); 
    sleep(1); 
    pthread_mutex_lock(&mutex1); 
    printf("Thread 2: Захватил mutex1\n"); 
    pthread_mutex_unlock(&mutex1); 
    pthread_mutex_unlock(&mutex2); 
    return NULL; 
}

int main() {
    pthread_t t1, t2; // Объявляем переменные для потоков t1 и t2
    pthread_create(&t1, NULL, thread1, NULL); // Создаем поток t1, который выполняет функцию thread1
    pthread_create(&t2, NULL, thread2, NULL); 
    
    pthread_join(t1, NULL); // Ждем завершения потока t1
    pthread_join(t2, NULL); 
    return 0; 
}