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

#include "cmdline.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

#include "path.h"

static char* cmdLineBuf = NULL;
static size_t cmdLineBufLen = 10;
static char* cmdLineBufCurPos = NULL;
static char* cmdLineBufTailPos = NULL;

static void
cs_cmdline_printPromote()
{
	printf("\n"
	       "\x1B[1m" "\x1B[34m" "# " "\033[0m" "%s in %s\n",
	       getenv("USER"), cs_path_getWorkingPath());
	printf("$ ");
}

static void
cs_cmdline_enlargeCmdLineBuf(size_t newSize)
{
	int offset;

	if (newSize <= cmdLineBufLen)
		return;

	offset = cmdLineBufCurPos - cmdLineBuf;

	cmdLineBuf = (char*) realloc(cmdLineBuf, newSize);
	if (!cmdLineBuf) {
		fprintf(stderr, "Out of memory\n");
		exit(1);
	}

	cmdLineBufLen = newSize;
	cmdLineBufCurPos = cmdLineBuf + offset;
	cmdLineBufTailPos = cmdLineBuf + cmdLineBufLen - 1;
}

int
cs_cmdline_init()
{
	struct termios oldt, newt;

	cmdLineBuf = (char*) malloc(sizeof(char) * cmdLineBufLen);
	if (!cmdLineBuf) {
		return -1;
	}

	cmdLineBufCurPos = cmdLineBuf;
	cmdLineBufTailPos = cmdLineBuf + cmdLineBufLen - 1;

	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= !ECHO;
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);

	return 0;
}

void
cs_cmdline_quit()
{
	free(cmdLineBuf);
	cmdLineBuf = NULL;
}

void
cs_cmdline_handleUserInput()
{
	int c;

	cmdLineBufCurPos = cmdLineBuf;
	*cmdLineBufCurPos = '\0';

	cs_cmdline_printPromote();

	while (1) {
		c = getchar();

		switch (c) {
		case CS_KEY_BACKSPACE:
			if (cmdLineBufCurPos != cmdLineBuf) {
				--cmdLineBufCurPos;
				*cmdLineBufCurPos = '\0';
			}
			break;
		case CS_KEY_RETURN:
			printf("\n");
			return;
		default:
			if (cmdLineBufCurPos == cmdLineBufTailPos) 
				cs_cmdline_enlargeCmdLineBuf(cmdLineBufLen * 2);

			*cmdLineBufCurPos = c;
			++cmdLineBufCurPos;
			*cmdLineBufCurPos = '\0';
			break;
		}

		/* XXX gonna find out other method */
		printf("%c[2K", 27);
		printf("\r%s", cmdLineBuf);
	}
}

const char*
cs_cmdline_getCmdBuf()
{
	return cmdLineBuf;
}
