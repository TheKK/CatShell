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
#include <stdint.h>
#include <string.h>
#include <dirent.h>
#include <time.h>

#include "path.h"

#include "builtInFuncs.h"

#define BUILTIN(cmd) {#cmd, cmd}

/* XXX This might not a good idea */
extern uint8_t shellIsRunning_;

typedef int(*builtinFuncPtr)(int, char**);

struct builtin {
	char* name;
	builtinFuncPtr cmd;
};

static struct builtin builtins[] = {
	BUILTIN(echo),
	BUILTIN(date),
	BUILTIN(pwd),
	BUILTIN(cd),
	BUILTIN(bye),
	BUILTIN(hell)
};

int
echo(int argc, char* argv[])
{
	for (uint16_t i = 1; i < argc; i++)
		printf("%s ", argv[i]);

	printf("\n");

	return 0;
}

int
date(int argc, char* argv[])
{
	time_t rawTime;

	rawTime = time(NULL);
	printf("%s", ctime(&rawTime));

	return 0;
}

int
pwd(int argc, char* argv[])
{
	printf("%s\n", cs_path_getWorkingPath());
	return 0;
}

int
cd(int argc, char* argv[])
{
	const char* currentPath;
	const char* cdTo;
	char* fullPath;
	size_t fullPathLen;

	/* XXX Not good */
	if (argc == 1)
		return 0;

	cdTo = argv[1];
	currentPath = cs_path_getWorkingPath();

	if (cdTo[0] == '/') {
		fullPath = (char*) cdTo;
	} else {
		/* "currentPath/cdTo", that's what we need */
		fullPathLen = strlen(currentPath) + 1 + strlen(cdTo);
		fullPath = (char*) malloc(sizeof(char) * fullPathLen + 1);

		strcpy(fullPath, currentPath);
		strcat(fullPath, "/");
		strcat(fullPath, cdTo);
	}

	/* Conver "/home/xxx/" to "/home/xxx" */
	if (fullPath[strlen(fullPath) - 1] == '/')
		fullPath[strlen(fullPath) - 1] = '\0';

	if (cs_path_changeWorkingPath(fullPath)) {
		printf("no such file or directory: %s\n", cdTo);
		return 1;
	}

	if (fullPath != cdTo)
		free(fullPath);

	return 0;
}

int
bye(int argc, char* argv[])
{
	shellIsRunning_ = 0;

	return 0;
}

int
hell(int argc, char* argv[])
{
	printf("this is hell!!\n");

	return 0;
}

int
doBuiltinCmd(int argc, char* argv[])
{
	static int  builtinCmdNum = sizeof(builtins) / sizeof(struct builtin);

	for (int i = 0; i < builtinCmdNum; i++) {
		if (strcmp(argv[0], builtins[i].name) == 0)
			return builtins[i].cmd(argc, argv);
	}

	printf("Command not found\n");

	return 2525;
}
