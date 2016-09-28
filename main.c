#include "simplexe.h"
#include "fichier.h"


/*
	Programme réalisé par :
		- Bianco Andy
		- Dervaux Xavier.
	
	Le programme a été entiérement réalisé sous linux et compilé avec gcc.
	Valgrind ne détecte aucune erreur dans la manipulaiton mémoire.
*/
int main (int argc, char **argv){
	int erreur = 0;
	
	erreur = traitementPrincipal(argv[1],argv[2]);
	ecrireErreur(argv[2], erreur);
	
	return 0;
}