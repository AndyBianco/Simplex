#ifndef _FICHIER
#define _FICHIER
#include <stdio.h>

/********************************************************/
/*			      Les prototypes de fonction		    */
/********************************************************/ 
void ecrireErreur                   (char* nfs, int erreur);
void fichierSortie                  (char* nfs, char* message);

void imprimerTitre                  (FILE*, char* titre);
void imprimerSigneEqual             (FILE*, int sign);
void imprimerTransformation         (FILE*, int type, int num);
void imprimerChangementBase         (FILE*, int ancB, int nouvB);
void imprimerConditionExistance     (FILE*, int nbr);
void imprimerMessageSimplex         (FILE*, int type);
#endif