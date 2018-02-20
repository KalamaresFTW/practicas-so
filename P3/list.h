#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <unistd.h>
#include <dirent.h>

#define MAX_LIST_SIZE 4096
#define MAX_FILENAME_CHARS 512

//Distintos tipos de nodo.
#define M_EMPTY 0
#define M_MALLOC 1
#define M_MMAP 2
#define M_SHMEM 3

struct node{
    int mode;
    void *ptr;
    time_t time;
    size_t tam;
    int fd;
    char filename[MAX_FILENAME_CHARS];
    key_t key;
};

typedef struct node node;

struct list{
    struct node list[MAX_LIST_SIZE];
    int index;
};

typedef struct list list;

void initList();
bool insertNode(node n);
void removeMalloc(size_t tam);
void removeMmap(char *filename);
void removeShmem(key_t key);
void removeNode(int mode, void *param);
void printList(int mode);
void clearList();
void removeByAddress(void *ptr);
