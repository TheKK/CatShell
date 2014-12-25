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
#include <unistd.h>
#include <sys/stat.h>

#include "path.h"

#include "sysfilesystem.h"

static char* workingPath_ = NULL;
static size_t workingPathLen_ = 100;

static int
cs_path_updateWorkingPath()
{
	while (getcwd(workingPath_, workingPathLen_) == NULL) {
		workingPathLen_ *= 2;

		workingPath_ = (char*) malloc(sizeof(char) * workingPathLen_);
		if (!workingPath_) {
			/* XXX should not put this here */
			fprintf(stderr, "out of memory\n");
			return -1;
		}
	}

	return 0;
}

int
cs_path_init()
{
	workingPath_ = (char*) malloc(sizeof(char) * workingPathLen_);
	if (!workingPath_) {
		/* XXX should not put this here */
		fprintf(stderr, "out of memory\n");
		return -1;
	}

	if (cs_path_updateWorkingPath())
		return -1;

	return 0;
}

void
cs_path_quit()
{
	free(workingPath_);
	workingPath_ = NULL;
}

const char*
cs_path_getWorkingPath()
{
	return workingPath_;
}

int
cs_path_changeWorkingPath(const char* newPath)
{
	if (chdir(newPath))
		return -1;

	if (cs_path_updateWorkingPath())
		return -1;

	return chdir(newPath);
}

int
cs_path_mkdir(const char* path)
{
	return mkdir(path, 0777);
}
