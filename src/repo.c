/* repo.c -- This file is part of DPM.
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

#include <sys/utsname.h>
#include <errno.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xalloc.h>

static char **repo_list;
static size_t repo_count;
static int sys_version;

static void
get_sys_version (void)
{
  struct utsname buffer;
  if (uname (&buffer) == -1)
    error (1, errno, "failed to determine system version");
  sys_version = strtol (buffer.release, NULL, 10);
}

void
repo_load (void)
{
  FILE *file = fopen (REPO_LIST_PATH, "r");
  char *line = NULL;
  size_t len = 0;
  if (!file)
    error (1, errno, "failed to open repository list: " REPO_LIST_PATH);
  get_sys_version ();
  while (getline (&line, &len, file) != EOF)
    {
      size_t i = strlen (line) - 1;
      if (line[i] == '\n')
	line[i] = '\0';
      repo_count++;
      repo_list = xrealloc (repo_list, sizeof (char *) * repo_count);
      repo_list[repo_count - 1] = xstrdup (line);
    }
  fclose (file);
  free (line);
}