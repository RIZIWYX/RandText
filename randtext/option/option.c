//  option.c : partie implantation du module option.
//
//  Définit la variable globale opt initialisée avec les valeurs par défaut
//    et la fonction get_option qui analyse les arguments de la ligne de
//    commande pour mettre à jour opt en conséquence.
//
//  La logique d'analyse repose presque entièrement sur les macros définies
//    dans define.h (PARSE_OPT, SET_MODE_SOPT, SET_MODE_LOPT,
//    DISPLAY_AND_EXIT, PRINT_ERREUR). Chacune de ces macros encapsule un
//    motif de reconnaissance d'option et ses effets de bord, ce qui réduit
//    la duplication mais induit un fort couplage entre option.c et define.h.
//
//  Limite générale : la cascade de blocs « else { ... } » imbriqués qui
//    enchaîne les macros produit un code à indentation profondément
//    déportée vers la droite. Ce style est volontaire (chaque macro émet un
//    « if » qu'il faut faire suivre par un « else » pour qu'il ne se
//    rattache pas au if suivant) mais il rend la lecture pénible. Une
//    réorganisation en cascade plate (else if explicites, ou table de
//    correspondance entre nom d'option et fonction de traitement) serait
//    plus lisible si le temps le permet.

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "option.h"
#include "malloccp.h"

//  option : type et nom de type regroupant les informations nécessaires pour
//    gérer les options du programme randtext.
typedef struct {
  unsigned int limit;
  unsigned int order;
  unsigned int seed;
  unsigned int size_max;
  unsigned int width;
  size_t col_word;
  int mode;
  int end_of_opts;
  const char *filename;
} option;

static option opt = {
  .limit = LIMIT_DEF,
  .order = ORDER_DEF,
  .seed = SEED_DEF,
  .size_max = SIZE_DEF,
  .width = WIDTH_DEF,
  .col_word = COL_WORD,
  .mode = MODE_TEXT,
  .end_of_opts = 0,
  .filename = EMPTY(),
};

int get_option(int nb_arg, char *opt_argtab[]) {
  long int val;
  int i = 1;
  while (i < nb_arg) {
    if (!opt.end_of_opts && strcmp(opt_argtab[i], XSTR(END_OPT)) == 0) {
      opt.end_of_opts = 1;
      i++;
      continue;
    }
    if (!opt.end_of_opts && (
          strncmp(opt_argtab[i], XSTR(LONG_OPT), SOPT_LEN) == 0
          || (opt_argtab[i][0] == XSTR(SHORT_OPT)[0]
          && opt_argtab[i][1] != '\0')
          )) {
      //  Le « if (0) { } else { ... } » initial sert d'amorce : il permet
      //    d'enchaîner systématiquement chaque macro par un « else ».
      //    Chaque macro PARSE_OPT, SET_MODE_SOPT émet un « if »
      //    isolé ; sans le else qui le précède, ce if absorberait le else
      //    de la macro précédente, ce qui briserait la logique exclusive.
      if (0) {
      } else {
        //  Options à valeur numérique.
        PARSE_OPT(opt.limit, LIMIT_SOPT, LIMIT_LOPT, LIMIT_LOPT_LEN, LIMIT_MIN,
            LIMIT_MAX)
        else {
          PARSE_OPT(opt.order, ORDER_SOPT, ORDER_LOPT, ORDER_LOPT_LEN,
              ORDER_MIN, ORDER_MAX)
          else {
            PARSE_OPT(opt.seed, SEED_SOPT, SEED_LOPT, SEED_LOPT_LEN, SEED_MIN,
                SEED_MAX)
            else {
              PARSE_OPT(opt.size_max, SIZE_SOPT, SIZE_LOPT, SIZE_LOPT_LEN,
                  SIZE_MIN, SIZE__MAX)
              else {
                PARSE_OPT(opt.width, WIDTH_SOPT, WIDTH_LOPT, WIDTH_LOPT_LEN,
                    WIDTH_MIN, WIDTH_MAX)
                else {
                  //  Options de basculement de mode sans valeur :
                  //    -t / --table et -w / --words sont des alias directs
                  //    de --reveal=table et --reveal=words.
                  SET_MODE_SOPT(TABLE_SOPT, TABLE_LOPT, MODE_TABLE)
                  else {
                    SET_MODE_SOPT(WORD_SOPT, WORD_LOPT, MODE_WORD)
                    else if (strcmp(opt_argtab[i],
                          XSTR(REVEAL_SOPT)) == 0 && i + 1 < nb_arg) {
                      char *arg = opt_argtab[++i];
                      if (strcmp(arg, XSTR(STR_TABLE)) == 0) {
                        opt.mode = MODE_TABLE;
                      } else if (strcmp(arg, XSTR(STR_WORDS)) == 0) {
                        opt.mode = MODE_WORD;
                      } else {
                        fprintf(stderr,
                            XSTR(NAME) ": Invalid argument in option '" XSTR(
                            REVEAL_SOPT) "'.\n"
                            "Try '" XSTR(NAME) " " XSTR(
                            HELP_LOPT) "' for more information.\n");
                        return ERREUR_OPTION;
                      }
                    } else {
                      SET_MODE_LOPT(STR_TABLE, MODE_TABLE)
                      else {
                        SET_MODE_LOPT(STR_WORDS, MODE_WORD)
                        else {
                          DISPLAY_AND_EXIT(HELP_SOPT, HELP_LOPT, display_help)
                          else {
                            DISPLAY_AND_EXIT(MAN_SOPT, MAN_LOPT, display_man)
                            else {
                              DISPLAY_AND_EXIT(DOPT_SOPT, DOPT_LOPT,
                                  display_options)
                              else {
                                DISPLAY_AND_EXIT(USE_SOPT, USE_LOPT,
                                    display_usage)
                                else {
                                  DISPLAY_AND_EXIT(VER_SOPT, VER_LOPT,
                                      display_version)
                                  else {
                                    PRINT_ERREUR(i);
                                    return ERREUR_OPTION;
                                  }
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    } else {
      if (IS_EMPTY(opt.filename)) {
        if (strcmp(opt_argtab[i], "-") == 0) {
        } else {
          opt.filename = opt_argtab[i];
        }
      } else {
        fprintf(stderr,
            XSTR(NAME) ": Extra operand '%s'.\n"
            "Try '" XSTR(NAME) " " XSTR(HELP_LOPT)
            "' for more information.\n",
            opt_argtab[i]);
        return ERREUR_OPTION;
      }
    }
    i++;
  }
  srand(opt.seed);
  return OPTION_VALID;
}

unsigned int opt_get_limit() {
  return opt.limit;
}

unsigned int opt_get_order() {
  return opt.order;
}

unsigned int opt_get_seed() {
  return opt.seed;
}

unsigned int opt_get_size_max() {
  return opt.size_max;
}

unsigned int opt_get_width() {
  return opt.width;
}

size_t opt_get_col_word() {
  return opt.col_word;
}

void opt_reset_col_word() {
  opt.col_word = 0;
}

void opt_set_col_word(size_t k) {
  opt.col_word = k;
}

int opt_get_mode() {
  return opt.mode;
}

const char *opt_get_filename() {
  return opt.filename;
}
