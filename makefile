# MAKEFILE
# Simplex
# BIANCO Andy && DERVAUX Xavier
#
#Compilation flags
.PHONY : test clean
CC = gcc
#----------------------
#   COMMANDES
#----------------------
debug: FLAGS = -Wall -g -O3
debug: main.o simplexe.o modele.o bitmap.o vecteurI.o vecteurF.o fichier.o
	$(CC) $(FLAGS) main.o modele.o simplexe.o bitmap.o vecteurI.o vecteurF.o fichier.o -o debug
#----------------------
release: FLAGS = -Wall -O3
release: main.o simplexe.o modele.o bitmap.o vecteurI.o vecteurF.o fichier.o
	$(CC) $(FLAGS) main.o modele.o simplexe.o bitmap.o vecteurI.o vecteurF.o fichier.o -o release
#----------------------
test: 
	./.debugScript.sh
#----------------------    
clean:
	rm -f *.o 
#----------------------    
trueclean:
	rm -f *.o 
	rm -f valgrind/*.txt
	rm -f out/*.txt
#----------------------
#   FICHIERS
#----------------------
main.o: simplexe.h main.c
	$(CC) $(FLAGS) -c main.c -o main.o
#----------------------
simplexe.o: fichier.h  vecteurI.h vecteurF.h bitmap.h modele.h simplexe.h simplexe.c
	$(CC) $(FLAGS) -c simplexe.c -o simplexe.o
#----------------------
modele.o: fichier.h vecteurI.h vecteurF.h bitmap.h modele.h modele.c
	$(CC) $(FLAGS) -c modele.c -o modele.o
#----------------------
bitmap.o: bitmap.h bitmap.c
	$(CC) $(FLAGS) -c bitmap.c -o bitmap.o
#----------------------
vecteurI.o: bitmap.h vecteurI.h vecteurI.c
	$(CC) $(FLAGS) -c vecteurI.c -o vecteurI.o
#----------------------
vecteurF.o: vecteurF.h vecteurF.c
	$(CC) $(FLAGS) -c vecteurF.c -o vecteurF.o
#----------------------
fichier.o: modele.h simplexe.h fichier.h fichier.c
	$(CC) $(FLAGS) -c fichier.c -o fichier.o
#