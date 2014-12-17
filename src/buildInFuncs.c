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

/* FIXME Not place here */
static char workPath[500];

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
	printf("%s\n", workPath);

	return 0;
}

int
cd(int argc, char* argv[])
{
	/* TODO Check if dir exist */
	if (argc == 1) {
		memset(workPath, 0, strlen(workPath));
		strcat(workPath, "/home/");
		strcat(workPath, getenv("USER"));
	} else {
		strcpy(workPath, argv[1]);
	}

	return 0;
}

int
hell(int argc, char* argv[])
{
	printf("welcom to hell!!\n");

	return 0;
}
