/* list.c -- This file is part of DPM.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <errno.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xalloc.h>
#include "list.h"

#define INSTALLED_LIST LIST_DIR "/installed"

struct pkg_list installed_pkgs;

void
load_installed_pkgs (void)
{
  FILE *file = fopen (INSTALLED_LIST, "r");
  char *line = NULL;
  size_t len = 0;
  ssize_t size;
  if (!file)
    return;
  while ((size = getline (&line, &len, file)) != EOF)
    {
      char *equals;
      if (line[size - 1] == '\n')
	line[size - 1] = '\0';
      equals = strchr (line, '=');
      if (!equals)
	continue;
      *equals = '\0';
      installed_pkgs.len++;
      installed_pkgs.names =
	xrealloc (installed_pkgs.names, sizeof (char *) * installed_pkgs.len);
      installed_pkgs.versions =
	xrealloc (installed_pkgs.versions,
		  sizeof (char *) * installed_pkgs.len);
      installed_pkgs.names[installed_pkgs.len - 1] = xstrdup (line);
      installed_pkgs.versions[installed_pkgs.len - 1] = xstrdup (equals + 1);
    }
  fclose (file);
  free (line);
}

char *
check_pkg_installed (const char *name)
{
  size_t i;
  for (i = 0; i < installed_pkgs.len; i++)
    {
      if (!strcmp (installed_pkgs.names[i], name))
	return installed_pkgs.versions[i];
    }
  return NULL;
}

void
mark_pkg_installed (const struct package *package)
{
  installed_pkgs.len++;
  installed_pkgs.names =
    xrealloc (installed_pkgs.names, sizeof (char *) * installed_pkgs.len);
  installed_pkgs.versions =
    xrealloc (installed_pkgs.versions, sizeof (char *) * installed_pkgs.len);
  installed_pkgs.names[installed_pkgs.len - 1] = xstrdup (package->name);
  installed_pkgs.versions[installed_pkgs.len - 1] = xstrdup (package->version);
}

void
save_pkg_install_list (void)
{
  FILE *file = fopen (INSTALLED_LIST, "w");
  size_t i;
  if (!file)
    error (1, errno, "failed to update installed package list");
  for (i = 0; i < installed_pkgs.len; i++)
    fprintf (file, "%s=%s\n", installed_pkgs.names[i],
	     installed_pkgs.versions[i]);
  fclose (file);
}
