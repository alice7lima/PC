/*Trabalho 1 - Alice da Silva de Lima 180112601*/

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N_ALUNOS 13
#define N_MESAS 7
#define L 4         /*numero de latas*/

/*cores usadas para o output no terminal*/
#define AZUL "\x1b[34m"
#define CINZA "\x1b[90m"
#define AZULC "\x1b[94m"
#define VERDE "\x1b[92m"
#define MAG "\x1b[35m"
#define RESET "\x1b[0m"
#define AMARELO "\x1b[93m"
#define CIANO "\x1b[96m"

sem_t sem_mesas;        /*mesas da sala*/
pthread_cond_t alunos = PTHREAD_COND_INITIALIZER;   /*variavel de condicao dos alunos*/
pthread_cond_t robo = PTHREAD_COND_INITIALIZER;     /*variavel de condicao dos professores*/
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;   /*mutex para exclusao mutua nas regioes criticas*/

int latas = 4;                  /*contador auxiliar das latas*/


void * f_robo(void *v) {                  

  while(1) {
    pthread_mutex_lock(&lock);

    while(latas > 0){                       /*enquanto tem latas no frigobar o robo dorme*/
      printf(AMARELO"Robo dormindo, ainda tem lata no frigobar\n"RESET);
      sleep(1);
      pthread_cond_wait(&robo,&lock);                       
    }

    printf(CIANO"Robo: vou levar as latas e recolher o dinheiro\n"RESET);     /*quando as latas acabaram o robo faz a reposicao e pega o dinheiro*/
    sleep(4);
    latas = L;
    pthread_cond_broadcast(&alunos);      /*robo acorda os alunos pois ja tem latas disponiveis*/
    pthread_mutex_unlock(&lock);
  }
  pthread_exit(0);
}

void frigobar(int id){

  pthread_mutex_lock(&lock);

  while(latas == 0){                  /*enquanto o frigobar estiver vazio os alunos que desejam comprar um energetico aguardam*/
    printf(AMARELO"Aluno %d esperando o robo repor as latas\n"RESET, id);
    sleep(1);                           
    pthread_cond_wait(&alunos,&lock);         
  }
  /*se o frigobar tem energeticos um aluno pode entrar e comprar*/
  printf(AZULC"Aluno %d: vou comprar uma latinha, ainda tem %d\n"RESET, id, latas);

  latas--;            /*decremento do contador auxiliar*/
  sleep(1);
  if(latas == 0){         /*se as latas acabaram o aluno que comprou a ultima deve chamar o robo*/
    printf(VERDE"%d: Vou chamar o robo\n"RESET, id);
    pthread_cond_signal(&robo);     /*acordando o robo*/
  }

  printf(AZULC"%d jogando a latinha vazia fora\n", id);
  pthread_mutex_unlock(&lock);
}

void estudo(int id){
  int aux = rand() % 10;                /*gerando um numero randomico para decidir se o aluno compra ou nao um energetico*/

  printf("Aluno %d estudando...\n", id);

  if(aux%2 == 0){                      /*se o numero gerado e par entao o aluno pode comprar um energetico*/
    frigobar(id);
  }

  sleep(rand() % 4);                  /*permanencia na sala*/

}

void* sala(void* v) {
  int id = *(int*) v;

  while(1){
    sleep(1);
    if(sem_trywait(&sem_mesas) == 0){    /*o aluno pega uma mesa na sala se houver disponibilidade*/
      printf(AZUL"Aluno %d entrou na sala \n"RESET,id);
      
      sleep(2);
      
        estudo(id);                       /*aluno vai estudar*/

      sem_post(&sem_mesas);             /*aluno sai da sala e libera uma mesa*/        
      printf(MAG"Aluno %d esta indo embora \n"RESET,id);

    }
    else{ /*se a sala esta cheia*/
      printf(CINZA"Sala cheia, aluno %d vai ter que andar ate a BCE...\n"RESET,id);
      sleep(2);
    }
  }

  pthread_exit(0);
}

int main() {
  pthread_t alunos[N_ALUNOS], robo;
  int i, id[N_ALUNOS];
  int *a;


  sem_init(&sem_mesas, 0, N_MESAS);          /*inicializacao do semaforo das mesas*/
  
  for (i = 0; i < N_ALUNOS; i++) {          /*inicializacao das threads dos alunos*/
    id[i] = i;
    pthread_create(&alunos[i], NULL, sala, (void*) &id[i]);
  }
  
  a = (int *) malloc(sizeof(int));
  *a = 0;
  pthread_create(&(robo), NULL, f_robo, (void*) (a));     /*inicializacao da thread do robo*/
  pthread_join(robo,NULL);
  
  return 0;
}