/*
 * CatShell
 * Copyright (C) 2014 TheKK <thumbd03803@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/*
 * These functions reference to the following project:
 *
 * Simple DirectMedia Layer                                                       
 * Copyright (C) 1997-2014 Sam Lantinga <slouken@libsdl.org>
 *
 * file src/filesystem/unix/SDL_sysfilesystem.c
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "sysfilesystem.h"

void
cs_Free(void* ptr)
{
	if (!ptr)
		return;

	free(ptr);
}

static void*
cs_realloc(void* ptr, size_t size)
{
	void* retval = NULL;

	retval = malloc(size);	
	if (!retval)
		return NULL;

	if (!ptr) {
		return retval;
	} else {
		memcpy(retval, ptr, size);
		free(ptr);
	}

	return retval;
}

char*
cs_readSymLink(const char* path)
{
	char* retval = NULL;
	size_t len = 64;
	size_t rc = -1;

	while (1) {
		char* ptr = (char*) cs_realloc(retval, len);
		if (!ptr) {
			printf("Out of memory\n");
			break;
		}

		retval = ptr;

		rc = readlink(path, retval, len);
		if (rc == -1) {
			break;
		} else if (rc < len) {
			retval[rc] = '\0';
			return retval;
		}

		len *= 2;
	}

	free(retval);

	return NULL;
}

char*
cs_GetBasePath()
{
	char* retval = NULL;

	retval = cs_readSymLink("/proc/self/exe");
	if (!retval)
		return NULL;

	if (retval) {
		char* ptr = strrchr(retval, '/');
		if (ptr) {
			*(ptr + 1) = '\0';
		} else {
			cs_Free(retval);
			return NULL;
		}
	}

	if (retval) {
		char* ptr = cs_realloc(retval, strlen(retval) + 1);
		if (ptr)
			retval = ptr;
	}

	return retval;
}
