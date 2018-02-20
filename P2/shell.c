/*
	Autor: Pablo Costa Oubiña - p.costa
	Grupo: 3.2  (Viernes 8:30 - 10:30)
*/
#include "f.h"
#include "list.h"

bool terminar = false;
bool recursivo = false;
char entrada[MAX_CHARS_ENTRADA];

bool procesar_entrada(char *entrada, bool *recursivo){
	char *trozos[MAX_CHARS_ENTRADA*4];
	int numtrozos;
	numtrozos = trocear_cadena(entrada, trozos);
	if (trozos[0] == NULL)
		return false;
	else if (!strcmp(trozos[0], "exit"))
		return true;
	else if (!strcmp(trozos[0], "fin"))
		return true;
	else if (!strcmp(trozos[0], "quit"))
		return true;
	else if (!strcmp(trozos[0], "autores"))
		return mostrar_autores(trozos[1]);
	else if (!strcmp(trozos[0], "pid"))
		return mostrar_pid(trozos[1]);
	else if (!strcmp(trozos[0], "recursive"))
		return recursive(trozos[1], recursivo);
	else if (!strcmp(trozos[0], "info"))
		return info(numtrozos - 1, &trozos[1]);
	else if (!strcmp(trozos[0], "list")){
		int numdirs = 0;
		if (trozos[1] == NULL) { //list -> list  .
			trozos[1] = ".";
			return ll(1, &trozos[1], CORTO, recursivo);
		} else if (!strcmp("-l",trozos[1])) {
			if (numtrozos == 2) {
				trozos[2] = ".";
				numdirs = numtrozos - 1;
			} else numdirs = numtrozos - 2;
			return ll(numdirs, &trozos[2], LARGO, recursivo);
		} else {
			if (numtrozos == 1) {
				trozos[1] = ".";
				numdirs = 1;
			} else numdirs = numtrozos - 1;
			return ll(numdirs, &trozos[1], CORTO, recursivo);
		}
	} else if(!strcmp(trozos[0], "eliminate")) {
		if (trozos[1] != NULL) {
			if (!strcmp(trozos[1], "-f")){
				if (trozos[2] != NULL)
					eliminate(trozos[2], true);
			} else {
				eliminate(trozos[1], false);
			}
		}
	} else if (!strcmp(trozos[0], "malloc")){
		malloc_cmd(&trozos[1]);
	} else if (!strcmp(trozos[0], "mmap")){
		mmap_cmd(&trozos[1]);
	} else if (!strcmp(trozos[0], "sharednew")){
		sharednew_cmd(&trozos[1]);
	} else if (!strcmp(trozos[0], "shared")){
		shared_cmd(&trozos[1]);
	} else if (!strcmp(trozos[0], "rmkey")){
		rmkey_cmd(&trozos[1]);
	} else if (!strcmp(trozos[0], "allocation")){
		allocation_cmd();
	} else if (!strcmp(trozos[0], "deallocate")){
		deallocate_cmd(&trozos[1]);
	} else if (!strcmp(trozos[0], "mem")){
		mem_cmd();
	} else if (!strcmp(trozos[0], "recursivefunction")){
		recursivefunction_cmd(&trozos[1]);
	} else if (!strcmp(trozos[0], "memdump")){
		memdump_cmd(&trozos[1]);
	} else if (!strcmp(trozos[0], "read")){
		read_cmd(&trozos[1]);
	} else if (!strcmp(trozos[0], "write")){
		write_cmd(&trozos[1]);
	} else
		printf("[%s] -> comando desconocido.\n", trozos[0]);
	return false;
}

void mem_cmd(){
    short a, b, c;
    printf("Variables locales: %p %p %p\n",&a,&b,&c);
    printf("Variables globales: %p %p %p\n",&terminar,&recursivo,&entrada);
	printf("Functiones: %p %p %p\n",&terminar,&recursivo,&entrada);
}

int main(){
	initList();
	while(!terminar){
		printf("%s", "$ "); //Imprimimos el prompt
		fgets(entrada, MAX_CHARS_ENTRADA, stdin); //Leemos de teclado
		terminar = procesar_entrada(entrada, &recursivo); //Procesamos la entrada
	}
	clearList();
	return EXIT_SUCCESS;
}
