/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * Mantra: Man page bookmarker                                           *
 * Copyright (C) 2015  Tom Pickering                                     *
 * Code in this file derived from code (C) 2015 Michael Kerrisk          *
 *                                                                       *
 * This program is free software: you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                       *
\* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "layout.h"

#include <stdlib.h>

Layout* new_layout() {
    /* Zeroed memory; ncols = 0, cols = NULL */
    return (Layout *)calloc(1, sizeof(Layout));
}

void add_column(Layout *l, unsigned int weight, unsigned int max_w) {
    l->ncols++;
    if (!l->cols)
        l->cols = (Column *)malloc(sizeof(Column));
    else
        l->cols = (Column *)realloc(l->cols, l->ncols * sizeof(Column));

    l->cols[l->ncols - 1].weight = weight;
    l->cols[l->ncols - 1].max_width = max_w;
}

/* Compute column x-positions and widths based on a layout.
 * Pass in a layout and total width, along with buffers
 * in which to write each pair of column parameters.
 */
void get_field_attrs(Layout *l, unsigned int width_total, unsigned int **x, unsigned int **w) {
    unsigned int nspaces = l->ncols + 1;
    unsigned int *w_col_widths;
    unsigned int total_weight = 0;
    unsigned int col_x = 1;
    unsigned int i;
    Column* c;

              *x = (unsigned int *)calloc(l->ncols, sizeof(unsigned int));
              *w = (unsigned int *)calloc(l->ncols, sizeof(unsigned int));
    w_col_widths = (unsigned int *)calloc(l->ncols, sizeof(unsigned int));

    for (i = 0; i < l->ncols; ++i) {
        c = &(l->cols[i]);
        w_col_widths[i] = (width_total - nspaces) * c->weight;
        total_weight += c->weight;
    }

    for (i = 0; i < l->ncols; ++i) {
        (*x)[i] = col_x;
        (*w)[i] = (unsigned int)(((double)w_col_widths[i] / (double)total_weight) + 0.5);
        col_x += (*w)[i];
        col_x++; /* Account for spaces */
    }

    free(w_col_widths);
}
