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
#include <xalloc.h>
#include "package.h"
#include "version.h"

static const struct option longopts[] = {
  {"arch", required_argument, NULL, 'a'},
  {"build", no_argument, NULL, 'b'},
  {"configure", no_argument, NULL, 'c'},
  {"dist", no_argument, NULL, 'd'},
  {"help", no_argument, NULL, 'h'},
  {"install", no_argument, NULL, 'i'},
  {"output", required_argument, NULL, 'o'},
  {"sysversion", required_argument, NULL, 's'},
  {"version", no_argument, NULL, 'v'},
  {"clean", no_argument, NULL, 'x'},
  {"distclean", no_argument, NULL, 'X'},
  {NULL, 0, NULL, 0}
};

static void
error_prefix (void)
{
  fprintf (stderr, "\033[31;1mError:\033[0m ");
}

static void
usage (void)
{
  fprintf (stderr, "Usage: dpm [-i] PACKAGES\n"
	   "       dpm [-a ARCH] [-s VERSION] [-o DIR] [-bcdhvxX]\n");
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
  int err = 0;
  int opt;
  int i;
  error_print_progname = error_prefix;
  if (argc == 1)
    {
      usage ();
      exit (1);
    }
  default_version ();
  default_arch ();
  while ((opt = getopt_long (argc, argv, "a:bcdhio:s:vxX", longopts, NULL))
	 != -1)
    {
      switch (opt)
	{
	case 'a':
	  strncpy (arch, optarg, sizeof (arch) - 1);
	  break;
	case 'b':
	  pkg_option = PKG_BUILD;
	  break;
	case 'c':
	  pkg_option = PKG_CONFIGURE;
	  break;
	case 'd':
	  pkg_option = PKG_DIST;
	  break;
	case 'h':
	  usage ();
	  exit (0);
	case 'i':
	  pkg_option = PKG_INSTALL;
	  break;
	case 'o':
	  output_dir = optarg;
	  break;
	case 's':
	  parse_version (optarg);
	  break;
	case 'v':
	  version ();
	  exit (0);
	case 'x':
	  pkg_option = PKG_CLEAN;
	  break;
	case 'X':
	  pkg_option = PKG_DISTCLEAN;
	  break;
	default:
	  err = 1;
	}
    }
  if (err)
    exit (1);
  if (!output_dir || !*output_dir)
    output_dir = ".";

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
    case PKG_BUILD:
      pkg_build ();
      break;
    case PKG_CONFIGURE:
      pkg_configure ();
      break;
    case PKG_DIST:
      pkg_dist ();
      break;
    case PKG_INSTALL:
      pkg_install ();
      break;
    case PKG_CLEAN:
      pkg_clean ();
      break;
    case PKG_DISTCLEAN:
      pkg_distclean ();
      break;
    }
  return 0;
}
