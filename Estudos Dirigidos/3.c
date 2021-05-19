/*Estudo Dirigido 3 - Alice da Silva de Lima 18/0112601*/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MA 10 //macacos que andam de A para B
#define MB 10 //macacos que andam de B para A

pthread_mutex_t lock_ab = PTHREAD_MUTEX_INITIALIZER;     /*lock para incremento e decremento individuais do contados dos macacos em ab*/
pthread_mutex_t lock_ba = PTHREAD_MUTEX_INITIALIZER;     /*lock para incremento e decremento individuais do contados dos macacos em ba*/
pthread_mutex_t lock_g = PTHREAD_MUTEX_INITIALIZER;       /*lock que da preferencia ao gorila*/

pthread_mutex_t lock_passa = PTHREAD_MUTEX_INITIALIZER;   /*lock para a travessia*/

/*locks auxiliares*/
pthread_mutex_t lock_pa = PTHREAD_MUTEX_INITIALIZER;      
pthread_mutex_t lock_pb = PTHREAD_MUTEX_INITIALIZER;

int g_ab = 0;           /*contador dos macacos que querem atravessar de a para b*/
int g_ba = 0;           /*contador dos macacos que querem atravessar de b para a*/


void * macacoAB(void * a) {
  int i = *((int *) a);    
  while(1){

    if(g_ba > 0){                    /*se existir macacos do outro lado querendo atravessar, os novos macacos sao impedidos de*/    
			pthread_mutex_lock(&lock_pb);  /*comecar a travessia*/
		}
    //Procedimentos para acessar a corda
		pthread_mutex_lock(&lock_ab);           /*lock que permite o incremento do contador dos macacos um de cada vez*/
		g_ab++;

		if(g_ab == 1){                          /*caso seja o primeiro macaco o lock e ativado para a travessia*/
			pthread_mutex_lock(&lock_passa);
		}

    pthread_mutex_unlock(&lock_ab);         /*libera o incremento de g_ab para proximo macaco*/
    printf("Macaco %d passado de A para B \n",i);
    sleep(1);

    pthread_mutex_lock(&lock_ab);           /*lock que permite o decremento do contador de macacos um de cada vez*/
    g_ab--;

    if(g_ab == 0){                        /*caso seja o ultimo macaco que atravessou libera para os do outro lado*/
			pthread_mutex_unlock(&lock_passa);
		}
    pthread_mutex_unlock(&lock_ab);         /*libera o decremento de g_ab para proximo macaco*/

    /*ao finalizar a travessia, e o lock que esta em macacoBA e liberado e os macacos de la podem atravessar*/
    pthread_mutex_unlock(&lock_pa);
	  //Procedimentos para quando sair da corda
  }
    pthread_exit(0);
}

void * macacoBA(void * a) {
  int i = *((int *) a);    
  while(1){

    /*if(num_gorilas > 0){
      pthread_mutex_lock(&lock_g);
    }*/

    if(g_ab > 0){                        /*se existir macacos do outro lado querendo atravessar, os novos macacos sao impedidos de*/ 
			pthread_mutex_lock(&lock_pa);     /*comecar a travessia*/
		}
    //Procedimentos para acessar a corda
    pthread_mutex_lock(&lock_ba);                  /*lock que permite o incremento do contador dos macacos um de cada vez*/
		g_ba++;

		if(g_ba == 1){                                 /*caso seja o primeiro macaco o lock e ativado para a travessia*/
			pthread_mutex_lock(&lock_passa);
		}

    pthread_mutex_unlock(&lock_ba);                /*libera o incremento de g_ba para proximo macaco*/
    printf("Macaco %d passado de B para A \n",i);
    sleep(1);

    pthread_mutex_lock(&lock_ba);             /*lock que permite o decremento do contador de macacos um de cada vez*/
    g_ba--;

    if(g_ba == 0){                           /*caso seja o ultimo macaco que atravessou libera para os do outro lado*/
      pthread_mutex_unlock(&lock_passa);
    }
    pthread_mutex_unlock(&lock_ba);          /*libera o decremento de g_ba para proximo macaco*/

    /*ao finalizar a travessia, e o lock que esta em macacoAB e liberado e os macacos de la podem atravessar*/
    pthread_mutex_unlock(&lock_pb);
    //Procedimentos para quando sair da corda
  }
  pthread_exit(0);
}



void * gorila(void * a){
  while(1){
    //Procedimentos para acessar a corda
    pthread_mutex_lock(&lock_passa);      /*lock ativado para a travessia do gorila*/
      printf("Gorila passado de A para B \n");
    pthread_mutex_unlock(&lock_passa);    /*lock desativado apos a travessia*/
    sleep(5);
    //Procedimentos para quando sair da corda
  }
  pthread_exit(0);
}

int main(int argc, char * argv[])
{
    pthread_t macacos[MA+MB];
    int *id;
    int i = 0;

    for(i = 0; i < MA+MB; i++){
        id = (int *) malloc(sizeof(int));
        *id = i;
        if(i%2 == 0){
          if(pthread_create(&macacos[i], NULL, &macacoAB, (void*)id)){
            printf("Não pode criar a thread %d\n", i);
            return -1;
          }
        }else{
          if(pthread_create(&macacos[i], NULL, &macacoBA, (void*)id)){
            printf("Não pode criar a thread %d\n", i);
            return -1;
          }
        }
    }
    pthread_t g;
    pthread_create(&g, NULL, &gorila, NULL);

  
    pthread_join(macacos[0], NULL);
    return 0;
}
