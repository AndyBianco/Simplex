#ifndef _SIMPLEXE
#define _SIMPLEXE

#include <stdio.h>

#include "modele.h"
#include "bitmap.h"
#include "vecteurF.h"
#include "vecteurI.h"

#define EPSILON 0.00001 //Précision 10^-5

struct simplexe {
    vecteurI*         Base;     //Est là pour stocker les variables en base.
    struct vecteurF*  Zj;       //Est le résultat du zj
    struct vecteurF*  Res;      //Est le récultat du calcul zj-cj
    
    int iPiv, jPiv;             //Les coordonnées du pivot
};

/*
	Les erreurs potenciels qui peuvent être rencontrées :
*/
#define ERROR_ALLOC_BITMAP_TRANS        -8
#define ERROR_ALLOC_STRUCT_SIMPLEXE     -9
#define ERROR_ALLOC_VECT_SIMPLEX        -10

/********************************************************/
/*			      Les prototypes de fonction		    */
/********************************************************/

/*
    Les fonctions d'allocation, libération et d'initialisation
*/
struct simplexe* allocSimplexe  (int n, int m);
void libererSimplexe            (struct simplexe**);
void initSimplexBase            (struct simplexe*, struct modele*, struct bitmap* bitS);

/*
    Les fonctions principales du programmes
*/

int traitementPrincipal     (char* fichierEntree, char* fichierSortie);
void imprimerFichierSortie  (char* nomF, struct modele*, struct simplexe*, struct bitmap* flag, struct bitmap* arti, struct vecteurF* ecoS, struct bitmap* bitS);
void traitementDeuxPhases   (FILE*, struct modele*, struct simplexe*, struct bitmap* arti, struct bitmap* bitS, struct vecteurF* ecoS);

/*
    Les fonctions d'impressions relative au simplex
*/
void imprimerSimplex    (FILE*, struct simplexe*, struct modele*, int bornJ, struct vecteurF* ecoS, struct bitmap* bitS);
void imprimerResultat   (FILE*, struct simplexe*, struct modele*);
void imprimerVectZj	        (FILE*, struct vecteurF*, struct bitmap* bitS, int posArti, int born);
void imprimerVectResultat   (FILE*, struct vecteurF* vect, struct bitmap* bitS, int posArti, int born);

/*
	Méthode du Simplexe
*/
int traitementMethodeSimplex    (FILE*, struct simplexe*, struct modele*,  int born, struct bitmap* bitS, struct vecteurF* ecoS);
int traitementOptimum           (struct simplexe*, struct modele*, struct bitmap* bitS, int siSolInf);
void interpreterSolution        (FILE *fs, struct simplexe*, struct modele*, struct bitmap* bitS, int *res, int phaseUn);

//Les fonctions relative au pivot.
int detectionPivot (FILE*, struct simplexe*, struct modele*, struct bitmap* bitS);
int introBase       (struct simplexe*, struct bitmap* bitS, int type);
int retraitBase     (struct modele*, int nb);

//Les fonctions relative au calcul.
void calculValeurs      (struct simplexe*, struct modele*, struct bitmap* bitS, int born);
void divisionLignePivot (struct simplexe*, struct modele*, struct bitmap* bitS,int born);
void calculerZj         (struct simplexe* simplex, struct modele*, int born, struct bitmap* bitS, struct vecteurF* ecoS);
void calculerResultat   (struct simplexe* simplex, struct modele*, int born, struct bitmap* bitS, struct vecteurF* ecoS);

//Les fonctions relativeà la vérification.
int verificationOptimum     (struct simplexe*, int type);
int verifAucuneSolution     (struct modele*, struct bitmap* bitS);
int verifInfiniteSolutions  (struct simplexe*, struct bitmap* bitS);
#endif