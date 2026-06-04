# randtext

Générateur de texte aléatoire  : le programme lit
un texte sur l'entrée standard (ou dans un fichier) et produit en sortie un texte
généré dont chaque mot dépend statistiquement des *n* mots qui le précèdent dans le
texte d'origine.

> **Auteur et périmètre.** Code écrit par **Rayane Graine** et **Accyl Benajaoud** : le dossier `randtext/`
> — point d'entrée `main.c`, cœur algorithmique `randtext.c`, et le sous-module
> `option/` (ligne de commande) — ainsi que l'intégration et toute la gestion
> mémoire. Les bibliothèques `hashtable/`, `holdall/` et `malloccp/` sont
> **fournies par l'UE** (Algorithmique 3) et utilisées sans modification.

## Architecture

```
Randtext/
├── randtext/                 # Programme principal (code écrit par l'auteur)
│   ├── main.c                #   Point d'entrée : lecture, construction, génération
│   ├── randtext.c/.h/_ip.h   #   Cœur : chaîne de Markov + génération du texte
│   ├── option/               #   Sous-module : ligne de commande
│   │   ├── option.c/.h/_ip.h #     Analyse des arguments et options
│   │   ├── help.c/.h         #     Affichage de l'aide (--help)
│   │   └── define.h          #     Constantes et bornes des options
│   └── makefile              #   Build de l'exécutable randtext
│
├── hashtable/                # Bibliothèque FOURNIE : table de hachage par
│   └── hashtable.c/.h/_ip.h  #   chaînage séparé, polymorphe (void *)
├── holdall/                  # Bibliothèque FOURNIE : « fourre-tout »,
│   └── holdall.c/.h/_ip.h    #   liste de références polymorphes (void *)
├── malloccp/                 # Bibliothèque FOURNIE : « malloc crash planner »,
│   └── malloccp.c/.h         #   simule des échecs d'allocation pour les tests
│
├── tests/                    # Tests
│   ├── test_randtext.sh      #   Compare la sortie à celle de randtext_ref
│   ├── test_malloccp.sh      #   Vérifie la robustesse aux échecs d'allocation
│   ├── randtext_ref          #   Binaire de référence FOURNI (oracle de test)
│   ├── knock.txt             #   Petit texte de test
│   └── lesmiserables.txt     #   Texte volumineux (~3 Mo) pour stress test
│
├── doc/
│   ├── randtext.pdf          # Documentation du projet
│   └── algo3_projet.pdf      # Sujet
│
├── makefile                  # Cibles globales (all, clean, dist)
├── .gitignore
└── README.md
```

**Convention des en-têtes** :

- `module.h` — interface publique du module ;
- `module_ip.h` — *interface privée* (`_ip`), réservée à l'implémentation,
  à ne pas inclure depuis l'extérieur du module ;
- `module.c` — implémentation.

**Dépendances** : `randtext` utilise `hashtable` (stockage des contextes et de
leurs successeurs), `holdall` (suivi des mots alloués dynamiquement pour les
libérer en fin de programme) et `malloccp` (instrumentation des `malloc` pendant
les tests). Les trois bibliothèques sont indépendantes entre elles.

## Compilation

Depuis la racine du projet **ou** depuis `randtext/` :

```sh
make
```

Cela produit l'exécutable `randtext/randtext`. La compilation se fait en mode
strict  `-std=c2x -Wall -Wextra -Wpedantic -Wconversion -Werror -Wwrite-strings -O2`
donc **aucun avertissement n'est toléré**.

Pour nettoyer :

```sh
make clean
```

## Utilisation

```sh
./randtext [OPTIONS] [FICHIER]
```

Sans fichier (ou avec `-`), le programme lit sur l'entrée standard.
Aide complète des options : `./randtext --help`.

### Exemple

```sh
cd randtext
make
./randtext --order=2 ../tests/knock.txt
```

### Exemple de sortie (extrait, ordre 2 sur `knock.txt`)

```
Knock: De quoi souffrez-vous? Le Tambour: Par ici... ou cela, par ici?
Le Tambour: Oui, on dirait que ca vous grattouille? Attendez que je
reflechisse! (Il rit.) Voila. Quand j'ai dine, il y a des fois que je
sens une espece de demangeaison ici.
```

### Modes de sortie

Plusieurs modes sont disponibles (voir `--help`) :

- **mode texte** *(par défaut)*  génère un texte aléatoire ;
- **mode table** (`--reveal=table`)  affiche la table des contextes et de leurs
  successeurs ;
- **mode mots** (`--reveal=words`)  affiche la liste des mots distincts de l'entrée.

L'ordre de génération (`--order`), le nombre de mots (`--limit`), la largeur de
sortie (`--width`) et la graine aléatoire sont paramétrables.

## Tests

Deux scripts sont fournis dans `tests/` :

```sh
./tests/test_randtext.sh
./tests/test_malloccp.sh
```

- `test_randtext.sh` compare la sortie de `./randtext` à celle du **binaire de
  référence** `tests/randtext_ref` sur les fichiers d'exemple (validation de la
  conformité).
- `test_malloccp.sh` réinjecte des **échecs d'allocation** pour vérifier que le
  programme libère correctement ses ressources et ne plante pas.

> Le binaire `tests/randtext_ref` est **conservé volontairement** dans le dépôt :
> c'est l'oracle de test fourni par l'enseignant, qui sert de référence pour
> valider la sortie de l'implémentation.

## Ce que j'ai appris

- Un contexte (n-uplet de mots)
  est associé à la liste pondérée de ses successeurs, et la génération tire un mot
  au hasard selon ces fréquences.
- Utiliser une **table de hachage polymorphe** (clé = tableau de références) et des
  **tableaux dynamiques** redimensionnés à la demande.
- Pratiquer une **gestion mémoire rigoureuse en C** : vérification systématique de
  chaque allocation, libération complète via un « fourre-tout », et **garde contre
  les dépassements d'entier** avant réallocation.
- Concevoir une **interface en ligne de commande** propre (options courtes/longues,
  bornes, `--help`) et documenter des **contrats de fonctions** précis dans les
  en-têtes.
- Valider le tout par des **tests automatisés**, y compris l'injection d'échecs
  d'allocation.

## Limites connues

- Les mots sont des suites de caractères non-`isspace` **tronquées** au-delà d'une
  longueur maximale : un mot très long est découpé en plusieurs.
- Le module `option` repose sur un **état global** (couplage  `randtext`  `option`).
- Le texte généré ne reflète que les statistiques de l'entrée : répétitions ou
  enchaînements inattendus sont possibles.

## Licence

Projet académique — usage pédagogique.
