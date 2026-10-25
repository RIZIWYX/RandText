//  help.c : partie implantation du module help.
//
//  Les fonctions de ce module se contentent d'écrire des chaines composées
//    par concaténation de littéraux et de stringifications XSTR. Le recours
//    massif à XSTR (défini dans define.h) garantit qu'une modification du
//    nom d'une option ou d'une de ses bornes dans define.h se propage
//    automatiquement à tous les messages d'aide, sans risque de désynchro-
//    nisation entre le comportement réel et la documentation affichée.
//
//  Limite : la lisibilité du code en pâtit, les blocs printf devenant longs
//    et fragmentés. Le compromis a été assumé pour éviter la duplication
//    de chaines de caractères entre la logique d'analyse (option.c) et la
//    documentation (help.c).

#include <stdio.h>
#include "help.h"
#include "option.h"
#include "malloccp.h"

void display_help(void) {
  printf(
      XSTR(USAGE_STR) ": " XSTR(NAME) " [" XSTR(OPTION_STR) "] ["
      XSTR(FILE_STR) "]\n\n"
      "Generate on the standard output a random text based on some statistical"
      " aspects\nof " XSTR(FILE_STR) ". The generation depends on a given"
      " order n. The generated text begins\nwith the n first words of "
      XSTR(FILE_STR) ". Then, the next generated word is randomly chosen\n"
      "from the list of words which immediately follow the n previous generated"
      " words\nin " XSTR(FILE_STR) ". The process continues, until one of the"
      " following situations occurs: a\ngiven number of words has been"
      " generated or the set is empty.\n\n"
      "With no " XSTR(FILE_STR) ", or when " XSTR(FILE_STR) " is '-', read"
      " standard input. The first of any\npossible '--' arguments on the"
      " command line indicates the end of options.\n\n"
      XSTR(MANDATORY_STR) "\n\n"
      "  " XSTR(LIMIT_SOPT) ", " XSTR(LIMIT_LOPT) XSTR(SYMBOL)
      XSTR(LIMIT_STR) "    Print " XSTR(LIMIT_STR) " words, if possible. "
      XSTR(LIMIT_STR) " must range from " XSTR(LIMIT_MIN) "\n"
      "            to 4294967295. The default value for "
      XSTR(LIMIT_STR) " is " XSTR(LIMIT_DEF) ".\n\n"
      "  " XSTR(ORDER_SOPT) ", " XSTR(ORDER_LOPT) XSTR(SYMBOL)
      XSTR(ORDER_STR) "    Fix at " XSTR(ORDER_STR) " the "
      XSTR(ORDER_STR) " of generation. " XSTR(ORDER_STR)
      " must range from " XSTR(ORDER_MIN) " to " XSTR(ORDER_MAX)
      ". The default value for " XSTR(ORDER_STR) " is "
      XSTR(ORDER_DEF) ".\n\n"
      "  " XSTR(REVEAL_SOPT) ", " XSTR(REVEAL_LOPT) XSTR(SYMBOL)
      XSTR(FEATURE_STR) "    Suppress normal output. When "
      XSTR(FEATURE_STR) " is '" XSTR(STR_TABLE) "', display the table which"
      " associates, to each n-uple of words\n"
      "            occurring in the source, the list of the words which"
      " immediately follow this n-uple. When " XSTR(FEATURE_STR) " is '"
      XSTR(STR_WORDS) "', display the\n"
      "            different words occurring in the source.\n\n"
      "  " XSTR(SEED_SOPT) ", " XSTR(SEED_LOPT) XSTR(SYMBOL)
      XSTR(SEED_STR) "    Fix at " XSTR(SEED_STR) " the "
      XSTR(SEED_STR) " of the sequence of pseudo-random numbers used to"
      " generate the random text. " XSTR(SEED_STR) " must range\n"
      "            from " XSTR(SEED_MIN) " to " XSTR(SEED_MAX)
      ". The default value for " XSTR(SEED_STR) " is "
      XSTR(SEED_DEF) ".\n\n"
      "  " XSTR(SIZE_SOPT) ", " XSTR(SIZE_LOPT) XSTR(SYMBOL)
      XSTR(SIZE_STR) "    Fix at " XSTR(SIZE_STR) " the maximum size of the"
      " significant prefix of words. This size is expressed in terms of"
      " single-byte\n"
      "            characters. " XSTR(SIZE_STR) " must range from "
      XSTR(SIZE_MIN) " to " XSTR(SIZE__MAX) ". The default value for "
      XSTR(SIZE_STR) " is " XSTR(SIZE_DEF) ".\n\n"
      "  " XSTR(TABLE_SOPT) ", " XSTR(TABLE_LOPT) "    Same as '"
      XSTR(REVEAL_SOPT) " " XSTR(STR_TABLE) "'.\n\n"
      "  " XSTR(WORD_SOPT) ", " XSTR(WORD_LOPT) "    Same as '"
      XSTR(REVEAL_SOPT) " " XSTR(STR_WORDS) "'.\n\n"
      "  " XSTR(WIDTH_SOPT) ", " XSTR(WIDTH_LOPT) XSTR(SYMBOL)
      XSTR(WIDTH_STR) "    Set normal output " XSTR(WIDTH_STR) " to "
      XSTR(WIDTH_STR) ". " XSTR(WIDTH_DEF) " means no limit. "
      XSTR(WIDTH_STR) " must range from " XSTR(WIDTH_MIN) " to "
      XSTR(WIDTH_MAX) ". The default value for " XSTR(WIDTH_STR) "\n"
      "            is " XSTR(WIDTH_DEF) ".\n\n"
      "  " XSTR(HELP_SOPT) ", " XSTR(HELP_LOPT)
      "    Display this help and exit.\n\n"
      "  " XSTR(MAN_SOPT) ", " XSTR(MAN_LOPT)
      "    Display this help in a style resembling that of man and exit.\n\n"
      "  " XSTR(DOPT_SOPT) ", " XSTR(DOPT_LOPT)
      "    Display current options and exit.\n\n"
      "  " XSTR(USE_SOPT) ", " XSTR(USE_LOPT)
      "    Display a short usage message and exit.\n\n"
      "  " XSTR(VER_SOPT) ", " XSTR(VER_LOPT)
      "    Output version information and exit.\n\n"
      XSTR(EXIT_STATUS_LINE_STR) " " XSTR(EXIT_OK) " "
      XSTR(ON_SUCCESS_STR) ", " XSTR(EXIT_ERR) " " XSTR(IF_ERROR_STR) ", "
      XSTR(ERREUR_OPTION) " " XSTR(IF_OPT_ERR_STR) ".\n"
      );
}

void display_man() {
  printf(
      //  NAME.
      XSTR(NAME_STR) "\n"
      "    " XSTR(NAME) " - " XSTR(TAGLINE_STR) "\n\n"
      //  SYNOPSIS.
      XSTR(SYNOPSIS_STR) "\n"
      "    " XSTR(NAME) " " XSTR(LBRACKET) XSTR(OPTION_STR) XSTR(
      RBRACKET) " " XSTR(LBRACKET) XSTR(FILE_STR) XSTR(RBRACKET) "\n\n"
      //  DESCRIPTION.
      XSTR(DESCRIPTION_STR) "\n"
      "    Generate on the standard output a random text based on some statistical aspects of "
      XSTR(FILE_STR) ". The generation depends on a  given  order\n"
      "    n.  The  generated  text  begins with the n first words of "
      XSTR(
      FILE_STR)
      ". Then, the next generated word is randomly chosen from the list of\n"
      "    words which immediately follow the n previous generated words  in  "
      XSTR(
      FILE_STR) ".  The  process  continues,  until  one  of  the  following\n"
      "    situations occurs: a given number of words has been generated or the set is empty.\n\n"
      "    With no " XSTR(FILE_STR) ", or when " XSTR(FILE_STR) " is '" XSTR(
      NO_FILE) "', read standard input. The first of any possible '" XSTR(
      END_OPT) "' arguments on the command line indicates the\n"
      "    end of options.\n\n"
      "    " XSTR(MANDATORY_STR) "\n\n"
      //  -l, --limit.
      "    " XSTR(LIMIT_SOPT) ", " XSTR(LIMIT_LOPT) XSTR(SYMBOL) XSTR(
      LIMIT_STR) "\n"
      "        Print " XSTR(LIMIT_STR) " words, if possible. " XSTR(
      LIMIT_STR) " must range from " XSTR(LIMIT_MIN) " to " XSTR(
      LIMIT_MAX) ". " XSTR(DEFAULT_VALUE_IS_STR) " " XSTR(
      LIMIT_STR) " is %u.\n\n"
      //  -o, --order.
      "    " XSTR(ORDER_SOPT) ", " XSTR(ORDER_LOPT) XSTR(SYMBOL) XSTR(
      ORDER_STR) "\n"
      "        Fix at " XSTR(ORDER_STR) " the " XSTR(ORDER_STR)
      " of generation. " XSTR(ORDER_STR) " must range from " XSTR(
      ORDER_MIN) " to " XSTR(ORDER_MAX) ". " XSTR(
      DEFAULT_VALUE_IS_STR) " " XSTR(ORDER_STR) " is %u.\n\n"
      //  -r, --reveal.
      "    " XSTR(REVEAL_SOPT) ", " XSTR(REVEAL_LOPT) XSTR(SYMBOL) XSTR(
      FEATURE_STR) "\n"
      "        Suppress normal output. When " XSTR(FEATURE_STR) " is '" XSTR(
      STR_TABLE)
      "', display the table which associates, to each n-uple of words occurring in  the\n"
      "        source,  the  list  of  the  words  which immediately follow this n-uple. When "
      XSTR(FEATURE_STR) " is '" XSTR(
      STR_WORDS) "', display the different words\n"
      "        occurring in the source.\n\n"
      //  -s, --seed.
      "    " XSTR(SEED_SOPT) ", " XSTR(SEED_LOPT) XSTR(SYMBOL) XSTR(
      SEED_STR) "\n"
      "        Fix  at  " XSTR(SEED_STR) "  the  " XSTR(SEED_STR)
      "  of the sequence of pseudo-random numbers used to generate the random text. "
      XSTR(SEED_STR) " must range from " XSTR(SEED_MIN) " to\n"
      "        " XSTR(SEED_MAX) ". " XSTR(DEFAULT_VALUE_IS_STR) " " XSTR(
      SEED_STR) " is %u.\n\n"
      //  -S, --size.
      "    " XSTR(SIZE_SOPT) ", " XSTR(SIZE_LOPT) XSTR(SYMBOL) XSTR(
      SIZE_STR) "\n"
      "        Fix at " XSTR(SIZE_STR)
      " the maximum size of the significant prefix of words. This size is expressed in terms  of  single-byte  characters.\n"
      "        " XSTR(SIZE_STR) " must range from " XSTR(SIZE_MIN) " to " XSTR(
      SIZE__MAX) ". " XSTR(DEFAULT_VALUE_IS_STR) " " XSTR(
      SIZE_STR) " is %u.\n\n"
      //  -t, --table.
      "    " XSTR(TABLE_SOPT) ", " XSTR(TABLE_LOPT) "\n"
      "        Same as '" XSTR(REVEAL_SOPT) " " XSTR(STR_TABLE) "'.\n\n"
      //  -w, --words.
      "    " XSTR(WORD_SOPT) ", " XSTR(WORD_LOPT) "\n"
      "        Same as '" XSTR(REVEAL_SOPT) " " XSTR(STR_WORDS) "'.\n\n"
      //  -W, --width.
      "    " XSTR(WIDTH_SOPT) ", " XSTR(WIDTH_LOPT) XSTR(SYMBOL) XSTR(
      WIDTH_STR) "\n"
      "        Set normal output " XSTR(WIDTH_STR) " to " XSTR(
      WIDTH_STR) ". " XSTR(WIDTH_DEF) " means no limit. " XSTR(
      WIDTH_STR) " must range from " XSTR(WIDTH_MIN) " to " XSTR(
      WIDTH_MAX) ". " XSTR(DEFAULT_VALUE_IS_STR) " " XSTR(
      WIDTH_STR) " is %u.\n\n"
      //  Options informationnelles.
      "    " XSTR(HELP_SOPT) ", " XSTR(HELP_LOPT) "\n"
      "        Display this help and exit.\n\n"
      "    " XSTR(MAN_SOPT) ", " XSTR(MAN_LOPT) "\n"
      "        Display this help in a style resembling that of man and exit.\n\n"
      "    " XSTR(DOPT_SOPT) ", " XSTR(DOPT_LOPT) "\n"
      "        Display current options and exit.\n\n"
      "    " XSTR(USE_SOPT) ", " XSTR(USE_LOPT) "\n"
      "        Display a short usage message and exit.\n\n"
      "    " XSTR(VER_SOPT) ", " XSTR(VER_LOPT) "\n"
      "        Output version information and exit.\n\n"
      //  EXIT STATUS.
      XSTR(EXIT_STATUS_STR) "\n"
      "    " XSTR(EXIT_STATUS_LINE_STR) " " XSTR(EXIT_OK) " " XSTR(
      ON_SUCCESS_STR) ", " XSTR(EXIT_ERR) " " XSTR(
      IF_ERROR_STR) ", " XSTR(ERREUR_OPTION) " " XSTR(IF_OPT_ERR_STR) ".\n\n"
      //  AUTHOR.
      "AUTHOR\n"
      "    Written by A.R.Graine & A.Benadjaoud.\n\n"
      //  COPYRIGHT.
      "COPYRIGHT\n"
      "    " XSTR(NAME) " - " XSTR(VERSION) "\n"
      "    This is free software: you are free to redistribute it.\n",
      LIMIT_DEF, ORDER_DEF, SEED_DEF, SIZE_DEF, WIDTH_DEF
      );
}

void display_options() {
  printf(
      "Current options: " XSTR(NAME) " "
      XSTR(LIMIT_SOPT) " %u "
      XSTR(ORDER_SOPT) " %u "
      XSTR(SEED_SOPT) " %u "
      XSTR(SIZE_SOPT) " %u "
      XSTR(WIDTH_SOPT) " %u\n",
      opt_get_limit(), opt_get_order(), opt_get_seed(), opt_get_size_max(),
      opt_get_width()
      );
}

void display_usage() {
  printf(
      XSTR(USAGE_STR) ": " XSTR(NAME) " "
      XSTR(LBRACKET) XSTR(FLAGS_STR) XSTR(RBRACKET) " "
      XSTR(LBRACKET) XSTR(LIMIT_SOPT) " " XSTR(LIMIT_STR) XSTR(RBRACKET) " "
      XSTR(LBRACKET) XSTR(ORDER_SOPT) " " XSTR(ORDER_STR) XSTR(RBRACKET) " "
      XSTR(LBRACKET) XSTR(REVEAL_SOPT) " " XSTR(FEATURE_STR) XSTR(RBRACKET) " "
      XSTR(LBRACKET) XSTR(SEED_SOPT) " " XSTR(SEED_STR) XSTR(RBRACKET) "\n"
      "    "
      XSTR(LBRACKET) XSTR(SIZE_SOPT) " " XSTR(SIZE_STR) XSTR(RBRACKET) " "
      XSTR(LBRACKET) XSTR(WIDTH_SOPT) " " XSTR(WIDTH_STR) XSTR(RBRACKET) " "
      XSTR(LBRACKET) XSTR(LIMIT_LOPT) XSTR(SYMBOL) XSTR(LIMIT_STR)
      XSTR(RBRACKET) " "
      XSTR(LBRACKET) XSTR(ORDER_LOPT) XSTR(SYMBOL) XSTR(ORDER_STR)
      XSTR(RBRACKET) "\n"
      "    "
      XSTR(LBRACKET) XSTR(REVEAL_LOPT) XSTR(SYMBOL) XSTR(FEATURE_STR)
      XSTR(RBRACKET) " "
      XSTR(LBRACKET) XSTR(SEED_LOPT) XSTR(SYMBOL) XSTR(SEED_STR)
      XSTR(RBRACKET) " "
      XSTR(LBRACKET) XSTR(SIZE_LOPT) XSTR(SYMBOL) XSTR(SIZE_STR)
      XSTR(RBRACKET) " "
      XSTR(LBRACKET) XSTR(TABLE_LOPT) XSTR(RBRACKET) " "
      XSTR(LBRACKET) XSTR(WORD_LOPT) XSTR(RBRACKET) "\n"
      "    "
      XSTR(LBRACKET) XSTR(WIDTH_LOPT) XSTR(SYMBOL) XSTR(WIDTH_STR)
      XSTR(RBRACKET) " "
      XSTR(LBRACKET) XSTR(HELP_LOPT) XSTR(RBRACKET) " "
      XSTR(LBRACKET) XSTR(MAN_LOPT) XSTR(RBRACKET) " "
      XSTR(LBRACKET) XSTR(DOPT_LOPT) XSTR(RBRACKET) " "
      XSTR(LBRACKET) XSTR(USE_LOPT) XSTR(RBRACKET) " "
      XSTR(LBRACKET) XSTR(VER_LOPT) XSTR(RBRACKET) " "
      XSTR(LBRACKET) XSTR(NO_FILE) XSTR(RBRACKET) " "
      XSTR(LBRACKET) XSTR(END_OPT) XSTR(RBRACKET) "\n"
      "    "
      XSTR(LBRACKET) XSTR(FILE_STR) XSTR(RBRACKET) "\n"
      );
}

void display_version() {
  printf(
      XSTR(NAME) " - " XSTR(VERSION) "\n"
      "Licence MIT : logiciel libre, vous pouvez le redistribuer.\n"
      "Écrit par Rayane Graïne et Accyl Benadjaoud.\n"
      );
}
