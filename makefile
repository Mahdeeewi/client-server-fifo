# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
LDFLAGS = 

# Directories
SRCDIR = src
INCDIR = include
OBJDIR = obj
BINDIR = bin

# Source files
SERVER_SRC = $(SRCDIR)/serveur.c
CLIENT_SRC = $(SRCDIR)/client.c

# Object files
SERVER_OBJ = $(OBJDIR)/serveur.o
CLIENT_OBJ = $(OBJDIR)/client.o

# Executables
SERVER_EXEC = $(BINDIR)/serveur
CLIENT_EXEC = $(BINDIR)/client

# Default target
all: directories $(SERVER_EXEC) $(CLIENT_EXEC)

# Create necessary directories
directories:
	@mkdir -p $(OBJDIR) $(BINDIR)

# Build server
$(SERVER_EXEC): $(SERVER_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Build client
$(CLIENT_EXEC): $(CLIENT_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile server object file
$(SERVER_OBJ): $(SERVER_SRC) $(INCDIR)/serv_cli_fifo.h $(INCDIR)/Handlers_Serv.h
	$(CC) $(CFLAGS) -c -o $@ $<

# Compile client object file
$(CLIENT_OBJ): $(CLIENT_SRC) $(INCDIR)/serv_cli_fifo.h $(INCDIR)/Handlers_Cli.h
	$(CC) $(CFLAGS) -c -o $@ $<

# Clean build artifacts
clean:
	rm -rf $(OBJDIR) $(BINDIR)

# Clean and rebuild
rebuild: clean all

# Run server (for testing)
run-server: $(SERVER_EXEC)
	./$(SERVER_EXEC)

# Run client (for testing)
run-client: $(CLIENT_EXEC)
	./$(CLIENT_EXEC)

# Show help
help:
	@echo "Available targets:"
	@echo "  all       - Build server and client (default)"
	@echo "  clean     - Remove all build artifacts"
	@echo "  rebuild   - Clean and rebuild"
	@echo "  run-server- Run the server"
	@echo "  run-client- Run a client"
	@echo "  help      - Show this help message"

.PHONY: all clean rebuild run-server run-client help directories
