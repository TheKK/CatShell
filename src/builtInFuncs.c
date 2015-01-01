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
#include "history.h"

#include "builtInFuncs.h"

#define BUILTIN(cmd) {#cmd, cmd}
#define BUILTIN_WITH_NAME(cmdName, cmdFunc) {#cmdName, cmdFunc}

/* XXX This might not a good idea */
extern uint8_t shellIsRunning_;

static int echo(int argc, const char* argv[]);
static int date(int argc, const char* argv[]);
static int pwd(int argc, const char* argv[]);
static int cd(int argc, const char* argv[]);
static int bltMkdir(int argc, const char* argv[]);
static int cat(int argc, const char* argv[]);
static int bltTime(int argc, const char* argv[]);
static int ls(int argc, const char* argv[]);
static int du(int argc, const char* argv[]);
static int ps(int argc, const char* argv[]);
static int history(int argc, const char* argv[]);
static int help(int argc, const char* argv[]);

static int bye(int argc, const char* argv[]);
static int hell(int argc, const char* argv[]);


typedef int(*builtinFuncPtr)(int, const char**);

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
	BUILTIN(ps),
	BUILTIN(help),
	BUILTIN(history),
	BUILTIN(bye),
	BUILTIN(hell)
};

static int
echo(int argc, const char* argv[])
{
	for (uint16_t i = 1; i < argc; i++)
		fprintf(cs_pipe_getOutputStream(),
			"%s ", argv[i]);

	fprintf(cs_pipe_getOutputStream(),
		"\n");

	return 0;
}

static int
date(int argc, const char* argv[])
{
	time_t rawTime;

	rawTime = time(NULL);
	fprintf(cs_pipe_getOutputStream(), "%s", ctime(&rawTime));

	return 0;
}

static int
pwd(int argc, const char* argv[])
{
	fprintf(cs_pipe_getOutputStream(), "%s\n", cs_path_getWorkingPath());
	return 0;
}

static int
cd(int argc, const char* argv[])
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
		fprintf(stderr, "no such file or directory: %s\n", cdTo);
		return 1;
	}

	if (fullPath != cdTo)
		free(fullPath);

	return 0;
}

static int
bltMkdir(int argc, const char* argv[])
{
	const char* currentPath = NULL;
	const char* dirPath = NULL;
	char* fullPath = NULL;
	size_t fullPathLen;

	if (argc == 1) {
		fprintf(stderr, "%s: missing operand\n", argv[0]);
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
			fprintf(stderr, "%s: can not create directory '%s': "
				"File exist\n",
			       argv[0], argv[i]);
	}

	return 0;
}

static int
cat(int argc, const char* argv[])
{
	const char* currentPath = NULL;
	const char* filePath = NULL;
	char* fullPath = NULL;
	size_t fullPathLen;
	FILE* fd = NULL;
	char* strBuf = NULL;
	size_t strLen;

	if (argc == 1) {
		fprintf(stderr, "%s: missing operand\n", argv[0]);
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
			fprintf(stderr, "%s: %s: No such file or directory\n",
			       argv[0], filePath);
			continue;
		}

		while (getline(&strBuf, &strLen, fd) != -1)
			fprintf(stderr, "%s", strBuf);

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
bltTime(int argc, const char* argv[])
{
	time_t startTime, stopTime;
	int newArgc;
	const char** newArgv;

	if (argc == 1) {
		fprintf(stderr, "%s: missing operand\n", argv[0]);
		return 1;
	}

	newArgc = argc - 1;
	newArgv = &argv[1];

	/* XXX Is this way okay? */
	startTime = time(NULL);
	doBuiltinCmd(newArgc, (char**) newArgv);
	stopTime = time(NULL);

	fprintf(cs_pipe_getOutputStream(),
		"Start clock: %li, stop clock: %li\n", startTime, stopTime);
	fprintf(cs_pipe_getOutputStream(),
		"%f user\n",
		(float) (stopTime - startTime) / (float) CLOCKS_PER_SEC);

	return 0;
}

static int
ls(int argc, const char* argv[])
{
	const char* currentPath = NULL;
	const char* targetPath = NULL;
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
			fullPath = (char*) targetPath;
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
		fprintf(stderr,
			"%s: cannot access %s: No such file or directory\n",
			argv[0], targetPath);
		return 2;
	}

	while ((ent = readdir(dirp))) {
		if (ent->d_name[0] == '.')
			continue;
		fprintf(cs_pipe_getOutputStream(), "%s\t", ent->d_name);
	}
	fprintf(cs_pipe_getOutputStream(), "\n");

	closedir(dirp);
	dirp = NULL;

	if ((fullPath != targetPath) && (fullPath != currentPath))
		free(fullPath);
	fullPath = NULL;

	return 0;
}

static int
du(int argc, const char* argv[])
{
	const char* currentPath = NULL;
	const char* targetPath = NULL;
	char* fullPath = NULL;
	size_t fullPathLen;
	struct stat st;

	if (argc == 1) {
		fprintf(stderr, "%s: missing operand\n", argv[0]);
		return 1;
	}

	currentPath = cs_path_getWorkingPath();
	targetPath = argv[1];

	if (targetPath[0] == '/') {
		fullPath = (char*) targetPath;
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

		fprintf(stderr, "%s: file or directory not exist: %s\n",
		       argv[0], targetPath);

		return 1;
	}

	fprintf(cs_pipe_getOutputStream(),
		"%fK\t%s\n", ((float)st.st_size / 1024.0), targetPath);

	if (fullPath != targetPath)
		free(fullPath);
	fullPath = NULL;

	return 0;
}

static int
ps(int argc, const char* argv[])
{
	DIR* dirp = NULL;
	struct dirent* ent = NULL;
	char* fullPath = NULL;
	FILE* fd = NULL;
	char buf[300];
	char* tok = NULL;;
	int i, pid, utime, tty;
	char* cmd = NULL;

	dirp = opendir("/proc");
	if (!dirp) {
		fprintf(cs_pipe_getOutputStream(), "sorry, but Linux only\n");

		return 1;
	}

	fprintf(cs_pipe_getOutputStream(),
		"%6s %-10s %10s %-10s\n", "PID", "TTY", "TIME", "CMD");
	while ((ent = readdir(dirp))) {
		if (ent->d_type == DT_DIR) {
			if (strcspn(ent->d_name, "0123456789") == 0) {
				fullPath =
					(char*) malloc(sizeof(char) *
						       (strlen("/proc/") +
							strlen(ent->d_name) +
							strlen("/stat")));
				strcpy(fullPath, "/proc/");
				strcat(fullPath, ent->d_name);
				strcat(fullPath, "/stat");
				fd = fopen(fullPath, "rb");
				if (!fd) {
					fprintf(stderr,
						"this should not happend\n");
					exit (1);
				}

				fgets(buf, sizeof(buf), fd);
				fclose(fd);

				i = 1;
				tok = strtok(buf, " ()");
				while (tok) {
					switch (i) {
					case 1: /* pid */
						pid = atoi(tok);
						break;
					case 2: /* file name */
						cmd = tok;
						break;
					case 7: /* tty_nr */
						tty = atoi(tok);
						break;
					case 14: /* utime */
						utime = atoi(tok);
						break;
					}
					++i;
					tok = strtok(NULL, " ()");
				}
				fprintf(cs_pipe_getOutputStream(),
					"%6d %-10d %10d %-10s\n",
					pid, tty, utime, cmd);

				free(fullPath);
			}
		}
	}

	closedir(dirp);
	dirp = NULL;

	return 0;
}

static int
history(int argc, const char* argv[])
{
	cs_history_showAllHistory();

	return 0;
}

static int
help(int argc, const char* argv[])
{
	/* FIXME same code */
	static int  builtinCmdNum = sizeof(builtins) / sizeof(struct builtin);

	fprintf(cs_pipe_getOutputStream(), "Avaliable builtin command:\n");
	for (int i = 0; i < builtinCmdNum; i++)
		fprintf(cs_pipe_getOutputStream(), "%s\n", builtins[i].name);

	return 0;
}

static int
bye(int argc, const char* argv[])
{
	shellIsRunning_ = 0;

	return 0;
}

static int
hell(int argc, const char* argv[])
{
	fprintf(cs_pipe_getOutputStream(), "this is hell!!\n");

	return 0;
}

int
doBuiltinCmd(int argc, char** argv)
{
	static int  builtinCmdNum = sizeof(builtins) / sizeof(struct builtin);
	const char* cmdName = NULL;

	cmdName = argv[0];

	if (strcmp(cmdName, "") == 0)
		return 0;

	for (int i = 0; i < builtinCmdNum; i++) {
		if (strcmp(cmdName, builtins[i].name) == 0) {
			/* TODO add cs_state or something */
			builtins[i].cmd(argc, (const char**) argv);
			return 0;
		}
	}

	return BLTCMD_NOT_FOUND;
}
