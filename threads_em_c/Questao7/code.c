#define _POSIX_C_SOURCE 200112L
#define _XOPEN_SOURCE   700
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#define INTER_N 5
#define TRAIN_N 10

pthread_mutex_t mutex[INTER_N];
pthread_cond_t is_free[INTER_N];
int inter[INTER_N] = {0};
pthread_t thread[TRAIN_N];

void* train(void* i);

int main() {
	for (int i = 0; i < INTER_N; i++) { pthread_mutex_init(&mutex[i], NULL); }
	printf("Mutex inicializado.\n");
	for (int i = 0; i < INTER_N; i++) { pthread_cond_init(&is_free[i], NULL); }
	printf("Intersecções inicializadas.\n");
	for (int i = 0; i < TRAIN_N; i++) { pthread_create(&thread[i], NULL, train, (void*) &i); }

	// Limpeza
	for (int i = 0; i < TRAIN_N; i++) { pthread_join(thread[i], NULL); }
	for (int i = 0; i < INTER_N; i++) {
		pthread_mutex_destroy(&mutex[i]);
		pthread_cond_destroy(&is_free[i]);
	}
	return 0;
}

void* train(void* num) {
	int id = *((int*) num);
	printf("Trem %d inicializado.\n", id);

	int contador = 0;
	while (true) {
		for (int i = 0; i < INTER_N; i++) {
			pthread_mutex_lock(&mutex[i]);
			while (inter[i] >= 2) { pthread_cond_wait(&is_free[i], &mutex[i]); }
			inter[i]++;
			printf("Trem %d entrou na intersecção %d. Total: %d\n", id, i, inter[i]);
			printf("[%d][%d][%d][%d][%d]\n", inter[0], inter[1], inter[2], inter[3], inter[4]);
			pthread_mutex_unlock(&mutex[i]);

			sleep(1);  // Trem na intersecção

			pthread_mutex_lock(&mutex[i]);
			inter[i]--;
			printf("Trem %d saiu da intersecção %d. Total: %d\n", id, i, inter[i]);
			printf("[%d][%d][%d][%d][%d]\n", inter[0], inter[1], inter[2], inter[3], inter[4]);
			pthread_cond_signal(&is_free[i]);
			pthread_mutex_unlock(&mutex[i]);

			sleep(1);  // Trem no caminho
		}
		contador++;
		printf("Trem %d chegou ao fim pela #%d vez.\n", id, contador);
		if (contador == __INT_MAX__) { printf("É preciso imaginar sísifo feliz..."); }
	}
	return NULL;
}