/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\
 * Mantra: Man page bookmarker                                           *
 * Copyright (C) 2015  Tom Pickering                                     *
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

#ifndef GUARD_LAYOUT_H
#define GUARD_LAYOUT_H

typedef struct _Column Column;
typedef struct _Layout Layout;

/* Parameters defining a column in the context of a dynamic space.
 * The column's width will be computed as a relative weight
 * compared to the total width available, capped at max_width.
 */
struct _Column {
    unsigned int weight;
    unsigned int max_width;
};

struct _Layout {
    Column *cols;
    unsigned int ncols;
};

Layout* new_layout();
void add_column(Layout *, unsigned int, unsigned int);
void get_field_attrs(Layout *, unsigned int, unsigned int **, unsigned int **);

#endif
