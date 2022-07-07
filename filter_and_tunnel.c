/*Si scriva una funzione di codice C con la seguente interfaccia: 
int filter_and_tunnel(int descriptors[], int count, char c, int fd_log)
Tale funzione porta l’applicazione a gestire, per ogni file-descriptor dell’array descriptors l’inoltro e 
filtro del flusso dei dati in ingresso verso il file-descriptor fd_log. 
Nello specifico:
• il parametro count indica di quanti elementi è costituito l’array descriptors
• gli stream producono sequenze di 49 caratteri terminanti con ‘\n’, chiamate linee
• la funzione ridireziona su fd_log le sole linee che iniziano per il carattere c
• l’inoltro dovrà essere attuato in modo concorrente per i diversi canali
• l’inoltro termina non appena si riceve una linea vuota
• quando tutti gli stream sono terminati, la funzione stampa il numero di linee 
complessivamente redirezionate su fd_log*/

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

pthread_spinlock_t spin;
int counto=0;

void* foo(void* f){
    int fd= (int)f;
    int r=0;
    int size= 50*sizeof(char);
    char* buffer= malloc(size);

    do{
        r= read(fd, buffer, size);
        if(buffer[0]=='c'){
            pthread_spin_lock(spin);
            write(fd_log, buffer, size);
            counto++;
            pthread_spin_unlock(spin);
        }

    }while(r!=0);
    free(buffer);
}


int filter_and_tunnel(int descriptors[], int count, char c, int fd_log){
    pthread_spin_init(&spin, PTHREAD_PROCESS_PRIVATE);
    pthread *array= malloc(count * sizeof(pthread));


    for(int i=0; i<count; i++){
        pthread_create(array+i, NULL, foo, (void*)descriptors[i]);
    }

    for(int i=0; i<count; i++){
        pthread_join(array+i, NULL);
    }

    printf("conto: %d\n", counto);
    free(array);

}