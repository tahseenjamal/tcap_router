# Compiler
CC = gcc

# Target binary
TARGET = tcap-router

# Compiler flags
CFLAGS = -O2 -Wall -pthread

# Libraries
LIBS = -lsctp -losmocore -losmo-sigtran

# Source files
SRC = \
main.c \
core/backend_pool.c \
core/worker_pool.c \
core/transaction_table.c \
router/router.c \
network/sctp_server.c \
sigtran/sigtran_stack.c

# Object files
OBJ = $(SRC:.c=.o)

# Default target
all: $(TARGET)

# Link
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(CFLAGS) $(LIBS)

# Compile
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	rm -f $(OBJ) $(TARGET)

# Rebuild
rebuild: clean all

.PHONY: all clean rebuild
