 #
 # CatShell
 # Copyright (C) 2014 TheKK <thumbd03803@gmail.com>
 #
 # This program is free software; you can redistribute it and/or modify
 # it under the terms of the GNU General Public License as published by
 # the Free Software Foundation; either version 2 of the License, or
 # (at your option) any later version.
 #
 # This program is distributed in the hope that it will be useful,
 # but WITHOUT ANY WARRANTY; without even the implied warranty of
 # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 # GNU General Public License for more details.
 #
 # You should have received a copy of the GNU General Public License
 # along with this program; if not, write to the Free Software
 # Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 #

CC = gcc
CC_FLAGS = -Wall -std=gnu99 -g

CXX = g++
CXX_FLAGS = -Wall -std=c++11 -g

# Libs flags
LIBS = -lpthread -lgtest

# Direcotries
BUILD_DIR = build
SRC_DIR = src
INC_DIR = include
MK_DIR = mk

# Source files and header files
SRCS = $(wildcard $(addsuffix /*.c, $(SRC_DIR)))
INCLUDES = $(addprefix -I, $(INC_DIR))
OBJS = $(addprefix $(BUILD_DIR)/, $(SRCS:.c=.o))
MKS = $(wildcard $(MK_DIR)/*.mk)

OUT_EXE = cs

all: $(OUT_EXE)
	@echo "===========[[Everything done!!]]============"

include $(MKS)

$(OUT_EXE): $(OBJS)
	@echo "    LD    " $(notdir $@)
	@$(CC) $^ $(CC_FLAGS) $(LIBS) -o $@

$(BUILD_DIR)/%.o: %.c
	@echo "    CC    " $(notdir $@)
	@mkdir -p $(dir $@)
	@$(CC) -c $< $(CC_FLAGS) $(INCLUDES) -o $@

$(BUILD_DIR)/%.o: %.cpp
	@echo "    CXX   " $(notdir $@)
	@mkdir -p $(dir $@)
	@$(CXX) -c $< $(CXX_FLAGS) $(INCLUDES) -o $@

.PHONY: clean pot
clean:
	@rm -frv $(BUILD_DIR)
	@echo "===========[[Everything removed!!]]============"

pot:
	@xgettext $(SRCS) -k_ -o ./pot/CatShell.pot
	@echo "===========[[POT file created!!]]============"
