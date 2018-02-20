/*
	Autor: Pablo Costa Oubiña - p.costa
	Grupo: 3.2  (Viernes 8:30 - 10:30)
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <string.h>
#include <pwd.h>
#include <unistd.h>
#include <grp.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#define LARGO false
#define CORTO true
#define MAX_CHARS_CADENA 1024
#define MAX_CHARS_ENTRADA 256
#define MMAP_PERM_LEN 4
#define RECURSIVA_ARRAY_SIZE 2048
#define MEMDUMP_LINE 25
#define LEERCOMPLETO ((ssize_t)-1)

void write_cmd(char *params[]);
void read_cmd(char *params[]);
void memdump_cmd(char *params[]);
void recursivefunction_cmd(char *params[]);
void mem_cmd();
void deallocate_cmd(char *params[]);
void allocation_cmd();
void rmkey_cmd(char *params[]);
void *obtenerMemoriaShmget(key_t key, off_t tam);
void sharednew_cmd(char *params[]);
void shared_cmd(char *params[]);
void *mmap_fichero(char *fichero, int protection);
void mmap_cmd(char *params[]);
void malloc_cmd(char *params[]);
void eliminate(char *dir, bool recursivo);
void eliminar_entrada (char *path);
void eliminar_directorio_rec(char *directorio);
bool es_directorio_vacio (char *path);
int es_directorio(const char *path);
void procesar_fichero(char* fichero, bool formato);
char *convierte_modo (mode_t m, char *permisos);
char tipo_fichero(mode_t m);
bool info(int nfich, char **fich);
bool ll(int ndirs, char **dirs, bool formato, bool *recursivo);
bool recursive(char *param, bool *recursivo);
bool mostrar_pid(char *param);
bool mostrar_autores(char *param);
int trocear_cadena(char *cadena, char *trozos[]);
bool procesar_entrada(char *entrada, bool *recursivo);
