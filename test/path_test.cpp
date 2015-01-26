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

#include <gtest/gtest.h>

extern "C"
{
#include "dirent.h"
#include "errno.h"
#include "string.h"
#include "path.h"
}

class PathTest : public ::testing::Test
{
protected:
	virtual void SetUp()
	{
		ASSERT_EQ(cs_path_init(), 0);
	}

	virtual void TearDown()
	{
		cs_path_quit();
	}
};

TEST_F (PathTest, getWorkingPath)
{
	char* cwd = NULL;
	size_t len = 30;

	cwd = (char*) malloc(sizeof(char) * len);
	while (getcwd(cwd, len) == NULL) {
		len *= 2;
		cwd = (char*) realloc(cwd, sizeof(char) * len);
	}

	ASSERT_TRUE(cs_path_getWorkingPath() != nullptr);
	ASSERT_STREQ(cs_path_getWorkingPath(), cwd);

	free(cwd);
}

TEST_F (PathTest, changeWorkingPath)
{
	const char existPath[] = "/usr";
	const char nonexistPath[] = "/vvvvvvsdjfkdssdjkv";

	ASSERT_EQ(cs_path_changeWorkingPath(existPath), 0);
	ASSERT_EQ(cs_path_changeWorkingPath(nonexistPath), -1);
}

TEST_F (PathTest, mkdir)
{
	const char newDirName[] = "/tmp/kyoka";
	DIR* dirp = nullptr;

	/* If directory exist, remove it */
	dirp = opendir(newDirName);
	if (dirp) {
		ASSERT_EQ(rmdir(newDirName), 0);
		closedir(dirp);
	}

	ASSERT_EQ(cs_path_mkdir(newDirName), 0);

	dirp = opendir(newDirName);
	ASSERT_TRUE(dirp != nullptr);
	closedir(dirp);

	ASSERT_EQ(rmdir(newDirName), 0);
}
