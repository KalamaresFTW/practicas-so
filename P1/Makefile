#Autor: Pablo Costa Oubiña - p.costa
#Grupo: 3.2  (Viernes 8:30 - 10:30)
CFLAGS = -Wall -Wextra -g -O0
TARNAME = p1

all: clean f.o
	gcc $(CFLAGS) -o shell shell.c f.o
	gcc $(CFLAGS) -o list list.c f.o
	gcc $(CFLAGS) -o eliminate eliminate.c f.o

f.o:
	gcc $(CFLAGS) -c f.c

#Elimina todos los binarios(sin extensión), *.o y *.tar.gz.
clean:
	ls | grep -v "\." | grep -v Makefile | xargs rm -f *.o $(TARNAME).tar.gz

valgr:
	valgrind --leak-check=full --show-reachable=yes ./shell

tar:
	tar czvf $(TARNAME).tar.gz *.c *.h Makefile
