# Variables
CC = gcc
CFLAGS = -Wall -Wextra -I./
SRC_DIR = serveur
OBJ_DIR = ./build
BIN_DIR = ./bin
CLIENT_EXEC = $(BIN_DIR)/client
SERVER_EXEC = $(BIN_DIR)/server

# Récupérer tous les fichiers source
SERVER_SRCS = $(wildcard $(SRC_DIR)/**/*.c $(SRC_DIR)/*.c)
CLIENT_SRCS = client.c util.c

# Générer les fichiers objets
SERVER_OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SERVER_SRCS))
CLIENT_OBJS = $(patsubst %.c, $(OBJ_DIR)/%.o, $(CLIENT_SRCS))

# Règle par défaut
all: setup $(SERVER_EXEC) $(CLIENT_EXEC)

# Créer les dossiers nécessaires
setup:
	mkdir -p $(OBJ_DIR)
	mkdir -p $(BIN_DIR)

# Compiler le serveur
$(SERVER_EXEC): $(SERVER_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compiler le client
$(CLIENT_EXEC): $(CLIENT_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compiler les fichiers objets pour le serveur
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Compiler les fichiers objets pour le client
$(OBJ_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Exécuter le serveur
run_server: $(SERVER_EXEC)
	$(SERVER_EXEC) $(PORT)

# Exécuter le client
run_client: $(CLIENT_EXEC)
	$(CLIENT_EXEC) 127.0.0.1 $(PORT)

# Ajout dynamique du port
PORT ?= 8080

