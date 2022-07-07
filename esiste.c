/*Scrivere un programma C in cui dato un file A, una stringa B e un intero N,
vengano creati N thread/processi che cerchino se all’interno del file A esiste 
una linea uguale a B.*/


pthread_spinlock_t lock;
pthread_spinlock_t lock2;
int risultato=0;
FILE* fp;

void foo(void* b){
    int esiste=0;
    int lungh= 512;
    char* buffer= malloc(sizeof(lungh));

    while(risultato!=1){
        
        pthread_spin_lock(&lock); //non serve perchè fgets è thread safe
        res= fgets(buffer, lungh, fp);
        pthread_spin_unlock(&lock); //non serve perchè fgets è thread safe

        if(strcmp((char*)b, buffer)==0){
            pthread_spin_lock(&lock2);
            risultato=1;
            pthread_spin_unlock(&lock2);
        }
    }
    free(buffer);

}


void esiste(file* a, char* b, int n){

    pthread_spin_init(&lock, PTHREAD_PROCESS_PRIVATE);
    pthread_spin_init(&lock2, PTHREAD_PROCESS_PRIVATE);
    fp= fopen(a, "r");
    pthread_t *array= malloc(n* sizeof(pthread_t))

    for(int i=0; i<n; i++)
        pthread_create(array+i, NULL, funzione, (void*) b);

    for(int i=0; i<n; i++)
        pthread_join(array[i], NULL);

    if(risultato==1)
        printf("si");
    else
        printf("no");
    
    free(array);
    fclose(fp);
}


int main(){
    if (argc != 3) //se nel main passo un numero di parametri !=3 vuol dire che è sbagliato l'input quindi non eseguo nulla
		abort("usage: <file name>  <string> <n threads>");

	esiste((char*)argv[1], (char*)argv[2], atoi(argv[3])); //passo i 3 argomenti (il terzo ha atoi che serve a convertirlo in int)
}