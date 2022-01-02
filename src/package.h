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
  PKG_INSTALL,
  PKG_CONFIGURE,
  PKG_BUILD,
  PKG_CLEAN,
  PKG_DISTCLEAN,
  PKG_DIST
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
  char *extract_dir;
};

struct package_stack
{
  struct package **packages;
  size_t len;
};

extern struct package_req **packages;
extern struct package_stack package_stack;
extern char *output_dir;
extern int sys_version;

void remove_dir (const char *dir);

void pkg_extract (struct package *package);
char *pkg_archive_data (char *destdir);
void pkg_archive (char *data_path);

void pkg_process_req (struct package_req *req);
void pkg_resolve_dependencies (const char *name);
void pkg_stack_insert (struct package *package);
void pkg_stack_install (void);

void pkg_install (void);
void pkg_configure (void);
void pkg_build (void);
void pkg_clean (void);
void pkg_distclean (void);
void pkg_postinstall (void);
void pkg_dist (void);

#endif
