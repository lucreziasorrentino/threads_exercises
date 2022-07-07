/*Scrivere un programma C invert che dato un file A ne inverte il contenuto e lo memorizza in nuovo file B. Il programma deve:
riportare il contenuto di A in memoria;
invertire la posizione di ciascun byte utilizzando un numero N di thread/processi concorrenti;
scrivere il risultato in un nuovo file B.
A, B e N sono parametri che il programma deve acquisire da linea di comando.*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>


pthread_spinlock_t spinny;
int fd=-1;
int fdB=-1;
int n;

void* foo(void* par){
    int lungh= 5*sizeof(char);
    char* buffer= malloc(lungh);
    char temp;
    int res= 0;
    int prima=1;
    int nThread= (int)par;
    do{
        res= read(fd, buffer, lungh);
        //buffer[strlen(buffer)-1] = '\0';
        printf("buffer: %s\n", buffer);
        for(int i=0; i<strlen(buffer)/2; i++){
            if(buffer[i]!='\n'){
                temp= buffer[strlen(buffer)-1-i]; //ultima lettera in temp
                printf("ultima: %c\n", temp);
                buffer[strlen(buffer)-1-i]=buffer[i];
                printf("nuova ultima: %c\n", buffer[strlen(buffer)-1-i]);
                buffer[i]= temp;
            }
        }
        if(prima==1){
                prima=0;
                printf("strlen buff: %ld", strlen(buffer));
                for(int i=0; i<strlen(buffer); i++){
                    buffer[i]=buffer[i+1];
                }
            }
        printf("parola al contrario: %s\n", buffer);

        if(res!=0){
        pthread_spin_lock(&spinny);
        lseek(fdB, (n-nThread)*sizeof(buffer), SEEK_SET);
        write(fdB, buffer, lungh);
        pthread_spin_unlock(&spinny);
        }

    }while(res!=0);
    free(buffer);
}



int invert(char* a, char* b, int nTh){
    n=nTh;
    pthread_spin_init(&spinny, PTHREAD_PROCESS_PRIVATE);
    pthread_t *array= malloc(n* sizeof(pthread_t));
    fd= open(a, O_RDONLY);
    fdB= open(b, O_WRONLY);

    lseek(fd, 0, SEEK_SET);
    lseek(fdB, 0, SEEK_SET);

    for(int i=0; i<n; i++){
        pthread_create(array+i, NULL, foo, (void*)i);
    }

    for(int i=0; i<n; i++){
        pthread_join(array[i], NULL);
    }

    free(array);
    close(fd);
    close(fdB);

}

int main(int argc, const char *argv[]){
     int risposta= invert((char*)argv[1], (char*)argv[2], atoi(argv[3])); //1par: file A, 2par: fileB, 3par: N thread
}