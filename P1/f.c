/*
	Autor: Pablo Costa Oubiña - p.costa
	Grupo: 3.2  (Viernes 8:30 - 10:30)
*/

#include "f.h"

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

bool list(int ndirs, char **dirs, bool formato, bool *recursivo){
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
						list(1, dir_aux_recursivo, formato, recursivo);
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
