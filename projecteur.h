/* Nom: projecteur.h
 * Description: lecture et test de l'entité projecteur
 * Date: 21.05.2016
 * version : 3.0
 * Auteur: David Sanchez Del Rio, Frank Dupont
 */
 
#include <stdio.h>
#include <stdbool.h>
#include "utilitaire.h" 
 
#ifndef PROJECTEUR_H
#define PROJECTEUR_H

typedef struct projecteur PROJECTEUR;

bool projecteur_lecture_nb(char* ligne, int* nb_proj);

bool projecteur_lecture(char* ligne);

int projecteur_info(void);

void projecteur_save(FILE *fichier);

void projecteur_destruction(void);

void projecteur_dessine(PROJECTEUR* p);

void projecteur_dessine_tout(void);

void projecteur_def_limites(float *x_min, float *y_min, float *x_max, 
												float *y_max, bool *def);
		
void projecteur_sim(void);

VECTEUR get_proj(int i);

PROJECTEUR* get_pointeur_proj(PROJECTEUR* courant);

PROJECTEUR* get_tete_proj(void);

void projecteur_creation(POINT souris_p, POINT souris);

void projecteur_selection(POINT souris);

void projecteur_suppression_selec(void);

void enlever_tete_proj(void);

#endif
