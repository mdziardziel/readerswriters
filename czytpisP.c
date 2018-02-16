#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define MAX 10

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
        printf("sdfsdf");
        int shmid, semid, i;
        int *buf;

        shmid = shmget(45281, (MAX+2)*sizeof(int), IPC_CREAT|0600);
        if (shmid == -1) {
                perror("Utworzenie segmentu pamieci wspoldzielonej");
                exit(1);
        }

        buf = (int*)shmat(shmid, NULL, 0);
        if (buf == NULL) {
                perror("Przylaczenie segmentu pamieci wspoldzielonej");
                exit(1);
        }

   #define indexZ buf[MAX]
   #define indexO buf[MAX+1]

        semid = semget(45281, 4, IPC_CREAT|IPC_EXCL|0600);
        if (semid == -1) {
                semid = semget(45281, 4, 0600);
                if (semid == -1) {
                        perror("Utworzenie tablicy semaforow");
                        exit(1);
                }
        }
        else{
                indexZ = 0;
                indexO = 0;
                if (semctl(semid, 0, SETVAL, (int)MAX) == -1) {
                        perror("Nadanie wartosci semaforowi 0");
                        exit(1);
                }
                if (semctl(semid, 1, SETVAL, (int)0) == -1) {
                        perror("Nadanie wartosci semaforowi 1");
                        exit(1);
                }
                if (semctl(semid, 2, SETVAL, (int)1) == -1) {
                        perror("Nadanie wartosci semaforowi 2");
                        exit(1);
                }
                if (semctl(semid, 3, SETVAL, (int)1) == -1) {
                        perror("Nadanie wartosci semaforowi 3");
                        exit(1);
                }
        }

        for (i=0; i<10000; i++) {
                opusc(semid, 1);
                opusc(semid, 3);
                printf("Numer: %5d   Wartosc: %5d\n", i, buf[indexO]);
                indexO = (indexO+1)%MAX;
                podnies(semid, 3);
                podnies(semid, 0);
        }
}
