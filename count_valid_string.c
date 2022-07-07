/*int count_valid_string (char *filename, char *charset, int n).
Tale funzione deve lanciare un numero di thread/processi pari a n per leggere dal file 
filename una sequenza di stringhe e, per ciascuna di esse, contare quante di queste 
contengono tutti i caratteri contenuti nella stringa charset. Infine, la funzione stampa su 
standard output e ritorna il numero totale di stringhe identificato. 
Il file è tale per cui, ciascuna stringa è lunga 50 caratteri incluso il terminatore di linea.*/
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

pthread_spinlock_t lock;
pthread_spinlock_t lock2;
int fd=-1;
int count;

void* foo(void* charset){
    int res=0;
    int lungh= 50*sizeof(char);
    char* buffer= malloc(lungh);

    char* nuovo= (char*) charset;
    //nuovo[strlen(nuovo) - 1] = '\0';
    //printf("eseguo foo");
    do{
        int bla=0;
        int esiste=1;

        pthread_spin_lock(&lock);
        res= read(fd, buffer, lungh); //DIVERSO
        pthread_spin_unlock(&lock);

        buffer[strlen(buffer) - 1] = '\0';
        printf("\n stringa da cercare: %s\n", nuovo);
        printf("\n stringa nel buffer: %s\n", buffer);
        int i=0;

        while(esiste==1 && i<strlen((char*) charset)){
            for(int j=0; j<strlen(buffer); j++){
                if(nuovo[i]==buffer[j])
                    bla=1;
                printf("nuovo: %c  buffer: %c  bla: %d\n", nuovo[i], buffer[j], bla);
            }
            if(bla!=1) //se non ho mai trovato c in cziao
                esiste=0;//smetto di scorrere ciao
            i++;
            bla=0;
            printf("esiste: %d\n", esiste);
        }
        if(esiste!=0){
            pthread_spin_lock(&lock2);
            printf("count: %d\n", count);
            count++;
            printf("count: %d\n", count);
            pthread_spin_unlock(&lock2);
             printf("fine spin\n");
        }
        printf("count nel while: %d\n", count);
    }while(res!=0); //finchè non sono a fine file
    printf("count finale: %d\n", count);
    free(buffer);
}

int count_valid_string (char *filename, char *charset, int n){
    count=0;
    pthread_spin_init(&lock, PTHREAD_PROCESS_PRIVATE);
    pthread_spin_init(&lock2, PTHREAD_PROCESS_PRIVATE);
    pthread_t *array= malloc(n* sizeof(pthread_t));
    fd= open(filename, O_RDONLY);

    lseek(fd, 0, SEEK_SET);

    for(int i=0; i<n; i++){
        pthread_create(array+i, NULL, foo, (void*)charset);
    }

    for(int i=0; i<n; i++){
        pthread_join(array[i], NULL);
    }

    free(array);
    close(fd);
    return count;
}

int main(int argc, const char *argv[]){
    int risposta= count_valid_string((char*)argv[1], (char*)argv[2], 3);
    printf("risposta: %d\n", risposta);
}