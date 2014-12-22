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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "path.h"

#include "sysfilesystem.h"

static char* workingPath_ = NULL;

int
cs_path_init()
{
	const char* currentPath = NULL;
	char* ptr = NULL;

	currentPath = getenv("PWD");
	if (currentPath) {
		size_t len;

		len = strlen(currentPath) + 1;
		ptr = (char*) malloc(sizeof(char) * len);
		strcat(ptr, currentPath);
	} else {
		ptr = (char*) malloc(sizeof(char) * 2);
		strcpy(ptr, "/");
	}

	workingPath_ = ptr;

	return 0;
}

void
cs_path_quit()
{
	if (workingPath_) {
		free(workingPath_);
		workingPath_ = NULL;
	}
}

const char*
cs_path_getWorkingPath()
{
	return workingPath_;
}

int
cs_path_changeWorkingPath(const char* newPath)
{
	DIR* dirp;

	if(!newPath)
		return 1;

	dirp = opendir(newPath);
	if (!dirp)
		return 1;

	closedir(dirp);

	workingPath_ = realloc(workingPath_, strlen(newPath) + 1);
	strcpy(workingPath_, newPath);

	return 0;
}
