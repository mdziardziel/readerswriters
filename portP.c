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
                if (semctl(semid, 1, SETVAL, tugboats) == -1) {
                        perror("Nadanie wartosci semaforowi 1");
                        exit(1);
                }
                if (semctl(semid, 2, SETVAL, ports) == -1) {
                        perror("Nadanie wartosci semaforowi 2");
                        exit(1);
                }
        }




        int weight; //waga statku, jednoczesnie to ile potrzebuje holownikow
        for(int i = 1; i <= 10; i++) {
                if(i<10) {
                        if(fork()) {
                                weight = i;
                                break;
                        }
                }else {
                        weight = i;
                }

        }


        while(1) {

                opusc(semid, 2, -1); //najpierw rezerwuje port, zeby statki ciezsze tez mialy szanse wejsc
                opusc(semid, 1, weight*(-1)); //jesli bedzie wystarczajaco holownikow to przechodze do sekcji


                ports = semctl(semid, 2, GETVAL, ports); // pobieram ilosc wolnych poertow i holownikow
                tugboats = semctl(semid, 1, GETVAL, ports);
                if (ports == -1 || tugboats == -1) {
                        perror("Pobieranie wartości semaforow");
                        exit(1);
                }
                else{
                        printf("W PORCIE id %d ---WOLNE: porty %d, holowniki %d\n", weight,ports, tugboats);
                }

                sleep(2);

                podnies(semid, 1, weight); //zwalniam holowniki
                podnies(semid, 2, 1); //zwalniam port



        }
}
