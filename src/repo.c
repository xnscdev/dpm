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

#include <errno.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xalloc.h>
#include "download.h"
#include "package.h"
#include "repo.h"

#define REPO_LIST_PATH CONFIG_DIR "/repo"

static char **repo_list;
static size_t repo_count;

void
repo_load (void)
{
  FILE *file = fopen (REPO_LIST_PATH, "r");
  char *line = NULL;
  size_t len = 0;
  if (!file)
    error (1, errno, "failed to open repository list: " REPO_LIST_PATH);
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

struct package *
repo_search_package (struct package_req *req)
{
  struct package *package;
  size_t i;
  FILE *archive = tmpfile ();
  if (!archive)
    return NULL;
  for (i = 0; i < repo_count; i++)
    {
      char *url;
      CURLcode ret;
      if (!req->version)
	{
	  char *latesturl;
	  char *buffer;
	  size_t len;
	  asprintf (&latesturl, "%s/%s/LATEST", repo_list[i], req->name);
	  ret = download_to_buffer ((void **) &buffer, latesturl);
	  free (latesturl);
	  if (ret != CURLE_OK)
	    {
	      fclose (archive);
	      return NULL;
	    }
	  len = strlen (buffer) - 1;
	  if (buffer[len] == '\n')
	    buffer[len] = '\0';
	  req->version = buffer;
	}

      asprintf (&url, "%s/%s/%s-%s.dpm", repo_list[i], req->name,
		req->name, req->version);
      ret = download_to_file (archive, url);
      free (url);
      if (ret != CURLE_OK)
	{
	  fclose (archive);
	  return NULL;
	}
      rewind (archive);

      package = xcalloc (1, sizeof (struct package));
      package->name = xstrdup (req->name);
      package->version = xstrdup (req->version);
      package->archive = archive;
      return package;
    }
  return NULL;
}
