/* Nom: absorbeur.c
 * Description: lecture et test de l'entité absorbeur
 * Date: 22.05.2016
 * version : 3.0
 * Auteur: David Sanchez Del Rio, Frank Dupont
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "absorbeur.h"
#include "error.h"
#include "utilitaire.h"
#include "constantes.h"
#include "graphic.h"



struct absorbeur 
{
	int nbp;
	POINT tab_pt[MAX_PT];
	ABSORBEUR* suivant;
	
};

static bool ajouter_absorbeur(int nbp, POINT tab_pt[MAX_PT]);
static void vider_liste(ABSORBEUR *p);

static ABSORBEUR *tetedeliste = NULL;
static ABSORBEUR* selec = NULL;
static int nb_absorbeurs = 0; 
static int nb_absorbeurs_max = 0;
static POINT tab_pt_abs[MAX_PT];

//---------------------------------------------------------------------
// lecture et test du nb absorbeurs

bool absorbeur_lecture_nb(char* ligne, int* nb_abso)
{
	*nb_abso = 0;
	if(sscanf(ligne, "%d", &nb_absorbeurs_max) != 1 ||
				  nb_absorbeurs_max < 0)
    {
	   error_lect_nb_elements(ERR_ABSORBEUR);
	   return false;
    }
	*nb_abso = nb_absorbeurs_max;
    return true;	
}

//---------------------------------------------------------------------
// lecture, test des absorbeurs

bool absorbeur_lecture(char *ligne)
{
	int i = 0;
	ABSORBEUR* nouveau=NULL;
	//scane les fichiers et les mets dans la liste chainee
    if (!( nouveau = (ABSORBEUR*) malloc (sizeof(ABSORBEUR))))   
    {
        printf("Pb d'allocation dans %s\n",__func__);
        return false;
    }
    int compteur = 0;
    char *deb = ligne, *fin = NULL;
    strtol(deb,&fin,0);
    deb = fin;
    if(sscanf(ligne, "%d", &nouveau->nbp) != 1 || nouveau->nbp < 2 ||
													nouveau->nbp > 6)
    {
		error_lect_nb_points_absorbeur();
		return false;
	}
    for(i=0; i < nouveau->nbp && i< MAX_PT; i++)
    {
		if(sscanf(deb,"%lf %lf",
             &(nouveau->tab_pt[i].posx),
             &(nouveau->tab_pt[i].posy)) != 2)
        {
			error_lecture_elements(ERR_ABSORBEUR, ERR_PAS_ASSEZ);
			return false;
		}
		strtod(deb,&fin);
		deb = fin;
		strtod(deb,&fin);
		deb = fin;
		compteur++;
	}
	if(compteur != nouveau->nbp)
	{
		error_lecture_elements(ERR_ABSORBEUR, ERR_PAS_ASSEZ);
	    return false;
    }
    

    
//---------------------------------------------------------------------
//vérifie le nombre de points d'un absorbeur

    if(nouveau->nbp < 2 ||    //checker ça demain
       nouveau->nbp > MAX_PT)
    {
		error_lect_nb_points_absorbeur();
		return false;
	}

//---------------------------------------------------------------------
//vérifie la distance entre deux points d'un aborbeur

    for(i = 1; i < nouveau->nbp; i++)
    {
        if(distance2(nouveau->tab_pt[i],
                     nouveau->tab_pt[i-1] ) <= EPSIL_CREATION)
        {
            error_lecture_point_trop_proche(ERR_ABSORBEUR, nb_absorbeurs);
            return false;
        }
    }
    
    if(!(ajouter_absorbeur(nouveau->nbp,nouveau->tab_pt)))
    {
		return false;
	}
	
	nb_absorbeurs++;
    return true;	
}

/*--------------------------------------------------------------------*/
static bool ajouter_absorbeur(int nbp, POINT tab_pt[MAX_PT])
{
	int i=0;
    ABSORBEUR* nouveau = NULL;
    if (!( nouveau = (ABSORBEUR*) malloc (sizeof(ABSORBEUR))))
    {
        printf("Pb d'allocation dans %s\n",__func__);
        return false;
    }
    
    nouveau->nbp = nbp;
    
    for(i = 0; i < nouveau->nbp; i++)
	{
		nouveau->tab_pt[i].posx = tab_pt[i].posx; 
		nouveau->tab_pt[i].posy = tab_pt[i].posy;
	}
	 
    nouveau->suivant = tetedeliste; 
    tetedeliste=nouveau;
    return true; 
}

/*--------------------------------------------------------------------*/
//retourne le nombre d'absorbeurs
int absorbeur_info()
{
	return nb_absorbeurs;
}
 
/*--------------------------------------------------------------------*/ 
//sauvegarde les données des absorbeurs  
void absorbeur_save(FILE *fichier)
{
	ABSORBEUR* courant = tetedeliste;
	int i = 0;
	
	while(courant != NULL)
    {
		fprintf(fichier,"%d ",courant->nbp);
		
		for(i = 0; i < courant->nbp; i++)
		{
			
			fprintf(fichier,"%g %g ", 
							courant->tab_pt[i].posx,
											courant->tab_pt[i].posy);
		}
		courant = courant->suivant;
    }
    fprintf(fichier, "\nFIN_LISTE\n");
}

/*--------------------------------------------------------------------*/
void absorbeur_dessine_tout(void)
{
	ABSORBEUR* courant = tetedeliste;
	
	if(tetedeliste)
		while(courant)
		{
			absorbeur_dessine(courant);
			courant = courant->suivant;
		}
}

/*--------------------------------------------------------------------*/
//dessine les absorbeurs
void absorbeur_dessine(ABSORBEUR* p)
{	
	int i=0;
	
	graphic_set_line_width(2*EPAISS_TRAIT);
	if(p==selec)
		 graphic_set_color_3f(1,1,0);
	else
		graphic_set_color_3f(0,0,1);
	
	for(i=1;i<p->nbp;i++)
	{
		graphic_draw_segment(p->tab_pt[i-1].posx, p->tab_pt[i-1].posy,
								p->tab_pt[i].posx, p->tab_pt[i].posy);
	}
}

/*--------------------------------------------------------------------*/
//détruit les absorbeurs
void absorbeur_destruction(void)
{
	ABSORBEUR* elem = tetedeliste;
	
	while(elem)
	{
		vider_liste(elem);
		elem = elem->suivant;
	}
	
	nb_absorbeurs_max = 0;
	nb_absorbeurs = 0;
	
}

/*--------------------------------------------------------------------*/
void absorbeur_def_limites(float *x_min, float *y_min, float *x_max, 
												float *y_max, bool *def)
{
	int i=0;
	ABSORBEUR* courant=tetedeliste;
	
	if(tetedeliste)
		while(courant)
		{
			float xM = courant->tab_pt[0].posx;
			float yM = courant->tab_pt[0].posy;
			float xm = xM;
			float ym = yM;
			
			if(*def)
			{
				*x_max = *x_min = xM;
				*y_max = *y_min = yM;
				*def = false;
			}
			
			for(i=0;i<courant->nbp;i++)
			{
				xM = (courant->tab_pt[i].posx>xM) ? 
											courant->tab_pt[i].posx : xM;
				xm = (courant->tab_pt[i].posx<xm) ? 
											courant->tab_pt[i].posx : xm;
				yM = (courant->tab_pt[i].posy>yM) ? 
											courant->tab_pt[i].posy : yM;
				ym = (courant->tab_pt[i].posy<ym) ? 
											courant->tab_pt[i].posy : ym;
			}
			
			*x_max = (xM>*x_max) ? xM : *x_max;
			*y_max = (yM>*y_max) ? yM : *y_max;
			*x_min = (xm<*x_min) ? xm : *x_min;
			*y_min = (ym<*y_min) ? ym : *y_min;
			
			courant = courant->suivant;
		}
	
}

/*--------------------------------------------------------------------*/
//renvoie un point qui va former un vecteur avec un autre
POINT get_point_abs(int i, int j)
{
	POINT retour_point;
	ABSORBEUR* courant = tetedeliste;
	int a = 0;
	for(a=0; a != i; a++) //on arrive au ième absorbeur noté courant
		courant=courant->suivant;
		
	retour_point = courant->tab_pt[j];
	
	return retour_point;
	
}

/*--------------------------------------------------------------------*/
VECTEUR get_abs(int i, int j)
{
	VECTEUR get_abs;
	POINT premier = get_point_abs(i, j);
	POINT deuxieme = get_point_abs(i, j+1);
	
	get_abs.deb.posx = premier.posx;
	get_abs.deb.posy = premier.posy;
	get_abs.fin.posx = deuxieme.posx;
	get_abs.fin.posy = deuxieme.posy;
	
	return get_abs;
}

/*--------------------------------------------------------------------*/
ABSORBEUR* get_pointeur_abs(ABSORBEUR* courant) 
{
	ABSORBEUR* temp = courant;
	
	return temp->suivant;
}

/*--------------------------------------------------------------------*/
int get_nbp(int a)
{
	ABSORBEUR* temp = tetedeliste;
	int i = 0;
	
	while(i!=a)
	{
		temp = temp->suivant;
		i++;
	}
	
	return temp->nbp;
	
}

/*--------------------------------------------------------------------*/
static void vider_liste(ABSORBEUR *p)
{
	ABSORBEUR* courant = tetedeliste;
    ABSORBEUR* precedent = tetedeliste;
    
    while(courant !=p && courant)
    {
		precedent = courant;
		courant = courant->suivant;
	}
	
	if(courant != NULL)
    {
        if (courant != tetedeliste)
        {
            precedent -> suivant = courant -> suivant;
            free (courant);
        }
        else
        {
            tetedeliste = courant -> suivant;
            free(courant);
        }
        nb_absorbeurs--;
    }
}

/*--------------------------------------------------------------------*/
ABSORBEUR* get_tete_abs(void)
{
	return tetedeliste;
}

/*--------------------------------------------------------------------*/
void absorbeur_creation(POINT souris_p, POINT souris, int* compteur)
{
	//créer les 2 premiers points de l'absorbeur
	if(*compteur == 0)
	{
		tab_pt_abs[*compteur].posx=souris_p.posx;
		tab_pt_abs[*compteur].posy=souris_p.posy;
		tab_pt_abs[*compteur+1].posx=souris.posx;
		tab_pt_abs[*compteur+1].posy=souris.posy;
		ajouter_absorbeur(2, tab_pt_abs);
	}
	//créer si l'absorbeur a plus de deux point
	if(*compteur > 0 && *compteur < MAX_PT)
	{
		tab_pt_abs[*compteur].posx=souris_p.posx;
		tab_pt_abs[*compteur].posy=souris_p.posy;
		tetedeliste->tab_pt[*compteur+1].posx = souris.posx;
		tetedeliste->tab_pt[*compteur+1].posy = souris.posy;
		tetedeliste->nbp+=1;
		
	}
	souris_p=souris;
	
	//lorsque qu'on atteint le nombre max de point :
	*compteur=*compteur+1;
	if(*compteur == MAX_PT-1) 
		*compteur = 0;

}

/*--------------------------------------------------------------------*/
void absorbeur_selection(POINT souris)
{
	int i=0;
	double dist_inf=0, distance = INF;
	double dist_deb=INF, dist_fin=INF;
	ABSORBEUR* temp = tetedeliste;
	double a=0, b=0;
	
    while(temp)
    {
		for(i=0; i<temp->nbp;i++)
		{
			a = distance2(temp->tab_pt[i], souris);
			if(a<dist_deb) dist_deb=a;
		}
		for(i=0; i<temp->nbp;i++)
		{
			b = distance2(temp->tab_pt[i], souris);
			if(b<dist_fin) dist_fin=b;			
		}
		if(dist_deb<dist_fin)
			dist_inf = dist_deb;
			
		else dist_inf = dist_fin;
		
		if(dist_inf<distance)
		{
			selec = temp;
			distance = dist_inf;
		}
		
		temp = temp->suivant;	
    }	
    absorbeur_dessine(selec);
}

/*--------------------------------------------------------------------*/
//supprime l'élement selectionner
void absorbeur_suppression_selec(void)
{
	vider_liste(selec);
	
	selec = NULL;
}

/*--------------------------------------------------------------------*/
// enlève la tete de liste pour l'absorbeur si intersection
void enlever_tete_abs(void)
{
	ABSORBEUR* old = tetedeliste;
	tetedeliste=tetedeliste->suivant;
	
	free(old);
	old=NULL;
	
}
