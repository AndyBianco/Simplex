#ifndef _vecteurI
#define _vecteurI

#include "bitmap.h"

	typedef struct bitmap vecteurI;
	
/********************************************************/
/*			      Les prototypes de fonction		    */
/********************************************************/

	vecteurI* allocVecteurI		(int taille);
	void libererVecteurI		(vecteurI**);
	int verifcationVecteurI		(vecteurI*, int val);
#endif