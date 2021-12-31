/* download.h -- This file is part of DPM.
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

#ifndef __DOWNLOAD_H
#define __DOWNLOAD_H

#include <curl/curl.h>

CURLcode download_data (void *data, const char *url,
			size_t (*callback) (void *, size_t, size_t, void *));
CURLcode download_to_file (FILE *file, const char *url);
CURLcode download_to_buffer (void **buffer, const char *url);

#endif
