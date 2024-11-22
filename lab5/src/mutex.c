/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *  Modified by A.Kostin
 ********************************************************
 * mutex.c
 *
 * Simple multi-threaded example with a mutex lock.
 */
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>


// указатели позволяют потокам работать с одной и той же переменной (common).
void do_one_thing(int *); // инициализация трёх функций 
void do_another_thing(int *);
void do_wrap_up(int);
int common = 0; /* общая переменная для двух потоков */
int r1 = 0, r2 = 0, r3 = 0; // не используются в коде...
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER; //инициализация замка в исходном состоянии(ожидает захвата)

int main() {
  pthread_t thread1, thread2; // создаётся два потока 


  // Создаем поток thread1,
  // который будет выполнять функцию do_one_thing
  if (pthread_create(&thread1, NULL, (void *)do_one_thing,
			  (void *)&common) != 0) { // и передаем ей адрес переменной common
    perror("pthread_create"); //Если создание потока не удалось, выводим ошибку
    exit(1); // выходим из программы
  }

  if (pthread_create(&thread2, NULL, (void *)do_another_thing,
                     (void *)&common) != 0) {
    perror("pthread_create");
    exit(1);
  }



  if (pthread_join(thread1, NULL) != 0) { // ожидаем завершение потока
  // первый параметр функции - идентификатор потока,
  //второй  — указатель на место, где сохр. возр. значение
    perror("pthread_join"); 
    exit(1);
  }

  if (pthread_join(thread2, NULL) != 0) {
    perror("pthread_join");
    exit(1);
  }

  do_wrap_up(common);

  return 0;
}

void do_one_thing(int *pnum_times) {
  int i, j, x; // j и x не используются в коде..
  unsigned long k; //беззнаковое длинное число
  int work;
  for (i = 0; i < 50; i++) { // проходим по циклу 50 раз

  // тут происходит блокировка мьютекса mut,
  // чтобы *pnum_times не изменялось
  // одновременно несколькими потоками.
  // !иными словами предотвращает гонку потоков
    pthread_mutex_lock(&mut);
    printf("doing one thing\n"); 

    // Сохраняем в work то, 
    // что находится по адресу, на который указывает pnum_times
    work = *pnum_times; 
    printf("counter = %d\n", work);
    work++; /* увеличиваем, но не записываем */
    for (k = 0; k < 500000; k++)
      ;                 /* длинный цикл */
    *pnum_times = work; /* обратная запись */
	  pthread_mutex_unlock(&mut);
  }
}

void do_another_thing(int *pnum_times) {
  int i, j, x;
  unsigned long k;
  int work;
  for (i = 0; i < 50; i++) {
    pthread_mutex_lock(&mut);
    printf("doing another thing\n");
    work = *pnum_times;
    printf("counter = %d\n", work);
    work++; /* increment, but not write */
    for (k = 0; k < 500000; k++)
      ;                 /* long cycle */
    *pnum_times = work; /* write back */
    pthread_mutex_unlock(&mut);
  }
}

//завершение программы и вывод счетчика
void do_wrap_up(int counter) {
  int total;
  printf("All done, counter = %d\n", counter); 
}
