/*Estudo Dirigido 2 - Alice da Silva de Lima 18/0112601*/
#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "pthread.h"

#define TRUE 1

#define NE 3 //numero de escritores
#define NL 10 //numero de leitores

pthread_mutex_t lock_bd = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_nl = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_w = PTHREAD_MUTEX_INITIALIZER;		//lock auxiliar para liberar acesso ao bd para escritores

int num_leitores = 0;
int escritores = 0;				//contador auxiliar de escritores

void * reader(void *arg);
void * writer(void *arg);
void read_data_base();
void use_data_read();
void think_up_data();
void write_data_base();

int main() {

	pthread_t r[NL], w[NE];
	int i;
        int *id;
        /* criando leitores */
    	for (i = 0; i < NL ; i++) {
	   id = (int *) malloc(sizeof(int));
           *id = i;
		 pthread_create(&r[i], NULL, reader, (void *) (id));
	}
	 /* criando escritores */
	for (i = 0; i< NE; i++) {
	   id = (int *) malloc(sizeof(int));
           *id = i;
		 pthread_create(&w[i], NULL, writer, (void *) (id));
	}
	pthread_join(r[0],NULL);
	return 0;
}

void * reader(void *arg) {
	int i = *((int *) arg);
	
	while(TRUE) {                            /* repete para sempre */

		if(escritores > 0){   /*se existir algum escritor querendo escrever no banco, os novos leitores sao impedidos de ler*/ 
			pthread_mutex_lock(&lock_w); /*do banco pelo lock_w*/
		}
		pthread_mutex_lock(&lock_nl);		/*lock que permite o incremento do contador de leitores um de cada vez*/
			num_leitores++;

		if(num_leitores == 1){ 				/*caso seja o primeiro leitor o lock e ativado para que apenas leitores utilizem*/
			pthread_mutex_lock(&lock_bd);
		}
		pthread_mutex_unlock(&lock_nl);		/*libera o incremento de num_leitores para proximo leitor*/

			read_data_base(i);              /* acesso aos dados */

		pthread_mutex_lock(&lock_nl);       /*lock que permite o decremento do contador de leitores um de cada vez*/
		num_leitores--;

		if(num_leitores == 0){ 	            /*caso seja o ultimo leitor que acessou o banco o acesso e liberado*/
			pthread_mutex_unlock(&lock_bd);
		}
		pthread_mutex_unlock(&lock_nl);     /*libera o decremento de num_leitores para proximo leitor*/
		use_data_read(i);                  /* região não crítica */
	}
    pthread_exit(0);
}

void * writer(void *arg) {
	int i = *((int *) arg);
	while(TRUE) {               /* repete para sempre */
		think_up_data(i);      /* região não crítica */
	
		escritores++;   				    /*contador que identifica se ha algum escritor querendo escrever no banco*/
        pthread_mutex_lock(&lock_bd);       /*lock para que o escritor escreva no banco sem que outro tente realizar transacao*/
            write_data_base(i);             /*atualiza os dados */
	    pthread_mutex_unlock(&lock_bd);	    /*libera acesso ao banco*/
		escritores--;					    /*decrementa quantidade de escritores*/	

		/*ao finalizar a escrita, e o lock que esta em reader e liberado e leitores podem ler do banco*/
		pthread_mutex_unlock(&lock_w);	    
    }
    pthread_exit(0);
}

void read_data_base(int i) {
	printf("Leitor %d está lendo os dados! Número de leitores: %d\n", i,num_leitores);
	sleep(rand() % 5);
}

void use_data_read(int i) {
	printf("Leitor %d está usando os dados lidos! Número de leitores: %d\n", i,num_leitores);
	sleep(rand() % 5);
}

void think_up_data(int i) {
	printf("Escritor %d está pensando no que escrever!\n", i);
	sleep(rand() % 5);
}

void write_data_base(int i) {
	printf("Escritor %d está escrevendo os dados! Número de leitores: %d\n", i,num_leitores);
	sleep( rand() % 5 + 5);
}
