/*Estudo Dirigido 8 - Alice da Silva de Lima 180112601*/
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>

#define N 5
#define ESQ(n) (n+N-1)%N
#define DIR(n)	(n+1)%N

/*estados*/
#define PENSANDO 0
#define FOME 1
#define COMENDO 2

/*vetor de estados*/
int estados[N];

sem_t lock;		/*controle da regiao critica*/

sem_t s[N];		/*filosofos*/

void *filosofos (void *arg);

void pega_talher (int n);
void devolve_talher (int n);
void controle (int n);

int main () {
	int i;
	int *id;
	
	sem_init(&lock, 0, 1);
	//semaforo 
	for (i=0; i<N; i++) {
		sem_init(&s[i], 0, 0);
	}
	pthread_t r[N]; 

	//criacao das threads de filosofos
	for (i = 0; i < N ; i++) {
		id = (int *) malloc(sizeof(int));
		*id = i;
		pthread_create(&r[i], NULL, filosofos, (void*) (id));
	}

        pthread_join(r[0],NULL);
	return 0;
}


void *filosofos (void *arg) {

	int n = *((int *) arg);
	while(1) {
		//pensar
		printf("Filosofo %d pensando ...\n", n);
		sleep(3);
		pega_talher(n);
		//comer
		printf("\tFilosofo %d comendo ...\n", n);
		sleep(3);
                printf("\tFilosofo %d acabou de comer ...\n", n);
		devolve_talher(n);
	} 
}

void pega_talher (int n) {

	sem_wait(&lock);
	estados[n] = FOME;				/*registra que o filosofo esta com fome*/
	sleep(1);
	controle(n);					/*tenta pegar dois garfos */
	sem_post(&lock);				
	sem_wait(&s[n]);				/*bloqueia filosofo caso este nao tenha conseguido pegar os garfos*/


}

void devolve_talher (int n) {
	sem_wait(&lock);				
	estados[n] = PENSANDO;			/*registra que o filosofo esta pensando*/
	controle(ESQ(n));				/*verifica se o filosofo a esquerda deste pode pegar os garfos*/
	controle(DIR(n));				/*verifica se o filosofo a direita deste pode pegar os garfos*/	
	sem_post(&lock);				
}

void controle(int n){
	/*verifica se o filosofo da direita e o da esquerda nao estao comendo*/
	if(estados[n] == FOME && estados[ESQ(n)] != COMENDO && estados[DIR(n)] != COMENDO){		
		estados[n] = COMENDO;  		/*caso nao estejam o filosofo come*/																	
		sem_post(&s[n]);			
	}
}
