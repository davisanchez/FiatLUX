/* Nom: modele.h
 * Description: module qui gère les opérations de lecture, écriture etc.
 * Date: 21.05.2016
 * version : 3.0
 * Auteur: David Sanchez Del Rio, Frank Dupont
 */

#ifndef MODELE_H
#define MODELE_H

#include "utilitaire.h"
#include "photon.h"
#include <stdbool.h>

typedef struct pint PINT;

enum Mode {MODE_DEFAULT, MODE_ERROR, MODE_VERIF, MODE_GRAPHIC,
															MODE_FINAL};

void modele_lecture(int mode, char* nom_fichier);

int modele_lecture_fichier(char* nom_fichier);

void modele_info(int* nb_phot, int* nb_proj, int* nb_abso, int* nb_refl);

void modele_save(char* nom_fichier);

bool modele_load(char *nom_fichier);

void modele_dessine(void);

void modele_destruction(void);

void sim_update(void);

void modele_def_limites(float *x_min, float *y_min, float *x_max,
														float *y_max);

void modele_set_mouse(POINT souris);

bool modele_veri(VECTEUR s1, VECTEUR s2, POINT* pi);

POINT calc_inter(VECTEUR s1, VECTEUR s2);

int nb_pi(VECTEUR phot);

POINT vecteur_reflexion(POINT pdi, VECTEUR photo, VECTEUR refl);

POINT point_intersection(VECTEUR s1, POINT u1, POINT u2, POINT vd1d2);

void modele_selection(int a);

void modele_efface_selection(int a);

void modele_beyond(float x_min, float x_max, float y_min, float y_max);

bool modele_verification_rendu2(void);

#endif
