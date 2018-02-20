#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/shm.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>

#define TAM_LISTA 2048

typedef struct dataL{
    int priority, pid, signal;
    time_t date;
    char status[80];
    char command[80];
} dataL;

typedef struct nodeL{
    struct dataL * data;
    struct nodeL * next;
} nodeL;

typedef struct linkedList{
    nodeL * head;
} linkedList;

void inicializar_linkedList();
void insertar_LinkedList(dataL data);
void imprimir_LinkedList(int pid);
void imprimir_nodo(nodeL * node);
void limpiar_LinkedList();
void destruir_LinkedList();
