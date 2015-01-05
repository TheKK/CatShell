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
#include <string.h>

#include "path.h"
#include "history.h"
#include "color.h"

static char* cmdLineBuf = NULL;
static size_t cmdLineBufLen = 10;
static char* cmdLineBufCurPos = NULL;
static char* cmdLineBufTailPos = NULL;
static struct termios oldt_, newt_;

static void
cs_cmdline_printPromote()
{
	printf("\n" KBLU "# " KCYN "%s " KWHT "in " KYEL "%s\n" RESET,
	       getenv("USER"), cs_path_getWorkingPath());
	printf(KRED "$ " RESET);
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
	cmdLineBuf = (char*) malloc(sizeof(char) * cmdLineBufLen);
	if (!cmdLineBuf) {
		return -1;
	}

	cmdLineBufCurPos = cmdLineBuf;
	cmdLineBufTailPos = cmdLineBuf + cmdLineBufLen - 1;

	tcgetattr(STDIN_FILENO, &oldt_);
	newt_ = oldt_;
	newt_.c_lflag &= !ECHO;
	tcsetattr(STDIN_FILENO, TCSANOW, &newt_);

	return 0;
}

void
cs_cmdline_quit()
{
	free(cmdLineBuf);
	cmdLineBuf = NULL;
}

void
cs_cmdline_enableEcho()
{
	tcsetattr(STDIN_FILENO, TCSANOW, &newt_);
}

void
cs_cmdline_disableEcho()
{
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt_);
}

void
cs_cmdline_handleUserInput()
{
	int c;
	const char* historyCmd = NULL;

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

			cs_history_addNewHistory(cmdLineBuf);
			return;
		case CS_KEY_CTLP:
			historyCmd = cs_history_getPrevHistory();
			if (strlen(historyCmd) > cmdLineBufLen) {
				cs_cmdline_enlargeCmdLineBuf(cmdLineBufLen * 2);
			}

			strcpy(cmdLineBuf, historyCmd);
			break;
		case CS_KEY_CTLN:
			historyCmd = cs_history_getNextHistory();
			if (strlen(historyCmd) > cmdLineBufLen) {
				cs_cmdline_enlargeCmdLineBuf(cmdLineBufLen * 2);
			}

			strcpy(cmdLineBuf, historyCmd);
			break;
		default:
			if (!(
					((c >= '0') && (c <= '9')) ||
					((c >= 'a') && (c <= 'z')) ||
					((c >= 'A') && (c <= 'Z')) ||
					(c == ' '))
			)
				break;

			if (cmdLineBufCurPos == cmdLineBufTailPos)
				cs_cmdline_enlargeCmdLineBuf(cmdLineBufLen * 2);

			*cmdLineBufCurPos = c;
			++cmdLineBufCurPos;
			*cmdLineBufCurPos = '\0';
			break;
		}

		/* XXX gonna find out other method */
		printf("%c[2K", 27);
		printf("\r" KRED "$ " RESET "%s", cmdLineBuf);
	}
}

const char*
cs_cmdline_getCmdBuf()
{
	return cmdLineBuf;
}
