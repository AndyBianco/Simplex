#include "simplexe.h"
#include "fichier.h"

#include <stdlib.h>
#include <float.h>
#include <math.h>

/****************************************************************/
/*		Allocation(s) / libération(s)	/ initialisation(s)		*/
/****************************************************************/

struct simplexe* allocSimplexe (int n, int m){
    struct simplexe* simplex = NULL;
    
    simplex = (struct simplexe*) malloc(sizeof(struct simplexe));
    
    if(simplex){
        simplex->Base = allocVecteurI(n);
        simplex->Zj = allocVecteurF(m+1);
        simplex->Res = allocVecteurF(m);
        
        if(!simplex->Base || !simplex->Zj || !simplex->Res){
            libererSimplexe(&simplex);
        }
    }
	
	return simplex;
}

void libererSimplexe(struct simplexe **simplex){
    if(*simplex){
        libererVecteurI(&(*simplex)->Base);
        libererVecteurF(&(*simplex)->Zj);
        libererVecteurF(&(*simplex)->Res);
        
        free(*simplex);
        *simplex = NULL;
    }
}

/*
	On initialise dans model->Base les variables qui sont en base pour l'algorithme du simplex.
	On marque uniquement les variables d'ecarts en base dans une bitmap. 
	Les artificielles seront marqués quand elles sortent de base pour éviter qu'il rerentre en base.
*/
 void initSimplexBase(struct simplexe* simplex, struct modele* model, struct bitmap* bitS){
	 int i, j, pos = 0; 

	 for(i= 0 ; i < model->n; i++){
		 for(j= model->m; j < model->posExe; j++){
			 if(model->M[i][j] > 0){
				 simplex->Base->V[pos]  = j; 
				 bitS->V[j] = 1;	//On marque les variables en base.
				 pos++;
			 }
		 }
		 for(j= model->posArti; j < model->o; j++){
			 if(model->M[i][j] > 0){
				simplex->Base->V[pos]  = j; 
				pos++;
			 }
		 }
	 }
 }

/****************************************************************/
/*			  Les fonctions principales du programme			*/
/****************************************************************/


/*
	traitementPrincipal est la fonction principale du programme, elle alloues / libères les ressources et 
	appeles les fonctions mères du programme. 
*/
int traitementPrincipal(char* fichierEntree, char* fichierSortie){
	int erreur = 0;
	
	struct modele* 		model = NULL;
	struct simplexe* 	simplex = NULL;
    struct bitmap* 		flag = NULL; 		//Flag les valeurs à inverser;
	struct bitmap* 		arti = NULL; 		//Utiliser pour ajouter les variables artificielles
	
	struct vecteurF* 	ecoS = NULL;		//Fonction économique de la phase 1.
	struct bitmap* 		bitS = NULL;		//Bitmap qui gére les variables en bases et les artificielles.
	
	if(!(erreur = chargementModele(&model,fichierEntree, &flag))){
        if((arti = allocBitmap(model->n))){
			if((simplex = allocSimplexe(model->n, model->o))){
				if((ecoS = allocVecteurF(model->o)) && (bitS = allocBitmap(model->o))){
					imprimerFichierSortie(fichierSortie, model, simplex, flag, arti, ecoS, bitS);
				}else{
						erreur = ERROR_ALLOC_VECT_SIMPLEX;
				}
			}
			else{
				erreur = ERROR_ALLOC_STRUCT_SIMPLEXE;
			}		
		}else{
			erreur = ERROR_ALLOC_BITMAP_TRANS;
		}

        //Libération des ressouces potienciellement alloués
        libererBitmap(&arti);
		libererBitmap(&flag);
		libererBitmap(&bitS);
		libererVecteurF(&ecoS);
	
		libererSimplexe(&simplex);
		liberationModele(&model, model->n);
	}
	return erreur;
}


void imprimerFichierSortie(char* nomF, struct modele* model, struct simplexe* simplex, struct bitmap* flag, struct bitmap* arti, struct vecteurF* ecoS, struct bitmap* bitS){
	FILE* fs = NULL;
	fs = fopen(nomF, "w");
	
	if(fs){
		imprimerTitre(fs,"Soit le programme linéaire suivant");
        imprimerStrucureModele(fs,model, model->m, 1);
		
		imprimerTitre(fs,"Transformation du programme linèaire");
		traitementTransformation(fs, model, arti, flag);
		
		imprimerTitre(fs,"Programme linéaire transformé");
		imprimerStrucureModele(fs,model, model->o, 0);
		
		imprimerTitre(fs,"PREMIERE PHASE");
	
		traitementDeuxPhases(fs, model, simplex, arti, bitS, ecoS);
		fclose(fs);
	}
}

/*
	Traite l'algorithme des deux phases.
	Entrée :
			[struct simplexe*] simplex : La structure simplexe.
			[const struct modele*] model : La structure modele.
			
			[struct bitmap*] arti : La bitmap qui gére les artifielles.
			[struct vecteurF*] ecoS : La fonction économique de la phase 1.
			[struct*] bitS : La bitmap qui gére les entrées / sorties de base
	
*/
void traitementDeuxPhases(FILE *fs, struct modele *model, struct simplexe* simplex, struct bitmap* arti, struct bitmap* bitS, struct vecteurF* ecoS){
	int res;
	initSimplexBase(simplex, model, bitS);
	
	if(verifcationBitmap(arti, 1)){ //phase 2		
	
		imprimerMessageSimplex(fs,0);
		imprimerTitre(fs,"DEUXIEME PHASE");

		res = traitementMethodeSimplex(fs,simplex, model, model->posArti, bitS, NULL);
		interpreterSolution(fs, simplex, model, bitS, &res, 0);
	}else{ // phase 1 & 2
		initVecteurF(ecoS, model->posArti, model->o, -1.0*model->type);
		res = traitementMethodeSimplex(fs,simplex, model, model->o, bitS, ecoS);
		interpreterSolution(fs, simplex, model, bitS, &res, 1);
		if(!res){
			imprimerTitre(fs,"DEUXIEME PHASE");
			res = traitementMethodeSimplex(fs,simplex, model, model->posArti,bitS, NULL );
			interpreterSolution(fs, simplex, model, bitS, &res, 0);
		}
		
	}
}

/****************************************************************/
/*		   Fonctions d'impression relative au simplex	        */
/****************************************************************/

/*
	Imprime le simplexe 
		Entrée :
			[struct simplexe*] simplex : La structure simplexe.
			[const struct modele*] model : La structure modele.
			
			[struct vecteurF*] ecoS : La fonction économique de la phase 1.
			[struct bitmap*] bitS : La bitmap qui gére les entrées / sorties de base.
*/
void imprimerSimplex (FILE* fs, struct simplexe* simplex, struct modele* model, int bornJ, struct vecteurF* ecoS, struct bitmap* bitS){
	int i,j;
	
	fprintf(fs, "\t\t\t\t");
	if(ecoS){
		imprimerVecteurF(fs, ecoS, bornJ);
	}else{
		imprimerVecteurF(fs, model->Eco, bornJ);
	}

	fprintf(fs, "Base\tck\tP0\t");
	for(i = 1; i <= bornJ; i++){
		fprintf(fs, "\t  X%d", i);
	}
	
	fprintf(fs, "\n");
	for(i=0; i < model->n; i++){
		if(ecoS)
			fprintf(fs, "X%d\t %.2f\t%.2f   ",simplex->Base->V[i]+1, ecoS->V[simplex->Base->V[i]], model->Sm->V[i]);
		else
			fprintf(fs, "X%d\t %.2f\t%.2f   ",simplex->Base->V[i]+1, model->Eco->V[simplex->Base->V[i]], model->Sm->V[i]);
		for(j=0; j < bornJ; j++){
			if(j >= model->posArti && bitS->V[j]){//Si un tableau est marqué, il ne faut pas l'afficher!
				fprintf(fs, "/////    ");
			}else{
				fprintf(fs,"%.2f    ", model->M[i][j]);
			}
		}
		fprintf(fs, "\n");
	}
	
	fprintf(fs, "Zj			");
	imprimerVectZj(fs, simplex->Zj, bitS, model->posArti, bornJ+1);
	fprintf(fs, "Zj-Cj			   ");
	imprimerVectResultat(fs, simplex->Res, bitS, model->posArti, bornJ);
}

/*
	Imprime les résultats finaux (l'optimum).
*/
void imprimerResultat(FILE *fs, struct simplexe* simplex, struct modele* model){
	int i;
	
	fprintf(fs, "\n\t");
	for(i=0; i < model->Sm->n; i++){
		fprintf(fs, "X%d =%.2f, ", simplex->Base->V[i]+1, model->Sm->V[i]);
	}
	fprintf(fs,"z = %.2f", simplex->Zj->V[0]);
}

/*
	Imprime un vecteur de float.
	les foncitons sont ici pour éviter de faire un lien de bitmap dans vecteurF.
	On a besoin d'une bitmap pour ne pas imprimer les variables artificielles.
*/

void imprimerVectZj(FILE* fs, struct vecteurF* vect, struct bitmap* bitS, int posArti, int born){
	int i;
	for(i=0; i < born; i++){
		if( i > 0 && i > posArti && bitS->V[i-1]){
			fprintf(fs, "/////    ");
		} else{
			fprintf(fs, "%.2f    ", vect->V[i]);
		}
	}
	fprintf(fs, "\n");
}

void imprimerVectResultat(FILE* fs, struct vecteurF* vect, struct bitmap* bitS, int posArti, int born){
	int i;
	for(i=0; i < born; i++){
		if( i >= posArti && bitS->V[i]){
			fprintf(fs, "/////   ");
		} else{
			fprintf(fs, "%.2f   ", vect->V[i]);
		}
	}
	fprintf(fs, "\n");
}

/****************************************************************/
/*			          Méthode du Simplexe	                    */
/****************************************************************/

int traitementMethodeSimplex(FILE *fs, struct simplexe* simplex, struct modele* model, int born, struct bitmap* bitS, struct vecteurF* ecoS){
	int first = 1, res, solInf = 0;
		
	do{
		if(!first){
			solInf = detectionPivot(fs, simplex, model, bitS);
			
			if(!solInf){//Si pas de solution infinie !
				calculValeurs(simplex, model, bitS, born);
				divisionLignePivot(simplex, model, bitS, born);
			}
		}
		first = 0;
		
		if(!solInf){//Si pas de solution infinie !
			calculerZj(simplex, model, born, bitS, ecoS);
			calculerResultat(simplex, model, born, bitS, ecoS);

			imprimerSimplex(fs, simplex,  model, born, ecoS,  bitS);
		}
	  }while(!(res = traitementOptimum(simplex, model, bitS, solInf)));
	
	return res; 
}

int traitementOptimum(struct simplexe* simplex, struct modele* model, struct bitmap* bitS, int siSolInf){
	int erreur = 0;
	
	if(siSolInf){
		erreur = -1;
	} else if(verificationOptimum(simplex, model->type)){
		erreur = 1;
	}
	return erreur;
}

void interpreterSolution(FILE *fs, struct simplexe *simplex, struct modele *model, struct bitmap* bitS, int *res, int phaseUn){
	if(*res < 0){ //Solution infinie
		imprimerTitre(fs,"SOLUTION");
		imprimerMessageSimplex(fs, 2);
	} else{
		if(phaseUn){ //si phase 1
			if(verifAucuneSolution(model, bitS)){ //Aucune solution
				*res = -2;
				imprimerTitre(fs,"SOLUTION");
				imprimerMessageSimplex(fs, 1);
			} else{
				*res = 0;
			}
		} else{
			imprimerTitre(fs,"SOLUTION");
			if(verifInfiniteSolutions(simplex, bitS)){//Infinité de solution.
				*res = -3;  //Infinité de Solutions
				imprimerMessageSimplex(fs, 3);
			}
			imprimerResultat(fs, simplex, model);
		}
	}
}

/*************************************/
/*   Les fonctions relative au pivot */
/*************************************/

/*
	La fonction detectionPivot détecte la variable en base qui va entrée, sortir et 
	imprime ensuite les résultats dans le fichier de sortie.
		Entrée :
			[FILE*] fs : Fichier de sortie.
			[struct simplexe*] simplex : La structure simplexe.
			[struct modele*] model : La structure modele.
			[struct vecteurF*] ecoS : La fonction économique de la phase 1.
			[struct*] bitS : La bitmap qui gére les entrées / sorties de base.
*/
int detectionPivot(FILE* fs, struct simplexe* simplex, struct modele* model, struct bitmap* bitS){
	int erreur = 0;
	simplex->jPiv = introBase(simplex, bitS, model->type);         //Entrée en base
	
	if((simplex->iPiv = retraitBase(model, simplex->jPiv)) > -1){ //Si pas d'infinité de solution.
		imprimerChangementBase(fs, simplex->jPiv, 1);
		imprimerChangementBase(fs,simplex->Base->V[simplex->iPiv], 0);
		
		/*
		1er if : On retire les variables qui sorte de base de la bitmap sauf les artificielles!
		Comme ça on a pas besoin de réinitialiser la bitmap avant d'entrée en phase 2.
		
		
		2eme if : On vérifie si la variable sortant est une artificielle, comme ça on la marque pour plus qu'elle ne rentre en base!
		*/
		if(simplex->Base->V[simplex->iPiv] < model->posArti){	
			bitS->V[simplex->Base->V[simplex->iPiv]] = 0;
		} else if(simplex->Base->V[simplex->iPiv] >= model->posArti){	
			bitS->V[simplex->Base->V[simplex->iPiv]] = 1;
		}
		
		//On place la nouvelle variable à la place de l'ancienne.
		simplex->Base->V[simplex->iPiv] = simplex->jPiv;
	} else{
		erreur = 1;
	}	
	
	return erreur;
}

/*
	La fonction introBase cherche la variable à entrée en base.
		Entrée :
			[struct simplexe*] simplex : La structure simplexe.
			[const struct modele*] model : La structure modele.
			[struct*] bitS : La bitmap qui gére les entrées / sorties de base.
			[int] type : Le type de la fonction economique.
		Sortie :
			- Retourne la position de la variable à entrée en base.   
*/
int introBase(struct simplexe* simplex, struct bitmap* bitS, int type){
	int i = 0, pos = -1;
	
	while (pos < 0 && i < simplex->Res->n){ 
		if(simplex->Res->V[i] * type < -EPSILON){
			if(!bitS->V[i]){ //Si la variable n'est pas marqué!
				pos = i;
				bitS->V[i] = 1; 
			} 	
		}
		i++;
	}

	return pos;
}

/*
	La fonction retraitBase cherche la valeur à sortie de base.
		Entrée :
			[const struct modele*] model : La structure modele.
			[struct*] bitS : La bitmap qui gére les entrées / sorties de base.
			[int] nb : Position de la variable entrante.
		
		Sortie :
			- > 0: Retourne la position de la variable à sortie de base.
			- -1 : Indique qu'il n'existe pas de variable à sortie de base. 
*/
int retraitBase(struct modele *model, int nb){		
	int i, pos = 0;

	/*
		On recherche la 1er valeur non négatif pour l'utiliser comme comparaison.
		Avec la vérification de l'infinité de solution on sait qu'il aura forcement
		une valeur à sortie de base.
	*/
	while(pos < model->Sm->n && model->M[pos][nb] < EPSILON){	
		pos++;
	}
	if(pos < model->Sm->n){	//Si on n'est pas à la fin du tableau. 					
		for(i=pos; i < model->Sm->n; i++){ 
			if(model->M[i][nb] > EPSILON){	//Si le diviseur n'est pas négatif
				if( (model->Sm->V[pos] / model->M[pos][nb]) > (model->Sm->V[i] / model->M[i][nb]) ){ 
					pos = i;
				}
			}
		}
	} else{
		pos = -1;
	}
	
	return pos;
}


/*************************************/
/*  Les fonctions relative au calcul */
/*************************************/

/*
	Fonction qui calcule :
		- Les seconds membres sauf celui du pivot.
		- Le simplexe sauf la ligne du pivot et les variables artificielles marquées.
	
		Entrée :
			[struct simplexe*] simplex : La structure simplexe.
			[struct modele*] model : La structure modele.
			[struct*] bitS : La bitmap qui gére les entrées / sorties de base.
			[int] born : la position d'arrêt des calculs.	
*/
void calculValeurs(struct simplexe* simplex, struct modele *model, struct bitmap* bitS, int born){
	int i, j;
	
	for(i=0; i < model->n; i++){ //Calcul des second membres
		if(i != simplex->iPiv) //Si ce n'est pas le second membre du pivot.
			model->Sm->V[i] -= (model->M[i][simplex->jPiv] / model->M[simplex->iPiv][simplex->jPiv]) * model->Sm->V[simplex->iPiv]; //Second Membre
	}
	
	for(i=0; i < model->n; i++){ 
		for(j=0; j < born; j++){
			if(i != simplex->iPiv && j != simplex->jPiv){ //Si on est pas à la ligne et à la colonne du pivot.
				if(j < model->posArti || (j >= model->posArti && !bitS->V[j])){	//Si on est pas dans les artificielles ou qu'on y soit mais que la variable n'est pas marqué.
					model->M[i][j] -= (model->M[i][simplex->jPiv] / model->M[simplex->iPiv][simplex->jPiv]) * model->M[simplex->iPiv][j]; //Matrice
				}
			}
		}
	}
	
	//Calcule la colonne du pivot sauf le pivot.
	for(i=0; i < model->n; i++){ 
		if(i != simplex->iPiv){
			model->M[i][simplex->jPiv] -= (model->M[i][simplex->jPiv] / model->M[simplex->iPiv][simplex->jPiv]) * model->M[simplex->iPiv][simplex->jPiv];
		}
	}
}


/*
	La fonction divisionLignePivot divise :
		- la ligne du pivot par le pivot mais pas le pivot.
		- divise le second membre de la ligne du pivot par le pivot.
		
		Entrée :
			[struct simplexe*] simplex : La structure simplexe.
			[const struct modele*] model : La structure modele.
			[struct*] bitS : La bitmap qui gére les entrées / sorties de base.
			[int] born : la position d'arrêt des calculs.	
*/
void divisionLignePivot(struct simplexe* simplex, struct modele *model, struct bitmap* bitS, int born){
	int j;
	float pivot = model->M[simplex->iPiv][simplex->jPiv];
	
	for(j=0; j < born; j++){
		if(j < model->posArti || (j >= model->posArti && !bitS->V[j])){	//Si on est pas dans les artificielles ou qu'on y soit mais que la variable n'est pas marqué.
			model->M[simplex->iPiv][j] /= pivot;  
		} 
	}
	
	model->Sm->V[simplex->iPiv] /= pivot; //Second membre de la ligne du pivot.
}


void calculerZj(struct simplexe* simplex, struct modele* model, int born, struct bitmap* bitS, struct vecteurF* ecoS){
	int i, j;
	float result;
	struct vecteurF* eco;
	
	/*
		Vérifie et copie le pointeur du la fonction economique qu'on utilise
		 pour simplifier le if/else dans la boucle for ci-dessous.
	*/
	if(ecoS)
		eco = ecoS;
	else
		eco = model->Eco;
	
	for(i = 0; i <= born; i++){ 
		result = 0;
		for(j = 0; j < model->n; j++){
			if(i == 0)
				result += eco->V[simplex->Base->V[j]]* model->Sm->V[j];
			else{
				if(j < model->posArti || ( j >= model->posArti && !bitS->V[j])){
					result += eco->V[simplex->Base->V[j]]* model->M[j][i-1]; //-1 car sinon on calcule avec une colonne de décallage à cause de second membre.
				}
			}
		}
		simplex->Zj->V[i] = result;
	}	
}

/*
	Calcule le Zj-Cj
		Entrée :
			[struct simplexe*] simplex : La structure simplexe.
			[const struct modele*] model : La structure modele (en lecture seule).
			[int] born : la position d'arrêt des calculs.	
			[struct vecteurF*] ecoS : La fonction économique de la phase 1.
*/
void calculerResultat(struct simplexe* simplex, struct modele* model, int born, struct bitmap* bitS, struct vecteurF* ecoS){
	int i;
	
	for(i= 0; i < born; i++){
		if(ecoS){	//Si phase 1.
			if(i < model->posArti || ( i >= model->posArti && !bitS->V[i])){
				simplex->Res->V[i] =  simplex->Zj->V[i+1] - ecoS->V[i];
			}
		}
		else{	//Pas besoin de vérifier si les artifielles sont marqué dans la phase 2
			simplex->Res->V[i] =  simplex->Zj->V[i+1] - model->Eco->V[i]; // +1 car le zj[0] correspond au calcul avec les seconds membres
		}
	}
}

/********************************************/
/*  Les fonctions relativeà la vérification */
/********************************************/

/*
	Retourne vrai si Zj-Cj est à son optimum.
	
	Entrée :
			[struct simplexe*] simplex : La structure simplexe.
			[int] type : Type d'optimum (maximisation ou minimisation).
	Sortie :
			- 1 : Zj-Cj est à l'optimum.
			- 0 : Zj-Cj n'est pas à l'optimum.
*/
int verificationOptimum(struct simplexe *simplex, int type){
	int i = 0;
	int optimum = 1;
	
	while(optimum && i < simplex->Res->n){
			if(simplex->Res->V[i]*type < -EPSILON) 
				optimum = 0;
		i++;
	}
	return optimum;
}

/*
	Vérifie que toute les artificielles sont sorties de base
	à l'optimum.
	
	Entrée :
			[const struct modele*] model : La structure du modele.
			[struct*] bitS : La bitmap qui gére les entrées / sorties de base.
	Sortie :
			- 1 : Aucune solution a été détecté (artificielle non NULL).
			- 0 : Toute les artificielles sont sorti de base (toutes les artificiells sont égale à NULL).
*/
int verifAucuneSolution(struct modele *model, struct bitmap* bitS){
	int i, erreur = 0;
	i= model->posArti;
	while(!erreur && i < model->o){
		if(!bitS->V[i]){
			erreur = 1;
		}
		i++;
	}
	return erreur;
}

/*
	Vérifie qu'il n'éxiste pas de Zj-Cj qui n'est pas en base et qui est égale à 0.
	Entrée :
			[const struct modele*] model : La structure du modele.
			[struct*] bitS : La bitmap qui gére les entrées / sorties de base.
	Sortie :
			- 1 : Il éxiste une infinité de solution.
			- 0 : Il n'éxiste qu'une solution.
	
*/
int verifInfiniteSolutions(struct simplexe* simplex, struct bitmap* bitS){
	int i = 0, trouver = 0;
	
	while(!trouver && i < simplex->Res->n){	
		if(!bitS->V[i] && fabs(simplex->Res->V[i]) < EPSILON){
				trouver = 1;
		}
		i++;
	}
	
	return trouver;
}