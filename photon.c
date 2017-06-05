/* Nom: photon.c
 * Description: lecture et test de l'entité photon
 * Date: 21.05.2016
 * version : 3.0
 * Auteur: David Sanchez Del Rio, Frank Dupont
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "photon.h"
#include "error.h"
#include "graphic.h"
#include "utilitaire.h"
#include "constantes.h"
#include "modele.h"   
#include "absorbeur.h"
#include "reflecteur.h"

struct photon 
{
	POINT pos;
	double alpha;
	int id_refl;
	PHOTON* suivant;
};

static void vider_liste(PHOTON *p);
static bool ajouter_photon(POINT pos, double alpha);

static PHOTON *tetedeliste = NULL;
static int nb_photons = 0;
static int nb_photons_max = 0; 

//---------------------------------------------------------------------
// lecture et test du nb photons

bool photon_lecture_nb(char* ligne, int* nb_phot)
{
	*nb_phot = 0;
	if(sscanf(ligne, "%d", &nb_photons_max) != 1 || nb_photons_max < 0)
    {
	   error_lect_nb_elements(ERR_PHOTON);
	   return false;
    }
	*nb_phot = nb_photons_max;
    return true;
}

//---------------------------------------------------------------------
// lecture, test des photons

bool photon_lecture(char* ligne)
{
    POINT pos;
    double alpha = 0;
    //scane les fichiers et les mets ensuite dans la liste chainee
	if(sscanf(ligne,"%lf %lf %lf",&pos.posx,&pos.posy,&alpha) != 3)
	{
		
		error_lecture_elements(ERR_PHOTON, ERR_PAS_ASSEZ);
	    return false;
    }
    
	if(!(ajouter_photon(pos, alpha)))
	{
		return false;
	}
	
    return true;
}


/*--------------------------------------------------------------------*/
static bool ajouter_photon(POINT pos, double alpha)
{
    PHOTON* photo = (PHOTON*)calloc(1,sizeof(PHOTON));
    if(photo==NULL)
        error_msg("allocation pour générateur");
    photo->suivant = tetedeliste;
    tetedeliste = photo;
 
    photo->pos = pos;
    photo->alpha = alpha;
    photo->id_refl = -1;;
    
    nb_photons++;
    return true;
}

/*--------------------------------------------------------------------*/
int photon_info(void)
{
	return nb_photons;
}

/*--------------------------------------------------------------------*/
void photon_save(FILE *fichier)
{
	PHOTON* courant = tetedeliste;
	
	while(courant != NULL)
    {
        fprintf(fichier, "%g %g %g\n", 
				courant->pos.posx, courant->pos.posy, courant->alpha);
        courant = courant->suivant;
    }
    fprintf(fichier, "FIN_LISTE\n");
}

/*--------------------------------------------------------------------*/
void photon_dessine(void)
{
	PHOTON* elem_select = tetedeliste;
    double r=0, g=0, b=0;
     
    while(elem_select != NULL)
    {           
        graphic_set_color_3f(r, g, b);
        graphic_set_line_width(EPAISS_TRAIT);
        graphic_draw_circle(elem_select->pos.posx, elem_select->pos.posy,
											EPSIL_PROJ/2);
         
        elem_select = elem_select->suivant;
    }
}

/*--------------------------------------------------------------------*/
void photon_destruction(void)
{
	PHOTON* elem;
	
	while(tetedeliste)
	{
		elem = tetedeliste;
		vider_liste(elem);
	}
	
	nb_photons_max = 0;
	nb_photons = 0;
	
}
/*--------------------------------------------------------------------*/
void vider_liste(PHOTON *p)
{
	PHOTON* courant = tetedeliste;
    PHOTON* precedent = tetedeliste;
    
    while(courant!=p && courant)
    {
		precedent = courant;
		courant = courant->suivant;
	}
	
	if(courant)
    {
        if (courant != tetedeliste)
        {
            precedent->suivant = courant->suivant;
            free(courant);
            courant = NULL;
        }
        else
        {
            tetedeliste = courant->suivant;
            free(courant);
            courant = NULL;
        }
        nb_photons--;
    }
}

/*--------------------------------------------------------------------*/
void photon_def_limites(float *x_min, float *y_min, float *x_max, 
												float *y_max, bool *def)
{
	PHOTON* courant=tetedeliste;
	
	if(tetedeliste)
		while(courant)
		{
			float x = courant->pos.posx;
			float y = courant->pos.posy;
			
			if(*def)
			{
				*x_max = *x_min = x;
				*y_max = *y_min = y;
				*def = false;
			}
			
			*x_max = (x>*x_max) ? x : *x_max;
			*y_max = (y>*y_max) ? y : *y_max;
			*x_min = (x<*x_min) ? x : *x_min;
			*y_min = (y<*y_min) ? y : *y_min;
			
			courant = courant->suivant;
		}
}	

/*--------------------------------------------------------------------*/
//effetue la simulation pour chaque photon
void photon_sim(void)
{
    PHOTON* temp = tetedeliste;
     
    while(temp)
    {
        photon_inter(temp,-1,VPHOT*DELTA_T);
        temp = temp->suivant;
    }
}

/*--------------------------------------------------------------------*/
void photon_inter(PHOTON* temp, int prec, float l)
{
    VECTEUR s_p, s_a, s_r;
    POINT pi_a, pi_r, pi_temp;
    int i_a=-1, j_a=-1, i_r=-1;
    int i,j;
    float d1,d2,di;
    POINT t, u, n, w, pi;
    pi_a.posx = INF;
    pi_a.posy = INF;
    pi_r.posx = INF;
    pi_r.posy = INF;
    s_p.deb.posx = temp->pos.posx;
    s_p.deb.posy = temp->pos.posy;
    s_p.fin.posx = temp->pos.posx + l*cos(temp->alpha);
    s_p.fin.posy = temp->pos.posy + l*sin(temp->alpha);
    for(i=0;i<absorbeur_info();i++)
        for(j=0;j<get_nbp(i)-1;j++)
        {
            s_a = get_abs(i,j);
            if(modele_veri(s_p,s_a, &pi))
            {
                pi_temp = calc_inter(s_p,s_a);
                d1 = distance2(pi_temp,temp->pos);
                d2 = distance2(pi_a,temp->pos);
                if(d1<d2)
                {
                    pi_a.posx = pi_temp.posx;
                    pi_a.posy = pi_temp.posy;
                    i_a = i;
                    j_a = j;
                }
            }
        }            
    for(i=0;i<reflecteur_info();i++)
        if(i!=prec && i!=temp->id_refl)
        {
            s_r = reflecteur_get_vecteur(i);
            if(modele_veri(s_p,s_r, &pi))
            {
                pi_temp = calc_inter(s_p,s_r);
                d1 = distance2(pi_temp,temp->pos);
                d2 = distance2(pi_r,temp->pos);
                if(d1<d2)
                {
                    u.posx = (s_r.fin.posx - s_r.deb.posx)/
														norme_vect(s_r);
                    u.posy = (s_r.fin.posy - s_r.deb.posy)/
														norme_vect(s_r);
                    pi_r.posx = pi_temp.posx;
                    pi_r.posy = pi_temp.posy;
                    i_r = i;
                }
            }
        }
    d1 = distance2(pi_a,temp->pos);
    d2 = distance2(pi_r,temp->pos);
    if(i_a==-1 && j_a == -1 && i_r == -1)
    {
        temp->pos.posx = temp->pos.posx + l*cos(temp->alpha);
        temp->pos.posy = temp->pos.posy + l*sin(temp->alpha);
    }
    else if(d1<d2)
        vider_liste(temp);
    else
    {
        di = l-d2;
        t.posx = l*cos(temp->alpha);
        t.posy = l*sin(temp->alpha);
        n.posx = u.posy;
        n.posy = -u.posx;
        if(produit_scalaire(t,n)>0)
        {
            n.posx = -n.posx;
            n.posy = -n.posy;
        } 
        w.posx = t.posx - 2*produit_scalaire(t,n)*n.posx;
        w.posy = t.posy - 2*produit_scalaire(t,n)*n.posy;
        temp->id_refl = i_r;
        temp->pos.posx = temp->pos.posx + d2*cos(temp->alpha);
        temp->pos.posy = temp->pos.posy + d2*sin(temp->alpha);
        if(w.posx>=0)
            temp->alpha = atan(w.posy/w.posx);
        else
            temp->alpha = M_PI + atan(w.posy/w.posx);
        photon_inter(temp,i_r,di);
    }
}

/*--------------------------------------------------------------------*/
PHOTON* get_tete_phot(void)
{
	return tetedeliste;
}

/*--------------------------------------------------------------------*/
void projection_photon(double projx, double projy, double angle)
{
	PHOTON nouveau;
	int i = 0;
	
	for(i=0;i<NBPH;i++)
	{
		//donne les coordonnées initiales des photons (avant proj)
		nouveau.pos.posx = projx + i*EPSIL_PROJ*cos(angle+M_PI/2);
		nouveau.pos.posy = projy + i*EPSIL_PROJ*sin(angle+M_PI/2);
		nouveau.alpha = angle;
		
		ajouter_photon(nouveau.pos, nouveau.alpha);
	}
}

/*--------------------------------------------------------------------*/
PHOTON* creation_photon(PHOTON * tetedeliste, POINT position, 
														double alpha)
{
	PHOTON* nouveau=NULL;
	if (!( nouveau = (PHOTON*) malloc (sizeof(PHOTON)))) 
    {
        printf("Pb d'allocation dans %s\n",__func__);
        exit(EXIT_FAILURE);
    }
    
    nouveau->pos = position;
    nouveau->alpha = alpha;
    nouveau->suivant = tetedeliste;
    return nouveau;
}

/*--------------------------------------------------------------------*/
void photon_transfert(POINT pos, double alpha)
{
    ajouter_photon(pos,alpha);
}

/*--------------------------------------------------------------------*/
void photon_beyond(float x_min, float x_max, float y_min, float y_max)
{
	PHOTON* temp = tetedeliste;
	
	while(temp)
	{
		if(!(temp->pos.posx>=x_min && temp->pos.posx<=x_max &&
							temp->pos.posy>=y_min && 
												temp->pos.posy<=y_max))
			vider_liste(temp);
		
		temp = temp->suivant;
	}
								
}
