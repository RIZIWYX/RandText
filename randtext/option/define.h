//  define.h : regroupe les constantes, alias textuels et macros utilitaires
//    partagés par l'ensemble du programme randtext.
//
//  Trois familles de définitions :
//  - les constantes numériques (bornes des options, valeurs par défaut,
//    codes de retour, capacités initiales des structures) ;
//  - les alias textuels destinés à être stringifiés via XSTR pour construire
//    les messages d'aide et les littéraux d'options de manière homogène ;
//  - les macros à corps de code qui factorisent la logique d'analyse de la
//    ligne de commande et l'accès aux champs de struct ctrvalref.
//
//  Le double passage STR/XSTR permet la stringification d'un identificateur
//    après expansion macro : XSTR(NAME) produit la chaine "randtext" alors
//    que STR(NAME) produirait "NAME".

#ifndef DEFINE_H
#define DEFINE_H

//-- Stringification ----------------------------------------------------------

//  STR : opérateur # du préprocesseur, sans expansion préalable de l'argument.
//  XSTR : applique STR après expansion macro complète de l'argument.
#define STR(s)  #s
#define XSTR(s) STR(s)

//-- Identité du programme ----------------------------------------------------

//  NAME : nom de l'exécutable, utilisé dans les messages d'erreur et l'aide.
//  VERSION : numéro de version affiché par --version.
#define NAME     randtext
#define VERSION  1.0

//-- Capacités initiales des tableaux dynamiques de ctrvalref -----------------

//  CAP_MUL : facteur de croissance lors d'une réallocation.
//  CAP_MIN : capacité initiale d'un ctrvalref vide.
//  ARRAY_EMPTY_VAL : valeur de remplissage pour un tableau de clés vide,
//    correspondant au mot vide ε du sujet.
#define CAP_MUL 2
#define CAP_MIN 1
#define ARRAY_EMPTY_VAL ""

//-- Pointeur nul et accès aux champs de ctrvalref ----------------------------

//  EMPTY/IS_EMPTY : uniformisent la manipulation du pointeur nul.
//  ARRAY, ARRAY_VAL, CAP, LEN : raccourcis d'accès aux champs internes d'une
//    structure ctrvalref. 
#define EMPTY()     nullptr
#define IS_EMPTY(p) ((p) == nullptr)
#define ARRAY(p)     ((p)->valarray)
#define ARRAY_VAL(p,i)     ((p)->valarray[i])
#define CAP(p)    ((p)->cap)
#define LEN(p)      ((p)->length)

//-- Fragments textuels utilisés par help.c -----------------------------------

//  Ces alias servent à composer les messages d'aide (--help, --man, --usage)
//    via concaténation de littéraux stringifiés. Ils permettent que toute
//    modification d'un nom (par exemple FEATURE_STR) soit répercutée partout.
#define NAME_STR        NAME
#define TAGLINE_STR     generate a random text
#define SYNOPSIS_STR    SYNOPSIS
#define DESCRIPTION_STR DESCRIPTION
#define USAGE_STR       Usage
#define OPTION_STR      OPTION...
#define FEATURE_STR     FEATURE
#define FILE_STR        FILE
#define MANDATORY_STR   Mandatory arguments to long options                    \
                         are mandatory for short options too.

//-- Ponctuation et marqueurs de la ligne de commande -------------------------

//  SYMBOL : séparateur entre option longue et valeur.
//  LBRACKET/RBRACKET : crochets pour les sections optionnelles du synopsis.
//  NO_FILE : opérande "-" signifiant lecture sur l'entrée standard.
//  END_OPT : opérande "--" signifiant la fin des options.
//  LONG_OPT/SHORT_OPT : préfixes d'options longues et courtes.
//  BASE : base de transcodage des arguments numériques (décimal).
#define SYMBOL =
#define LBRACKET [
#define RBRACKET ]

#define NO_FILE -
#define END_OPT --
#define LONG_OPT --
#define SHORT_OPT -
#define BASE 10

//-- Noms des options ---------------------------------------------------------

//  Pour chaque option, une forme courte (xxx_SOPT) et longue (xxx_LOPT).
//  Le suffixe _STR n'est pas systématiquement ajouté ici : l'expansion finale
//    en chaine est faite via XSTR au moment de l'usage dans help.c et option.c.

//  -o, --order : ordre n de la génération.
#define ORDER_LOPT --order
#define ORDER_SOPT -o

//  -l, --limit : nombre maximal de mots à générer.
#define LIMIT_LOPT --limit
#define LIMIT_SOPT -l

//  -r, --reveal=FEATURE : remplace la sortie normale par table ou words.
#define REVEAL_LOPT --reveal
#define REVEAL_SOPT -r

//  -s, --seed : germe du générateur pseudo-aléatoire.
#define SEED_LOPT --seed
#define SEED_SOPT -s

//  -S, --size : taille maximale du préfixe significatif d'un mot.
#define SIZE_LOPT --size
#define SIZE_SOPT -S

//  -t, --table : alias de --reveal=table.
#define TABLE_LOPT --table
#define TABLE_SOPT -t

//  -w, --words : alias de --reveal=words.
#define WORD_LOPT --words
#define WORD_SOPT -w

//  -W, --width : largeur d'affichage en colonnes.
#define WIDTH_LOPT --width
#define WIDTH_SOPT -W

//  Options informationnelles.
#define HELP_LOPT --help
#define HELP_SOPT -?

#define MAN_LOPT --man
#define MAN_SOPT -M

#define DOPT_LOPT --options
#define DOPT_SOPT -O

#define USE_LOPT --usage
#define USE_SOPT -U

#define VER_LOPT --version
#define VER_SOPT -V

//  FLAGS_STR : agrégat des options courtes sans argument, affiché en synopsis.
#define FLAGS_STR -tw?MOUV

//-- Bornes et valeurs par défaut des options numériques ----------------------

//  Pour chaque option à valeur entière, le triplet MIN/DEF/MAX impose le
//    domaine de validité et la valeur attribuée en l'absence de l'option sur
//    la ligne de commande. Le suffixe _STR fournit le nom de l'option
//    affichée dans l'aide.
//  Remarque : SIZE__MAX (avec deux underscores) évite de heurter la
//    macroconstante SIZE_MAX de <stdint.h>.

#define ORDER_MIN 1
#define ORDER_DEF 1
#define ORDER_MAX 100
#define ORDER_STR ORDER

#define LIMIT_MIN 0
#define LIMIT_DEF 100
#define LIMIT_MAX 4294967295
#define LIMIT_STR LIMIT

#define SEED_MIN 0
#define SEED_DEF 1
#define SEED_MAX 4294967295
#define SEED_STR  SEED

#define SIZE_MIN 1
#define SIZE_DEF 63
#define SIZE__MAX 255
#define SIZE_STR  SIZE

#define WIDTH_MIN 0
#define WIDTH_DEF 0
#define WIDTH_MAX 255
#define WIDTH_STR WIDTH

#define WORD_LEN_MAX 4056

//-- Modes d'affichage --------------------------------------------------------

//  MODE_TEXT : sortie normale, texte aléatoire généré (par défaut).
//  MODE_TABLE : table des n-uplets et de leurs successeurs.
//  MODE_WORD : liste des mots distincts du texte source.
#define MODE_TEXT  0
#define MODE_TABLE 1
#define MODE_WORD  2

//  COL_WORD : valeur initiale du curseur de colonne pour la mise en forme -W.
#define COL_WORD 0

//-- Arguments littéraux pour --reveal ----------------------------------------

//  STR_TABLE/STR_WORDS : valeurs acceptées par --reveal=FEATURE.
#define STR_TABLE table
#define STR_WORDS words

//-- Longueurs des préfixes d'options longues ---------------------------------

//  Ces longueurs servent à isoler la partie "valeur" d'un argument de la
//    forme --option=VALUE via strncmp.
//  SOPT_LEN : longueur de tout préfixe court (-x), à savoir 2.
//  Limite : maintenir manuellement la longueur d'une chaine fixe est fragile :
//    si --order devient --rang, il faut penser à modifier ORDER_LOPT_LEN.
//    Une alternative serait d'utiliser strlen(XSTR(...)) à la compilation,
//    mais cela complique les macros qui s'en servent.
#define SOPT_LEN       2
#define LIMIT_LOPT_LEN 8
#define ORDER_LOPT_LEN 8
#define SEED_LOPT_LEN  7
#define SIZE_LOPT_LEN  7
#define WIDTH_LOPT_LEN 8

//-- Codes de retour ----------------------------------------------------------

//  OPTION_VALID/ERREUR_OPTION : codes retournés par get_option.
//  EXIT_OK/EXIT_ERR : codes de fin d'exécution du programme.
//  ERREUR_OPTION = 64 : valeur imposée par le sujet pour les erreurs de
//    syntaxe de la ligne de commande.
#define OPTION_VALID  0
#define ERREUR_OPTION 64

#define EXIT_OK          0
#define EXIT_ERR         1

//-- Étiquettes des messages d'aide -------------------------------------------

//  Fragments textuels destinés aux sections EXIT STATUS et descriptions des
//    valeurs par défaut dans --man, --help et --options.
#define EXIT_STATUS_STR  EXIT STATUS
#define EXIT_STATUS_LINE_STR Exit status are
#define ON_SUCCESS_STR   on success
#define IF_ERROR_STR     if an error occurs during execution
#define IF_OPT_ERR_STR   if an error occurs on command-line
#define DEFAULT_VALUE_IS_STR The default value is
#define DEFAULT_VALUE_STR    Default value is
#define SUCCESS_STR      success
#define ERROR_STR        error

//-- Macros utilitaires d'analyse de la ligne de commande ---------------------

//  PARSE_UINT :
//    Tente de transcoder src en entier (strtol) puis valide la plage
//    [min, max]. En cas d'erreur de transcodage ou de dépassement, émet
//    le message d'erreur standard et retourne ERREUR_OPTION depuis la
//    fonction appelante. Si succès, affecte la valeur convertie à dest.
//  AE : la macro est appelée dans une fonction qui
//    déclare 'val' et 'i', et dont le type de retour est compatible
//    avec ERREUR_OPTION.
#define PARSE_UINT(dest, src, min, max)  {                                     \
    char *endptr;                                                              \
    errno = 0;                                                                 \
    val = strtol((src), &endptr, BASE);                                        \
    if (errno || *endptr || val < (min)                                        \
        || (unsigned long) val > (max)) {                                      \
      PRINT_ERREUR(i);                                                         \
      return ERREUR_OPTION;                                                    \
    }                                                                          \
    (dest) = (unsigned int) val;                                               \
  }

//  DISPLAY_AND_EXIT :
//    Si l'argument courant correspond à l'option courte sopt ou longue lopt,
//    appelle fn puis termine le programme avec EXIT_SUCCESS. Utilisée pour
//    les options --help, --man, --options, --usage, --version.
#define DISPLAY_AND_EXIT(sopt, lopt, fn)                                       \
  if (strcmp(opt_argtab[i], XSTR(sopt)) == 0                                   \
      || strcmp(opt_argtab[i], XSTR(lopt)) == 0) {                             \
    fn();                                                                      \
    exit(EXIT_SUCCESS);                                                        \
  }

//  SET_MODE_SOPT :
//    Bascule opt.mode sur m si l'argument courant est l'option courte ou
//    longue spécifiée. 
#define SET_MODE_SOPT(sopt, lopt, m)                                           \
  if (strcmp(opt_argtab[i], XSTR(sopt)) == 0                                   \
      || strcmp(opt_argtab[i], XSTR(lopt)) == 0) {                             \
    opt.mode = (m);                                                            \
  }

//  SET_MODE_LOPT :
//    Variante pour la forme --reveal=str. Compare l'argument à la
//    concaténation "REVEAL_LOPT=str".
#define SET_MODE_LOPT(str, m)                                                  \
  if (strcmp(opt_argtab[i],                                                    \
        XSTR(REVEAL_LOPT) XSTR(SYMBOL) XSTR(str)) == 0) {                      \
    opt.mode = (m);                                                            \
  }

//  PARSE_OPT :
//    Trois formes reconnues :
//      - courte attachée : -xVALUE
//      - courte séparée  : -x VALUE  (consomme opt_argtab[i+1])
//      - longue          : --xxx=VALUE
//    Pour la forme courte séparée, vérifie qu'il existe bien un argument
//    suivant ; sinon, déclenche PRINT_ERREUR comme pour une option mal formée.
#define PARSE_OPT(dest, sopt, lopt, lopt_len, min, max)                        \
  if (strncmp(opt_argtab[i], XSTR(sopt), SOPT_LEN) == 0                        \
      && opt_argtab[i][SOPT_LEN] != '\0') {                                    \
    PARSE_UINT(dest, &opt_argtab[i][SOPT_LEN], min, max);                      \
  } else if (strcmp(opt_argtab[i], XSTR(sopt)) == 0) {                         \
    if (i + 1 >= nb_arg) {                                                     \
      PRINT_ERREUR(i);                                                         \
      return ERREUR_OPTION;                                                    \
    }                                                                          \
    i++;                                                                       \
    PARSE_UINT(dest, opt_argtab[i], min, max);                                 \
  } else if (strncmp(opt_argtab[i], XSTR(lopt) XSTR(SYMBOL),                   \
        lopt_len) == 0) {                                                      \
    PARSE_UINT(dest, &opt_argtab[i][lopt_len], min, max);                      \
  }

//  PRINT_ERREUR :
//    Émet sur la sortie d'erreur standard le message d'option non reconnue,
//    suivi de la suggestion de consulter --help. Le paramètre i est l'indice
//    de l'argument problématique dans opt_argtab.
#define PRINT_ERREUR(i) { fprintf(stderr,                                      \
      XSTR(NAME) ": Unrecognized option in '%s'.\n"                            \
      "Try '"XSTR (NAME) " "XSTR (HELP_LOPT) "' for more information.\n",      \
      opt_argtab[i]); }

#endif
