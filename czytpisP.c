#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

static struct sembuf buf;

void podnies(int semid, int semnum, int ile){
        buf.sem_num = semnum;
        buf.sem_op = ile;
        buf.sem_flg = 0;
        if (semop(semid, &buf, 1) == -1) {
                perror("Podnoszenie semafora");
                exit(1);
        }
}

void opusc(int semid, int semnum, int ile){
        buf.sem_num = semnum;
        buf.sem_op = ile;
        buf.sem_flg = 0;
        if (semop(semid, &buf, 1) == -1) {
                perror("Opuszczenie semafora");
                exit(1);
        }
}

int main(){
        int semid;

        int tugboats = 10;
        int ports = 3;
        int waiting = 0;
        int in = 0;




        semid = semget(45281, 4, 0600);
        if (semid == -1) {
                perror("Utworzenie tablicy semaforow");
                exit(1);
        }
        else{
                if (semctl(semid, 0, SETVAL,1)  == -1) {
                        perror("Nadanie wartosci semaforowi 0");
                        exit(1);
                }
                if (semctl(semid, 1, SETVAL, tugboats) == -1) {
                        perror("Nadanie wartosci semaforowi 1");
                        exit(1);
                }
                if (semctl(semid, 2, SETVAL, ports) == -1) {
                        perror("Nadanie wartosci semaforowi 2");
                        exit(1);
                }
        }




        int pom;
        for(int i = 1; i <= 10; i++) {
                if(i<10) {
                        if(fork()) {
                                opusc(semid, 0,-1);
                                pom = i;
                                podnies(semid, 0,1);
                                break;
                        }
                }else {
                        opusc(semid, 0,-1);
                        pom = i;
                        podnies(semid, 0,1);
                }

        }

        opusc(semid, 0,-1);
        int weight = pom;
        podnies(semid, 0,1);

        while(1) {

                // opusc(semid, 0,-1);
                // waiting++;
                // podnies(semid, 0,1);


                opusc(semid, 1, weight*(-1));
                opusc(semid, 2, -1);

                // opusc(semid, 0,-1);
                // waiting--;
                // in++;
                // podnies(semid, 0,1);

                // opusc(semid, 0,-1);
                ports = semctl(semid, 2, GETVAL, ports);
                tugboats = semctl(semid, 1, GETVAL, ports);
                if (ports == -1 || tugboats == -1) {
                        perror("Pobieranie wartości semaforow");
                        exit(1);
                }
                else{
                        //printf("W PORCIE id %d ---WOLNE: porty %d, holowniki %d, czeka %d, w %d \n", weight,ports, tugboats, waiting, in);
                        printf("W PORCIE id %d ---WOLNE: porty %d, holowniki %d\n", weight,ports, tugboats);
                }
                // podnies(semid, 0,1);

                sleep(2);

                // opusc(semid, 0,-1);
                // in--;
                // podnies(semid, 0,1);

                podnies(semid, 2, 1);
                podnies(semid, 1, weight);


        }
}
