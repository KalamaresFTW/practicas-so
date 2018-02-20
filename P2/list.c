#include "list.h"

struct list l;

void initList(){
    for (int i = 0; i < MAX_LIST_SIZE; i++) {
        l.list[i].mode = M_EMPTY;
    }
    l.index = 0;
}

bool insertNode(node n){
    if (l.index >= MAX_LIST_SIZE) {
        perror("error: array is full");
        return false;
    } else {
        for (int i = 0; i < MAX_LIST_SIZE; i++) {
            if (l.list[i].tam == 0) {
                l.list[i] = n;
                l.index++;
                return true;
            }
        }
    }
    return false;
}

void removeShmem(key_t key){
    for (int i = 0; i < MAX_LIST_SIZE; i++) {
        if ((l.list[i].mode==M_SHMEM) && (l.list[i].key==key)){
            if (shmdt(l.list[i].ptr) == 1) {
                perror("removeShmem: shmdt");
                return;
            }
            printf("deattached %ld shared memory (key: %d) at %p\n",
                l.list[i].tam, l.list[i].key, l.list[i].ptr);
            l.list[i].mode = M_EMPTY;
            l.index--;
            return;
        }
    }
}

void removeMmap(char *filename){
    for (int i = 0; i < MAX_LIST_SIZE; i++) {
        if (l.list[i].mode==M_MMAP){
            if ((l.list[i].filename!=NULL) && (filename!=NULL)
                && (!strcmp(l.list[i].filename,filename))) {
                if (munmap(l.list[i].ptr, l.list[i].tam) < 0) {
                    perror("removeMmap: munmap");
                    return;
                }
                if (close(l.list[i].fd)<0) {
                    perror("removeMmap: close");
                    return;
                }
                printf("unmapped %s at %p\n", l.list[i].filename, l.list[i].ptr);
                l.list[i].mode = M_EMPTY;
                l.list[i].tam = 0;
                l.index--;
                return;
            }
        }
    }
    printf("error: file %s not mapped\n", filename);
}

void removeMalloc(size_t tam){
    for (int i = 0; i < MAX_LIST_SIZE; i++) {
        if ((l.list[i].mode == M_MALLOC)&&(l.list[i].tam = tam)) {
            printf("deallocated %ld at %p\n", l.list[i].tam, l.list[i].ptr);
            l.list[i].mode = M_EMPTY;
            l.list[i].tam = 0;
            l.index--;
            free(l.list[i].ptr);
            return;
        }
    }
    printf("error: no allocation with size %ld\n", tam);
}

void removeNode(int mode, void *param){
    size_t *tam;
    char *filename;
    key_t *key;
    switch (mode) {
        case M_MALLOC:
            tam = param;
            removeMalloc(*tam);
            break;
        case M_MMAP:
            filename = param;
            removeMmap(filename);
            break;
        case M_SHMEM:
            key = param;
            removeShmem(*key);
            break;
    }
}

void printList(int mode){
    char timestr[30];
    node n;
    for (int i = 0; i < MAX_LIST_SIZE; i++) {
        if ((l.list[i].mode == mode) && (l.list[i].tam != 0)) {
            n = l.list[i];
            strftime(timestr, 30, "%a %b %d %k:%M:%S", localtime(&n.time));
            printf("%p: size:%li. ", n.ptr, n.tam);
            if (n.mode == M_MALLOC) {
                printf("malloc  %s\n", timestr);
            } else if (n.mode == M_MMAP) {
                printf("mmap  %s (fd:%d) %s\n", n.filename, n.fd, timestr);
            } else if (n.mode == M_SHMEM) {
                printf("shared memory (key %d) %s\n", n.key, timestr);
            }
        }
    }
}

void clearList(){
    for (int i = 0; i < MAX_LIST_SIZE; i++) {
        if (l.list[i].mode != M_EMPTY)
            removeByAddress(l.list[i].ptr);
    }
}

void removeByAddress(void *ptr){
    for (int i = 0; i < MAX_LIST_SIZE; i++) {
        if ((l.list[i].mode != M_EMPTY) && (l.list[i].ptr == ptr)) {
            switch (l.list[i].mode) {
                case M_MALLOC:
                    removeNode(M_MALLOC, &l.list[i].tam);
                    return;
                case M_MMAP:
                    removeNode(M_MMAP, l.list[i].filename);
                    return;
                case M_SHMEM:
                    removeNode(M_SHMEM, &l.list[i].key);
                    return;
            }
        }
    }
}
