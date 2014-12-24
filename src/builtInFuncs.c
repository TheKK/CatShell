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
#include <sys/stat.h>

#include "path.h"
#include "pipe.h"

#include "builtInFuncs.h"

#define BUILTIN(cmd) {#cmd, cmd}
#define BUILTIN_WITH_NAME(cmdName, cmdFunc) {#cmdName, cmdFunc}

/* XXX This might not a good idea */
extern uint8_t shellIsRunning_;

static int echo(int argc, char* argv[]);
static int date(int argc, char* argv[]);
static int pwd(int argc, char* argv[]);
static int cd(int argc, char* argv[]);
static int bltMkdir(int argc, char* argv[]);
static int cat(int argc, char* argv[]);
static int bltTime(int argc, char* argv[]);
static int ls(int argc, char* argv[]);
static int du(int argc, char* argv[]);

static int bye(int argc, char* argv[]);
static int hell(int argc, char* argv[]);


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
	BUILTIN_WITH_NAME(mkdir, bltMkdir),
	BUILTIN(cat),
	BUILTIN_WITH_NAME(time, bltTime),
	BUILTIN(ls),
	BUILTIN(du),
	BUILTIN(bye),
	BUILTIN(hell)
};

static int
echo(int argc, char* argv[])
{
	for (uint16_t i = 1; i < argc; i++)
		fprintf(cs_pipe_getOutputStream(),
			"%s ", argv[i]);

	fprintf(cs_pipe_getOutputStream(),
		"\n");

	return 0;
}

static int
date(int argc, char* argv[])
{
	time_t rawTime;

	rawTime = time(NULL);
	printf("%s", ctime(&rawTime));

	return 0;
}

static int
pwd(int argc, char* argv[])
{
	printf("%s\n", cs_path_getWorkingPath());
	return 0;
}

static int
cd(int argc, char* argv[])
{
	const char* currentPath = NULL;
	const char* cdTo = NULL;
	char* fullPath = NULL;
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

	/* XXX Conver "/home/xxx/" to "/home/xxx" */
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

static int
bltMkdir(int argc, char* argv[])
{
	const char* currentPath = NULL;
	const char* dirPath = NULL;
	char* fullPath = NULL;
	size_t fullPathLen;

	if (argc == 1) {
		printf("%s: missing operand\n", argv[0]);
		return 1;
	}

	currentPath = cs_path_getWorkingPath();
	for (int i = 1; i < argc; ++i) {
		dirPath = argv[i];
		fullPathLen = strlen(currentPath) + 1 + strlen(dirPath);
		fullPath = realloc(fullPath, fullPathLen + 1);

		strcpy(fullPath, currentPath);
		strcat(fullPath, "/");
		strcat(fullPath, dirPath);

		if (cs_path_mkdir(fullPath))
			printf("%s: can not create directory '%s': "
			       "File exist\n",
			       argv[0], argv[i]);
	}

	return 0;
}

static int
cat(int argc, char* argv[])
{
	const char* currentPath = NULL;
	const char* filePath = NULL;
	char* fullPath = NULL;
	size_t fullPathLen;
	FILE* fd = NULL;
	char* strBuf = NULL;
	size_t strLen;

	if (argc == 1) {
		printf("%s: missing operand\n", argv[0]);
		return 1;
	}

	currentPath = cs_path_getWorkingPath();
	for (int i = 1; i < argc; ++i) {
		filePath = argv[i];

		if (filePath[0] == '/') {
			fullPath = (char*) filePath;
		} else {
			fullPathLen =
				strlen(currentPath) + 1 + strlen(filePath);
			fullPath = realloc(fullPath, fullPathLen + 1);
			strcpy(fullPath, currentPath);
			strcat(fullPath, "/");
			strcat(fullPath, filePath);
		}

		fd = fopen(fullPath, "rb");
		if (!fd) {
			printf("%s: %s: No such file or directory\n",
			       argv[0], filePath);
			continue;
		}

		while (getline(&strBuf, &strLen, fd) != -1)
			printf("%s", strBuf);

		free(strBuf);
		strBuf = NULL;

		fclose(fd);
		fd = NULL;

		if (fullPath != argv[i])
			free(fullPath);
		fullPath = NULL;
	}

	return 0;
}

static int
bltTime(int argc, char* argv[])
{
	time_t startTime, stopTime;
	int newArgc;
	char** newArgv;

	if (argc == 1) {
		printf("%s: missing operand\n", argv[0]);
		return 1;
	}

	newArgc = argc - 1;
	newArgv = &argv[1];

	/* XXX Is this way okay? */
	startTime = time(NULL);
	doBuiltinCmd(newArgc, newArgv);
	stopTime = time(NULL);

	printf("Start clock: %li, stop clock: %li\n", startTime, stopTime);
	printf("%f user\n",
	       (float) (stopTime - startTime) / (float) CLOCKS_PER_SEC);

	return 0;
}

static int
ls(int argc, char* argv[])
{
	const char* currentPath = NULL;
	char* targetPath = NULL;
	char* fullPath = NULL;
	size_t fullPathLen;
	DIR* dirp = NULL;
	struct dirent* ent = NULL;

	currentPath = cs_path_getWorkingPath();
	if (argc == 1) {
		fullPath = (char*) currentPath;
		targetPath = "";
	} else {
		targetPath = argv[1];
		if (targetPath[0] == '/') {
			fullPath = targetPath;
		} else {
			fullPathLen =
				strlen(currentPath) + 1 + strlen(targetPath);
			fullPath =
				(char*) malloc(sizeof(char) * fullPathLen + 1);

			strcpy(fullPath, currentPath);
			strcat(fullPath, "/");
			strcat(fullPath, targetPath);
		}
	}

	dirp = opendir(fullPath);
	if (!dirp) {
		printf("%s: cannot access %s: No such file or directory\n",
		       argv[0], targetPath);
		return 2;
	}

	while ((ent = readdir(dirp))) {
		if (ent->d_name[0] == '.')
			continue;
		printf("%s\t", ent->d_name);
	}

	closedir(dirp);
	dirp = NULL;

	if ((fullPath != targetPath) && (fullPath != currentPath))
		free(fullPath);
	fullPath = NULL;

	return 0;
}

static int
du(int argc, char* argv[])
{
	const char* currentPath = NULL;
	char* targetPath = NULL;
	char* fullPath = NULL;
	size_t fullPathLen;
	struct stat st;

	if (argc == 1) {
		printf("%s: missing operand\n", argv[0]);
		return 1;
	}

	currentPath = cs_path_getWorkingPath();
	targetPath = argv[1];

	if (targetPath[0] == '/') {
		fullPath = targetPath;
	} else {
		fullPathLen = strlen(currentPath) + 1 + strlen(targetPath);
		fullPath = (char*) malloc(sizeof(char) * fullPathLen + 1);

		strcpy(fullPath, currentPath);
		strcat(fullPath, "/");
		strcat(fullPath, targetPath);
	}

	if (stat(fullPath, &st) == -1) {
		if (fullPath != targetPath)
			free(fullPath);
		fullPath = NULL;

		printf("%s: file or directory not exist: %s\n",
		       argv[0], targetPath);

		return 1;
	}

	printf("%fK\t%s\n", ((float)st.st_size / 1024.0), targetPath);

	if (fullPath != targetPath)
		free(fullPath);
	fullPath = NULL;

	return 0;
}

static int
bye(int argc, char* argv[])
{
	shellIsRunning_ = 0;

	return 0;
}

static int
hell(int argc, char* argv[])
{
	printf("this is hell!!\n");

	return 0;
}

int
doBuiltinCmd(int argc, char* argv[])
{
	static int  builtinCmdNum = sizeof(builtins) / sizeof(struct builtin);
	const char* cmd = NULL;

	cmd = argv[0];

	/* No command */
	if (strcmp(cmd, "") == 0)
		return 0;

	for (int i = 0; i < builtinCmdNum; i++) {
		if (strcmp(argv[0], builtins[i].name) == 0)
			return builtins[i].cmd(argc, argv);
	}

	printf("catshell: command not found: %s\n", argv[0]);

	return 2525;
}
