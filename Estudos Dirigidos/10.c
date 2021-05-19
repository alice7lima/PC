/*Estudo Dirigido 10 - Alice da Silva de Lima 180112601*/
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "unistd.h"

#define N 10 //número de usuários

#define CARTAS 20 //quantidade de cartas na mochila

pthread_cond_t p_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t u_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int cont = 0; 
int espera = 0;

void * f_usuario(void *arg);
void * f_pombo(void *arg);

int main(int argc, char **argv){
    int i;

    pthread_t usuario[N];
    int *id;
    for(i = 0; i < N; i++){
         id = (int *) malloc(sizeof(int));
          *id = i;
	pthread_create(&(usuario[i]),NULL,f_usuario,  (void *) (id));
    }
     pthread_t pombo;
     id = (int *) malloc(sizeof(int));
     *id = 0;
     pthread_create(&(pombo),NULL,f_pombo, (void*) (id));

     pthread_join(pombo,NULL);
   
}


void * f_pombo(void *arg){
  
    while(1){
        pthread_mutex_lock(&lock);
        while(cont < CARTAS){                   /*enquanto a mochila nao encheu o pombo dorme (20 cartas)*/
            printf("pombo dormindo\n");
            sleep(1);
            pthread_cond_wait(&p_cond,&lock);
        }

        printf("pombo levando as cartas para B...\n");

        /*leva as cartas para B e volta para A*/
            cont = 0;
            pthread_cond_broadcast(&u_cond);            /*acorda os usuarios*/
            printf("pombo esta voltando para A...\n");      
            espera = 0;                                 /*altera o valor da flag*/
        pthread_mutex_unlock(&lock);  
    }
}

void * f_usuario(void *arg){
    
    //int id = *(int*) arg;
    while(1){
	//Escreve uma carta
        sleep(2);
        pthread_mutex_lock(&lock);
        while(espera || cont == CARTAS){           /*caso o pombo não esteja em A ou a mochila estiver cheia, então dorme*/
            printf("usuario %d dormindo\n", *(int*) arg);
            sleep(1);
            pthread_cond_wait(&u_cond,&lock);   
        }

            cont++;                              /*posta sua carta na mochila do pombo*/
            printf("%d: Carta postada! Numero de cartas na mochila: %d\n", *(int*) arg, cont); 
            sleep(2);

        if(cont == CARTAS){                     /*caso a mochila fique cheia, acorda o pombo*/
            espera = 1;                         /*faz os novos usuarios esperarem o pombo voltar*/
            printf("%d: hora de acordar o pombo\n", *(int*) arg); 
            pthread_cond_signal(&p_cond);       /*acorda o pombo*/
        }
        pthread_mutex_unlock(&lock);
    }
}
