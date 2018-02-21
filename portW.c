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


int tugboats = 14; //holowniki
int ports = 3; //porty
int waiting = 0; // czekajace statki
int pom = 0;


int print = 9999999;
int spij = 1;

int waiting2=0, pom2=0;


/*
   obsluga kolejki za pomoca tablicy
   wejdz dodaje na koncu kolejki statek o danym wej(czyli id, czyli wadze)
   wyjdz wyrzuca pierwszy statek z kolejki i przesuwa reszte
 */
int buf[10];
void wejdz(int wej){
        for(int i = 0; i <10; i++) {
                if(buf[i]>0) continue;
                buf[i] = wej;
                break;
        }
}

void wyjdz(){
        int x = buf[0];
        for(int i = 0; i <9; i++) {
                buf[i] = buf[i+1];
                buf[9] = 0;
        }

}


void* ship(void *p) {
        while(1) {
                int weight = *(int *)p; //pobieram weight z procesu i zamieniam na it
                pthread_mutex_lock(&mutex2); //wykluczanie waiting, ports, tugboats
                waiting++; //zwiekszam liczbe czekujacych
                if(ports - waiting<0) { //jesli liczba wolnych portow minus liczba tych ktorzy czekaja byla by mniejsza od 0
                        pthread_cond_wait(&cond2, &mutex2); //wtedy czekaj
                }
                wejdz(weight); //dodaj sie do kolejki

                waiting2++; //liczba bedacych w petli
                while(tugboats<weight || weight != buf[0] ) {
                        if(pom2 < waiting2) { //jesli pom2 jest mniejsza od liczby statkow w petli
                                pthread_cond_signal(&cond1); //to obudz nastepny
                        }else{
                                pom2 = 0; //jesli nie to wyzeruj pom2
                        }
                        pthread_cond_wait(&cond1, &mutex2); //uspij sie i czekaj na sygnal
                        pom2++; //inkrementuj pom2
                }
                wyjdz(); //usun z kolejki
                waiting2--; //odejmij liczbe w petli
                ports--; //odejmij port
                tugboats-=weight; //odejmij tyle ile potrzeba holownikow
                waiting--; //odejmuje czekajacych
                pthread_cond_signal(&cond1); //sygnal zeby znowu zdbadac w petli czy jest wystarczajaca liczba holownikow
                pthread_mutex_unlock(&mutex2);


                printf("W PORCIE --- WOLNE: holowniki %d porty %d | czeka %d waga %d --- id procesu: %lu \n",tugboats, ports, waiting, weight, pthread_self());
                sleep(1);

                pthread_mutex_lock(&mutex2);
                tugboats+=weight; //dodaje liczbe wolnych portow
                ports++; //dodaje port
                pthread_cond_signal(&cond2);
                pthread_mutex_unlock(&mutex2);

                //sleep(1);
        }
}




int main(){
        for(int i = 0; i<10; i++) {
                buf[i]=0; //zeruje kolejke
        }
        int weight1=1,weight2=2,weight3=3,weight4=4,weight5=5,weight6=6,weight7=7,weight8=8,weight9=9,weight10=10;
        //nadaje wagi, ktore pozniej nadaje watka
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



        while(1) ;
}
