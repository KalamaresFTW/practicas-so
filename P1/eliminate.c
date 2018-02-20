/*
	Autor: Pablo Costa Oubiña - p.costa
	Grupo: 3.2  (Viernes 8:30 - 10:30)
*/
#include "f.h"

int main (int argc, char *argv[]){
	if (argc > 1) {
		if (argv[1] != NULL) {
			if (!strcmp(argv[1], "-f")){
				if (argv[2] != NULL)
					eliminate(argv[2], true);
			} else {
				eliminate(argv[1], false);
			}
		}
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}
