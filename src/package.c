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

#include <archive.h>
#include <archive_entry.h>
#include <errno.h>
#include <error.h>
#include <stdlib.h>
#include "package.h"

#define ARCHIVE_FLAGS (ARCHIVE_EXTRACT_TIME		\
		       | ARCHIVE_EXTRACT_PERM		\
		       | ARCHIVE_EXTRACT_ACL		\
		       | ARCHIVE_EXTRACT_FFLAGS)

struct package_req **packages;
struct package_stack package_stack;

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

void
pkg_extract (struct package *package)
{
  struct archive *ar;
  struct archive *ext;
  char dir[] = "/tmp/dpm.XXXXXX";
  int ret;

  mkdtemp (dir);
  if (chdir (dir) == -1)
    error (1, errno, "failed to change directory");

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
