# Awal'Online
Le serveur référence pour les fan d'Awale !

## Comment se connecter ?


### Débuter 
#### Compilation
Avant de jouer vos meilleurs parties d'Awale, veuillez compiler le projet en utilisant la commande 
```
$ make
```
Cette commande vous permettra de compiler le serveur et le client !

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

### Comment jouer ?

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
| **/defier** *<nom>*           | Envoyer une demande de défi à un joueur    |
| **/defierBot**                | Lancer une partie contre un bot !          |
| **/messageGlobal** *<message>*| Envoie un message global                   |
| **/modifierBio** *<message>*  | Modifie la description de votre profil     |
| **/consulterBio** *<nom>*     | Consulte la bio d'autres joueurs           |
| **/classement**               | Regarde le classement elo du serveur       |
| **/aide**                     | Affiche la liste des commandes disponibles |