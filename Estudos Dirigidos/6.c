/*Estudo dirigido 6 - Alice da Silva de Lima 180112601*/ 

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define PR 4 //número de produtores
#define CN 4 // número de consumidores
#define N 5  //tamanho do buffer

int cont = 0;

sem_t empty;            /*semaforo para controle dos lugares vazios do buffer*/
sem_t full;             /*semaforo para controle dos lugares preenchidos do buffer*/
sem_t lock;             /*semaforo para controle da regiao critica*/

void * produtor(void * meuid);
void * consumidor (void * meuid);


void main(argc, argv)
int argc;
char *argv[];
{

  int erro;
  int i, n, m;
  int *id;

  pthread_t tid[PR];

  /*inicializacao dos semaforos*/
  sem_init(&empty,0,N);
  sem_init(&full,0,0);
  sem_init(&lock,0,1);
   
  for (i = 0; i < PR; i++){
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tid[i], NULL, produtor, (void *) (id));

    if(erro){
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  pthread_t tCid[CN];

  for (i = 0; i < CN; i++){
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tCid[i], NULL, consumidor, (void *) (id));

    if(erro){
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }
 
  pthread_join(tid[0],NULL);

}

void insere_item(){           /*funcao auxiliar que simula insercao*/
  printf("inserindo item... quantidade: %d\n", cont);
}

void * produtor (void* pi){
 
  while(1){
    printf("produzindo item...\n");
    sleep(1);
    sem_wait(&empty);                 /*decresce empty*/

    sem_wait(&lock);                 /*regiao critica*/
      cont++;
      insere_item();
    sem_post(&lock);

    sem_post(&full);                /*incrementa os lugares preenchidos*/

  }
  pthread_exit(0);
  
}

void remove_item(){                /*funcao auxiliar que simula remocao*/
  printf("removendo item... quantidade: %d\n", cont);
}

void * consumidor (void* pi){
  
  while(1){
    sleep(1);
    sem_wait(&full);                 /*decrementa full*/

    sem_wait(&lock);                 /*regiao critica*/
      cont--;
      remove_item();
    sem_post(&lock);                

    sem_post(&empty);               /*incrementa os lugares vazios*/
    printf("o item está sendo consumido...\n");
  }
  pthread_exit(0);
  
}
