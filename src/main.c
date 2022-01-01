/* main.c -- This file is part of DPM.
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
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <xalloc.h>
#include "package.h"

static const struct option longopts[] = {
  {"build", no_argument, NULL, 'b'},
  {"configure", no_argument, NULL, 'c'},
  {"help", no_argument, NULL, 'h'},
  {"install", no_argument, NULL, 'i'},
  {"version", no_argument, NULL, 'v'},
  {"clean", no_argument, NULL, 'x'},
  {"distclean", no_argument, NULL, 'X'},
  {NULL, 0, NULL, 0}
};

static void
error_prefix (void)
{
  if (isatty (STDERR_FILENO))
    fprintf (stderr, "\033[31;1mError:\033[0m ");
  else
    fprintf (stderr, "Error: ");
}

static void
usage (void)
{
  fprintf (stderr, "Usage: dpm [-bcixX] PACKAGES\n       dpm [-h | -v]\n");
}

static void
version (void)
{
  fprintf (stderr, "DPM " PACKAGE_VERSION "\n");
}

int
main (int argc, char **argv)
{
  enum pkg_op pkg_option = PKG_INSTALL;
  int opt;
  int i;
  error_print_progname = error_prefix;
  if (argc == 1)
    {
      usage ();
      exit (1);
    }
  while ((opt = getopt_long (argc, argv, "bchivxX", longopts, NULL)) != -1)
    {
      switch (opt)
	{
	case 'b':
	  pkg_build ();
	  exit (0);
	case 'c':
	  pkg_configure ();
	  exit (0);
	case 'h':
	  usage ();
	  exit (0);
	case 'i':
	  pkg_option = PKG_INSTALL;
	  break;
	case 'v':
	  version ();
	  exit (0);
	case 'x':
	  pkg_clean ();
	  exit (0);
	case 'X':
	  pkg_distclean ();
	  exit (0);
	}
    }

  argc -= optind;
  argv += optind;
  packages = xcalloc (argc + 1, sizeof (struct package_req *));
  for (i = 0; i < argc; i++)
    {
      char *package = xstrdup (argv[i]);
      char *equals = strchr (package, '=');
      packages[i] = xcalloc (1, sizeof (struct package_req));
      packages[i]->name = package;
      if (equals != NULL)
	packages[i]->version = equals + 1;
    }

  switch (pkg_option)
    {
    case PKG_INSTALL:
      pkg_install ();
      break;
    }
  return 0;
}
