/*Estudo dirigido 11 - Alice da Silva de Lima 180112601*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAXSIZE 10000  /* maximum matrix size */

pthread_barrier_t barrier;  
int size;

int resultado = 0;
int somas[MAXSIZE];                   /*vetor que guarda as somas dos elementos de cada linha da matriz*/
int matrix[MAXSIZE][MAXSIZE];         /*matriz de input*/

void *Worker(void *);

int main(int argc, char *argv[]) {
  int i, j;
 
  
  size = atoi(argv[1]);
  
  if(size > MAXSIZE){
	  printf("Tamanho muito grande!\n");
    return 0;	
  }

  pthread_t workerid[size];
  

  pthread_barrier_init(&barrier, NULL, size);

  for (i = 0; i < size; i++){
    for (j = 0; j < size; j++){
      matrix[i][j] = 1;
    }
  }


  for (i = 0; i < size; i++){
    somas[i] = 0;
  }


  int * id;
  for (i = 0; i < size; i++){
     id = (int *) malloc(sizeof(int));
     *id = i;
     pthread_create(&workerid[i], NULL, Worker, (void *) (id));

  }

  for (i = 0; i < size; i++){
       if(pthread_join(workerid[i], NULL)){
          printf("\n ERROR joining thread");
          exit(1);
       }
  }
  printf("Bye!\n");
}


void *Worker(void *arg) {
  int myid = *(int *)(arg);
  int j, k;

  int self = pthread_self();
 
  printf("worker %d (pthread id %d) has started\n", myid, self);


  for(j = 0; j < size; j++){
    somas[myid] += matrix[myid][j];         /*a soma dos elementos de cada linha da matriz e guardada numa posicao de um vetor de somas*/
  }

  pthread_barrier_wait(&barrier);           /*aguarda o calculo da soma de todas as linhas*/

  if(myid == 0){
    for(k = 0; k < size; k++){
      resultado += somas[k];                /*o resultado e calculando somando cada valor contido no vetor de soma das linhas*/
    }
    printf("Resultado: %d\n", resultado);
  }



}
