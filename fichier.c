#include "fichier.h"
#include "modele.h"
#include "simplexe.h"

#include <stdlib.h>
#include <string.h>

/* 
	Ecrit le message d'erreur correspondant au code d'erreur. 
*/
void ecrireErreur (char* nfs, int erreur){
	switch(erreur){
		case ERROR_OPEN_FILE : 
			fichierSortie(nfs, "[Erreur] Echec d'ouverture du fichier d'entrée dans chargementModele();.\n");
		 	break;
		case ERROR_ALLOC_MODELE : 
			fichierSortie(nfs, "[Erreur] Echec d'allocation de la structure modèle dans allocModeleFirstParts().\n");
			break;
		case ERROR_ALLOC_MODELE_SIGN : 
			fichierSortie(nfs, "[Erreur] Echec d'allocation du vecteur de int \"Sgn\" dans allocModeleFirstParts().\n");
			break;
		case ERROR_ALLOC_MODELE_SM : 
			fichierSortie(nfs, "[Erreur] Echec d'allocation du vecteur de float \"Sm\" dans allocModeleFirstParts().\n");
			break;
		case ERROR_ALLOC_MODELE_ECO : 
			fichierSortie(nfs, "[Erreur] Echec d'allocation du vecteur de float \"Eco\" dans allocModeleLastParts().\n");
			break;
		case ERROR_ALLOC_MODELE_M : 
			fichierSortie(nfs, "[Erreur] Echec d'allocation de la matrice \"M\" dans allocModeleLastParts().\n");
			break;
		case ERROR_ALLOC_BITMAP_FLAG : 
			fichierSortie(nfs, "[Erreur] Echec d'allocation de la bitmap \"flag\" dans chargementModele().\n");
			break;
		case ERROR_ALLOC_BITMAP_TRANS : 
			fichierSortie(nfs, "[Erreur] Echec d'allocation de la bitmap \"arti\" dans traitementPrincipal().\n");
			break;
		case ERROR_ALLOC_STRUCT_SIMPLEXE : 
			fichierSortie(nfs, "[Erreur] Echec d'allocation de la structure \"Simplexe\" dans traitementPrincipal().\n");
			break;
		case ERROR_ALLOC_VECT_SIMPLEX : 
			fichierSortie(nfs, "[Erreur] Echec d'allocation du vecteur \"ecoS\" dans traitementPrincipal().\n");
			break;
	}
}

void fichierSortie(char* nfs, char* message){
	FILE *fs = fopen(nfs, "w");

	if (fs){
		fprintf(fs,"%s", message);
		fclose(fs);
	}
}

/*
	Imprime un titre entouré d'étoile (*)
*/
void imprimerTitre(FILE* fs, char* titre){
	int i, count = strlen(titre)+2;
	fprintf(fs,"\n\n");
	for(i=0; i < count; i++){
		fprintf(fs,"*");
	}
    fprintf(fs,"\n");
	fprintf(fs,"*%s*\n", titre);
	for(i=0; i < count; i++){
		fprintf(fs,"*");
	}
    fprintf(fs,"\n\n");
}

void imprimerSigneEqual(FILE* fs, int sign){
	switch(sign){
		case -1 : fprintf(fs, "<="); break;
		case 0 : fprintf(fs, "="); break;
		case 1 : fprintf(fs, ">="); break;
	}
}

void imprimerTransformation (FILE* fs, int type, int num){
	switch(type){
		case -1 : 
			fprintf(fs, "Ajout d'une variable d'écart : x%d\n",num); 
			break;
		case 0 : 
			fprintf(fs, "Ajout d'une variable artificielle : x%d\n",num); 
			break;
		case 1 : 
			fprintf(fs, "Ajout d'une variable d'excédant : x%d\n",num); 
			break;
	}
}

void imprimerChangementBase(FILE* fs, int valeur, int type){
	if(type == 1){ //Entrée en base
		fprintf(fs,"\nx%d entre en base.\n", valeur + 1);
	} else { //Sortie de base
		fprintf(fs,"x%d sort de base.\n\n" , valeur + 1);
	}
}

void imprimerConditionExistance(FILE* fs, int nbr){
	int i;
	
	for(i=1 ; i <= nbr; i++){
		fprintf(fs,"x%d	>= 0\n", i);
	}
}

void imprimerMessageSimplex(FILE* fs, int type){
	switch(type){
		case 0 : 
			fprintf(fs, "Aucune variable artificielle n'a été trouvé !\n"); 		
			break;
		case 1 : 
			fprintf(fs, "Il n'éxiste pas de solution !\n");
			break;
		case 2 : 
			fprintf(fs, "Il existe une solution infinie !\n");
			break;
		case 3 : 
			fprintf(fs, "Il existe une infinité de solution !\n");		
			break;
	}
}