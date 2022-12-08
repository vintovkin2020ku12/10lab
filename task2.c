#include <stdio.h>
#include <pthread.h>
#include <time.h>

int value; 		//значение
pthread_cond_t cond; //условная переменная (предоставляет потокам своеобразное место встречи)
pthread_mutex_t mutex; //мьютекс
int flag;		//флаг, действие будет выполняться только в том случае, если он установлен


void *produce(void *arg){
	srand(time(NULL));
	while(1){
		int val = (rand() % 10) + 1;
		pthread_mutex_lock(&mutex);			//???
		value = val;
		printf("Производитель: %d\n", value);
		flag = 1;
		pthread_cond_signal(&cond); //возобновляет работу потока, ожидающего наступления события
		pthread_mutex_unlock(&mutex);
		sleep(2);	
	}
}

void *consume(void *arg){
	while(1){
		pthread_mutex_lock(&mutex);
		while(!flag){			//флаг снят, дожидаемся сигнала о состоянии переменной, указывающей, что значение.флага изменилось.Приходит сигнал - поток разблокируется, зацикливается,
			pthread_cond_wait(&cond, &mutex);	//ожидает, пока переменная не перейдет в истинное состояние					проверяет флаг снова.
		}			//на этом моменте флаг уже должен быть установлен
		pthread_mutex_unlock(&mutex);
		printf("Потребитель: %d\n", value);
		value = 0;
		flag = 0;
	}
}

int main(int argc, char **arcv){
	pthread_t producer, consumer;
	printf("Потоки будут отрабатывать 15 секунд\n");
	value = 0;
	flag = 0;
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);		//NULL 

    if (pthread_create(&producer, NULL, &produce, NULL) != 0) {
       	fprintf(stderr, "Error thread\n");
        return 1;
    }
  	
    if (pthread_create(&consumer, NULL, &consume, NULL) != 0) {
        fprintf(stderr, "Error thread\n");
        return 1;
      }
  	
  	sleep(15);

	pthread_cancel(producer);		//отменяем поток-производитель, проверяем, чтобы он завершился
	if (pthread_join(producer, NULL) != 0) {
	    printf("Error\n");   
	}

  	pthread_cancel(consumer);		//отменяем поток-потребитель, проверяем, чтобы он завершился
    if (pthread_join(consumer, NULL) != 0) {
      	printf("Error\n");   
    }
  	
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
	return 0;
}
