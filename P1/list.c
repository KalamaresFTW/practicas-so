/*
	Autor: Pablo Costa Oubiña - p.costa
	Grupo: 3.2  (Viernes 8:30 - 10:30)
*/

#include "f.h"

int main (int argc, char *argv[]){
	bool recursivo = false;
	bool formato = CORTO;
	int numdirs = 0;
	int numparams = 0;

	if ((argv[1] != NULL && !strcmp("-l",argv[1])) ||
		(argv[2] != NULL && !strcmp("-l",argv[2]))){
			formato = LARGO;
			numparams++;
	}
	if ((argv[1] != NULL && !strcmp("-r",argv[1])) ||
		(argv[2] != NULL && !strcmp("-r",argv[2]))){
			recursivo = true;
			numparams++;
	}

	numdirs = argc - numparams - 1;

	if (numdirs == 0) {
		char *_dir = ".";
		char *__dir[] = {_dir}; //h4x0r
		list(1, __dir, formato, &recursivo);
	}
	list(numdirs, &argv[argc-numdirs], formato, &recursivo);
	return EXIT_SUCCESS;
}
