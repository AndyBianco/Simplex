#include "vecteurI.h"

vecteurI* allocVecteurI(int taille){
	return allocBitmap(taille);
}

void libererVecteurI(vecteurI** v){
	libererBitmap(v);
}

int verifcationVecteurI(vecteurI* v, int val){
	return verifcationBitmap(v, val);	
}