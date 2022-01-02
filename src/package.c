/* package.c -- This file is part of DPM.
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
#include <archive.h>
#include <archive_entry.h>
#include <errno.h>
#include <error.h>
#include <fcntl.h>
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xalloc.h>
#include "list.h"
#include "package.h"
#include "repo.h"

#define BUFFER_SIZE 4096

#define ARCHIVE_FLAGS (ARCHIVE_EXTRACT_TIME		\
		       | ARCHIVE_EXTRACT_PERM		\
		       | ARCHIVE_EXTRACT_ACL		\
		       | ARCHIVE_EXTRACT_FFLAGS)

#define AR_ENT(x) do					\
    {							\
      if (x)						\
	error (1, 0, "failed to add archive entries");	\
    }							\
  while (0)

struct package_req **packages;
struct package_stack package_stack;
char *output_dir;

static struct archive *curr_archive;
static char ar_buffer[BUFFER_SIZE];

static char *
read_file_string (const char *path)
{
  FILE *file = fopen (path, "r");
  size_t len;
  char *buffer;
  if (!file)
    error (1, errno, "reading from %s", path);
  fseek (file, 0, SEEK_END);
  len = ftell (file);
  rewind (file);
  buffer = xmalloc (len + 1);
  fread (buffer, 1, len, file);
  buffer[len] = '\0';
  if (buffer[len - 1] == '\n')
    buffer[len - 1] = '\0';
  return buffer;
}

static int
add_archive_entry (struct archive *ar, const struct stat *st, unsigned int type,
		   const char *name, const char *path)
{
  struct archive_entry *entry = archive_entry_new ();
  size_t len;
  struct stat stat;

  if (!st)
    {
      st = &stat;
      if (lstat (path, (struct stat *) st) == -1)
	return -1;
    }

  if (type == AE_IFDIR)
    len = 0;
  else
    len = st->st_size;
  archive_entry_set_pathname (entry, name);
  archive_entry_set_size (entry, len);
  archive_entry_set_filetype (entry, type);
  archive_entry_set_perm (entry, st->st_mode & 07777);
  if (type == AE_IFLNK)
    {
      ssize_t ret = readlink (path, ar_buffer, BUFFER_SIZE);
      ar_buffer[ret] = '\0';
      archive_entry_set_symlink (entry, ar_buffer);
    }
  archive_write_header (ar, entry);

  if (len && type != AE_IFLNK)
    {
      int fd = open (path, O_RDONLY | O_SYMLINK);
      if (fd == -1)
	error (1, errno, "open() archive: /%s", name);
      while (len)
	{
	  ssize_t ret = read (fd, ar_buffer, BUFFER_SIZE);
	  if (ret == -1)
	    error (1, errno, "read() archive: /%s", name);
	  archive_write_data (ar, ar_buffer, ret);
	  len -= ret;
	}
      close (fd);
    }
  archive_entry_free (entry);
  return 0;
}

static int
archive_copy_data (struct archive *ar, struct archive *aw)
{
  int ret;
  const void *buffer;
  size_t size;
  la_int64_t offset;
  while (1)
    {
      ret = archive_read_data_block (ar, &buffer, &size, &offset);
      if (ret == ARCHIVE_EOF)
	return ARCHIVE_OK;
      if (ret < ARCHIVE_OK)
	return ret;
      ret = archive_write_data_block (aw, buffer, size, offset);
      if (ret < ARCHIVE_OK)
	return ret;
    }
}

static int
remove_dir_callback (const char *path, const struct stat *st, int type,
		     struct FTW *ftw)
{
  int ret = remove (path);
  if (ret == -1)
    error (0, errno, "remove(): %s", path);
  return ret;
}

static int
archive_dir_callback (const char *path, const struct stat *st, int type,
		      struct FTW *ftw)
{
  unsigned int file_type = AE_IFREG;
  if (!strcmp (path, "."))
    return 0;
  path += 2;

  switch (type)
    {
    case FTW_D:
      file_type = AE_IFDIR;
      break;
    case FTW_SL:
    case FTW_SLN:
      file_type = AE_IFLNK;
      break;
    }
  return add_archive_entry (curr_archive, st, file_type, path, path);
}

void
remove_dir (const char *dir)
{
  nftw (dir, remove_dir_callback, 64, FTW_DEPTH | FTW_PHYS);
}

void
pkg_extract (struct package *package)
{
  struct archive *ar;
  struct archive *ext;
  char *dir = xstrdup ("/tmp/dpm.XXXXXX");
  int ret;

  mkdtemp (dir);
  if (chdir (dir) == -1)
    error (1, errno, "chdir()");
  package->extract_dir = dir;

  ar = archive_read_new ();
  archive_read_support_format_all (ar);
  archive_read_support_filter_all (ar);
  ext = archive_write_disk_new ();
  archive_write_disk_set_options (ext, ARCHIVE_FLAGS);
  archive_write_disk_set_standard_lookup (ext);
  ret = archive_read_open_FILE (ar, package->archive);
  if (ret)
    error (1, 0, "failed to extract package archive: %s %s", package->name,
	   package->version);

  while (1)
    {
      struct archive_entry *entry;
      ret = archive_read_next_header (ar, &entry);
      if (ret == ARCHIVE_EOF)
	break;
      if (ret < ARCHIVE_OK)
	error (ret < ARCHIVE_WARN, 0, "archive: %s", archive_error_string (ar));
      ret = archive_write_header (ext, entry);
      if (ret < ARCHIVE_OK)
	error (0, 0, "archive: %s", archive_error_string (ext));
      else if (archive_entry_size (entry) > 0)
	{
	  ret = archive_copy_data (ar, ext);
	  if (ret < ARCHIVE_OK)
	    error (ret < ARCHIVE_WARN, 0, "archive: %s",
		   archive_error_string (ext));
	}
      ret = archive_write_finish_entry (ext);
      if (ret < ARCHIVE_OK)
	error (ret < ARCHIVE_WARN, 0, "archive: %s", archive_error_string (ar));
    }

  archive_read_close (ar);
  archive_read_free (ar);
  archive_write_close (ext);
  archive_write_free (ext);
}

char *
pkg_archive_data (char *destdir)
{
  char *filename = xstrdup ("/tmp/dpm.data.XXXXXX");
  int fd;
  mktemp (filename);
  curr_archive = archive_write_new ();
  archive_write_add_filter_xz (curr_archive);
  archive_write_set_format_pax_restricted (curr_archive);
  archive_write_open_filename (curr_archive, filename);

  fd = open (".", O_RDONLY);
  if (fd == -1)
    error (1, errno, "open()");
  if (chdir (destdir) == -1)
    error (1, errno, "chdir()");
  nftw (".", archive_dir_callback, 64, FTW_PHYS);
  if (fchdir (fd) == -1)
    error (1, errno, "fchdir()");

  archive_write_close (curr_archive);
  archive_write_free (curr_archive);
  return filename;
}

void
pkg_archive (char *data_path)
{
  struct archive *ar;
  char *name = read_file_string (".dpm/Name");
  char *version = read_file_string (".dpm/Version");
  char *filename;

  asprintf (&filename, "%s/%s-%s.dpm", output_dir, name, version);
  free (name);
  free (version);

  ar = archive_write_new ();
  archive_write_set_format_cpio (ar);
  if (archive_write_open_filename (ar, filename) != ARCHIVE_OK)
    error (1, archive_errno (ar), "failed to create archive");
  free (filename);

  AR_ENT (add_archive_entry (ar, NULL, AE_IFREG, "Name", ".dpm/Name"));
  AR_ENT (add_archive_entry (ar, NULL, AE_IFREG, "Version", ".dpm/Version"));
  AR_ENT (add_archive_entry (ar, NULL, AE_IFREG, "Makefile", ".dpm/Makefile"));
  AR_ENT (add_archive_entry (ar, NULL, AE_IFREG, "Data", data_path));
  add_archive_entry (ar, NULL, AE_IFREG, "Depends", ".dpm/Depends");

  archive_write_close (ar);
  archive_write_free (ar);
}

int
pkg_stack_contains (const char *name)
{
  size_t i;
  for (i = 0; i < package_stack.len; i++)
    {
      if (!strcmp (package_stack.packages[i]->name, name))
	return 1;
    }
  return 0;
}

void
pkg_process_req (struct package_req *req)
{
  struct package *package;
  char *installed_version;
  printf ("\033[1mProcessing package: \033[33m%s\033[0m\n", req->name);

  installed_version = check_pkg_installed (req->name);
  if (installed_version)
    {
      printf ("\033[33;1m%s\033[0m version \033[32m%s\033[0m "
	      "already installed\n", req->name, installed_version);
      return;
    }

  package = repo_search_package (req);
  if (!package)
    error (1, 0, "failed to find package: %s", req->name);

  pkg_extract (package);
  fclose (package->archive);

  pkg_resolve_dependencies (package->name);
  pkg_stack_insert (package);
}

void
pkg_resolve_dependencies (const char *name)
{
  FILE *file = fopen ("Depends", "r");
  char *line = NULL;
  size_t len = 0;
  ssize_t size;
  if (!file)
    return;
  while ((size = getline (&line, &len, file)) != EOF)
    {
      if (line[size - 1] == '\n')
	line[size - 1] = '\0';
      printf ("Processing dependency of \033[33m%s\033[0m: "
	      "\033[33;1m%s\033[0m\n", name, line);
      if (!pkg_stack_contains (line))
	{
	  struct package_req req;
	  req.name = line;
	  req.version = NULL;
	  pkg_process_req (&req);
	  free (req.version);
	}
    }
  free (line);
}

void
pkg_stack_insert (struct package *package)
{
  package_stack.packages =
    xrealloc (package_stack.packages,
	      sizeof (struct package *) * ++package_stack.len);
  package_stack.packages[package_stack.len - 1] = package;
}

void
pkg_stack_install (void)
{
  size_t i;
  for (i = 0; i < package_stack.len; i++)
    {
      struct package *package = package_stack.packages[i];
      if (chdir (package->extract_dir) == -1)
	error (1, errno, "chdir()");

      free (package->name);
      free (package->version);
      free (package->extract_dir);
      free (package);
    }
}
