# Компилятор и флаги
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude -ImyTerm/include -ImyBigChars/include -ImyReadKey/include -ISimpleAssembler/include
LDFLAGS = -Llib -lmySimpleComputer -lmyTerm -lmyBigChars -lmyReadKey -lncurses

# Директории
SRC_DIR_MYSC = mySimpleComputer
SRC_DIR_MYTERM = myTerm/src
SRC_DIR_MYBIGCHAR = myBigChars/src
SRC_DIR_MYREADKEY = myReadKey/src
SRC_DIR_MYASSEMBLER  = SimpleAssembler/src
SRC_DIR_BASIC2SA = SimpleAssembler/src
SRC_DIR_CONSOLE = consoles
OBJ_DIR = obj
LIB_DIR = lib

INCLUDE_DIR = include
INCLUDE_DIR_MYTERM = myTerm/include
INCLUDE_DIR_MYBIGCHAR = myBigChars/include
INCLUDE_DIR_MYREADKEY = myReadKey/include
INCLUDE_DIR_MYASSEMBLER = SimpleAssembler/include

LIB_SC = $(LIB_DIR)/libmySimpleComputer.a
LIB_TERM = $(LIB_DIR)/libmyTerm.a
LIB_CHARS = $(LIB_DIR)/libmyBigChars.a
LIB_READKEY = $(LIB_DIR)/libmyReadKey.a
LIB_CONSOLE = $(LIB_DIR)/libCons.a
LIB_ASSEMBLER = $(LIB_DIR)/libAssembler.a
CONSOLE = $(SRC_DIR_CONSOLE)/console.exe

# MySimpleComputer
SRCS_LIB_SC = $(SRC_DIR_MYSC)/memory.c $(SRC_DIR_MYSC)/registration.c $(SRC_DIR_MYSC)/control.c $(SRC_DIR_MYSC)/print.c $(SRC_DIR_MYSC)/globals.c
OBJS_LIB_SC = $(patsubst $(SRC_DIR_MYSC)/%.c, $(OBJ_DIR)/%.o, $(SRCS_LIB_SC))

# MyTerm
SRCS_LIB_TERM = $(SRC_DIR_MYTERM)/myTerm.c
OBJS_LIB_TERM = $(patsubst $(SRC_DIR_MYTERM)/%.c, $(OBJ_DIR)/%.o, $(SRCS_LIB_TERM))

# MyBigChars
SRCS_LIB_MYBIGCHAR = $(SRC_DIR_MYBIGCHAR)/myBigChars.c
OBJS_LIB_MYBIGCHAR = $(patsubst $(SRC_DIR_MYBIGCHAR)/%.c, $(OBJ_DIR)/%.o, $(SRCS_LIB_MYBIGCHAR))

# MyReadKey
SRCS_LIB_MYREADKEY = $(SRC_DIR_MYREADKEY)/myReadKey.c
OBJS_LIB_MYREADKEY = $(patsubst $(SRC_DIR_MYREADKEY)/%.c, $(OBJ_DIR)/%.o, $(SRCS_LIB_MYREADKEY))

# SimpleAssembler
SRCS_LIB_ASSEMBLER= $(SRC_DIR_MYASSEMBLER)/assembler.c
OBJS_LIB_ASSEMBLER = $(patsubst $(SRC_DIR_MYASSEMBLER)/%.c, $(OBJ_DIR)/%.o, $(SRCS_LIB_ASSEMBLER))

# Console
SRCS_CONSOLE = $(SRC_DIR_CONSOLE)/console.c
OBJ_CONSOLE = $(patsubst $(SRC_DIR_CONSOLE)/%.c, $(OBJ_DIR)/%.o, $(SRCS_CONSOLE))

SAT_SRC = SimpleAssembler/src/sat.c
SAT_BIN = sat

all: basic2sa assembler $(LIB_SC) $(LIB_TERM) $(LIB_CHARS) $(LIB_READKEY) $(CONSOLE)

$(CONSOLE): $(OBJ_CONSOLE) $(LIB_SC) $(LIB_CHARS) $(LIB_READKEY) $(LIB_TERM)
	$(CC) $(CFLAGS) -o $@ $(OBJ_CONSOLE) $(LDFLAGS)

test: $(SRC_DIR_CONSOLE)/console.c
	sh test_terminal.sh

test2: $(SRC_DIR_CONSOLE)/console.c
	sh test_terminal2.sh

basic2sa: $(SRC_DIR_BASIC2SA)/basic2sa.c
	$(CC) $(CFLAGS) -o $@ $<

assembler: $(SRC_DIR_MYASSEMBLER)/assembler.c $(LIB_SC)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

sat: all
	./basic2sa test.sb test.sa
	./assembler test.sa test.o
	@echo "Готово! Проверьте test.sa"

run: $(CONSOLE)
	./$(CONSOLE)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(LIB_DIR):
	mkdir -p $(LIB_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR_MYSC)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR_MYTERM)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR_MYBIGCHAR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR_MYREADKEY)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR_MYASSEMBLER)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR_CONSOLE)/console.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB_SC): $(OBJS_LIB_SC) | $(LIB_DIR)
	ar rcs $@ $^

$(LIB_TERM): $(OBJS_LIB_TERM) | $(LIB_DIR)
	ar rcs $@ $^

$(LIB_CHARS): $(OBJS_LIB_MYBIGCHAR) | $(LIB_DIR)
	ar rcs $@ $^

$(LIB_READKEY): $(OBJS_LIB_MYREADKEY) | $(LIB_DIR)
	ar rcs $@ $^

$(LIB_ASSEMBLER): $(OBJS_LIB_ASSEMBLER) | $(LIB_DIR)
	ar rcs $@ $^

$(LIB_CONSOLE): $(OBJ_CONSOLE)| $(LIB_DIR)
	ar rcs $@ $^

clean:
	rm -f basic2sa console test.sa test.o
	rm -rf $(OBJ_DIR) $(LIB_DIR) $(SRC_DIR_CONSOLE)/*.o console $(SRC_DIR_CONSOLE)/*.exe

.PHONY: all clean sat test test2