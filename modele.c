#include "modele.h"
#include <stdlib.h>

/****************************************************************/
/*			         Allocation(s) / libération(s)			    */
/****************************************************************/

/*
	Alloue une partie de la structure modèle.
		La fonction alloues :
			- La structure modele.
			- La structure du vecteur de signe
			- La structure du vecteur des seconds membres. 
*/
 int allocModeleFirstParts (struct modele** model, int n){
	int erreur = 0;
   	*model = (struct modele*)malloc(sizeof(struct modele));
   
	if(*model){
		if(!((*model)->Sgn = allocVecteurI(n)))
			erreur = ERROR_ALLOC_MODELE_SIGN;
		else if(!((*model)->Sm = allocVecteurF(n)))
			erreur = ERROR_ALLOC_MODELE_SM;
	}   
	else{
		erreur = ERROR_ALLOC_MODELE;
	}
   
   return erreur;
}

/*
	Alloue le reste de la structure du modele.
		La fonction alloues :
			- La matrice M
			- La structure du vecteur de la fonction économique.
*/
int allocModeleLastParts(struct modele **model, int n, int m){
	int i = -1, erreur = 0;
    
	if(*model){
        (*model)->M = (float**)malloc(sizeof(float*)*n);
		if((*model)->M){
			do{
				i++;
                (*model)->M[i] = (float*)calloc(m,sizeof(float));
			} while ((*model)->M[i] != NULL && i< n-1);
			if (!(*model)->M[i]){
				erreur = ERROR_ALLOC_MODELE_M;
				liberationModele(&(*model), n);
			}else{
				if(!((*model)->Eco = allocVecteurF(m))){
					erreur = ERROR_ALLOC_MODELE_ECO;
					liberationModele(&(*model), n);
				}
			}
		}
		else{ //Libére le model
			erreur = ERROR_ALLOC_MODELE_M;
			liberationModele(&(*model), n);
		}
	}
	return erreur;
}

/*
	Libére la matrice M de la structure modele.
*/
void libererMatriceModele(struct modele *mod, int n){
	int i;
    if (mod->M){
        for (i = 0; i < n; i++){
            free(mod->M[i]);
        }
        free(mod->M);
    }
}

/*
	Libére toute la structure modele.
*/
void liberationModele(struct modele** model, int n){
    if(*model){
		libererMatriceModele(*model, n);
		
		libererBitmap(&(*model)->Sgn);
		libererVecteurF(&(*model)->Sm);
		libererVecteurF(&(*model)->Eco);
		
		free(*model);
		*model = NULL;	
	}
}

/****************************************************************/
/*			  Les fonctions principales du programme			*/
/****************************************************************/

/*
    Charge le fichier d'entrée dans la structure modele.
        Entrée :
            - [struct modele*] mod : La structure modele.
            - [char*] nomFichier : Le nom du fichier.
            - [struct bitmap**] flag : Bitmap qui contient les valeurs négatives du second membre.
        Sortie :
            [int] 0 : Aucun problème rencontré.
            [int] < 0 : erreur d'allocation.                 
*/
int chargementModele(struct modele **model, char *fichierEntree, struct bitmap** flag){
	int i,j, n, m, t, erreur = 0, nbr=0;
	FILE *fd;
	
    fd = fopen(fichierEntree, "r");
    if (!fd){
		erreur = ERROR_OPEN_FILE;
	} else{
		fscanf(fd, "%d %d %d", &n, &m, &t);
        
        if(!(erreur = allocModeleFirstParts(&(*model), n))){
			 if (!erreur && !((*flag) = allocBitmap(n))){ 
                erreur = ERROR_ALLOC_BITMAP_FLAG;
            }
		}
		
		if(!erreur){
            //Si tout est alloué, on commence à remplir la structure modele
			(*model)->n = n;
			(*model)->type = t;
            (*model)->m = m;
            //On lit les signes et les variables du second membre
			for(i = 0; i < n; i++){
				fscanf(fd, "%d", &(*model)->Sgn->V[i]);
				fscanf(fd, "%f", &(*model)->Sm->V[i]);   
				nbr += nbrVarAjout(*model, i, *flag);
			}
	
            (*model)->o = m + nbr;    //Nombre totale de colonne à alloué
            
			 if(!(erreur = allocModeleLastParts(&(*model),n,(*model)->o))){
				for(j = 0; j < m; j++){
					for(i= 0; i <= n; i++){
                        if(i == 0)
                             fscanf(fd, "%f", &(*model)->Eco->V[j]);   
                        else
						    fscanf(fd, "%f", &(*model)->M[i-1][j]);
					}
				}
			}	
		}
		fclose(fd);
	}
	
	return erreur;
}


/*
	Ajoute les variables d'ecarts, d'excedants et d'artificielles.
        Entrée :
            - [FILE*] fs : Le fichier de sortie
            - [struct modele*] mod  : La structure modele.
			- [struct bitmap*] arti : Bitmap qui va permettre d'ajouter les artificielles.
            - [struct bitmap*] flag : Bitmap qui contient les valeurs négatives du second membre.
*/
void traitementTransformation(FILE *fs, struct modele *model, struct bitmap* arti, struct bitmap* flag){
	inversionSigne(model, flag);
	
	model->posExe = ajoutEcart(fs, model, arti);
	model->posArti = retraitExcedant(fs, model, model->posExe);	
	ajoutArtificielle(fs, model, arti, model->posArti);
}


/****************************************************************/
/*    Fonctions en relation avec la transformation du modèle    */
/****************************************************************/

/*
	Rend les seconds membres négatives en possitives et inverses les signes de la ligne correspondante.
		  Entrée :
		  	- [struct modele*] mod : La structure modele. 
			- [struct bitmap*] flag : Bitmap qui contient les valeurs négatives du second membre. 
*/
void inversionSigne(struct modele *mod, struct bitmap *flag){
	int i, j;
	
	for(i=0; i < flag->n; i++){
		if(flag->V[i]){ //Si la ligne a été marquée à la lecture du fichier
			for(j=0; j < mod->m; j++){
				mod->M[i][j] *= -1; //On inverse le signe de chaque élément de la ligne
			}
            mod->Sm->V[i]   *= -1; //On inverse le signe du second membre
			mod->Sgn->V[i]  *= -1; //Comme les deux membres de l'inéquation on été multipliés par un nombre négatif, on inverse le signe de cette inéquation.
		}
	}
}

/*
	Ajoute les variables d'ecarts.
		Entrée :
			- [FILE*] fs : Le fichier de sortie.
			- [struct modele*] mod : La structure modele. 
			- [struct bitmap*] arti : Bitmap qui va permettre d'ajouter les artificielles.
		Sortie : 
			- Retourne la position auquel on a arrêté d'ajouter des variables d'ecarts.
*/
int ajoutEcart(FILE *fs, struct modele *mod, struct bitmap *arti){
	int i;
	int position = mod->m;
	
	for(i=0; i < mod->n; i++){ //On parcourt la bitmap des signes d'égalité
		
		if(mod->Sgn->V[i] == -1){ //Si on trouve un <=
			mod->M[i][position] = 1; //Ajout de variable d'Ecart
			mod->Sgn->V[i] = 0; //Mise à jour du signe dans la bitmap des signes
			arti->V[i] = 1; //On marque la bitmap pour assurer aux fonctions suivantes que cette ligne à déjà été traité. (Car le signe d'égalité est désormais "=")
			imprimerTransformation(fs, -1, position+1);
			position++;
		}
	}	
	return position;
}

/*
	Ajoute les variables d'excédants.
		Entrée :
			- [FILE*] fs : Le fichier de sortie.
			- [struct modele*] mod : La structure modele.
			- [int] position : La position où il faut commencer à les ajouter.
		Sortie : 
			- Retourne la position auquel on a arrêté d'ajouter des variables d'excédant.
*/
int retraitExcedant(FILE *fs, struct modele *mod, int position){
	int i;
	
	for(i=0; i < mod->n; i++){ //On parcourt la bitmap des signes d'égalité
		if(mod->Sgn->V[i] == 1){ //Si on trouve un >=
			mod->M[i][position] = -1; //Ajout de variable d'excedant
			mod->Sgn->V[i] = 0; //Mise à jour du signe dans la bitmap des signes
			//NOTE: A cet instant l'égalité n'est pas encore exacte, toute fois on sait qu'elle le sera au passage de la fonction ajoutArtificielle()
			imprimerTransformation(fs, 1, position+1);
			position++;
		}
	}
	return position;
}

/*
	Ajoute les variables artificielles.
		Entrée : 
			- [FILE*] fs : Le fichier de sortie.
			- [struct modele*] mod : La structure modele.
			- [struct bitmap*] arti : Bitmap qui permet d'ajouter les artificielles.
			- [int] position : La position où il faut commencer à les ajouter.
*/
void ajoutArtificielle(FILE *fs, struct modele *mod, struct bitmap *arti, int position){
	int i;
	
	for(i=0; i < mod->n; i++){ //On parcourt la bitmap des signes d'égalité
		if(!arti->V[i]){ //Si on trouve une position non-marqué dans la bitmap de traitement on sait qu'il s'agit d'un "=" non-traité.
			mod->M[i][position] = 1; //Ajout de variable artificielle
			imprimerTransformation(fs, 0, position+1);
			position++;
		}
	}
}

/****************************************************************/
/*		     Les fonctions d'impressions du modele              */
/****************************************************************/

void imprimerStrucureModele(FILE *fs, struct modele* model, int m, int hidden){
	imprimerFonctEco(fs, model, hidden);
	imprimerModele(fs, model, m);
    imprimerConditionExistance(fs, m);
}


/*
	Imprime la fonction économique.
	
	Entrée :
		- [FILE *] fs : Le fichier de sortie.
		- [struct modele*] model : le modèle à utiliser.
		-[int] hidden : Affiche ou non les valeurs équales à 0.
*/
void imprimerFonctEco(FILE *fs, struct modele* model, int hidden){
	int i;
	
	if(model->type > 0)
		fprintf(fs,"Max Z = ");
	else
		fprintf(fs,"Min Z = ");
		
	for(i= 0; i < model->Eco->n; i++){
		if(hidden){
			 if(model->Eco->V[i] != 0.0){
				fprintf(fs,"%.3f x%d  ", model->Eco->V[i],i+1);
			 }
		}else{
            fprintf(fs,"%.3f x%d  ", model->Eco->V[i],i+1);
		}
	}
    fprintf(fs,"\n");
}

void imprimerModele(FILE* fs, struct modele* model, int nbrCol){
	int i,j;
	
	for(i = 0 ; i < model->n ; i++){
		for(j = 0; j < nbrCol; j++){
           if(model->M[i][j] == 0.0)
                fprintf(fs,"           ");
            else
			    fprintf(fs,"%.3f x%d   ", model->M[i][j],j+1);
		}
        fprintf(fs," %.3f\n", model->Sm->V[i]);
	}
    fprintf(fs,"\n");
}

/****************************************************************/
/*			                 Autres			                    */
/****************************************************************/

/*
    Anticipe les changements de signe potenciel et compte le nombre de
    variable qui va falloir ajouter pour chaque position i.
        Sortie : 
            - [int] nombre de colonne à ajouter.
			
	[NOTE] : On a mis en avant la lisibilité en priorité.
*/
int nbrVarAjout(struct modele *mod, int i, struct bitmap *flag){
	int retour = 0;
	
	if(mod->Sm->V[i] <= 0){ //Le second membre est négatif
		flag->V[i] = 1;
		switch(mod->Sgn->V[i]){
			case -1 : retour = 2; break;
			case 0  : retour = 1; break;
			case 1  : retour = 1; break;
		}
	} else { //Le second membre est positif
		switch(mod->Sgn->V[i]){
			case -1 : retour = 1; break;
			case 0  : retour = 1; break;
			case 1  : retour = 2; break;
		}
	}
	return retour;
}

/*
	Vérifie qu'une colonne de la matrice est strictement inférieur à une valeur en paramètre
*/
int verificationColMatrice(struct modele* model, int pos, float val){
	int erreur = 1, i = 0;
	
	while(erreur && i < model->n){
		if(model->M[i][pos] > val){	//On vérifie si il est supérieur
			erreur = 0;
		}
		i++;
	}
	return erreur;
}