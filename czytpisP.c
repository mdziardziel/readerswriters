#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <fcntl.h>

static struct sembuf buf;

void podnies(int semid, int semnum){
        buf.sem_num = semnum;
        buf.sem_op = 1;
        buf.sem_flg = 0;
        if (semop(semid, &buf, 1) == -1) {
                perror("Podnoszenie semafora");
                exit(1);
        }
}

void opusc(int semid, int semnum){
        buf.sem_num = semnum;
        buf.sem_op = -1;
        buf.sem_flg = 0;
        if (semop(semid, &buf, 1) == -1) {
                perror("Opuszczenie semafora");
                exit(1);
        }
}


int main(){
        int semid, shmid;
        int * buff;
        //miejsca zmiennych w buforze
        //int token = 0; buf0 - za pomoca niego uprzywilejowuje w danej chwili czytelnika/pisarza
        //int readers = 0; buf1 - czytelnikow w sekcji
        //int writers = 0; buf2 - pisarzy w sekcji
        //int writers_wait = 0; buf3 - pisarzy czekajacych
        //int readers_wait = 0; buf4 - czytelnikow czytajacych

        int *buf;
        shmid = shmget(45281, 5*sizeof(int), IPC_CREAT|0600);
        if (shmid == -1) {
                perror("Utworzenie segmentu pamieci wspoldzielonej");
                exit(1);
        }
        buf = (int*)shmat(shmid, NULL, 0);
        if (buf == NULL) {
                perror("Przylaczenie segmentu pamieci wspoldzielonej");
                exit(1);
        }



        semid = semget(45281, 4, 0600);
        semid = semget(45281, 4, IPC_CREAT|IPC_EXCL|0600);
        if (semid == -1) {
                semid = semget(45281, 4, 0600);
                if (semid == -1) {
                        perror("Utworzenie tablicy semaforow");
                        exit(1);
                }
        }
        if(semid) {
                if (semctl(semid, 0, SETVAL,1)  == -1) {
                        perror("Nadanie wartosci semaforowi 0");
                        exit(1);
                }
                if (semctl(semid, 1, SETVAL, 1) == -1) {
                        perror("Nadanie wartosci semaforowi 1");
                        exit(1);
                }
                if (semctl(semid, 2, SETVAL, 1) == -1) {
                        perror("Nadanie wartosci semaforowi 2");
                        exit(1);
                }
                if (semctl(semid, 3, SETVAL, 1) == -1) {
                        perror("Nadanie wartosci semaforowi 2");
                        exit(1);
                }
        }


        //INITIAL
        for(int i = 0; i<=4; i++) buf[i] = 0;  // zeruje tablice

        int id;
        //tworze procesy i nadaje im id (podzielne przez 2 - czytelnik, niepodzielne - pisarz)
        for(int i = 1; i <= 10; i++) {
                if(i<10) {
                        if(fork()) {
                                id = i;
                                break;
                        }
                }else {
                        id = i;
                }

        }
        //printf("%d",id);


        while(1) {
                if(id%2) { //czytelnicy
                        opusc(semid, 2); //semafor2 - wzajemne wykluczanie buf[3] i buf[4]
                        buf[3]++; //dodaje czekajacego czytelnika
                        podnies(semid, 2);

                        opusc(semid, 0); //semafor0 - wzjemne wykluczanie buf[0], buf[1], buf[2]
                        opusc(semid, 2);
                        if(buf[1]==0 || buf[4]>0) { // jesli brak czytelników w sekcji krytycznej lub pisrz czeka
                                podnies(semid,2);
                                podnies(semid, 0);
                                opusc(semid, 1); //nie wpuszczam do sekcji krytycznej ani pisarzy ani czytelnikow
                                /*
                                   -jesli nie czeka zaden pisarz to kolejni czytelnicy beda mogli wchodzic bez przeszkod
                                   -jesli bedzie pisarz w kolejce to czytelnicy beda blokowani
                                   nastepnie gdy wszyscy czytelnicy wyjdą z sekcji to odblokuje się semafor1
                                   nastepnie wejdzie ktoś kto czeka na semaforze1 - albo czytelnik albo pisarz, wiec nie faworyzuje nikogo
                                 */
                                opusc(semid, 0);
                        }
                        else podnies(semid,2);
                        opusc(semid, 2);
                        buf[3]--; // odejmuje czekajacego czytelnika
                        podnies(semid,2);
                        buf[1]++; // dodaje czytelnika w sekcji
                        podnies(semid, 0);

                        //czytelnika w sekcji krytycznej
                        opusc(semid, 2);
                        printf("CZYTAM --- w sekcji: %d czytelników, %d pisarzy --- oczekuje: %d czytelników, %d pisarzy --- id : %d \n", buf[1], buf[2], buf[3], buf[4], id);
                        podnies(semid, 2);
                        //sleep(1);

                        opusc(semid, 0);
                        buf[1]--; // zmniejszam czytelnikow w sekcji
                        if(buf[1]==0) podnies(semid, 1);  // jesli brak czytelnikow w sekcji to moge wpuscic pisarza
                        podnies(semid, 0);


                }else{ //pisarze

                        opusc(semid, 2);
                        buf[4]++; //dodaje czekajacego pisarza
                        podnies(semid, 2);

                        opusc(semid, 1); // wykluczam pisarza i czytelnika w sekcji krytycznej oraz 2 pisarzy
                        opusc(semid, 0);
                        opusc(semid, 2);
                        buf[4]--; // odejmuje czekajacego pisarza
                        podnies(semid, 2);
                        buf[2]++; // dodaje pisarza w sekcji
                        opusc(semid, 2);
                        printf("PISZE  --- w sekcji: %d czytelników, %d pisarzy --- oczekuje: %d czytelników, %d pisarzy --- id : %d \n", buf[1], buf[2], buf[3], buf[4], id);
                        podnies(semid, 2);
                        //sleep(1);
                        buf[2]--; // odejmuje pisarza w sekcji
                        podnies(semid, 0);
                        podnies(semid, 1);


                }

        }
}
