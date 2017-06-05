/* Nom: graphic.h
 * Description: module qui affiche un segment, un rectangle ou un cercle
 * Date: 21.05.2016
 * version : 3.0
 * Auteur: David Sanchez Del Rio, Frank Dupont
 */

#include "utilitaire.h" 

#ifndef GRAPHIC_H
#define GRAPHIC_H

void graphic_draw_segment(double x1, double y1, double x2, double y2);

void graphic_draw_circle(float xc, float yc, float r);

void graphic_draw_rect(POINT last_mouse, POINT mouse);

void graphic_set_color_3f(float r, float g, float b);

void graphic_set_line_width(float width);

#endif
