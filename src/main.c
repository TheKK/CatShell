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

#include "buildInFuncs.h"

#define _(STRING) gettext(STRING)

#define USER_INPUT_BUFFER_SIZE 200
#define MY_ARGC_MAX_COUNT 50

static uint8_t shellIsRunning_ = 1;

static char cmdBuffer_[USER_INPUT_BUFFER_SIZE];

static uint16_t myArgc_;
static char* myArgv_[MY_ARGC_MAX_COUNT];

static int32_t returnValue_;

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
l10nInit()
{
	setlocale(LC_ALL, "");
	bindtextdomain("CatChat_client", "po");
	textdomain("CatChat_client");
}

static void
printPromote()
{
	printf("[CatShell] $ ");
}

static void
doCmd()
{
	if (strcmp(myArgv_[0], "hell") == 0) {
		returnValue_ = hell(myArgc_, myArgv_);

	} else if (strcmp(myArgv_[0], "echo") == 0) {
		returnValue_ = echo(myArgc_, myArgv_);

	} else if (strcmp(myArgv_[0], "exit") == 0) {
		shellIsRunning_ = 0;

	} else if (strcmp(myArgv_[0], "\0") == 0) {
		printf("\n");

	} else {
		printf("command not found\n");
		returnValue_ = 2525;
	}
}

static void
resetMyArg()
{
	myArgc_ = 1;

	for (uint16_t i =  0; i < MY_ARGC_MAX_COUNT; i++)
		myArgv_[i] = NULL;
}


static void
splitCmd()
{
	uint16_t cmdBufferLen;

	resetMyArg();

	myArgv_[0] = cmdBuffer_;

	cmdBufferLen = strlen(cmdBuffer_);

	for (uint16_t i = 0; i < cmdBufferLen; i++) {
		if (cmdBuffer_[i] == ' ') {
			myArgv_[myArgc_] = &cmdBuffer_[i + 1];
			myArgc_++;
			cmdBuffer_[i] = '\0';
		}

		if (cmdBuffer_[i] == '\n') {
			cmdBuffer_[i] = '\0';
			break;
		}
	}
}

int
main(int argc, char* argv[])
{
	l10nInit();

	getOptions(argc, argv);

	while (shellIsRunning_) {
		printPromote();
		fgets(cmdBuffer_, USER_INPUT_BUFFER_SIZE, stdin);

		splitCmd();
		doCmd();
	}

	return EXIT_SUCCESS;
}
