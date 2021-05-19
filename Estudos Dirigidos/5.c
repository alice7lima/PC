/*Estudo dirigido 5 - Alice da Silva de Lima 180112601*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAXCANIBAIS 20
pthread_cond_t co_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t ca_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


int porcoes = 0;
int can = 0;

void *canibal(void*meuid);
void *cozinheiro(int m);

void main(argc, argv)
int argc;
char *argv[];
{
  int erro;
  int i, n, m;
  int *id;

  pthread_t tid[MAXCANIBAIS];

  if(argc != 3){
    printf("erro na chamada do programa: jantar <#canibais> <#comida>\n");
    exit(1);
  }
  
  n = atoi (argv[1]); //número de canibais
  m = atoi (argv[2]); // quantidade de porções que o cozinheiro consegue preparar por vez
  printf("numero de canibais: %d -- quantidade de comida: %d\n", n, m);

  if(n > MAXCANIBAIS){
    printf("o numero de canibais e' maior que o maximo permitido: %d\n", MAXCANIBAIS);
    exit(1);
  }
  
  for (i = 0; i < n; i++)  {
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tid[i], NULL, canibal, (void *) (id));

    if(erro){
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  cozinheiro(m);
}

void * canibal (void* pi){

  
  while(1) {
    pthread_mutex_lock(&lock);

    while(porcoes == 0){                                                        /*se nao ha pocoes o canibal deve dormir*/
      printf("%d: nao tem comida, vou dormir\n", *(int *)(pi));
      sleep(1);
      pthread_cond_wait(&ca_cond, &lock);                                       /*sleep do canibal*/
    }
  
    printf("%d: vou me servir, qtd de porcoes: %d\n", *(int *)(pi), porcoes);   /*se ha comida o canibal pode se servir*/

    
    porcoes--;                                                                  /*decremento de porcoes*/
    if(porcoes == 0){                                                           /*se nao ha porcoes, canibal acorda cozinheiro*/
      printf("acorda cozinheiro!!!\n");
      pthread_cond_signal(&co_cond);                                          
    }
    //pegar uma porção de comida e acordar o cozinheiro se as porções acabaram
    pthread_mutex_unlock(&lock);
    printf("%d: vou comer a porcao que peguei\n", *(int *)(pi));
    sleep(5);
  }
  
}

void *cozinheiro (int m){
 
  while(1){
     
    //dormir enquanto tiver comida
    pthread_mutex_lock(&lock);
    while(porcoes > 0){                                       /*se ha porcoes o cozinheiro dorme*/
      printf("tem porcao ainda, dormindo...\n");              
      sleep(1);
      pthread_cond_wait(&co_cond,&lock);                      /*sleep do cozinheiro*/
    }

    printf("cozinheiro: vou cozinhar\n");                     
    sleep(5);
    porcoes = m;                                              /*cozinha as porcoes*/
    pthread_cond_broadcast(&ca_cond);                         /*acorda canibais*/
    pthread_mutex_unlock(&lock);
    //acordar os canibais
   }

}
