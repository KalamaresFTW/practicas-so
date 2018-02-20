/*
	Autor: Pablo Costa Oubiña - p.costa
	Grupo: 3.2  (Viernes 8:30 - 10:30)
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <pwd.h>
#include <unistd.h>
#include <grp.h>
#include <time.h>
#define LARGO false
#define CORTO true
#define MAX_CHARS_CADENA 1024
#define MAX_CHARS_ENTRADA 256

void eliminate(char *dir, bool recursivo);
void eliminar_entrada (char *path);
void eliminar_directorio_rec(char *directorio);
bool es_directorio_vacio (char *path);
int es_directorio(const char *path);
void procesar_fichero(char* fichero, bool formato);
char *convierte_modo (mode_t m, char *permisos);
char tipo_fichero(mode_t m);
bool info(int nfich, char **fich);
bool list(int ndirs, char **dirs, bool formato, bool *recursivo);
bool recursive(char *param, bool *recursivo);
bool mostrar_pid(char *param);
bool mostrar_autores(char *param);
int trocear_cadena(char *cadena, char *trozos[]);
bool procesar_entrada(char *entrada, bool *recursivo);
