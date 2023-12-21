#define _POSIX_C_SOURCE 200112L
#define _XOPEN_SOURCE   700
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#define NUM_INCOGNITAS 3
//#define NUM_THREADS    3

typedef struct {
	float A[NUM_INCOGNITAS][NUM_INCOGNITAS];
	float X[NUM_INCOGNITAS];
	float B[NUM_INCOGNITAS];
	float Solucao[NUM_INCOGNITAS];
} SEL;  //Sistema de Equacoes Lineares

typedef struct {
	int min;
	int max;
} pair;


void* Metodo_jacobi(void* var);

pthread_barrier_t barrier;  //uso de barreira
pthread_t threads[NUM_INCOGNITAS];
SEL sistema;
//pair var[NUM_THREADS];

int main() {
	int val_A[NUM_INCOGNITAS][NUM_INCOGNITAS] = {{4, 3, -5}, {-2, 3, 4}, {3, -4, 3}};
	int val_X[NUM_INCOGNITAS] = {1, 1, 1};
	int val_B[NUM_INCOGNITAS] = {10, 13, -6};
	for (int i = 0; i < NUM_INCOGNITAS; i++) {
		for (int j = 0; j < NUM_INCOGNITAS; j++) { sistema.A[i][j] = val_A[i][j]; }
		sistema.X[i] = val_X[i];
		sistema.B[i] = val_B[i];
	}  // Inicializa o sistema

	int NUM_THREADS;
	printf("Numero de threads: ");
	scanf("%d", &NUM_THREADS);
	pair var[NUM_THREADS];
	if (NUM_THREADS > NUM_INCOGNITAS) {
		fprintf(stderr, "Numero de threads deve ser menor que %d \n", (int) NUM_INCOGNITAS);
		return 1;
	}

	int qtd = NUM_INCOGNITAS / NUM_THREADS;
	int resto = NUM_INCOGNITAS % NUM_THREADS;
	int cursor = 0;
	for (int i = 0; i < NUM_THREADS; i++) {
		var[i].min = cursor;
		cursor += qtd;
		if (resto > 0) {
			var[i].max = ++cursor;
			resto--;
		} else
			var[i].max = cursor;
	}  // Divide as variáveis entre as threads


	pthread_barrier_init(&barrier, NULL, NUM_THREADS);
	//barreira para aguardar todas as xk variaveis ficarem prontas para utilizar para o calculo de xk+1;

	clock_t timer_start = clock();
	for (int i = 0; i < NUM_THREADS; i++) { pthread_create(&threads[i], NULL, Metodo_jacobi, (void*) &var[i]); }
	for (int i = 0; i < NUM_THREADS; i++) { pthread_join(threads[i], NULL); }
	clock_t timer_end = clock();
	double timer_total = ((double) (timer_end - timer_start) * 1000.0) / CLOCKS_PER_SEC;
	printf("Tempo gasto: %.3lf\n", timer_total);

	pthread_barrier_destroy(&barrier);
	return 0;
}
void* Metodo_jacobi(void* var) {
	int start = ((pair*) var)->min;
	int end = ((pair*) var)->max;
	int k = 0, p = 15;
	while (k < p) {
		for (int i = start; i < end; i++) {
			int soma = 0;
			for (int j = 0; j < NUM_INCOGNITAS; j++) { soma += (j != i) ? (sistema.A[i][j]) * (sistema.X[j]) : 0; }
			sistema.X[i] = (sistema.B[i] - soma) / sistema.A[i][i];
		}
		k++;
		pthread_barrier_wait(&barrier);
	}
	return NULL;
}
