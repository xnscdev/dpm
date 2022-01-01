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
#include <human.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <xalloc.h>
#include "download.h"

struct buffer_mem
{
  char *data;
  size_t size;
};

static size_t
buffer_callback (void *data, size_t size, size_t block, void *user)
{
  struct buffer_mem *mem = user;
  size_t real = size * block;
  mem->data = xrealloc (mem->data, mem->size + real + 1);
  memcpy (mem->data + mem->size, data, real);
  mem->size += real;
  mem->data[mem->size] = '\0';
  return real;
}

static int
progress_callback (void *data, curl_off_t dltotal, curl_off_t dlnow,
		   curl_off_t ultotal, curl_off_t ulnow)
{
  static char dlnow_buffer[16];
  static char dltotal_buffer[16];
  int opts = human_round_to_nearest | human_base_1024 | human_SI;
  int bars;
  int i;
  if (dltotal == 0)
    return 0;
  bars = dlnow * 20 / dltotal;
  printf ("Downloading  \033[1m[\033[32;1m");
  for (i = 0; i < bars; i++)
    putchar ('=');
  printf ("\033[0m");
  for (; i < 20; i++)
    putchar ('-');
  printf ("\033[1m]\033[0m  %s/%-10s\r",
	  human_readable (dlnow, dlnow_buffer, opts, 1, 1024),
	  human_readable (dltotal, dltotal_buffer, opts, 1, 1024));
  fflush (stdout);
  return 0;
}

CURLcode
download_to_file (FILE *file, const char *url)
{
  CURL *curl = curl_easy_init ();
  CURLcode ret;
  if (!curl)
    return CURLE_FAILED_INIT;
  curl_easy_setopt (curl, CURLOPT_URL, url);
  curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, NULL);
  curl_easy_setopt (curl, CURLOPT_WRITEDATA, file);
  curl_easy_setopt (curl, CURLOPT_XFERINFOFUNCTION, progress_callback);
  curl_easy_setopt (curl, CURLOPT_NOPROGRESS, 0);
  curl_easy_setopt (curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
  ret = curl_easy_perform (curl);
  curl_easy_cleanup (curl);
  putchar ('\n');
  return ret;
}

CURLcode
download_to_buffer (void **buffer, const char *url)
{
  CURL *curl = curl_easy_init ();
  struct buffer_mem mem;
  CURLcode ret;
  mem.data = xmalloc (1);
  mem.size = 0;
  if (!curl)
    return CURLE_FAILED_INIT;
  curl_easy_setopt (curl, CURLOPT_URL, url);
  curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, buffer_callback);
  curl_easy_setopt (curl, CURLOPT_WRITEDATA, &mem);
  curl_easy_setopt (curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
  ret = curl_easy_perform (curl);
  curl_easy_cleanup (curl);
  if (ret != CURLE_OK)
    return ret;
  *buffer = mem.data;
  return CURLE_OK;
}
