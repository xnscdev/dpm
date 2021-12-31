/* package.h -- This file is part of DPM.
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

#ifndef __PACKAGE_H
#define __PACKAGE_H

#include <stdio.h>

enum pkg_op
{
  PKG_INSTALL
};

struct package_req
{
  char *name;
  char *version;
};

struct package
{
  char *name;
  char *version;
  FILE *archive;
};

struct package_stack
{
  struct package **packages;
  size_t len;
};

extern struct package_req **packages;
extern struct package_stack package_stack;

void pkg_extract (struct package *package);
void pkg_install (void);

#endif
