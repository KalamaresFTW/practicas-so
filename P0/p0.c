/* 
	Autor: Pablo Costa Oubiña - p.costa
	Grupo: 3.2  (Viernes 8:30 - 10:30)
*/
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#define MAX_CHARS_ENTRADA 256

bool mostrar_pid(char *param);
bool mostrar_autores(char *param);
int trocear_cadena(char *cadena, char *trozos[]);
bool procesar_entrada(char *entrada);

bool mostrar_pid(char *param){
	if (param == NULL)
		printf ("%d\n", getpid());
	else if (!strcmp ("-p", param))
		printf ("%d\n", getppid());
	else 
		printf("pid [%s] -> parámetro desconocido.\n", param);
	return false;
}

bool mostrar_autores(char *param){
	if (param == NULL)
		printf("%s\n", "Pablo Costa Oubiña - p.costa");
	else if (!strcmp(param, "-l")) 
		printf("%s\n", "p.costa");
	else if (!strcmp(param, "-n")) 
		printf("%s\n", "Pablo Costa Oubiña - p.costa");
	else 
		printf("autores [%s] -> parámetro desconocido.\n", param);
	return false;
}

int trocear_cadena(char *cadena, char *trozos[]){
	int i=1;
	if ((trozos[0]=strtok(cadena," \n\t"))==NULL)
		return 0;
	while ((trozos[i]=strtok(NULL," \n\t"))!=NULL)
		i++;
	return i;
}

bool procesar_entrada(char *entrada){
	char *trozos[MAX_CHARS_ENTRADA*4];
	trocear_cadena(entrada, trozos);
	if (trozos[0] == NULL) return false;
	if (!strcmp(trozos[0], "exit")) return true;
	else if (!strcmp(trozos[0], "fin")) return true;
	else if (!strcmp(trozos[0], "quit")) return true;
	else if (!strcmp(trozos[0], "autores")) 
		return mostrar_autores(trozos[1]);
	else if (!strcmp(trozos[0], "pid"))
		return mostrar_pid(trozos[1]);
	else printf("[%s] -> comando desconocido.\n", trozos[0]);
	return false;

}

int main(int argc, char **argv){
	bool terminar = false;
	char *entrada;
	entrada = (char*) malloc(MAX_CHARS_ENTRADA * sizeof(char));
	while(!terminar){
		printf("%s", "$"); //Imprimimos el prompt
		fgets(entrada, MAX_CHARS_ENTRADA, stdin); //Leemos de teclado
		terminar = procesar_entrada(entrada); //Procesamos la entrada
	}
	free(entrada);
	return EXIT_SUCCESS;
}