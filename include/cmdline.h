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

#define CS_KEY_RETURN 10
#define CS_KEY_BACKSPACE 127
#define CS_KEY_TAB 9
#define CS_KEY_CTLP 16
#define CS_KEY_CTLN 14

int cs_cmdline_init();
void cs_cmdline_quit();

void cs_cmdline_handleUserInput();
const char* cs_cmdline_getCmdBuf();
