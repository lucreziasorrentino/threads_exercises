/*Si scriva una funzione di codice C con la seguente interfaccia: 
int check_palindrome(char *filename, int n).
Tale funzione deve lanciare un numero di thread/processi pari a n per leggere dal file 
filename una sequenza di stringhe e contare quante di queste siano palindrome. Infine, la 
funzione stampa su standard output e ritorna il numero totale di stringhe identificato. 
Il file è tale per cui, ciascuna stringa è lunga 50 caratteri incluso il terminatore di linea.*/
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sched.h>

FILE* fp;
pthread_spinlock_t spinny;
pthread_spinlock_t spinny2;
int count;

void* foo(){
    int lung= 5* sizeof(char);
    char *res= NULL;
    char *buffer= calloc(5, sizeof(char));

    do{
        printf("nel while\n");
        int si=1;
        int i=0;

        pthread_spin_lock(&spinny);
        res= fgets(buffer, lung, fp);
        pthread_spin_unlock(&spinny);
        
        printf("stringa: %s\n", buffer);
        while(si==1 && i<(strlen(buffer)/2)){
            if(buffer[i] != buffer[strlen(buffer)-i-1]){
                si=0;
            }
            printf("%c, %c\n", buffer[i], buffer[strlen(buffer)-i-1]);
            i++;
        }
        if(si==1 && res != NULL && strlen(buffer) > 1){
            pthread_spin_lock(&spinny2);
            count++;
            pthread_spin_unlock(&spinny2);
        }

    }while(res!=NULL);
    free(buffer);
}

int check_palindrome(char *filename, int n){
    pthread_spin_init(&spinny, PTHREAD_PROCESS_PRIVATE);
    pthread_spin_init(&spinny2, PTHREAD_PROCESS_PRIVATE);
    
    pthread_t *array= malloc(n* sizeof(pthread_t));
    fp= fopen(filename, "r");
    count=0;
    fseek(fp, 0 , SEEK_SET);

    for(int i=0; i<n; i++)
        pthread_create(array+i, NULL, foo, NULL);

    for(int i=0; i<n; i++)
        pthread_join(array[i], NULL);

    free(array);
    fclose(fp);
    return count;
}

int main(int argc, const char *argv[]){
    int risposta= check_palindrome((char*)argv[1], 1);
    printf("risposta: %d\n", risposta);
}

