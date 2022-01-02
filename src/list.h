/* list.h -- This file is part of DPM.
   Copyright (C) 2021 XNSC

   DPM is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   DPM is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with DPM. If not, see <https://www.gnu.org/licenses/>. */

#ifndef __LIST_H
#define __LIST_H

#include <stddef.h>

struct pkg_list
{
  char **names;
  char **versions;
  size_t len;
};

extern struct pkg_list installed_pkgs;

void load_installed_pkgs (void);
char *check_pkg_installed (const char *name);

#endif
