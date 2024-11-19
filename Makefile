```
CC = gcc
CFLAGS = 
BIN_DIR = bin

# Création des exécutables
all: $(BIN_DIR)/server $(BIN_DIR)/client

# Compilation du serveur
$(BIN_DIR)/server: serveur/controller/controller.c serveur/model/donnees/awale.c serveur/model/services/lobbyManager.c serveur/model/services/messageManager.c serveur/model/services/partieManager.c serveur/vue/echange.c serveur/controller/commandes.c
    @mkdir -p $(BIN_DIR)
    $(CC) $(CFLAGS) -o $(BIN_DIR)/server serveur/controller/controller.c serveur/model/donnees/awale.c serveur/model/services/lobbyManager.c serveur/model/services/messageManager.c serveur/model/services/partieManager.c serveur/vue/echange.c serveur/controller/commandes.c

# Compilation du client
$(BIN_DIR)/client: client/controller/controller.c client/model/messageManager.c client/vue/display.c
<<<<<<< HEAD
    @mkdir -p $(BIN_DIR)
    $(CC) $(CFLAGS) -o $(BIN_DIR)/client client/controller/controller.c client/model/messageManager.c client/vue/display.c
=======
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/client client/controller/controller.c client/model/messageManager.c client/vue/display.c
>>>>>>> origin/merge

# Nettoyage des fichiers compilés
clean:
    rm -rf $(BIN_DIR)