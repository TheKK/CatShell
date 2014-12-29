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

#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pipe.h"

#define BUF_SIZE 50

static int argc_;
static char** rawArgv_ = NULL;
static char** argv_ = NULL;
static size_t argvLen_ = 2;

static void
cs_parser_enlargeArgv(size_t newSize)
{
	char** new_rawArgv = NULL;
	char** new_argv = NULL;

	if (newSize <= argvLen_)
		return;

	/* rawArgv_ */
	new_rawArgv = (char**) malloc(sizeof(char*) * newSize);
	if (!new_rawArgv) {
		fprintf(stderr, "Out of memory!!\n");
		exit(1);
	}

	memcpy(new_rawArgv, rawArgv_, sizeof(char*) * argvLen_);

	for (size_t i = argvLen_; i < newSize; ++i)
		new_rawArgv[i] = (char*) malloc(sizeof(char) * 20);

	/* argv_ */
	new_argv = (char**) malloc(sizeof(char*) * newSize);
	if (!new_argv) {
		fprintf(stderr, "Out of memory!!\n");
		exit(1);
	}

	free(rawArgv_);
	free(argv_);

	rawArgv_ = new_rawArgv;
	argv_ = new_argv;
	argvLen_ = newSize;
}

int
cs_parser_init()
{
	rawArgv_ = (char**) malloc(sizeof(char*) * argvLen_);
	if (!rawArgv_) {
		fprintf(stderr, "Out of memory!!\n");
		return -1;
	}

	/* XXX Bad hack */
	for (size_t i = 0; i < argvLen_; ++i)
		rawArgv_[i] = (char*) malloc(sizeof(char) * 20);

	argv_ = (char**) malloc(sizeof(char*) * argvLen_);
	if (!argv_) {
		fprintf(stderr, "Out of memory!!\n");
		return -1;
	}

	return 0;
}

void
cs_parser_quit()
{
	for (size_t i = 0; i < argvLen_; ++i) {
		free(rawArgv_[i]);
		rawArgv_[i] = NULL;
	}

	free(rawArgv_);
	rawArgv_ = NULL;

	free(argv_);
	argv_ = NULL;

	argvLen_ = 0;
}


void
cs_parser_parse(const char* cmdLine)
{
	char* tok = NULL;
	char* cmdLineCopy = NULL;
	int originArgc;

	cmdLineCopy = (char*) malloc(sizeof(char) * strlen(cmdLine) + 1);
	strncpy(cmdLineCopy, cmdLine, strlen(cmdLine) + 1);

	/* Split tokens */
	argc_ = 0;
	tok = strtok(cmdLineCopy, " ");
	while (tok) {
		if (strlen(tok) > strlen(rawArgv_[argc_])) {
			rawArgv_[argc_] =
				(char*) realloc(rawArgv_[argc_],
						strlen(tok) + 1);
		}
		strncpy(rawArgv_[argc_], tok, strlen(tok) + 1);

		++argc_;
		if (argc_ == argvLen_)
			cs_parser_enlargeArgv(argvLen_ * 2);

		tok = strtok(NULL, " ");
	}

	/* Connect ptr to rawArgv_ */
	for (int i = 0; i < argc_; ++i)
		argv_[i] = rawArgv_[i];
	argv_[argc_] = NULL;

	/* Handle special keywork */
	originArgc = argc_;
	for (int i = 0; i < originArgc; ++i) {
		if (strstr(argv_[i], ">")) {
			FILE* fd = NULL;

			fd = fopen(argv_[i + 1], "wb");
			cs_pipe_setOutputStream(fd);

			argc_ -= 2;
			for (int j = i; j < originArgc; ++j) {
				char* tmp;

				tmp = argv_[j];
				argv_[j] = argv_[j + 2];
				argv_[j + 2] = tmp;
			}
			break;
		} else {
			cs_pipe_setDefaultOutputStream();
		}
	}

	free(cmdLineCopy);
	cmdLineCopy = NULL;
	tok = NULL;
}

int
cs_parser_getArgc()
{
	return argc_;
}

char**
cs_parser_getArgv()
{
	return argv_;
}
