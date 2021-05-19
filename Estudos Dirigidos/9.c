/*Estudo Dirigido 9 - Alice da Silva de Lima 180112601*/
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "unistd.h"

#define N 10 //número de usuários

#define CARTAS 20 //quantidade de cartas na mochila

sem_t mochila;
sem_t pombo;
sem_t lock;
sem_t usuarios;

int contador = 0;

void * f_usuario(void *arg);
void * f_pombo(void *arg);

int main(int argc, char **argv){
    int i;

    sem_init(&pombo, 0, 0);
    sem_init(&lock, 0, 1);
    sem_init(&usuarios, 0, 0);
    sem_init(&mochila, 0, CARTAS);

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
        //Inicialmente está em A, aguardar/dorme a mochila ficar cheia (20 cartas)
        sem_wait(&pombo);


        /*leva as cartas para B e volta para A*/
        sleep(1);
        printf("pombo levando as cartas para B...\n");

        for(int i = 0; i < 20; i++){            /*da vinte posts no semaforo da mochila*/
            sem_post(&mochila);
        }


        printf("pombo esta voltando para A...\n");
        contador = 0;
        sem_post(&usuarios);        /*acorda os usuarios*/
    }
}

void * f_usuario(void *arg){
    
    while(1){
        int id = *(int*) arg;
        //Caso o pombo não esteja em A ou a mochila estiver cheia, então dorme	
        //Posta sua carta na mochila do pombo
        sleep(1);

        sem_wait(&lock);
        if(sem_trywait(&mochila) == 0){    /*tem vaga na mochila do pombo*/
            contador++;
            sleep(2);
            printf("Usuario %d postou a carta na mochila. Numero de cartas: %d\n",id, contador);
        } 
        else{                            //Caso a mochila fique cheia, acorda o pombo
            printf("Mochila encheu\n");
            sem_post(&pombo);
            sem_wait(&usuarios);        /*pombo nao esta em a ou mochila cheia*/
        }
        sem_post(&lock);          
    }
}
