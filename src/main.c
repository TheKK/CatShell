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

#include "path.h"
#include "builtInFuncs.h"
#include "sysfilesystem.h"
#include "pipe.h"
#include "parser.h"

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
}

static void
quit()
{
	cs_path_quit();
	cs_pipe_quit();
	cs_parser_quit();
}

static void
l10nInit()
{
	setlocale(LC_ALL, "");
	bindtextdomain("CatChat_client", "po");
	textdomain("CatChat_client");
}

static void
printPromote()
{
	printf("\n"
	       "\x1B[1m" "\x1B[34m" "# " "\033[0m" "%s in %s\n",
	       getenv("USER"), cs_path_getWorkingPath());
	printf("$ ");
}

int
main(int argc, char* argv[])
{
	const char** myArgv = NULL;
	int myArgc;

	init();
	l10nInit();

	getOptions(argc, argv);

	while (shellIsRunning_) {
		printPromote();
		cs_parser_readUserInput();

		myArgv = cs_parser_getArgv();
		myArgc = cs_parser_getArgc();

		if (doBuiltinCmd(myArgc, myArgv) == 0)
			continue;

		/* Command not found return value, dunno why */
		if (system(cs_parser_getRawUserInput()) != 127)
			continue;

		printf("catshell: command not found: %s\n",
		       cs_parser_getArgv()[0]);
	}

	quit();

	return EXIT_SUCCESS;
}
