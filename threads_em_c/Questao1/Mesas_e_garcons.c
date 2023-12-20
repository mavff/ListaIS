#include <stdio.h>
#include <pthread.h> 
#include <stdlib.h>
#include <time.h>
#define TAM_FILA 7
#define NUM_MESAS 15
#define NUM_GARCOM 5
typedef struct{
  int numero;
  int mesa;
}Pedido;
Pedido buff[TAM_FILA];  /* buffer size = 10; */
int items = 0;
int first = 0;
int last = 0; 
Pedido ped;
int contador=1;
pthread_barrier_t barrier;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t fill = PTHREAD_COND_INITIALIZER;
void *producer(void *);//o codigo em si se baseia na logica de produtor e consumidor vista em sala, a unica diferenca seria a quantidade de produtores e consumidores
void *consumer(void *);
int main() {
    pthread_t producer_thread[NUM_MESAS];
    pthread_t consumer_thread[NUM_GARCOM]; 
    int *GARCONS[NUM_GARCOM]; 
    int *MESAS[NUM_MESAS];
    srand(time(NULL)); 
    pthread_barrier_init(&barrier, NULL, NUM_MESAS + NUM_GARCOM);//apenas para esperar que todas as mesas e pedido inicializem juntos
    for(int i = 0; i < NUM_MESAS; i++) {
      MESAS[i] =(int *) malloc(sizeof(int)); 
      *MESAS[i] = i;
          pthread_create(&producer_thread[i],NULL,producer,(void *) MESAS[i]);  
    }
    for(int i = 0; i < NUM_GARCOM; i++) {
      GARCONS[i] =(int *) malloc(sizeof(int)); 
      *GARCONS[i] = i;
          pthread_create(&consumer_thread[i],NULL,consumer,(void *) GARCONS[i]);  
    }
    pthread_barrier_destroy(&barrier);
    pthread_exit(NULL);
}
void put(Pedido* atual){
    pthread_mutex_lock(&mutex);
    while(items == TAM_FILA) {
        pthread_cond_wait(&empty, &mutex);//atingimos o tamanho maximo de fila, logo, sem novos pedido
    }
    buff[last] = *atual;
    items++; last++;
    if(last==TAM_FILA) { last = 0; } 
    if(items == 1) { pthread_cond_broadcast(&fill); }//poucos pedido, logo, acordar os produtores/liberar entrada de novos pedidos
    pthread_mutex_unlock(&mutex); 
}
void *producer(void *thread_producer) {
    Pedido ped;
    int aleatorio;
    pthread_barrier_wait(&barrier);//apenas para esperar que todas as mesas e pedido inicializem juntos
    ped.mesa=*((int*) thread_producer);
    printf("PEDIDOS DA MESA %d INICIADOS\n", ped.mesa+1);
    while(1){
      aleatorio=rand()%5 +1;//apenas para chegar pedido se vier numero 3
      if(aleatorio==3){
        ped.numero=contador;
        put(&ped);
        printf("Coloquei pedido %d na fila pela mesa %d\n",contador,ped.mesa);
        contador++;
      }
    }
    printf("Produtor terminou\n");
    pthread_exit(NULL);
}
Pedido get() {
    Pedido finalizado;
    pthread_mutex_lock(&mutex);
    while(items == 0){  pthread_cond_wait(&fill, &mutex); }//se itens==0 parar de consumir
    finalizado = buff[first];
    items--;  first++;
    if(first==TAM_FILA) {first = 0; }
    if(items == TAM_FILA - 1){ pthread_cond_signal(&empty); }
    pthread_mutex_unlock(&mutex);
    return finalizado;
}
void *consumer(void *thread_consumer) {
    Pedido v;
    int aleatorio;
    pthread_barrier_wait(&barrier);//apenas para esperar que todas as mesas e pedido inicializem juntos
    printf("GARÇOM %d INICIOU \n", *((int*) thread_consumer));
    while(1){
      aleatorio=rand()%2 +1;//apenas para chegar pedido se vier numero 1
      if(aleatorio){
        v = get();
        printf(" Mesa %d consumiu pedido %d \n", v.mesa,v.numero);
      }   
    }
    printf("Consumidor %d terminou \n", *((int*) thread_consumer));
    pthread_exit(NULL);
}
