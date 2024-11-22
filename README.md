# Awal'Online
Le serveur référence pour les fan d'Awale !

## Comment se connecter ?


### Débuter 
#### Compilation
Avant de jouer vos meilleurs parties d'Awale, veuillez compiler le projet en utilisant la commande 
```
$ make
```
Cette commande vous permettra de compiler le serveur et le client !<>

#### Exécution 
Une fois la compilation effectuée, vous pouvez lancer les deux codes, en veillant à démarrer d'abord le serveur, puis le client 

[cheminVersServeur]/server [port]

```
$ ./bin/server 8080
```

Vous pouvez ensuite démarrer jusqu'à **10 clients** simultanément !

```
$ ./bin/client 127.0.0.1 8080
```

## Comment jouer ?

Vous avez pu vous connecter au serveur et vous avez saisi votre nom !
Une fois cette étape effectuée, vous pouvez saisir la commande 
`/aide` pour accéder à la liste des commandes disponibles

Si vous souhaitez jouer avec vos amis, vous pouvez utiliser `/defier <nom>`

Si vos amis ne sont pas disponible car ils sont occupés sur la faille, tapez `/defierBot` et vous trouverez un adversaire a la hauteur ! 

### Liste des fonctionnalités

De nombreuses fonctionnalitées ont été implémentée. 
| Commande                      | Effet                                      |
|-------------------------------|--------------------------------------------|
| **/listeJoueurs**             | Affiche la liste des joueurs connectés     |
| **/defier** *nom*           | Envoyer une demande de défi à un joueur    |
| **/defierBot**                | Lancer une partie contre un bot !          |
| **/messageGlobal** *message*| Envoie un message global                   |
| **/modifierBio** *message*  | Modifie la description de votre profil     |
| **/consulterBio** *nom*     | Consulte la bio d'autres joueurs           |
| **/classement**               | Regarde le classement elo du serveur       |
| **/forfait**                     | Abandonne la partie en cours |
| **/aide**                     | Affiche la liste des commandes disponibles |

## Comment ça marche

### Architecture
Pour organiser notre application nous avons utilisé une architecture de code appelé MVC (Model View Controller)

L'objectif était de fournir un code propre qui était facilement réutilisable, incrémentable (évolutif) et compréhensible. 

Pour cela nous avons veillé a décomposer les différentes dépendances du code entre les fichiers. Par exemple, la responsabilité de la vue est de générer les message à échanger, tandis que le lobby manager est composé des différents services utile pour gérer le lobby.


Nous avons également implémenté un pattern commande qui s'est avéré très utile pour ajouter des fonctionnalité sans surcharger notre code. Toutes les fonctionnalités sont donc facilement retrouvable dans notre fichier commande.c
### Communication
Pour assurer une communication fiable entre les différents composants du code, nous avons utilisé un format de message spécifique : 

/[commande] #[expediteur] #[destinataire] [message] 

Ainsi, tous les messages étant formattés de la même manière, il était super pratique de trouver des erreurs au sein des messages envoyés.
Nous avons pu utiliser une unique méthode pour vérifier la syntaxe des messages échangés.

### Gestion du lobby 

Nos objets métiers sont structurés grâce à Lobby. Il contient une liste de joueurs connectés ainsi qu'une liste de parties. Pour retrouver la partie d'un joueur, nous pouvons utiliser l'id partie stockées dans chaque joueurs. 

### Bots

Pour implémenter la fonctionnalité de bots, nous avons utilisé un champs stocké dans chaque joueur définissant si celui-ci était un bot ou un humain. Cela nous permet de vérifier avant chaque coup si on demande au joueur de jouer ou si on demande au bot.