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

static char buf[BUF_SIZE];

static char* rawUserInput_ = NULL;
static size_t rawUserInputLen_ = BUF_SIZE;
static int argc_;
static char** argv_ = NULL;
static size_t argvLen_ = 2;

static void
cs_parser_enlargeArgv(size_t newSize)
{
	char** ptrPtr = NULL;

	if (newSize <= argvLen_)
		return;

	ptrPtr = (char**) malloc(sizeof(char*) * newSize);
	if (!ptrPtr) {
		fprintf(stderr, "Out of memory!!\n");
		exit(1);
	}

	memcpy(ptrPtr, argv_, sizeof(char*) * argvLen_);

	for (size_t i = argvLen_; i < newSize; ++i)
		ptrPtr[i] = (char*) malloc(sizeof(char) * 20);

	free(argv_);
	argv_ = ptrPtr;
	argvLen_ = newSize;
}

static void
cs_parser_enlargeRawUserInpu(size_t newSize)
{
	char* ptr = NULL;

	if (newSize <= rawUserInputLen_)
		return;

	ptr = (char*) malloc(sizeof(char) * newSize);
	if (!ptr) {
		fprintf(stderr, "Out of memory!!\n");
		exit(1);
	}

	strncpy(ptr, rawUserInput_, rawUserInputLen_);

	free(rawUserInput_);
	rawUserInput_ = ptr;
	rawUserInputLen_ = newSize;
}

int
cs_parser_init()
{
	rawUserInput_ = (char*) malloc(sizeof(char) * rawUserInputLen_);
	if (!rawUserInput_) {
		fprintf(stderr, "Out of memory!!\n");
		return -1;
	}

	argv_ = (char**) malloc(sizeof(char*) * argvLen_);
	if (!argv_) {
		fprintf(stderr, "Out of memory!!\n");
		return -1;
	}

	/* XXX Bad hack */
	for (size_t i = 0; i < argvLen_; ++i)
		argv_[i] = (char*) malloc(sizeof(char) * 20);

	return 0;
}

void
cs_parser_quit()
{
	free(rawUserInput_);
	rawUserInput_ = NULL;
	rawUserInputLen_ = 0;

	for (size_t i = 0; i < argvLen_; ++i) {
		free(argv_[i]);
		argv_[i] = NULL;
	}
	free(argv_);
	argv_ = NULL;
	argvLen_ = 0;
}


void
cs_parser_readUserInput()
{
	size_t currentLen = 0;
	char* tok = NULL;
	char* rawUserInputCopy = NULL;
	int originArgc;

	/* Read */
	memset(rawUserInput_, 0, rawUserInputLen_);
	while (fgets(buf, BUF_SIZE, stdin)) {
		currentLen += BUF_SIZE;
		if (currentLen > rawUserInputLen_)
			cs_parser_enlargeRawUserInpu(rawUserInputLen_ * 2);

		strncat(rawUserInput_, buf, BUF_SIZE);

		if (strchr(buf, '\n'))
			break;
	}
	rawUserInput_[strlen(rawUserInput_) - 1] = '\0';

	/* Parse */
	/* token */
	rawUserInputCopy = (char*) malloc(sizeof(char) * rawUserInputLen_);
	strncpy(rawUserInputCopy, rawUserInput_, rawUserInputLen_);

	argc_ = 0;
	tok = strtok(rawUserInputCopy, " ");
	while (tok) {
		/* TODO try to record size of argv_[] entries */
		/*if (strlen(tok) > strlen(argv_[argc_])) {*/
			/*argv_[argc_] =*/
				/*(char*) realloc(argv_[argc_], strlen(tok) + 1);*/
		/*}*/
		strncpy(argv_[argc_], tok, strlen(tok) + 1);

		++argc_;
		if (argc_ == argvLen_)
			cs_parser_enlargeArgv(argvLen_ * 2);

		tok = strtok(NULL, " ");
	}

	/* special keywork */
	originArgc = argc_;
	for (int i = 0; i < originArgc; ++i) {
		if (strstr(argv_[i], ">")) {
			FILE* fd = NULL;

			fd = fopen(argv_[i +1], "wb");
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

	free(rawUserInputCopy);
	rawUserInputCopy = NULL;
	tok = NULL;
}


const char*
cs_parser_getRawUserInput()
{
	return rawUserInput_;
}

int
cs_parser_getArgc()
{
	return argc_;
}

const char**
cs_parser_getArgv()
{
	return (const char**) argv_;
}
