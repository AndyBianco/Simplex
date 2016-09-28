#ifndef _MODELE
#define _MODELE

#include <stdio.h>

#include "fichier.h"
#include "bitmap.h"
#include "vecteurF.h"
#include "vecteurI.h"



struct modele {
	int n, m, o;
	int type; // type de fonction économique (Max, Min)

	vecteurI* Sgn;
	struct vecteurF* Sm;
	struct vecteurF* Eco;	
	float **M;  //M** est alloué en 'n' : le nombre de ligne et 'm' : le nombre de colonne
	int posExe, posArti;
};

/*
	Les erreurs potenciels qui peuvent être rencontrées :
*/
#define ERROR_OPEN_FILE -1
#define ERROR_ALLOC_MODELE -2
#define ERROR_ALLOC_MODELE_SIGN -3
#define ERROR_ALLOC_MODELE_SM -4
#define ERROR_ALLOC_MODELE_ECO -5
#define ERROR_ALLOC_BITMAP_FLAG -6
#define ERROR_ALLOC_MODELE_M -7

/********************************************************/
/*			      Les prototypes de fonction		    */
/********************************************************/

/*
	Fonctions : d'allocations et de libérations du modele.
*/
int allocModeleFirstParts (struct modele** model, int n);
int allocModeleLastParts(struct modele **model, int n, int m);

void libererMatriceModele(struct modele *mod, int n);
void liberationModele(struct modele**, int n);

/*
	Les fonctions principales de modèle
*/
int chargementModele(struct modele**, char*fichierEntree, struct bitmap**);
void traitementTransformation(FILE *fs, struct modele *model, struct bitmap* arti, struct bitmap* flag);

/*
    Fonctions en relation avec la transformation du modèle
*/
void inversionSigne(struct modele *mod, struct bitmap *bmp);
int ajoutEcart(FILE *fs, struct modele *mod, struct bitmap *bmp);
int retraitExcedant(FILE *fs, struct modele *mod, int position);
void ajoutArtificielle(FILE *fs, struct modele *mod, struct bitmap *bmp, int position);

/*
	Les fonctions d'impression du modèle
*/
void imprimerModele(FILE*, struct modele*, int nbrCol);
void imprimerStrucureModele(FILE *fs, struct modele* model, int m, int hidden);
void imprimerFonctEco(FILE*, struct modele*, int hidden);

/*
	Autres
*/
int nbrVarAjout(struct modele *mod, int i, struct bitmap *flag);
int verificationColMatrice(struct modele* model, int pos, float val);
#endif
