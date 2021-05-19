/*Estudo Dirigido 4 - Alice da Silva de Lima 180112601*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define PR 1 //número de produtores
#define CN 1 // número de consumidores
#define N 5  //tamanho do buffer
int cont = 0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t pr_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cn_cond = PTHREAD_COND_INITIALIZER;


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
   
  for (i = 0; i < PR; i++)
  {
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tid[i], NULL, produtor, (void *) (id));

    if(erro)
    {
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

void produz_item(){
  printf("produzindo item....\n");
}

void insere(){
  printf("Item inserido, tamanho do buffer: %d\n", cont);
}

void * produtor (void* pi){
  
  while(1){
    pthread_mutex_lock(&lock);
    while(cont == N){                                          /*se o buffer esta cheio o produtor dorme*/
      printf("produtor foi dormir, buffer cheio\n");
      sleep(1);
      pthread_cond_wait(&pr_cond,&lock);                       /*sleep do produtor*/
    }

    pthread_mutex_unlock(&lock);

    produz_item();                                             /*caso o buffer seja menor que N-1 o produtor deve produzir item*/

    pthread_mutex_lock(&lock);
    cont++;                                                    /*incremento no buffer*/
    insere();                                                  /*insercao do item no buffer*/
    sleep(1);
    if(cont == 1){                                             /*se existe um item no buffer o consumidor deve ser acordado*/
      printf("acorda consumidor!!!\n");
      pthread_cond_signal(&cn_cond);                           /*acorda o consumidor*/
    }
    pthread_mutex_unlock(&lock);
  }

  pthread_exit(0);
  
}

void remocao(){
  printf("item removido\n");
}


void * consumidor (void* pi){
  while(1){
    pthread_mutex_lock(&lock);      
    while(cont == 0){                                             /*enquanto nao ha item no buffer o consumidor dorme*/
      printf("consumidor dormindo, nada pra cosumir :(\n");
      sleep(1);
      pthread_cond_wait(&cn_cond,&lock);                          /*sleep do consumidor*/
    }
    pthread_mutex_unlock(&lock);

    remocao();                                                    /*se existir item no buffer consumidor remove o item do buffer*/

    pthread_mutex_lock(&lock);                            
    cont--;                                                       /*decremento do buffer*/
    printf("Item consumido, tamanho do buffer: %d\n", cont);    
    sleep(1);
    if(cont == N-1){                                              /*se tem espaco no buffer pra produzir acorda o professor*/
      printf("acorda produtor!!!\n");
      pthread_cond_signal(&pr_cond);                              /*acorda o produtor*/
    }
    pthread_mutex_unlock(&lock);                  
  }
  pthread_exit(0);
  
}
