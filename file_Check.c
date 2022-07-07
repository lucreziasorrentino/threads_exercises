
/*TESTATO*/

/*Si scriva una funzione C con la seguente interfaccia void file_check(char *file_name, int num_threads). 
Tale funzione dovrà lanciare num_thread nuovi threads, in modo che ciascuno di essi legga stringhe dallo 
standard input, e per ogni stringa letta verifichi l’occorrenza di tale stringa all’interno di ciascuna 
riga del file il cui path è identificato tramite il parametro file_name, e stampi la stringa su standard 
output in caso affermativo.*/
/*l'utente inserisce la stringa "ciao" e in file_name c'è la stringa "ciao" in ogni riga, quindi ciao viene stampato*/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#define abort(msg)   \
    do               \
    {                \
        printf(msg); \
        exit(1);     \
    } while (0)

FILE *fd;

pthread_spinlock_t ptspin; //è uno spinlock globale perchè lo inizizalizzo in file_check ma poi voglio usarlo in foo e non posso passarlo come parametro nel create perchè i parametri li sono fissi

/****************************************************FUNZIONE RICHIESTA************************************************************/
void *foo(void *par)
{
	
	int length = 512;
	char *buffer = malloc(length); //buffer: a temporary storage area delle dimensioni di 512 Byte
	char *res = NULL;	//res inizializzato a NULL
	do
	{
		pthread_spin_lock(&ptspin);	//blocca lo spin lock passato come paramtro. Il thread chiamante acquisisce il lock se non ce l'ha un altro thread altrimenti aspetta (spin) senza tornare dalla chiamata pthread_spin_lock()
		res = fgets(buffer, length, fd); //fgets prende i dati del file fd e li mette nel buffer, si ferma a 512-11, a fine file o a /n
										//res= puntatore a dove è arrivato a leggere il thread (QUINDI IL THREAD CHE ENTRA NEL LOCK DOPO RICOMINCIA DA LI) oppure NULL se il thread ha finito di leggere il file
		pthread_spin_unlock(&ptspin); //rilascia lo spin lock passato come parametro che era stato bloccato da pthread_spin_lock()

		printf("stringa prima dell'if nel buffer con /n: %s \n", buffer);
		buffer[strlen(buffer) - 1] = '\0';
		printf("stringa prima dell'if nel buffer senza /n: %s \n", buffer);
		printf("stringa prima dell'if nel parametro: %s \n", (char *)par);
		if (strcmp(buffer, (char *)par) == 0) //se la stringa nel buffer è uguale a quella passata per parametro
		{
			printf("stringa: %s \n", buffer);
			res = NULL;	//res torna a NULL (cioè se la stringa è stata trovata si esce dal do while)
		}

	} while (res != NULL);

	free(buffer);	//dealloco buffer
}



/****************************************************FUNZIONE DI PREPARAZIONE THREAD************************************************************/
void file_check(char *file_name, int n_threads)
{
	pthread_spin_init(&ptspin, PTHREAD_PROCESS_PRIVATE); //alloca risorse necessarie allo spin lock passato come riferimento e inizializza il lock allo stato unlocked; PTHREAD_PROCESS_PRIVATE: lo spin lock effettuato solo da thread che chiamano pthread_spin_init
	fd = fopen(file_name, "r");	//apri il file che si chiama "file_name" in lettura ("r") e salvalo in fd
	pthread_t *array = malloc(n_threads * sizeof(pthread_t));	//crea un array CHE SI CHIAMA array di threads delle dimensioni della size= numero di thread * (size di un pthread)
	int fine = 1;
	do
	{
		printf("inserire stringa da cercare: \n");
		char string[100]; //preparo la lunghezza della stringa che inserirà l'utente
		fgets(string, 100, stdin);	//legge una riga da stdin (cioè da tastiera) e la conserva nella stringa string, si ferma a 100-1 caratteri, oppure a /n oppure alla fine del file char *fgets(char *str, int n, FILE *stream)
		string[strlen(string) - 1] = '\0';	//metto il carattere \0 all'ultimo posto della stringa

		fseek(fd, 0, SEEK_SET);	//si usa per muovere il puntatore nel file fd da SEEK_SET (cioè la posizione attuale)+ 0 (il secondo paramtro); SEEK_SET=inizio del file

		printf("stringa cercata: %s  stringa trovata: \n", string);
		for (int i = 0; i < n_threads; i++)	// creerò un numero di thread = a quello passato in n_threads
			pthread_create(array + i, NULL, foo, (void *)string);	//4  parametri:		1) puntatore a dove verrà conservato l'id del thread (all'inizio è il primo elemento dell'array di thread di nome array)
																	//					2) attributi: se NULL si usano quelli di default
																	//					3) nome della funzione che deve essere eseguita per creare i thread
																	//					4) i parametri della funzione scritta nel terzo parametro deve essere per forza di tipo void*
		for (int i = 0; i < n_threads; i++)	//per ogni thread
			pthread_join(array[i], NULL);	//blocca l'esecuzione del thread finchè il thread con id= al primo parametro termina
		printf("nessun'altra \n");

		printf("scrivere 1 per continuare, 0 per arrestarsi: \n"); //1 se vuoi cercare un'altra parola
		scanf("%d%*c", &fine);

	} while (fine == 1);

	pthread_spin_destroy(&ptspin);	//distrugge lo spin lock passato come parametro (e rilascia le risorse usate da esso)

	fclose(fd);	//chiusura file
	free(array);//dealloca memoria array
}


/*********************************************************MAIN*******************************************************************/
int main(int argc, const char *argv[])
{

	if (argc != 3) //se nel main passo un numero di parametri !=3 vuol dire che è sbagliato l'input quindi non eseguo nulla
		abort("usage: <file name> <n threads>");

	file_check((char*)argv[1], atoi(argv[2])); //passo a file_check i 2 argomenti (il secondo ha atoi che serve a convertirlo in int)
}
