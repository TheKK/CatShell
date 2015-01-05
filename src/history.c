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

#include "history.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static FILE* historyFd = NULL;
static char* historyCmdBuf = NULL;
static size_t historyCmdBufLen = 0;

static int totalHistoryLine = 0;
static int currentHistoryLine = 0;

static void
cs_history_getHistoryLine_(int targetLine)
{
	int c = 'k';
	int currentLine = 1;

	rewind(historyFd);

	do {
		if (c == '\n')
			++currentLine;

		if (currentLine == targetLine) {
			getline(&historyCmdBuf, &historyCmdBufLen, historyFd);
			historyCmdBuf[strlen(historyCmdBuf) - 1] = '\0';
			return;
		}

	} while ((c = fgetc(historyFd)) != EOF);
}

int
cs_history_init()
{
	int c;

	historyFd = fopen("hist", "ab+");
	if (!historyFd)
		return -1;

	while ((c = fgetc(historyFd)) != EOF) {
		if (c == '\n')
			++totalHistoryLine;
	}

	currentHistoryLine = totalHistoryLine + 1;

	return 0;
}

void
cs_history_quit()
{
	if (historyFd)
		fclose(historyFd);
	historyFd = NULL;

	if (historyCmdBuf)
		free(historyCmdBuf);
	historyCmdBuf = NULL;
}

void
cs_history_showAllHistory()
{
	fseek(historyFd, 0L, SEEK_SET);

	while (getline(&historyCmdBuf, &historyCmdBufLen, historyFd) != -1)
		printf("%s", historyCmdBuf);
}

void
cs_history_addNewHistory(char* const cmd)
{
	int result;

	++totalHistoryLine;
	currentHistoryLine = totalHistoryLine + 1;

	fseek(historyFd, 0L, SEEK_END);

	result = fputs(cmd, historyFd);
	if (result == EOF) {
		fprintf(stderr, "Error: this should not happend\n");
		fprintf(stderr, "Write histroy falied!\n");
		exit(1);
	}

	result = fputc('\n', historyFd);
	if (result == EOF) {
		fprintf(stderr, "Error: this should not happend\n");
		fprintf(stderr, "Write histroy falied!\n");
		exit(1);
	}
}

char* const
cs_history_getPrevHistory()
{
	if (currentHistoryLine != 1)
		--currentHistoryLine;

	cs_history_getHistoryLine_(currentHistoryLine);

	return historyCmdBuf;
}

char* const
cs_history_getNextHistory()
{
	if (currentHistoryLine != totalHistoryLine + 1)
		++currentHistoryLine;

	if (currentHistoryLine == totalHistoryLine + 1)
		strcpy(historyCmdBuf, "");
	else
		cs_history_getHistoryLine_(currentHistoryLine);

	return historyCmdBuf;
}
