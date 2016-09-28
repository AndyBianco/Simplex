#include "vecteurF.h"
#include <stdlib.h>

struct vecteurF* allocVecteurF(int taille){
	struct vecteurF* vect = (struct vecteurF*)malloc(sizeof(struct vecteurF));
	if (vect){
		vect->V = (float*)calloc(taille, sizeof(float));
		vect->n = taille;
		
		if (!vect->V){	
			libererVecteurF(&vect);
		}
	}
	else{	//Erreur allocation de la structure
		libererVecteurF(&vect);
	}

	return vect;
}

void libererVecteurF(struct vecteurF** vect){
	if (*vect){
		free((*vect)->V); //Pas besoin de vérification, free gére le NULL
		free(*vect);
		*vect = NULL;
	}
}

/*
	Initialise une partie du vecteur à une valeur donnée
*/
void initVecteurF(struct vecteurF* v, int deb, int fin, float val){
	int i;
	
	for(i = deb; i < fin; i++){
		v->V[i] = val;
	}
}

/*
	Imprime dans un fichier jusqu'à une valeur donnée
*/
void imprimerVecteurF(FILE* fs, struct vecteurF* vect, int born){
	int i;
	for(i=0; i < born; i++){
		fprintf(fs, "%.2f    ", vect->V[i]);
	}
	fprintf(fs, "\n");
}