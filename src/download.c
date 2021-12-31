/* download.c -- This file is part of DPM.
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

#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xalloc.h>
#include "download.h"

struct buffer_mem
{
  char *data;
  size_t size;
};

static size_t
download_buffer_callback (void *data, size_t size, size_t block, void *user)
{
  struct buffer_mem *mem = user;
  size_t real = size * block;
  mem->data = xrealloc (mem->data, mem->size + real + 1);
  memcpy (mem->data + mem->size, data, real);
  mem->size += real;
  return real;
}

CURLcode
download_data (void *data, const char *url,
	       size_t (*callback) (void *, size_t, size_t, void *))
{
  CURL *curl = curl_easy_init ();
  CURLcode ret;
  if (!curl)
    return CURLE_FAILED_INIT;
  curl_easy_setopt (curl, CURLOPT_URL, url);
  curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, callback);
  curl_easy_setopt (curl, CURLOPT_WRITEDATA, data);
  curl_easy_setopt (curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
  ret = curl_easy_perform (curl);
  curl_easy_cleanup (curl);
  return ret;
}

CURLcode
download_to_file (FILE *file, const char *url)
{
  return download_data (file, url, NULL);
}

CURLcode
download_to_buffer (void **buffer, const char *url)
{
  struct buffer_mem mem;
  CURLcode ret;
  mem.data = xmalloc (1);
  mem.size = 0;
  ret = download_data (&mem, url, download_buffer_callback);
  if (ret != CURLE_OK)
    return ret;
  *buffer = mem.data;
  return CURLE_OK;
}
