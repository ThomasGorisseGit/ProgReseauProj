CC = gcc
CFLAGS = 
BIN_DIR = bin

# Création des exécutables
all: $(BIN_DIR)/server $(BIN_DIR)/client

# Compilation du serveur
$(BIN_DIR)/server: serveur/controller/controller.c serveur/model/donnees/awale.c serveur/model/services/lobbyManager.c serveur/model/services/messageManager.c serveur/model/services/partieManager.c serveur/vue/echange.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/server serveur/controller/controller.c serveur/model/donnees/awale.c serveur/model/services/lobbyManager.c serveur/model/services/messageManager.c serveur/model/services/partieManager.c serveur/vue/echange.c

# Compilation du client
$(BIN_DIR)/client: util.c client.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/client util.c client.c

# Nettoyage des fichiers compilés
clean:
	rm -rf $(BIN_DIR)
