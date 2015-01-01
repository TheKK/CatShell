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

#include <getopt.h>
#include <libintl.h>
#include <locale.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

#include "path.h"
#include "builtInFuncs.h"
#include "sysfilesystem.h"
#include "pipe.h"
#include "parser.h"
#include "cmdline.h"
#include "history.h"

#define _(STRING) gettext(STRING)

#define USER_INPUT_BUFFER_SIZE 200
#define MY_ARGC_MAX_COUNT 50

/* TODO move this to other file */
uint8_t shellIsRunning_ = 1;

static void
showVersion()
{
	printf(_("CatShell (OS homework) 0.25\n"));
}

static void
showHelp()
{
	printf(_("CatShell usage: [-v version][-h help]\n"));
}

static void
getOptions(int argc, char* argv[])
{
	int opt;

	while ((opt = getopt(argc, argv, "vh")) != -1) {
		switch (opt) {
		case 'v':
			showVersion();
		case 'h':
		case '?':
		default:
			showHelp();
			exit(0);
			break;
		}
	}
}

static void
init()
{
	if (cs_path_init())
		exit(1);

	if (cs_pipe_init())
		exit(1);

	if (cs_parser_init())
		exit(1);

	if (cs_cmdline_init())
		exit(1);

	if (cs_history_init())
		exit(1);
}

static void
quit()
{
	cs_path_quit();
	cs_pipe_quit();
	cs_parser_quit();
	cs_cmdline_quit();
	cs_history_quit();
}

static void
l10nInit()
{
	setlocale(LC_ALL, "");
	bindtextdomain("CatChat_client", "po");
	textdomain("CatChat_client");
}

static int
doSystemCmd(int argc, char* argv[])
{
	int childStatus;
	pid_t pid, waitPid;

	cs_cmdline_disableEcho();

	pid = fork();
	if (pid == -1) {
		printf("%s: System call fork() falied\n",
		       argv[0]);
		return -1;
	}

	if (!pid) {
		execvp(cs_parser_getArgv()[0],
		       cs_parser_getArgv());

		printf("%s: Command not found\n",
		       cs_parser_getArgv()[0]);
		exit(127);
	} else {
		if ((waitPid =
		     waitpid(pid, &childStatus, 0)) == -1) {

			printf("%s: "
			       "System call waitpid() falied\n",
			       cs_parser_getArgv()[0]);
			return -1;
		}
	}

	cs_cmdline_enableEcho();

	return 0;
}

int
main(int argc, char* argv[])
{
	init();
	l10nInit();

	getOptions(argc, argv);

	if (argc > 1) {
		FILE* fd = NULL;
		char* lineBuf = NULL;
		size_t lineLen = 0;

		fd = fopen(argv[1], "rb");
		if (!fd) {
			fprintf(stderr, "%s: File %s not found\n",
				argv[0], argv[1]);
			exit(1);
		}

		while (getline(&lineBuf, &lineLen, fd) != -1) {
			if (lineBuf[0] == '#')
				continue;

			if (lineBuf[strlen(lineBuf) - 1] == '\n')
				lineBuf[strlen(lineBuf) - 1] = '\0';

			if (strlen(lineBuf) == 0)
				continue;

			cs_parser_parse(lineBuf);

			if (doBuiltinCmd(cs_parser_getArgc(),
					 cs_parser_getArgv()) == 0)
				continue;

			doSystemCmd(cs_parser_getArgc(),
				    cs_parser_getArgv());
		}

		free(lineBuf);
		lineBuf = NULL;

		fclose(fd);
		fd = NULL;

	} else if (argc == 1) {

		while (shellIsRunning_) {
			cs_cmdline_handleUserInput();

			cs_parser_parse(cs_cmdline_getCmdBuf());

			if (doBuiltinCmd(cs_parser_getArgc(),
					 cs_parser_getArgv()) == 0)
				continue;

			doSystemCmd(cs_parser_getArgc(),
				    cs_parser_getArgv());
		}
	}

	quit();

	return EXIT_SUCCESS;
}
