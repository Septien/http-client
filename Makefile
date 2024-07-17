# Building
CC = /usr/bin/gcc

CFLAGS = -fpic -fstack-protector-strong -Werror -Wall -Wextra -pedantic -g3 -Og -std=gnu11 -fstack-protector-all -ffunction-sections -fwrapv -Wstrict-overflow -fno-common -fdata-sections -Wmissing-include-dirs -fno-delete-null-pointer-checks -fdiagnostics-color -Wstrict-prototypes -Wold-style-definition -gz -Wformat=2 -Wformat-overflow -Wformat-truncation

INCLUDE = ./include
SRC = ./src
TST = ./tests
BIN = ./bin
# Add the tests include
INCLUDE += $(TST)/include

CFLAGS += $(addprefix -I, $(INCLUDE))
$(info $$BIN is $(BIN))
$(info $$SRC is $(SRC))

MKDIR_BIN = mkdir -p $(BIN)
MKDIR_SUB = mkdir -p $(BIN)/src $(BIN)/tests

test: mkd $(BIN)/main.o

build: mkd $(BIN)/src/http-client.o

mkd :
	$(MKDIR_BIN)
	$(MKDIR_SUB)

# Building the source
$(BIN)/src/http-client.o : $(SRC)/http-client.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN)/src/http-client.o : ./include/http-client.h

# Building cUnit
$(BIN)/src/cUnit.o : $(SRC)/cUnit.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN)/src/cUnit.o : ./include/cUnit.h

# Building the tests
$(BIN)/tests/http-client-test.o : $(TST)/http-client-test.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN)/tests/%.o : $(TST)/include/%.h

# Building main
$(BIN)/main.o : $(BIN)/src/http-client.o $(BIN)/src/cUnit.o $(BIN)/tests/http-client-test.o main.c
	$(CC) $(CFLAGS) -c main.c -o $@
	$(CC) -o $(BIN)/main $(BIN)/main.o $(BIN)/src/http-client.o $(BIN)/src/cUnit.o $(BIN)/tests/http-client-test.o
	