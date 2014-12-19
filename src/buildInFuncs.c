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
#include <time.h>

#include "path.h"

#include "buildInFuncs.h"

#define BUILTIN(cmd) {#cmd, cmd}

/* XXX This might not a good idea */
extern uint8_t shellIsRunning_;

typedef int(*buildinFuncPtr)(int, char**);

struct buildin {
	char* name;
	buildinFuncPtr cmd;
};

static struct buildin buildins[] = {
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
		printf("%s", argv[i]);

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
	printf("funciont not done, coming soon...\n");

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
doBuildinCmd(int argc, char* argv[])
{
	static int  builtinCmdNum = sizeof(buildins) / sizeof(struct buildin);

	for (int i = 0; i < builtinCmdNum; i++) {
		if (strcmp(argv[0], buildins[i].name) == 0)
			return buildins[i].cmd(argc, argv);
	}

	printf("Command not found\n");

	return 2525;
}
