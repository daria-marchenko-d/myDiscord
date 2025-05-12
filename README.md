# myDiscord

## 🇬🇧 ENGLISH

**myDiscord** is our home-grown reimagining of Discord — a real-time messaging platform that reflects your style. It’s a space where conversations flow instantly, emojis enhance interactions, and channels grow with the community’s needs.

Our goal? To make this digital utopia a reality with features like:
- Persistent message history across sessions
- Timestamped messages with author information
- Real-time updates and seamless UX

## 🇫🇷 FRANÇAIS

**myDiscord**, c’est notre version maison de Discord : une interface à votre image, un lieu où les messages fusent en temps réel, où les emojis ponctuent les discussions, et où les canaux se multiplient selon les envies des utilisateurs.

Notre objectif ? Offrir :
- Un historique des échanges disponible à chaque connexion
- L’identification de l’auteur et l’heure précise de chaque message
- Une expérience fluide, réactive et modulable

---

## 🗂️ Tree Structure / Structure Arborescente



MYDISCORDTRUE/

├── .vscode/                             # Configuration spécifique à VS Code

│  ├── c_cpp_properties.json            # Configuration des chemins d'inclusion pour C/C++

│   └── settings.json                    # Paramètres personnalisés de l'environnement VS Code
│
├── assets/                              # (Vide ou à compléter) Contient éventuellement des fichiers statiques (images, icônes, etc.)
│
├── build/                               # Dossier de génération contenant les fichiers compilés

│   ├── CMakeFiles/                      # Fichiers internes générés par CMake

│   ├── cmake_install.cmake              # Script d'installation généré par CMake

│   ├── CMakeCache.txt                   # Cache de configuration CMake

│   ├── Makefile                         # Makefile généré automatiquement par CMake

│   ├── mydiscord_client.exe             # Exécutable du client compilé

│   └── mydiscord_server.exe            # Exécutable du serveur compilé
│
├── include/                             # Fichiers d'en-tête (.h), déclarations des fonctions et structures

│   ├── app.h                            # Interface de gestion de l'application

│   ├── crypto.h                         # Fonctions de chiffrement/déchiffrement

│   ├── database.h                       # Fonctions de gestion de la base de données

│   ├── gui.h                            # Interface graphique (ou CLI)

│   ├── network.h                        # Gestion des communications réseau

│   ├── server.h                         # Interface du serveur

│   └── utils.h                          # Fonctions utilitaires diverses
│
├── server_main/                         # Contient le point d'entrée du serveur

│   └── main_server.c                    # Fichier principal du serveur
│
├── sql/                                 # Scripts SQL pour la base de données

│   └── database.sql                     # Script de création de la base ou des tables SQL
│
├── mydiscord.db                         # Fichier de base de données SQLite utilisé par l’application
│
├── src/                                 # Code source de l’application

│   ├── app.c                            # Logique principale de l'application

│   ├── crypto.c                         # Implémentation des fonctions de cryptographie

│   ├── database.c                       # Accès à la base de données (SQLite)

│   ├── gui.c                            # Interface utilisateur

│   ├── main.c                           # Point d'entrée du client (probablement)

│   ├── network.c                        # Implémentation réseau (sockets, etc.)

│   ├── server.c                         # Logique serveur

│   ├── test_db.c                        # Fichier de test pour la base de données

│   ├── test_db.exe                      # Exécutable compilé pour tester la base de données

│   └── utils.c                          # Fonctions utilitaires (conversion, parsing, etc.)
│
├── CMakeLists.txt                       # Script de configuration CMake pour compiler le projet

└── README.md                            # Documentation du projet (présentation, instructions, etc.)



---

## ⚙️ Technologies utilisées

- **Langages** : C, SQL, Bash
- **Technos** : Sockets, GTK, SQLite, CMake
- **Matériel** : Linux/Unix, compilateurs GCC/Clang

---

## 🚀 Compilation

Dans un terminal :

```bash
mkdir build
cd build
cmake ..
make
