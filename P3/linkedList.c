#include "linkedList.h"

char *NombreSenal(int sen);

struct linkedList *lL;

void inicializar_linkedList(){
	lL = malloc(sizeof(linkedList));
	lL->head = NULL;
}

nodeL * crear_nodo(dataL data){
	struct nodeL * node = (nodeL*) malloc(sizeof(nodeL));
	node->data = (dataL*) malloc(sizeof(dataL));

	node->data->pid = data.pid;
	node->data->date = data.date;
	node->data->priority = data.priority;
	strcpy(node->data->command, data.command);

	node->next = NULL;
	return node;
}

void insertar_LinkedList(dataL data){
	nodeL * iter = NULL;
	if (lL->head == NULL) {
		lL->head = crear_nodo(data);
	} else {
		iter = lL->head;
		while (iter->next != NULL) {
			iter = iter->next;
		}
		iter->next = crear_nodo(data);
	}
}

void manejar_senal(nodeL * node){
	int status, pid;
	pid = node->data->pid;
	if (waitpid(pid, &status,WNOHANG|WUNTRACED|WCONTINUED) == pid) {
		if (WIFCONTINUED(status)) {
			strcpy(node->data->status, "ACTIVE");
			node->data->signal = SIGCONT;
		}
		if (WIFSTOPPED(status)) {
			strcpy(node->data->status, "STOPED");
			node->data->signal = WSTOPSIG(status);
		}
		if (WIFEXITED(status)) {
			strcpy(node->data->status, "TERMINATED");
			node->data->signal = WEXITSTATUS(status);
		}
		if (WIFSIGNALED(status)) {
			strcpy(node->data->status, "SIGNALED");
			node->data->signal = WTERMSIG(status);
		}
	}
	node->data->priority = getpriority(PRIO_PROCESS, pid);
}

void imprimir_nodo(nodeL * node){
	struct tm * p;
	char formated_date[512];

	p = localtime(&node->data->date);
	strftime(formated_date, 1000, "%b %d %k:%M", p);
	manejar_senal(node);
	printf("%d %s (%s #%d) p=%d %s %s\n", node->data->pid,node->data->status,
		NombreSenal(node->data->signal), node->data->signal, node->data->priority,
		formated_date,node->data->command);
}

void imprimir_LinkedList(int pid){
	nodeL * iter = lL->head;
	if (lL->head == NULL) {
		printf("no jobs\n");
		return;
	}
	if (pid == -1) {
		while (iter->next != NULL) {
			imprimir_nodo(iter);
			iter = iter->next;
		}
		imprimir_nodo(iter);
	} else {
		while (iter->next != NULL) {
			if (iter->data->pid == pid) {
				imprimir_nodo(iter);
				return;
			}
			iter = iter->next;
		}
	}
}

void limpiar_LinkedList(){
	nodeL * current = lL->head;
	nodeL * next = current;
	while (current != NULL) {
		next = current->next;
		free(current->data);
		free(current);
		current = next;
	}
	lL->head = NULL;
}

void destruir_LinkedList(){
	free(lL);
}

struct SEN{
	char *nombre;
	int senal;
};

static struct SEN sigstrnum[]={
	{"HUP", SIGHUP},
	{"INT", SIGINT},
	{"QUIT", SIGQUIT},
	{"ILL", SIGILL},
	{"TRAP", SIGTRAP},
	{"ABRT", SIGABRT},
	{"IOT", SIGIOT},
	{"BUS", SIGBUS},
	{"FPE", SIGFPE},
	{"KILL", SIGKILL},
	{"USR1", SIGUSR1},
	{"SEGV", SIGSEGV},
	{"USR2", SIGUSR2},
	{"PIPE", SIGPIPE},
	{"ALRM", SIGALRM},
	{"TERM", SIGTERM},
	{"CHLD", SIGCHLD},
	{"CONT", SIGCONT},
	{"STOP", SIGSTOP},
	{"TSTP", SIGTSTP},
	{"TTIN", SIGTTIN},
	{"TTOU", SIGTTOU},
	{"URG", SIGURG},
	{"XCPU", SIGXCPU},
	{"XFSZ", SIGXFSZ},
	{"VTALRM", SIGVTALRM},
	{"PROF", SIGPROF},
	{"WINCH", SIGWINCH},
	{"IO", SIGIO},
	{"SYS", SIGSYS},
	/*senales que no hay en todas partes*/
	#ifdef SIGPOLL
	{"POLL", SIGPOLL},
	#endif
	#ifdef SIGPWR
	{"PWR", SIGPWR},
	#endif
	#ifdef SIGEMT
	{"EMT", SIGEMT},
	#endif
	#ifdef SIGINFO
	{"INFO", SIGINFO},
	#endif
	#ifdef SIGSTKFLT
	{"STKFLT", SIGSTKFLT},
	#endif
	#ifdef SIGCLD
	{"CLD", SIGCLD},
	#endif
	#ifdef SIGLOST
	{"LOST", SIGLOST},
	#endif
	#ifdef SIGCANCEL
	{"CANCEL", SIGCANCEL},
	#endif
	#ifdef SIGTHAW
	{"THAW", SIGTHAW},
	#endif
	#ifdef SIGFREEZE
	{"FREEZE", SIGFREEZE},
	#endif
	#ifdef SIGLWP
	{"LWP", SIGLWP},
	#endif
	#ifdef SIGWAITING
	{"WAITING", SIGWAITING},
	#endif
	{NULL,-1},
};

int Senal(char * sen){  /*devuel el numero de senial a partir del nombre*/
	int i;

	for (i=0; sigstrnum[i].nombre!=NULL; i++)
		if (!strcmp(sen, sigstrnum[i].nombre))
			return sigstrnum[i].senal;
	return -1;
}

char *NombreSenal(int sen){ /*devuelve el nombre senal a partir de la senal*/
	int i; /* para sitios donde no hay sig2str*/

	for (i=0; sigstrnum[i].nombre!=NULL; i++)
		if (sen==sigstrnum[i].senal)
			return sigstrnum[i].nombre;
	if (sen==-1)
		return ("");
	return ("SIGUNKNOWN");
}
