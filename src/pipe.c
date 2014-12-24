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

#include "pipe.h"

#include <stdio.h>
#include <stdlib.h>

static FILE* outputFd_ = NULL;

int
cs_pipe_init()
{
	outputFd_ = stdout;

	return 0;
}

void
cs_pipe_quit()
{
}

FILE*
cs_pipe_getOutputStream()
{
	return outputFd_;
}

void
cs_pipe_setOutputStream(FILE* stream)
{
	if (stream)
		outputFd_ = stream;
	else
		outputFd_ = stdout;
}

void
cs_pipe_setDefaultOutputStream()
{
	if (outputFd_ != stdout) {
		fclose(outputFd_);
		outputFd_ = stdout;
	}
}

