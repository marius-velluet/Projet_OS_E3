# Projet_OS_E3

Simulation d'une application de menus de restaurants en C, où un client, un routeur et un serveur de données sont trois processus distincts qui communiquent entre eux. Projet réalisé en binôme à l'ESIEE Paris (E3), dans le cadre du cours de systèmes d'exploitation.

> Cette version correspond à la branche `Branch_Utilisation_FIFOs`, où la communication inter-processus passe par des tubes nommés (FIFOs).

## Principe

L'utilisateur saisit un code de 12 chiffres, comme s'il scannait un QR code posé sur la table d'un restaurant. Ce code identifie un lieu et un menu. Le message traverse ensuite toute la chaîne :

```
Client  ──►  Routeur  ──►  Serveur de données
Client  ◄──  Routeur  ◄──  Serveur de données
```

Si le code correspond à un restaurant connu, le serveur affiche un message de bienvenue et ouvre l'image du menu avec `feh`. Sinon, il renvoie "Menu introuvable".

## Architecture

Au lancement, `main.c` crée les 4 FIFOs, puis fait un `fork()` pour lancer le routeur et le serveur. Le processus parent joue le rôle du client, ce qui permet de saisir le code au clavier. À la fin, les FIFOs sont supprimées avec `unlink`.

| Fichier | Rôle |
|---|---|
| `main.c` | Création des FIFOs, `fork()` des processus, nettoyage final |
| `ID.h` | Client : lit le code au clavier, l'envoie au routeur et affiche la réponse |
| `routage.h` | Routeur : relaie la requête du client vers le serveur, puis la réponse vers le client |
| `serveur.h` | Serveur de données : décode le code, retrouve le restaurant et affiche son menu |
| `threading.h` | File de messages partagée (tampon circulaire protégé par mutex et variable de condition) |
| `images/` | Images des menus affichées par le serveur |
| `Makefile` | Compilation du projet |

### Communication

Quatre FIFOs, deux par liaison, assurent les échanges dans chaque sens :

- `fifo_client_to_router` et `fifo_router_to_client`
- `fifo_router_to_server` et `fifo_server_to_router`

### Threads côté serveur

Le serveur de données utilise deux threads qui se partagent une file de messages (10 emplacements maximum) :

- **`reader_thread`** lit les requêtes arrivant du routeur et les place dans la file.
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

```bash
sudo apt install gcc make feh
```

## Compilation et exécution

```bash
git clone https://github.com/marius-velluet/Projet_OS_E3.git
cd Projet_OS_E3
git checkout Branch_Utilisation_FIFOs

make
./mon_programme
```

Le programme demande ensuite le code :

```
[Client] Prêt. Entrez le code (12 chiffres) :
```

Exemple avec `000100011234` (lieu `0001`, menu `1234`) :

```
[Client] Message envoyé au routeur.
...
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

- Création de processus (`fork`, `wait`)
- Communication inter-processus avec des tubes nommés (`mkfifo`, `open`, `read`, `write`)
- Programmation concurrente avec `pthread`
- Synchronisation par mutex et variables de condition (problème producteur/consommateur)

## Limites connues

- Le serveur ne traite qu'une seule requête par exécution, car le client, le routeur et le serveur effectuent chacun un seul échange.
- Les restaurants sont codés en dur dans `serveur.h`.
- Le routeur simule un temps de transit avec des `usleep`.
- La taille des messages est limitée à 256 octets.

## Auteurs

- Marius Velluet
- *(nom de ton binôme)*

Projet réalisé à l'ESIEE Paris, filière cybersécurité.
