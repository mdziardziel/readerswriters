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

int readers = 0; //czytelnikow  sekcji
int writers = 0; //pisarzy w sekcji
int writers_wait = 0; //czekajacych pisarzy
int readers_wait = 0; // czekajacych czytelnikow
int token = 0; //kto teraz priorytetowo wejdzie - czytelnik czy pisarz


int spij = 1;


void* writer(void *p) {
        while(1) {
                pthread_mutex_lock(&mutex2); // mutex2 - wykluczanie writers_wait i readers_wait
                writers_wait++; //dodaje czekajacego pisarza
                pthread_mutex_unlock(&mutex2);

                pthread_mutex_lock(&mutex); //mutex - wykluczanie readers, writers
                pthread_mutex_lock(&mutex2);
                if(readers > 0) { //jesli jest jakis czytelnik w sekcji
                        pthread_mutex_unlock(&mutex2);
                        pthread_cond_wait(&cond_writers, &mutex); //to czekaj
                }else if(readers_wait > 0 && token == 0) { //jesli brak czytelnika w sekcji i jakis czytelnik czeka i czytelnicy maja priorytet
                        pthread_mutex_unlock(&mutex2);
                        pthread_cond_wait(&cond_writers, &mutex); // to czekaj

                }else{
                        pthread_mutex_unlock(&mutex2);
                }
                pthread_mutex_lock(&mutex2);
                writers_wait--; // odejmuje czekajacego pisarza
                pthread_mutex_unlock(&mutex2);
                writers++; //dodaje pisarza w sekcji

                printf("PISZE  --- w sekcji: %d czytelników, %d pisarzy --- oczekuje: %d czytelników, %d pisarzy --- id : %lu \n", readers, writers, readers_wait, writers_wait, pthread_self());
                sleep(1);
                token = 0; // zmieniam priorytet na czytelnika
                writers--; // odejmuje pisarza w sekcji
                pthread_cond_signal(&cond_readers); //daje sygnal czytelnikowi, ze pisarz wyszedl
                pthread_mutex_unlock(&mutex);
        }
}

void* reader(void *p) {
        while(1) {
                pthread_mutex_lock(&mutex2);
                readers_wait++; //dodaje czytelnika do sekcji
                pthread_mutex_unlock(&mutex2);

                pthread_mutex_lock(&mutex);
                pthread_mutex_lock(&mutex2);
                if(writers_wait > 0 && token == 1) { //jesli pisarz czeka i ma priorytet
                        pthread_mutex_unlock(&mutex2);
                        pthread_cond_wait(&cond_readers, &mutex); // to czekaj
                }  else{ //jesli pisarz nie czeka lub nie ma priorytetu to przechodzi dalej
                        pthread_mutex_unlock(&mutex2);
                }
                pthread_mutex_lock(&mutex2);
                readers_wait--; // odejmuje czekajacego czytelnika
                pthread_mutex_unlock(&mutex2);
                readers++; // dodaje czytelnika w sekcji
                pthread_mutex_unlock(&mutex);


                printf("CZYTAM --- w sekcji: %d czytelników, %d pisarzy --- oczekuje: %d czytelników, %d pisarzy --- id : %lu \n", readers, writers, readers_wait, writers_wait, pthread_self());
                sleep(1);
                pthread_mutex_lock(&mutex);
                token = 1; //zmieniam priorytet na pisarza
                readers--; //odejmuje czytelnika w sekcji
                if(readers==0) { //jesli brak czytelnikow w sekcji
                        pthread_cond_signal(&cond_writers); //to daje sygnal czytelnikowi, ze moze wejsc
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
