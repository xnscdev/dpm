/* version.c -- This file is part of DPM.
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
#include "version.h"

char macos_version[16];
char arch[16];
int kernel_version;

void
parse_version (const char *arg)
{
  int major = strtol (arg, NULL, 10);
  if (major > 10)
    {
      kernel_version = major + 9;
      sprintf (macos_version, "%d", major);
    }
  else if (major == 10)
    {
      int minor = strtol (arg + 3, NULL, 10);
      if (minor > 15)
	error (1, 0, "bad minor version number: %d", minor);
      kernel_version = minor + 4;
      sprintf (macos_version, "%d.%d", 10, minor);
    }
  else
    error (1, 0, "bad system version string: %s", arg);
}

void
default_version (void)
{
  struct utsname buffer;
  int major;
  int minor;
  if (uname (&buffer) == -1)
    error (1, errno, "uname()");
  sscanf (buffer.release, "%d.%d", &major, &minor);
  kernel_version = major;
  if (major >= 20)
    sprintf (macos_version, "%d", major - 9);
  else
    sprintf (macos_version, "%d.%d", 10, major - 4);
}

void
default_arch (void)
{
  struct utsname buffer;
  if (uname (&buffer) == -1)
    error (1, errno, "uname()");
  strcpy (arch, buffer.machine);
}
