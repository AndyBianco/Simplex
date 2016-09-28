#ifndef _VECTEURFLOAT
#define _VECTEURFLOAT
	
#include <stdio.h>

struct vecteurF{
	float *V;
	int n; //taille
};
	
/********************************************************/
/*			      Les prototypes de fonction		    */
/********************************************************/
	
	struct vecteurF* allocVecteurF	(int taille);
	void libererVecteurF			(struct vecteurF**);
	
	void initVecteurF				(struct vecteurF*, int deb, int fin, float val);
	void imprimerVecteurF			(FILE*, struct vecteurF*, int born);
	
#endif