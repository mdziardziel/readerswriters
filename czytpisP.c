#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <pthread.h>


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_reader = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_writer = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_writers = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_readers = PTHREAD_COND_INITIALIZER;

int szansa = 0;
int readers = 0;
int writers = 0;
int writers_wait = 0;
int readers_wait = 0;
int token = 1;

int print = 1;
int petla = 100;
int npis = 0;
int nczyt = 0;
int spij = 1;

void* writer(void *p) {
        while(1) {

                pthread_mutex_lock(&mutex);
                writers_wait++;
                pthread_mutex_unlock(&mutex);


                pthread_mutex_lock(&mutex_writer);
                pthread_mutex_lock(&mutex);
                writers_wait--;
                writers++;
                for(int i = 0; i <print; i++)
                        printf("PISZE  --- w sekcji: %d czytelników, %d pisarzy --- oczekuje: %d czytelników, %d pisarzy --- id procesu: %lu \n", readers, writers, readers_wait, writers_wait, pthread_self());
                writers--;
                token = 0;

                pthread_mutex_unlock(&mutex);
                pthread_mutex_unlock(&mutex_writer);
                pthread_cond_signal(&cond_readers);


                sleep(spij);
        }
}

void* reader(void *p) {
        while(1) {
                pthread_mutex_lock(&mutex);
                readers_wait++;
                //printf("Czytelnicy czekajacy: %d\n", readers_wait);
                if(writers_wait > 0 && token == 1 ) {
                        pthread_cond_wait(&cond_readers, &mutex);
                }
                if(readers == 0) {
                        pthread_mutex_lock(&mutex_writer);
                }
                readers_wait--;
                readers++;
                pthread_mutex_unlock(&mutex);

                for(int i = 0; i <print; i++)
                        printf("CZYTAM --- w sekcji: %d czytelników, %d pisarzy --- oczekuje: %d czytelników, %d pisarzy --- id procesu: %lu \n", readers, writers, readers_wait, writers_wait, pthread_self());

                pthread_mutex_lock(&mutex);
                readers--;
                token = 1;
                if(readers==0) {
                        pthread_mutex_unlock(&mutex_writer);
                }
                pthread_mutex_unlock(&mutex);

                sleep(spij);
        }
}

int main(){



        pthread_t writer1th;
        pthread_create(&writer1th, NULL, writer, NULL);
        pthread_t writer2th;
        pthread_create(&writer2th, NULL, writer, NULL);
        pthread_t writer3th;
        pthread_create(&writer3th, NULL, writer, NULL);
        pthread_t writer4th;
        pthread_create(&writer4th, NULL, writer, NULL);

        pthread_t reader1th;
        pthread_create(&reader1th, NULL, reader, NULL);
        pthread_t reader2th;
        pthread_create(&reader2th, NULL, reader, NULL);
        pthread_t reader3th;
        pthread_create(&reader3th, NULL, reader, NULL);
        pthread_t reader4th;
        pthread_create(&reader4th, NULL, reader, NULL);

        while(1) ;
}
