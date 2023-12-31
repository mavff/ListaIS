#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define NUM_ARQ 5
#define NUM_Threads 3
#define NUM_Produtos 8
typedef struct{
    int* acesso_a_produtos;//ponteiro para acessar o vetor de produtos
    char** arquivos_da_thread;//para salvar o nome do aarquivo
    int id;
    bool ocupada;
    int num_arq;
}ACAO;//struct para passar como parametro
void* Ler_arquivo(ACAO*);
void inicializa_valores(ACAO*,int*);

pthread_mutex_t mutex[NUM_Produtos];

int main(){
    ACAO identificador[NUM_Threads];//tipo criado necessario para o funcionamento da thread
    pthread_t Ler_arq[NUM_Threads];
    int produtos[NUM_Produtos];//produtos vao variar de 1-8
    inicializa_valores(identificador,produtos);
    
    for (int i = 0; i < NUM_Produtos; i++) {
        produtos[i] = 0;
        pthread_mutex_init(&mutex[i], NULL); // Inicializa um mutex para cada posição do vetor, pois todas essas posicoes sewrao regioes criticas, dado que todas as threads podem acessar qualquer posicao
    }
    for(int i=0;i<NUM_Threads;i++){
        pthread_create(&Ler_arq[i],NULL,Ler_arquivo,&identificador[i]);//threads estao com seus respectivos arquivos e agora podem le-los.          
    }
    for (int i = 0; i < NUM_Threads; i++) {
        pthread_join(Ler_arq[i], NULL);
        pthread_mutex_destroy(&mutex[i]);
    }
    pthread_exit(NULL);
}
void inicializa_valores (ACAO * Leitura, int* produtos){
    int arq_por_thread=(NUM_ARQ/NUM_Threads) +1;
    char* filename[NUM_ARQ]={"arq1.in","arq2.in","arq3.in","arq4.in","arq5.in"};//nome dos arquivos
    int aux;
    for(int i=0;i<NUM_Threads;i++){//inicializar alguns valores da thread
        Leitura[i].num_arq=0;
        Leitura[i].id=i;
        Leitura[i].ocupada=false;
        Leitura[i].acesso_a_produtos=produtos;
        Leitura[i].arquivos_da_thread=(char**)malloc(sizeof(char*)*arq_por_thread);
        for (int j = 0; j < arq_por_thread; j++) {
            Leitura[i].arquivos_da_thread[j] = (char*)malloc(sizeof(char) * 15);//15 sera o tamanho maximo do nome do arquivo
        }
    }
    for(int i=0;i<NUM_ARQ;i++){//copiar os arquivos para cada struct ACAO
        aux=i%NUM_Threads;  // a logica aqui implementada foi: para cada thread com id "i", essa thread ira guardar o nome de n arquivos subdividos para cada thread de maneira "justa". 
                            // Por conta disso, foi feito um calculo para a quantidade de arq_por_thread e cada thread,na posicao aux=i%NUM_Thread(aux varia de 0-2)
                            //recebera o arquivo i de nome filename[i] dado a logica de modulo.
        strcpy(Leitura[aux].arquivos_da_thread[Leitura[aux].num_arq],filename[i]);
        Leitura[aux].num_arq++;//numero de arquivos de cada thread, onde este pode variar de thread para thread, pois 5/3=1,66. Diante disso, uma thread tera mais arquivos para ler 
    }   
}
void* Ler_arquivo (ACAO* thread_atual){//Agora a logica de leitura dos arquivos salvos na struct THREAD
    FILE* atual;
    int x;
    for(int i=0;i<thread_atual->num_arq;i++){
        printf("arquivo atual: %s\n",thread_atual->arquivos_da_thread[i]);
        atual=fopen(thread_atual->arquivos_da_thread[i],"r");
        if (atual == NULL){
            perror("Erro ao abrir o arquivo");
            exit(EXIT_FAILURE); // Encerrar o programa devido ao erro
        }
        while(fscanf(atual, "%d", &x)!=EOF){
            pthread_mutex_lock(&mutex[x-1]);
            thread_atual->acesso_a_produtos[x-1]++;
            pthread_mutex_unlock(&mutex[x-1]);
        }
        fclose(atual);
    }
    pthread_exit(NULL);
}
