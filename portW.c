#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <pthread.h>


pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2= PTHREAD_COND_INITIALIZER;


int tugboats = 10;
int ports = 3;
int waiting = 0;
int pom = 0;


int print = 9999999;
int spij = 1;
/*
   MUTEX2 CHRONI ZMIENNE WRITERS_WAIT ORAZ READERS_WAIT




 */


void* ship(void *p) {
        while(1) {
                int weight = *(int *)p;
                pthread_mutex_lock(&mutex1);
                waiting++;
                while(tugboats<weight || ports==0) {
                        if(pom < waiting) {
                                pthread_cond_signal(&cond1);
                        }else{
                                pom = 0;
                        }
                        pthread_cond_wait(&cond1, &mutex1);
                        pom++;
                }
                tugboats-=weight;
                ports--;
                waiting--;
                pthread_cond_signal(&cond1);
                pthread_mutex_unlock(&mutex1);

                printf("W PORCIE --- WOLNE: holowniki %d porty %d czeka %d --- id : %lu \n",tugboats, ports, waiting, pthread_self());
                sleep(weight/2);

                pthread_mutex_lock(&mutex1);
                tugboats+=weight;
                ports++;
                pthread_cond_signal(&cond1);
                pthread_mutex_unlock(&mutex1);

                sleep(spij + weight/3);
        }
}

void* test(void *p){
        while(1) {
                printf("TEST  --- WOLNE: holowniki %d porty %d czeka %d \n", tugboats, ports, waiting);
                sleep(1);
        }
}



int main(){

        int weight1=1,weight2=2,weight3=3,weight4=4,weight5=5,weight6=6,weight7=7,weight8=8,weight9=9,weight10=10;
        pthread_t ship1th;
        pthread_create(&ship1th, NULL, ship, &weight1);
        pthread_t ship2th;
        pthread_create(&ship2th, NULL, ship, &weight2);
        pthread_t ship3th;
        pthread_create(&ship3th, NULL, ship, &weight3);
        pthread_t ship4th;
        pthread_create(&ship4th, NULL, ship, &weight4);
        pthread_t ship5th;
        pthread_create(&ship5th, NULL, ship, &weight5);
        pthread_t ship6th;
        pthread_create(&ship6th, NULL, ship, &weight6);
        pthread_t ship7th;
        pthread_create(&ship7th, NULL, ship, &weight7);
        pthread_t ship8th;
        pthread_create(&ship8th, NULL, ship, &weight8);
        pthread_t ship9th;
        pthread_create(&ship9th, NULL, ship, &weight9);
        pthread_t ship10th;
        pthread_create(&ship10th, NULL, ship, &weight10);

        //pthread_t test1;
        //pthread_create(&test1, NULL, test,NULL);

        while(1) ;
}
