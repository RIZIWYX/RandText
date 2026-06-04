//  randtext.c : partie implantation du module randtext
//  Le module randtext fournit les fonctions nécessaires pour générer
//  un texte aléatoire à partir d’un texte donné.

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "malloccp.h"

#include "randtext.h"
#include "option.h"

struct ctrvalref {
  const void **valarray;
  long int cap;
  long int length;
};

ctrvalref *empty_ctrvalref() {
  ctrvalref *ctr = malloc(sizeof(ctrvalref));
  if (IS_EMPTY(ctr)) {
    return EMPTY();
  }
  CAP(ctr) = CAP_MIN;
  LEN(ctr) = 0;
  ARRAY(ctr) = malloc(sizeof(void *) * CAP_MIN);
  if (IS_EMPTY(ARRAY(ctr))) {
    free(ctr);
    return EMPTY();
  }
  ARRAY_VAL(ctr, 0) = EMPTY();
  return ctr;
}

void *array_empty() {
  const void **keyarray = malloc(sizeof(*keyarray) * (opt_get_order()));
  if (IS_EMPTY(keyarray)) {
    return EMPTY();
  }
  for (size_t k = 0; k < opt_get_order(); ++k) {
    keyarray[k] = ARRAY_EMPTY_VAL;
  }
  return (void *) keyarray;
}

void *new_key(const void **keyarray, const void *ref) {
  const void **a = malloc(sizeof(*a) * (opt_get_order()));
  if (IS_EMPTY(a)) {
    return EMPTY();
  }
  for (size_t k = 0; k < opt_get_order() - 1; ++k) {
    a[k] = keyarray[k + 1];
  }
  a[opt_get_order() - 1] = ref;
  return (void *) a;
}

ctrvalref *new_val(ctrvalref *ctr, const void *ref) {
  if (IS_EMPTY(ref)) {
    return EMPTY();
  }
  if (CAP(ctr) == LEN(ctr)) {
    const void **a;
    if ((size_t) CAP(ctr) > SIZE_MAX / (sizeof(*a) * CAP_MUL)) {
      return EMPTY();
    }
    a = realloc(ARRAY(ctr), sizeof(*a) * (size_t) (CAP(ctr)) * CAP_MUL);
    if (IS_EMPTY(a)) {
      return EMPTY();
    }
    ARRAY(ctr) = a;
    CAP(ctr) *= CAP_MUL;
  }
  ARRAY_VAL(ctr, LEN(ctr)) = ref;
  ++LEN(ctr);
  return ctr;
}

void *rand_val(ctrvalref *ctr) {
  size_t k = (size_t) ((double) LEN(ctr)
      * ((double) rand() / (RAND_MAX + 1.0)));
  return (void *) ARRAY_VAL(ctr, k);
}

void rand_key(const void **array, const void *ref) {
  memmove(array, array + 1, (opt_get_order() - 1) * sizeof(void *));
  array[opt_get_order() - 1] = ref;
}

int ctrvalref_is_empty(ctrvalref *ctr) {
  return LEN(ctr) == 0;
}

void display_table(const void **key, ctrvalref *ctr) {
  for (size_t k = 0; k < opt_get_order(); ++k) {
    printf("%s\t", (const char *) key[k]);
  }
  for (long int k = 0; k < LEN(ctr); ++k) {
    printf("%s", (const char *) ARRAY_VAL(ctr, k));
    if (k < LEN(ctr) - 1) {
      printf("\t");
    }
  }
  printf("\n");
}

void display_all_word(const void *ref) {
  printf("%s", (const char *) ref);
  printf("\n");
}

int display_randtext(size_t *count,
    ctrvalref *val_ref,
    const void **array) {
  if (IS_EMPTY(val_ref) || ctrvalref_is_empty(val_ref)) {
    return EXIT_ERR;
  }
  const char *r = rand_val(val_ref);
  size_t len = strlen(r);
  if (len == 0) {
    return EXIT_ERR;
  }
  if (opt_get_width() == 0) {
    /*
     * Largeur illimitée : mots séparés par une simple espace,
     * pas d'espace avant le premier mot.
     */
    if (*count > 0) {
      putchar(' ');
    }
    fputs(r, stdout);
  } else {
    if (*count == 0) {
      /*
       * Premier mot du texte : pas d'espace devant.
       */
      fputs(r, stdout);
      opt_set_col_word((len >= opt_get_width()) ? 0 : len);
    } else if (opt_get_col_word() + 1 + len <= opt_get_width()) {
      /*
       * Le mot tient dans la fenêtre courante avec son espace séparatrice.
       */
      putchar(' ');
      fputs(r, stdout);
      opt_set_col_word(opt_get_col_word() + 1 + len);
      if (opt_get_col_word() == opt_get_width()) {
        opt_reset_col_word();
      }
    } else {
      /*
       * Le mot ne rentre pas : on complète la fenêtre courante avec
       * des espaces "à droite", puis une espace "à gauche", puis le mot.
       * Pas de saut de ligne : le terminal wrappera de lui-même.
       */
      for (size_t k = opt_get_col_word(); k < opt_get_width(); ++k) {
        putchar(' ');
      }
      putchar(' ');
      fputs(r, stdout);
      opt_set_col_word((len >= opt_get_width()) ? 0 : 1 + len);
      if (opt_get_col_word() == opt_get_width()) {
        opt_reset_col_word();
      }
    }
  }
  rand_key(array, r);
  ++(*count);
  return EXIT_OK;
}

FILE *open_file() {
  FILE *in = EMPTY();
  if (opt_get_filename() != EMPTY()) {
    in = fopen(opt_get_filename(), "r");
    if (IS_EMPTY(in)) {
      fprintf(stderr, "randtext: Impossible to open file '%s'\n",
          opt_get_filename());
      exit(EXIT_FAILURE);
    }
  }
  return in;
}

size_t size() {
  return (size_t) opt_get_size_max();
}

size_t limit() {
  return (size_t) opt_get_limit();
}

size_t order() {
  return (size_t) opt_get_order();
}

int mode() {
  return opt_get_mode();
}

void dispose_ctr(ctrvalref *ctr) {
  free(ARRAY(ctr));
  free(ctr);
}

int getoption(int nb_arg, char *opt_argtab[]) {
  return get_option(nb_arg, opt_argtab);
}
