/*
	Autor: Pablo Costa Oubiña - p.costa
	Grupo: 3.2  (Viernes 8:30 - 10:30)
*/

#include "f.h"
#include "list.h"


ssize_t escribir_fichero(char *file, void *ptr, ssize_t n, bool overwrite){
    ssize_t nescritos;
    int fd, aux, modo = O_CREAT | O_EXCL | O_WRONLY;
    if(overwrite)
        modo=O_CREAT | O_WRONLY | O_TRUNC;
    if ((fd = open(file, modo)) == -1)
        return -1;
    if ((nescritos = write(fd, ptr, n)) == -1){
        aux=errno;
        close(fd);
        errno=aux;
        return -1;
    }
    close(fd);
    return nescritos;
}

void write_cmd(char *params[]){
    unsigned long pos;
    char *address;
    void *ptr = NULL;
    char *fich;
    ssize_t cont = LEERCOMPLETO;
    bool overwrite = false;
    if ((params[0] == NULL) || (params[1] == NULL)) {
        printf("%s\n", "uso: write fich address cont [-o]");
        return;
    } else {
        fich = params[0];
        address = params[1];
        sscanf(address,"%lx", &pos);
        ptr = (void*) pos;
        if (params[2] != NULL)
            cont = (int) atoi(params[2]);
        if ((params[3] != NULL) && !strcmp(params[3], "-o"))
            overwrite = true;
        if (overwrite) {
            printf("%s\n", "overwrite");
        }
        escribir_fichero(fich, ptr, cont, overwrite);
    }
}



ssize_t leer_fichero(char *fich, void *p, ssize_t n) {
    /*n=-1 indica que se lea todo*/
    ssize_t nleidos,tam=n;
    int df, aux;
    struct stat s;
    if (stat (fich,&s)==-1 || (df=open(fich,O_RDONLY))==-1)
    return ((ssize_t)-1);
    if (n==LEERCOMPLETO)
    tam=(ssize_t) s.st_size;
    if ((nleidos=read(df,p, tam))==-1){
        aux=errno;
        close(df);
        errno=aux;
        return ((ssize_t)-1);
    }
    close (df);
    return (nleidos);
}

void read_cmd(char *params[]){
    unsigned long pos;
    char *address;
    void *ptr = NULL;
    char *fich;
    ssize_t cont = LEERCOMPLETO;
    if ((params[0] == NULL) || (params[1] == NULL)) {
        printf("%s\n", "uso: read fich address cont ");
        return;
    } else {
        fich = params[0];
        address = params[1];
        sscanf(address,"%lx", &pos);
        ptr = (void*) pos;
        if (params[2] != NULL)
            cont = (int) atoi(params[2]);
        printf("Leidos %lu bytes.\n", (unsigned long)leer_fichero(fich, ptr, cont));
        ;
    }
}


void memdump(char *ptr, int cont){
    int linea = MEMDUMP_LINE;
    for (int i = 0; i < cont;) {
        for (int j = i; j < (i+linea); j++) {
            printf(" %02x ", ptr[j]);
        }
        printf("\n");
        for (int k = i; k < (i+linea); k++) {
            if (isprint(ptr[k])) {
                printf(" %2c ", ptr[k]);
            } else {
                printf(" %2c ", ' ');
            }
        }
        printf("\n");
        i += linea;
        if (i+MEMDUMP_LINE > cont) linea = cont % MEMDUMP_LINE;
    }
    printf("\n");
}

void memdump_cmd(char *params[]){
    void *ptr = NULL;
    char *address;
    unsigned long pos;
    int cont = MEMDUMP_LINE;
    if (params[0] != NULL) {
        address = params[0];
        sscanf(address,"%lx", &pos);
        ptr = (void*) pos;
        if (params[1] != NULL) {
            cont = (int) atoi(params[1]);
        }
        memdump(ptr, cont);
    } else {
        printf("uso: memdump addr [cont]\n");
    }
}

void recursiva(int n){
    char automatico[RECURSIVA_ARRAY_SIZE];
    static char estatico[RECURSIVA_ARRAY_SIZE];
    printf("parametro n:%d en %p\n", n, &n);
    printf("array automatico en %p\n", &automatico);
    printf("array estatico en %p\n", &estatico);
    n--;
    if (n>0) {
        recursiva(n);
    }
}

void recursivefunction_cmd(char *params[]){
    int n;
    if (params[0] != NULL) {
        n =(int) atoi(params[0]);
        recursiva(n);
    }
}

void allocation_cmd(){
    printList(M_MALLOC);
    printList(M_MMAP);
    printList(M_SHMEM);
}

void deallocate_cmd(char *params[]){
    void *ptr;
    char *address;
    if (params[0] != NULL) {
        address = params[0];
        sscanf(address,"%p", (void**)&ptr);
        removeByAddress(ptr);
    }
}


void rmkey_cmd(char *params[]){
    key_t clave;
    int id;
    char *key=params[0];

    if (key==NULL || (clave=(key_t) strtoul(key,NULL,10))==IPC_PRIVATE){
        printf ("invalid key\n");
        return;
    }
    if ((id=shmget(clave,0,0666))==-1){
        perror ("shmget: cannot get shared memory with that key");
        return;
    }
    if (shmctl(id,IPC_RMID,NULL)==-1)
        perror ("shmctl: imposible eliminar memoria compartida\n");
    else
        printf("shared memory with key %d was removed\n", clave);
}

void *obtenerMemoriaShmget(key_t key, off_t tam){
    void * ptr;
    int aux,id,flags=0777;
    struct shmid_ds s;
    if (tam)
        flags=flags | IPC_CREAT | IPC_EXCL;
    if (key==IPC_PRIVATE) {
        errno=EINVAL;
        return NULL;
    }
    if ((id=shmget(key, tam, flags)) == -1){
        perror("shmget");
        return (NULL);
    }
    if ((ptr=shmat(id,NULL,0))==(void*) -1){
        aux=errno;
        if (tam)
            shmctl(id,IPC_RMID,NULL);
        errno=aux;
        return (NULL);
    }
    shmctl (id,IPC_STAT,&s);
    node n;
    n.mode = M_SHMEM;
    n.ptr = ptr;
    time(&n.time);
    n.tam = s.shm_segsz;
    n.key = key;
    insertNode(n);
    return (ptr);
}

void sharednew_cmd(char *params[]){
    key_t k;
    size_t tam = 0;
    void *p;
    if (params[0] == NULL || params[1]==NULL){
        printList(M_SHMEM);
        return;
    }
    k=(key_t) atoi(params[0]);
    if (params[1] != NULL)
        tam = (size_t) atoll(params[1]);
    if ((p = obtenerMemoriaShmget(k,tam)) != NULL)
        printf ("allocated shared memory (key %d) at %p\n", k, p);
}

void shared_cmd(char *params[]){
    key_t key = -1;
    void *p;
    if (params[0] == NULL) {
        printList(M_SHMEM);
        return;
    }
    if (!strcmp(params[0], "-deallocate")){
        if (params[1] == NULL) {
            printList(M_SHMEM);
            return;
        } else {
            key = atoi(params[1]);
            removeNode(M_SHMEM, &key);
        }
    }  else {
        if (key != -1) {
            if ((p = obtenerMemoriaShmget(key,0)) != NULL)
                printf ("mapped shared memory (key %d) at %p\n", key, p);
        }

    }
}

void *mmap_fichero(char *fichero, int protection){
    int fd, map = MAP_PRIVATE, modo=O_RDONLY;
    struct stat s;

    void *ptr;
    node n;
    if (protection&PROT_WRITE) modo=O_RDWR;
    if ((fd=open(fichero, modo)) == -1){
        perror("open");
        return NULL;
    }
    if (lstat(fichero, &s)==-1){
        perror("stat");
        return NULL;
    }
    if ((ptr = mmap(NULL, s.st_size, protection, map, fd, 0))==MAP_FAILED){
        perror("error: mmap");
        return NULL;
    }
    n.mode = M_MMAP;
    n.ptr = ptr;
    time(&n.time);
    n.tam = s.st_size;
    n.fd = fd;
    strcpy(n.filename, fichero);
    if (!insertNode(n)){
        perror("error: insertNode");
    }
    return ptr;
}

void mmap_cmd(char *params[]){
    char *perm;
    void *ptr;
    int protection = 0;
    if (params[0] == NULL) {
        printList(M_MMAP);
        return;
    } else if (!strcmp(params[0], "-deallocate")) {
        if (params[1] == NULL) {
            printList(M_MMAP);
            return;
        } else {
            ptr = params[1];
            removeNode(M_MMAP, ptr);
            return;
        }
    } else if (params[1] != NULL){
        if ((perm = params[1]) && (strlen(perm)<MMAP_PERM_LEN)) {
            if (strchr(perm, 'r') != NULL) protection |= PROT_READ;
            if (strchr(perm, 'w') != NULL) protection |= PROT_WRITE;
            if (strchr(perm, 'x') != NULL) protection |= PROT_EXEC;
        }
    }
    if ((ptr=mmap_fichero(params[0], protection)) != NULL)
        printf ("file %s mapped at %p\n", params[0], ptr);
}

void malloc_cmd(char *params[]){
    node n;
    void *ptr;
    size_t size;
    if (params[0] == NULL) {
        printList(M_MALLOC);
        return;
    } else if (!strcmp(params[0], "-deallocate")) {
        if (params[1] == NULL) {
            printList(M_MALLOC);
        } else {
            size = atoll(params[1]);
            ptr = &size;
            removeNode(M_MALLOC, ptr);
        }
    } else {
        size = (size_t)atoi(params[0]);
        if ((ptr = malloc(size)) != NULL) {
            n.mode = M_MALLOC;
            n.ptr = ptr;
            n.tam = size;
            time(&n.time);
            if (insertNode(n)) {
                printf("allocated %ld at %p\n", size, ptr);
            }
        } else {
            perror("malloc: not enough space.");
        }
    }
}

void eliminate(char *dir, bool recursivo){
   if (dir!=NULL){
      if(recursivo) {
        eliminar_directorio_rec(dir);
      } else {
        eliminar_entrada(dir);
      }
   }
}

void eliminar_entrada (char *path) {
	struct stat buff;
	lstat(path, &buff);
	if (!S_ISDIR(buff.st_mode)){
		if (unlink(path) < 0)
			perror(path);
	}
	else {
		if (rmdir(path) < 0)
			perror(path);
	}
}

/*
  Elimina un directorio de manera recursiva
*/
void eliminar_directorio_rec(char *dir) {
	struct dirent *directorio;
	struct stat buff;
	char path[MAX_CHARS_CADENA];
    DIR *fd = NULL;
	if (es_directorio_vacio(dir)){
		if (rmdir(dir) < 0)
      perror(dir);
	}
	else {
		if ((fd = opendir(dir)) == NULL){
      perror(dir);
    } else {
      while ((directorio = readdir(fd)) != NULL) {
        sprintf(path, "%s/%s", dir, directorio->d_name);
        if (!strcmp(directorio->d_name, ".") ||
            !strcmp(directorio->d_name, ".."))
              continue;
        else {
          lstat(path, &buff);
          if (S_ISDIR(buff.st_mode))
            eliminar_directorio_rec(path);
          else
            unlink(path);
        }
      }
      closedir(fd);
      if (rmdir(dir) < 0)
        perror(dir);
    }
	}
}

bool es_directorio_vacio (char *directorio){
	struct dirent *entrada_directorio;
	int n = 0;
  DIR *fd;
	if ((fd = opendir(directorio)) == NULL){
		perror(directorio);
		return false;
	}
	while ((entrada_directorio = readdir(fd)) != NULL) {
		if (++n > 2)
			break;
	}
	closedir(fd);
	if (n<= 2)return true;
	else return false;
}

int es_directorio(const char *path) {
   struct stat buff;
   if (lstat(path, &buff) != 0)
       return 0;
   return S_ISDIR(buff.st_mode);
}

char *convierte_modo (mode_t m, char *permisos){
	strcpy (permisos,"---------- ");
	permisos[0]=tipo_fichero(m);
	if (m&S_IRUSR) permisos[1]='r'; //propietario
	if (m&S_IWUSR) permisos[2]='w';
	if (m&S_IXUSR) permisos[3]='x';
	if (m&S_IRGRP) permisos[4]='r'; //grupo
	if (m&S_IWGRP) permisos[5]='w';
	if (m&S_IXGRP) permisos[6]='x';
	if (m&S_IROTH) permisos[7]='r'; //resto
	if (m&S_IWOTH) permisos[8]='w';
	if (m&S_IXOTH) permisos[9]='x';
	if (m&S_ISUID) permisos[3]='s'; //setuid, setgid y stickybit
	if (m&S_ISGID) permisos[6]='s';
	if (m&S_ISVTX) permisos[9]='t';
	return permisos;
}

char tipo_fichero(mode_t m){
	switch (m&S_IFMT) { //and bit a bit con los bits de formato,0170000
		case S_IFSOCK: return 's'; //socket
		case S_IFLNK: return 'l'; //symbolic link
		case S_IFREG: return '-'; //fichero normal
		case S_IFBLK: return 'b'; //block device
		case S_IFDIR: return 'd'; //directorio
		case S_IFCHR: return 'c'; //char device
		case S_IFIFO: return 'p'; //pipe
		default: return '?'; //desconocido, no deberia aparecer
	}
}
//15888887 -rw-r--r-- 1 antonio antonio 4 Sep 15 13:35 enlace -> fich
void procesar_fichero(char* fichero, bool formato){
	struct stat buff;
	if (lstat(fichero, &buff) != -1){
		struct passwd *usuario;
    char user[MAX_CHARS_CADENA];
    if ((usuario = getpwuid(buff.st_uid)) != NULL)
      sprintf(user,"%s", usuario->pw_name);
    else
      sprintf(user,"%lu", (unsigned long)buff.st_uid);
	struct group *grupo;
    char group[MAX_CHARS_CADENA];
    if ((grupo = getgrgid(buff.st_gid)) != NULL)
      sprintf(group, "%s", grupo->gr_name);
    else
      sprintf(group, "%lu", (unsigned long)buff.st_gid);
		char f_modificacion[512];
		struct tm * p = localtime(&buff.st_mtim.tv_sec);
		strftime(f_modificacion, 1000, "%b %d %k:%M", p);
		char ruta[MAX_CHARS_CADENA];
		if (S_ISLNK(buff.st_mode)){ //Comprobamos si es un link
			char dest[MAX_CHARS_CADENA];
			int count = readlink(fichero,dest,sizeof(dest));
			if (count >= 0){
				dest[count] = '\0';
				sprintf(ruta, "%s -> %s", fichero, dest);
			}
		} else {
			sprintf(ruta, "%s", fichero);
		}
    char permisos[10];
		convierte_modo(buff.st_mode,permisos);
		if (formato) printf("%-20s %6lu\n",ruta,buff.st_size);
		else printf("%-8lu %-11s %-3lu %-6s %-6s %6lu %-12s %-20s\n",
      buff.st_ino, permisos, buff.st_nlink, user, group,
      buff.st_size, f_modificacion, ruta);
	} else
		perror(fichero);
}

bool info(int nfich, char **fich){
	for (int i=0; i<nfich; i++){
		if (fich[i] != NULL){
			procesar_fichero(fich[i], LARGO);
		}
	}
	return false;
}

bool ll(int ndirs, char **dirs, bool formato, bool *recursivo){
	char nombre_dir[MAX_CHARS_CADENA];
	char ruta[MAX_CHARS_CADENA];
	struct dirent *directorio;
	DIR *fd;
	for (int i = 0; i < ndirs; i++){
        if ((fd = opendir(dirs[i])) == NULL){
            perror("info error (opendir)"); continue;
		} else {
            while((directorio = readdir(fd)) != NULL){
				strcpy(nombre_dir, directorio->d_name);
				sprintf(ruta, "%s/%s", dirs[i], nombre_dir);
				if (!strcmp(".", dirs[i]))
					procesar_fichero(nombre_dir, formato);
				else
					procesar_fichero(ruta, formato);
				if (*recursivo && es_directorio(ruta)){
					if (!strcmp(".", nombre_dir) || !strcmp("..", nombre_dir))
						continue;
					else { //preparamos la llamada recursiva
						char * dir_aux_recursivo[] = {ruta};
						ll(1, dir_aux_recursivo, formato, recursivo);
					}
				}
			}
		}
		closedir(fd);
	}
	return false;
}

bool recursive(char *param, bool *recursivo){
	if (param == NULL){
		if (*recursivo) printf("%s\n", "recursive ON");
		else printf("%s\n", "recursive OFF");
	} else {
		if (!strcmp("on", param) || !strcmp("ON", param)){
			*recursivo = true;
			printf ("%s\n", "recursive -> ON");
		} else if (!strcmp("off", param)|| !strcmp("off", param)){
			*recursivo = false;
			printf ("%s\n", "recursive -> OFF");
		} else {
        	printf("recursive [%s] -> parámetro desconocido.\n", param);
        	printf("uso: recursive [ON | OFF]\n");
      	}
	}
	return false;
}

bool mostrar_pid(char *param){
	if (param == NULL)
		printf ("%d\n", getpid());
	else if (!strcmp ("-p", param))
		printf ("%d\n", getppid());
	else {
		printf("pid [%s] -> parámetro desconocido.\n", param);
		printf("uso: pid [-p]\n");
	}
  return false;
}

bool mostrar_autores(char *param){
	if (param == NULL)
		printf("%s\n", "Pablo Costa Oubiña - p.costa");
	else if (!strcmp(param, "-l"))printf("%s\n", "p.costa");
	else if (!strcmp(param, "-n"))printf("%s\n", "Pablo Costa Oubiña");
	else {
		printf("autores [%s] -> parámetro desconocido.\n", param);
		printf("uso: autores [-l|-n]\n");
	} return false;
}

int trocear_cadena(char *cadena, char *trozos[]){
	int i=1;
	if ((trozos[0]=strtok(cadena," \n\t"))==NULL)
		return 0;
	while ((trozos[i]=strtok(NULL," \n\t"))!=NULL)
		i++;
	return i;
}
