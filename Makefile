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

CXX = gcc
CXXFLAG = -Wall -std=gnu99 -g

# Direcotries
OUT_DIR = obj
SRC_DIR = src
INC_DIR = include
TEST_DIR = test

# Source files and header files
SRC = $(wildcard $(addsuffix /*.c, $(SRC_DIR)))
INCLUDE = $(addprefix -I, $(INC_DIR))
OBJ := $(addprefix $(OUT_DIR)/, $(SRC:.c=.o))
OBJ := $(filter-out $(OUT_DIR)/$(SRC_DIR)/main.o, $(OBJ))

MAIN_OBJ = $(OUT_DIR)/$(SRC_DIR)/main.o

TEST_SRC = $(wildcard $(addsuffix /*.c, $(TEST_DIR)))
TEST_OBJ = $(addprefix $(OUT_DIR)/, $(TEST_SRC:.c=.o))
TESTS = $(addprefix $(OUT_DIR)/, $(TEST_SRC:.c=))

# Libs flags
LIB = -lpthread

OUT_EXE = cs

all: $(OUT_EXE)
	@echo "===========[[Everything done!!]]============"

dotest: $(TESTS)
	@echo "===========[[Start test]]============"
	@$(foreach test, $^, echo [$(notdir $(test))]; ./$(test); echo;)
	@echo "===========[[Test all done]]============"

$(OUT_EXE): $(OBJ) $(MAIN_OBJ)
	@echo "    LD    " $(notdir $@)
	@$(CXX) $^ $(CXXFLAG) $(LIB) -o $@

$(OUT_DIR)/$(TEST_DIR)/%: $(OBJ) $(OUT_DIR)/$(TEST_DIR)/%.o
	@echo "    LD    " $(notdir $@)
	@$(CXX) $^ $(CXXFLAG) $(INCLUDE) $(LIB) -lcmocka -o $@

$(OUT_DIR)/%.o: %.c
	@echo "    CC    " $(notdir $@)
	@mkdir -p $(dir $@)
	@$(CXX) -c $< $(CXXFLAG) $(INCLUDE) -o $@

.PHONY: clean pot
clean:
	@rm -frv $(OUT_DIR)/*
	@echo "===========[[Everything removed!!]]============"

pot:
	@xgettext $(SRC) -k_ -o ./pot/CatShell.pot
	@echo "===========[[POT file created!!]]============"
