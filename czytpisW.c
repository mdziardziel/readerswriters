#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <pthread.h>


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_writer = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_writers = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_readers = PTHREAD_COND_INITIALIZER;

int szansa = 0;
int readers = 0;
int writers = 0;
int writers_wait = 0;
int readers_wait = 0;
int token = 0;

int print = 9999999;
int petla = 100;
int npis = 0;
int nczyt = 0;
int spij = 1;


/*
   MUTEX2 CHRONI ZMIENNE WRITERS_WAIT ORAZ READERS_WAIT




 */


void* writer(void *p) {
        while(1) {
                pthread_mutex_lock(&mutex2);
                writers_wait++;
                pthread_mutex_unlock(&mutex2);

                pthread_mutex_lock(&mutex);
                pthread_mutex_lock(&mutex2);
                if(readers > 0) {
                        pthread_mutex_unlock(&mutex2);
                        pthread_cond_wait(&cond_writers, &mutex);
                }else if(readers_wait > 0 && token == 0) {
                        pthread_mutex_unlock(&mutex2);
                        pthread_cond_wait(&cond_writers, &mutex);

                }else{
                        pthread_mutex_unlock(&mutex2);
                }
                pthread_mutex_lock(&mutex2);
                writers_wait--;
                pthread_mutex_unlock(&mutex2);
                writers++;
                //for(int i = 0; i <print; i++) ;
                printf("PISZE  --- w sekcji: %d czytelników, %d pisarzy --- oczekuje: %d czytelników, %d pisarzy --- id : %lu \n", readers, writers, readers_wait, writers_wait, pthread_self());
                sleep(1);
                token = 0;
                writers--;
                pthread_cond_signal(&cond_readers);
                pthread_mutex_unlock(&mutex);
        }
}

void* reader(void *p) {
        while(1) {
                pthread_mutex_lock(&mutex2);
                readers_wait++;
                pthread_mutex_unlock(&mutex2);

                pthread_mutex_lock(&mutex);
                pthread_mutex_lock(&mutex2);
                if(writers_wait > 0 && token == 1) {
                        pthread_mutex_unlock(&mutex2);
                        pthread_cond_wait(&cond_readers, &mutex);
                }  else{
                        pthread_mutex_unlock(&mutex2);
                }
                pthread_mutex_lock(&mutex2);
                readers_wait--;
                pthread_mutex_unlock(&mutex2);
                readers++;
                pthread_mutex_unlock(&mutex);

                //for(int i = 0; i <print; i++) ;
                printf("CZYTAM --- w sekcji: %d czytelników, %d pisarzy --- oczekuje: %d czytelników, %d pisarzy --- id : %lu \n", readers, writers, readers_wait, writers_wait, pthread_self());
                sleep(1);
                pthread_mutex_lock(&mutex);
                token = 1;
                readers--;
                if(readers==0) {
                        pthread_cond_signal(&cond_writers);
                }
                pthread_mutex_unlock(&mutex);
        }
}

int main(){

        pthread_t writer1th;
        pthread_create(&writer1th, NULL, writer, NULL);
        pthread_t reader1th;
        pthread_create(&reader1th, NULL, reader, NULL);
        pthread_t writer2th;
        pthread_create(&writer2th, NULL, writer, NULL);
        pthread_t reader2th;
        pthread_create(&reader2th, NULL, reader, NULL);
        pthread_t writer3th;
        pthread_create(&writer3th, NULL, writer, NULL);
        pthread_t reader3th;
        pthread_create(&reader3th, NULL, reader, NULL);
        pthread_t writer4th;
        pthread_create(&writer4th, NULL, writer, NULL);
        pthread_t reader4th;
        pthread_create(&reader4th, NULL, reader, NULL);

        while(1) ;
}
