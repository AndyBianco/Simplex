#ifndef _BITMAP
#define _BITMAP

struct bitmap{
	int *V;
	int n; 
}; 
	
/********************************************************/
/*			      Les prototypes de fonction		    */
/********************************************************/
	struct bitmap* allocBitmap		(int taille);
	void libererBitmap				(struct bitmap**);
	int verifcationBitmap			(struct bitmap*, int val);
#endif