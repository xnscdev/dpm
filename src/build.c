/* build.c -- This file is part of DPM.
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

#include <sys/stat.h>
#include <errno.h>
#include <error.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <xalloc.h>
#include "package.h"
#include "repo.h"
#include "version.h"

#define DPM_MAKE_DIR DATA_DIR "/make"

static void
touch (const char *path)
{
  close (open (path, O_WRONLY | O_CREAT | O_TRUNC,
	       S_IROTH | S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH));
}

static void
run_makefile_task_file (const char *target, const char *makefile)
{
  pid_t pid = fork ();
  int status;
  int ret;
  if (pid == -1)
    error (1, errno, "fork()");
  else if (!pid)
    {
      char *macos_version_flag;
      char *kernel_version_flag;
      char *arch_flag;
      asprintf (&macos_version_flag, "MACOS_VERSION=%s", macos_version);
      asprintf (&kernel_version_flag, "KERNEL_VERSION=%d", kernel_version);
      asprintf (&arch_flag, "ARCH=%s", arch);
      execlp ("make", "make", macos_version_flag, kernel_version_flag,
	      arch_flag, "-I", DPM_MAKE_DIR, "-f", makefile, target, NULL);
      error (1, errno, "exec()");
    }

  do
    ret = waitpid (pid, &status, 0);
  while (ret == -1 && errno == EINTR);
  if (WIFEXITED (status) && WEXITSTATUS (status))
    error (1, 0, "child process exited with code %d", WEXITSTATUS (status));
  else if (WIFSIGNALED (status))
    error (1, 0, "child process received signal %d", WTERMSIG (status));
}

static void
run_makefile_task (const char *target)
{
  run_makefile_task_file (target, ".dpm/Makefile");
}

static char *
install_destdir (void)
{
  char *destdir = xstrdup ("/tmp/dpm.destdir.XXXXXX");
  pid_t pid;
  int status;
  int ret;
  mkdtemp (destdir);
  pid = fork ();
  if (pid == -1)
    error (1, errno, "fork()");
  else if (!pid)
    {
      char *macos_version_flag;
      char *kernel_version_flag;
      char *arch_flag;
      char *destdir_flag;
      asprintf (&macos_version_flag, "MACOS_VERSION=%s", macos_version);
      asprintf (&kernel_version_flag, "KERNEL_VERSION=%d", kernel_version);
      asprintf (&arch_flag, "ARCH=%s", arch);
      asprintf (&destdir_flag, "DESTDIR=%s", destdir);
      execlp ("make", "make", macos_version_flag, kernel_version_flag,
	      arch_flag, destdir_flag, "-I", DPM_MAKE_DIR,
	      "-f", ".dpm/Makefile", "install", NULL);
      error (1, errno, "exec()");
    }

  do
    ret = waitpid (pid, &status, 0);
  while (ret == -1 && errno == EINTR);
  if (WIFEXITED (status) && WEXITSTATUS (status))
    error (1, 0, "child process exited with code %d", WEXITSTATUS (status));
  else if (WIFSIGNALED (status))
    error (1, 0, "child process received signal %d", WTERMSIG (status));
  return destdir;
}

void
pkg_configure (void)
{
  run_makefile_task ("configure");
  touch (".dpm/Configured");
}

void
pkg_build (void)
{
  if (access (".dpm/Configured", F_OK))
    pkg_configure ();
  run_makefile_task ("build");
}

void
pkg_clean (void)
{
  run_makefile_task ("clean");
}

void
pkg_distclean (void)
{
  run_makefile_task ("distclean");
  remove (".dpm/Configured");
}

void
pkg_postinstall (void)
{
  run_makefile_task_file ("postinstall", "Makefile");
}

void
pkg_dist (void)
{
  char *destdir;
  char *data_path;
  pkg_build ();
  destdir = install_destdir ();
  data_path = pkg_archive_data (destdir);
  remove_dir (destdir);
  free (destdir);
  pkg_archive (data_path);
  remove (data_path);
}
