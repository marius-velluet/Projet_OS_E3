# Projet_OS_E3

Simulation d'une application de menus de restaurants en C, où un client, un routeur et un serveur de données sont trois processus distincts qui communiquent par sockets TCP. Projet réalisé en binôme à l'ESIEE Paris (E3), dans le cadre du cours de systèmes d'exploitation.

> Cette version correspond à la branche `Branch_Utilisation_Sockets_TCP`. Une autre branche, `Branch_Utilisation_FIFOs`, implémente la même application avec des tubes nommés.

## Principe

L'utilisateur saisit un code de 12 chiffres, comme s'il scannait un QR code posé sur la table d'un restaurant. Ce code identifie un lieu et un menu. La requête traverse ensuite toute la chaîne :

```
Client  ──►  Routeur  ──►  Serveur de données
Client  ◄──  Routeur  ◄──  Serveur de données
```

Si le code correspond à un restaurant connu, le serveur affiche un message de bienvenue et ouvre l'image du menu avec `feh`. Sinon, il renvoie "Menu introuvable".

## Architecture

Le programme lance trois processus. `main.c` crée le routeur et le serveur de données avec `fork()`, et le processus parent devient le client, ce qui permet de saisir le code au clavier.

| Fichier | Rôle |
|---|---|
| `main.c` | Lance le routeur, puis le serveur, puis le client (avec une pause d'une seconde entre chaque étape) |
| `ID.h` | Client : lit le code au clavier, se connecte au routeur, envoie la requête et affiche la réponse |
| `routage.h` | Routeur : ouvre deux sockets d'écoute, accepte le serveur puis le client, et relaie la requête et la réponse |
| `serveur.h` | Serveur de données : se connecte au routeur, décode le code, retrouve le restaurant et affiche son menu |
| `threading.h` | File de messages partagée (tampon circulaire protégé par mutex et variable de condition) |
| `images/` | Images des menus affichées par le serveur |
| `Makefile` | Compilation du projet |

### Communication réseau

Toutes les connexions se font en local (`127.0.0.1`) avec des sockets TCP (`AF_INET`, `SOCK_STREAM`).

| Port | Écouté par | Connecté depuis |
|---|---|---|
| `9090` | Routeur (`PORT_CLIENTS`) | Client |
| `9091` | Routeur (`PORT_SERVERS`) | Serveur de données |

Le routeur est le seul processus à écouter (`bind`, `listen`, `accept`), le client et le serveur s'y connectent avec `connect`. Il accepte d'abord le serveur de données, puis le client, afin que le serveur soit prêt avant l'arrivée de la première requête.

Déroulement d'un échange :

1. Le routeur ouvre ses deux sockets d'écoute (ports `9090` et `9091`).
2. Le serveur de données se connecte au port `9091`.
3. Le client se connecte au port `9090` et envoie le code.
4. Le routeur transmet le code au serveur de données.
5. Le serveur répond (`OK` ou `Menu introuvable`) et le routeur renvoie la réponse au client, qui l'affiche.

### Threads côté serveur

Le serveur de données utilise deux threads qui se partagent une file de messages (10 emplacements maximum) :

- **`reader_thread`** lit les requêtes arrivant du routeur sur la socket et les place dans la file.
- **`worker_thread`** retire les requêtes de la file, les traite et renvoie le résultat au routeur.

L'accès à la file est protégé par un `pthread_mutex_t`, et `pthread_cond_t` endort le worker tant que la file est vide.

## Format du code

Le code est composé de 12 chiffres :

```
XXXX  YYYY  ZZZZ
      │     └── identifiant du menu
      └──────── identifiant du lieu
```

Seuls les chiffres 5 à 8 (lieu) et 9 à 12 (menu) sont utilisés par le serveur.

### Restaurants disponibles

| Lieu (`YYYY`) | Menu (`ZZZZ`) | Restaurant | Ville | Image |
|---|---|---|---|---|
| `0001` | `1234` | Burger Roi | Noisy | `menu1.jpeg` |
| `0001` | `5678` | Bahn mi | Noisy | `menu4.jpeg` |
| `0002` | `1234` | Tasty Crousty | Bussy | `menu2.jpeg` |
| `0002` | `5678` | Restaurant volonté Thaï | Bussy | `menu5.jpeg` |
| `0003` | `5678` | Shushii | Créteil | `menu3.jpeg` |
| `0003` | `9123` | Coré1 | Créteil | `menu6.jpeg` |

## Prérequis

- Linux (ou WSL)
- `gcc` et `make`
- La bibliothèque `pthread` (incluse avec la glibc)
- [`feh`](https://feh.finalrewind.org/) pour afficher les images, ainsi qu'un environnement graphique
- Les ports `9090` et `9091` libres sur la machine

```bash
sudo apt install gcc make feh
```

## Compilation et exécution

```bash
git clone https://github.com/marius-velluet/Projet_OS_E3.git
cd Projet_OS_E3
git checkout Branch_Utilisation_Sockets_TCP

make
./mon_programme
```

Le programme lance le routeur et le serveur, puis demande le code. Lance-le depuis la racine du dépôt, car les images sont chargées avec un chemin relatif (`images/`).

Exemple avec `000100011234` (lieu `0001`, menu `1234`) :

```
[Routeur] Démarré.
[Routeur] En écoute (clients:9090 / serveurs:9091).
[Routeur] Serveur de données connecté.
[Serveur] Connecté au routeur.
[Client] Prêt. Entrez le code (12 chiffres) : 000100011234
[Client] Connecté au routeur.
[Routeur] Reçu du client : 000100011234
[Reader] Reçu : 000100011234
[Worker] Traitement : 000100011234
[Routeur] Reçu du serveur de données : OK
Bienvenue au restaurant Burger Roi à Noisy
[Client] Résultat reçu : OK
```

L'image du menu s'ouvre dans une fenêtre `feh`.

### Commandes du Makefile

| Commande | Effet |
|---|---|
| `make` | Compile et produit l'exécutable `mon_programme` |
| `make clean` | Supprime les fichiers objets |
| `make fclean` | Supprime les fichiers objets et l'exécutable |
| `make re` | Recompile tout depuis zéro |

Le projet est compilé avec `-Wall -Wextra -Werror`.

## Concepts abordés

- Création de processus (`fork`, `waitpid`)
- Programmation réseau avec les sockets TCP (`socket`, `bind`, `listen`, `accept`, `connect`)
- Architecture client / routeur / serveur
- Programmation concurrente avec `pthread`
- Synchronisation par mutex et variables de condition (problème producteur/consommateur)

## Limites connues

- Le programme ne se termine pas tout seul après un échange : une fois la connexion fermée par le routeur, le thread lecteur s'arrête mais le worker reste en attente d'un message. Il faut interrompre le programme avec `Ctrl+C`.
- Le routeur ne traite qu'une seule requête par exécution (un serveur, un client, un échange).
- Les restaurants sont codés en dur dans `serveur.h`.
- Les sockets sont lues avec un seul `read`, sans gérer le cas d'un message reçu en plusieurs morceaux.
- La taille des messages est limitée à 256 octets.

## Auteurs

- Marius Velluet
- Adrian Mussat

Projet réalisé à l'ESIEE Paris
