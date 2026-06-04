//  Partie interface du module malloccp (memory allocation crash planner,
//    planificateur de plantage de demandes d'allocation mémoire).

//  AUCUNE MODIFICATION DE CE SOURCE N'EST AUTORISÉE.

#define __CH_VERSION_MALLOCCP_H__ 202602L

//  Le comportement du module est sensible aux définitions préalables de la
//    macro standard NDEBUG et de la macro MALLOCCP.
//  Il est attendu que MALLOCCP soit définie lorsque NDEBUG n'est pas définie.

//  Le module malloccp permet essentiellement de simuler le plantage d'une
//    demande d'allocation mémoire.

//  L'en-tête "malloccp.h" définit, redéfinit ou supprime des macrofonctions de
//    noms identiques aux quatre fonctions de demande d'allocation
//    aligned_alloc, calloc, malloc et realloc déclarées dans l'en-tête standard
//    <stdlib.h>, ainsi qu'à certaines fonctions qui font appel à malloc : fopen
//    et tmpfile de l'en-tête standard <stdio.h> ; strdup et strndup de
//    l'en-tête standard <string.h>.

//  Le fonctionnement du module suit celui du module standard assert en étant
//    sensible à la macro NDEBUG : les huit macrofonctions sont définies,
//    redéfinies ou supprimées selon l'état courant de NDEBUG à chaque fois que
//    l'en-tête est inclus.

#ifdef MALLOCCP__H

#undef MALLOCCP__H

#undef fopen
#undef tmpfile

#undef aligned_alloc
#undef calloc
#undef malloc
#undef realloc

#undef strdup
#undef strndup

#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MALLOCCP__H

#if defined NDEBUG || (defined MALLOCCP && MALLOCCP == 0)

//  Si NDEBUG est définie lors de l'inclusion de l'en-tête, aucune des huit
//    macrofonctions n'est définie. Ce sont donc les fonctions concrètes qui
//    sont directement utilisées.
//  Si NDEBUG n'est pas définie, un même comportement peut être obtenu en
//    définissant la macro MALLOCCP avec une valeur évaluable à zéro.

#else

//  Arrivé à ce point : soit ni NDEBUG ni MALLOCCP ne sont définies ; soit
//    NDEBUG n'est pas définie et MALLOCCP est définie avec une valeur non
//    nulle.

#if !defined MALLOCCP

//  Dans le premier cas, un avertissement est émis par le préprocesseur et les
//    huit macrofonctions sont définies de telle sorte que la valeur renvoyée
//    par chacune d'elles est un pointeur nul.

#warning One of the two macro NDEBUG or MALLOCCP shall be defined.

#define CP__NULL(...) nullptr

#define fopen   CP__NULL
#define tmpfile CP__NULL

#define aligned_alloc CP__NULL
#define calloc        CP__NULL
#define malloc        CP__NULL
#define realloc       CP__NULL

#define strdup  CP__NULL
#define strndup CP__NULL

#else

//  Dans le deuxième cas, les huit macrofonctions sont définies de telle sorte
//    qu'elles n'appellent la fonction concrète que si une limite de nombre
//    d'appels aux macrofonctions depuis le début de l'exécution du programme
//    n'est pas atteinte. Si la limite est atteinte, la valeur renvoyée par la
//    macrofonction invoquée est un pointeur nul.
//  La limite dépend de la valeur (non nulle) de MALLOCCP :
//  - si la valeur de MALLOCCP est positive, la limite est la valeur de
//    MALLOCCP ;
//  - sinon, la limite est choisie entre 1 et l'opposé de la valeur de MALLOCCP.
//    Le choix est basé sur la valeur courante du temps universel. Le choix ne
//    fait appel à aucune des deux fonctions standard rand et srand de
//    génération de nombres pseudo-aléatoires.
//  Il est attendu que la valeur de MALLOCCP puisse s'exprimer sur le type
//    « long long int ».

//  Le seul cas d'erreur est le suivant : les ressources ne sont pas suffisantes
//    pour installer la fonctionnalité associée aux informations produites en
//    fin d'exécution du programme.
//  En cas d'erreur, il est mis fin à l'exécution du programme par l'appel
//    « exit(EXIT_FAILURE) ».

//  Tous les messages, qu'ils soient d'information ou d'erreur, sont envoyés sur
//    la sortie erreur.

//  Lorsque la limite est atteinte, un message d'information est produit qui
//    précise l'appel à la fonction concrète non appelée : son identificateur,
//    le fichier source dans lequel figure l'appel, le numéro de la ligne à
//    laquelle il y figure ainsi que la fonction englobante.
//  À la fin de l'exécution du programme, un message d'information précise la
//    valeur de la limite, avec deux contenus distincts selon que cette limite a
//    été atteinte ou non. Dans le cas d'une valeur (strictement) négative de
//    MALLOCCP, le message est complété pour préciser la borne supérieure
//    fixée implicitement par l'utilisateurice de l'intervalle dans lequel est
//    choisi la limite.

extern bool malloccp__crash(
  long long int n,
  const char *file, int line, const char *func,
  const char *allocfunccalled);

#define MALLOCCP__CALL(allocfunccalled, ...)                                   \
  (malloccp__crash(                                                            \
      MALLOCCP,                                                                \
      __FILE__, __LINE__, __func__,                                            \
      #allocfunccalled)                                                        \
  ? nullptr                                                                    \
  : allocfunccalled(__VA_ARGS__))

#define fopen(...)    MALLOCCP__CALL(fopen, __VA_ARGS__)
#define tmpfile(...)  MALLOCCP__CALL(tmpfile, __VA_ARGS__)

#define aligned_alloc(...) MALLOCCP__CALL(aligned_alloc, __VA_ARGS__)
#define calloc(...)        MALLOCCP__CALL(calloc, __VA_ARGS__)
#define malloc(...)        MALLOCCP__CALL(malloc, __VA_ARGS__)
#define realloc(...)       MALLOCCP__CALL(realloc, __VA_ARGS__)

#define strdup(...)   MALLOCCP__CALL(strdup, __VA_ARGS__)
#define strndup(...)  MALLOCCP__CALL(strndup, __VA_ARGS__)

#endif
#endif
