/* install.c -- This file is part of DPM.
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

#include <error.h>
#include <stdlib.h>
#include "list.h"
#include "package.h"
#include "repo.h"

void
pkg_install (void)
{
  int i;
  if (!*packages)
    error (1, 0, "no packages specified");
  repo_load ();
  load_installed_pkgs ();
  for (i = 0; packages[i]; i++)
    {
      pkg_process_req (packages[i]);
      free (packages[i]->name);
      free (packages[i]->version);
      free (packages[i]);
    }
  pkg_stack_install ();
  save_pkg_install_list ();
}
