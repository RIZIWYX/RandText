// Génère sur la sortie standard un texte aléatoire à partir d’un texte lu
// sur l’entrée standard ou dans un fichier. Le texte produit est basé sur des
// propriétés statistiques du texte d’origine : chaque mot généré dépend des
// mots qui le précèdent dans le texte initial.
//
// Limites :
// - les mots sont obtenus par lecture sur l’entrée des suites consécutives
//   de caractères de longueur maximale mais majorée par la taille fixé par
//   lutilisateur ou la taille par defaut qui
//   ne sont pas de la catégorie isspace ;
// - toute suite de tels caractères de longueur strictement supérieure à
//   cette taille se retrouve ainsi découpée en plusieurs mots ;
// - le texte généré dépend uniquement des statistiques observées dans
//   le texte d’entrée et peut donc contenir des répétitions ou des
//   séquences inattendues.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashtable.h"
#include "holdall.h"
#include "malloccp.h"

#include "randtext.h"

#define RATIO 2.0
#define PREMIER_NUMBER 37
#define EXEC_CONTINUE 0

//  str_hashfun :
//    l'une des fonctions de pré-hachage conseillées par Kernighan
//    et Pike pour les chaines de caractères.
static size_t str_hashfun(const char *s);

//  context_cmp:
//    fonction de comparaison de chaine de caractére contenu  dans un tableau de
//    taille order
static int context_cmp(const void *a, const void *b);

//  context_hash :
//    fonction de hachage appliqué a chaque chaine du tableau
static size_t context_hash(const void *k);

//  rfree :
//    libère la zone mémoire pointée par ptr et renvoie zéro.
static int rfree(void *ptr);

//  display_rtable :
//    pour holdall_apply_context2 :
//    affiche la ligne de table
//    correspondant à la clé 'key' (tableau de pointeurs).
static int display_rtable(void *key, void *ctr);

//  dispose_rctr :  retire de la table de hachage associée
//    à h_keyword l'entrée de clé key et libère les ressources associées à ctr
//     si ctr est non nul. Libère ensuite la zone mémoire
//     pointée par key. Renvoie zéro.
static int dispose_rctr(void *h_keyword, void *key, void *ctr);

//  display_all_rword : affiche sur la sortie standard la chaîne pointée par
//    ref suivie d'un retour à la ligne. Renvoie zéro.
static int display_all_rword(void *ref);

int main(int argc, char *argv[]) {
  int ropt = getoption(argc, argv);
  if (ropt != OPTION_VALID) {
    return ropt;
  }
  FILE *in = open_file();
  if (IS_EMPTY(in)) {
    in = stdin;
  }
  int r = EXIT_SUCCESS;
  bool owned_by_hold_key = false;
  hashtable *h_word = hashtable_empty(
        (int (*)(const void *, const void *)) strcmp,
        (size_t (*)(const void *)) str_hashfun,
        RATIO);
  holdall *hold_word = holdall_empty();
  hashtable *h_keyword = hashtable_empty(
        (int (*)(const void *, const void *)) context_cmp,
        (size_t (*)(const void *)) context_hash,
        RATIO);
  holdall *hold_key = holdall_empty();
  const void **keyarray = array_empty();
  char *w = malloc(WORD_LEN_MAX);
  if (IS_EMPTY(h_word) || IS_EMPTY(h_keyword) || IS_EMPTY(hold_word)
      || IS_EMPTY(hold_key) || IS_EMPTY(keyarray) || IS_EMPTY(w)) {
    goto error_capacity;
  }
  int c = 0;
  bool last_iter = false;
  while (true) {
    if (!last_iter) {
      c = fscanf(in, "%s", w);
      if (c == EOF) {
        last_iter = true;
        w[0] = '\0';
      } else if (c == 0) {
        goto error_read;
      } else {
        size_t sz = size();
        if (strlen(w) > sz) {
          w[sz] = '\0';
          fprintf(stderr, XSTR(NAME) ": Word cut '%s...'.\n", w);
        }
      }
    }
    ctrvalref *val_ref = hashtable_search(h_keyword, keyarray);
    owned_by_hold_key = false;
    if (IS_EMPTY(val_ref)) {
      if (holdall_put(hold_key, keyarray) != 0) {
        goto error_capacity;
      }
      owned_by_hold_key = true;
      ctrvalref *ctrptr = empty_ctrvalref();
      if (IS_EMPTY(ctrptr)) {
        goto error_capacity;
      }
      if (hashtable_add(h_keyword, keyarray, ctrptr) == EMPTY()) {
        dispose_ctr(ctrptr);
        goto error_capacity;
      }
      val_ref = ctrptr;
    }
    if (last_iter) {
      if (new_val(val_ref, "") == EMPTY()) {
        goto error_capacity;
      }
      break;
    }
    const void *ref = hashtable_search(h_word, w);
    if (IS_EMPTY(ref)) {
      char *s = malloc(strlen(w) + 1);
      if (IS_EMPTY(s)) {
        goto error_capacity;
      }
      strcpy(s, w);
      if (hashtable_add(h_word, s, s) == EMPTY()) {
        free(s);
        goto error_capacity;
      }
      if (holdall_put(hold_word, s) != 0) {
        free(s);
        goto error_capacity;
      }
      ref = s;
    }
    if (new_val(val_ref, ref) == EMPTY()) {
      goto error_capacity;
    }
    if (!owned_by_hold_key) {
      rand_key(keyarray, ref);
    } else {
      const void **old = keyarray;
      keyarray = new_key(old, ref);
      if (IS_EMPTY(keyarray)) {
        keyarray = old;
        goto error_capacity;
      }
    }
  }
  if (!feof(in)) {
    goto error_read;
  }
  if (mode() == MODE_TEXT) {
    const void **array = array_empty();
    if (IS_EMPTY(array)) {
      goto error_capacity;
    }
    size_t count = 0;
    while (count < limit()) {
      ctrvalref *val_ref = hashtable_search(h_keyword, array);
      if (display_randtext(&count, val_ref, array) != EXIT_OK) {
        break;
      }
    }
    if (count > 0) {
      printf("\n");
    }
    free(array);
  } else if (mode() == MODE_TABLE) {
    holdall_apply_context(
        hold_key,
        h_keyword,
        (void *(*)(void *, void *)) hashtable_search,
        display_rtable);
  } else {
    holdall_apply(hold_word, display_all_rword);
    printf("\n");
  }
  goto dispose;
error_capacity:
  fprintf(stderr, "*** Error: Not enough memory\n");
  goto error;
error_read:
  fprintf(stderr, "*** Error: A read error occurs\n");
  goto error;
error:
  r = EXIT_FAILURE;
dispose:
  if (!owned_by_hold_key) {
    free(keyarray);
  }
  free(w);
  if (hold_key != EMPTY()) {
    holdall_apply_context2(
        hold_key,
        h_keyword,
        (void *(*)(void *, void *)) hashtable_search,
        h_keyword,
        dispose_rctr);
  }
  hashtable_dispose(&h_keyword);
  holdall_dispose(&hold_key);
  hashtable_dispose(&h_word);
  if (hold_word != EMPTY()) {
    holdall_apply(hold_word, rfree);
  }
  holdall_dispose(&hold_word);
  if (in != stdin) {
    fclose(in);
  }
  return r;
}

size_t str_hashfun(const char *s) {
  size_t h = 0;
  for (const unsigned char *p = (const unsigned char *) s; *p != '\0'; ++p) {
    h = PREMIER_NUMBER * h + *p;
  }
  return h;
}

int context_cmp(const void *a, const void *b) {
  const char **ctx_a = (const char **) a;
  const char **ctx_b = (const char **) b;
  for (size_t i = 0; i < order(); i++) {
    int r = strcmp(ctx_a[i], ctx_b[i]);
    if (r != 0) {
      return r;
    }
  }
  return 0;
}

size_t context_hash(const void *k) {
  const char * const *ctx = k;
  size_t h = 0;
  for (size_t i = 0; i < order(); i++) {
    for (const unsigned char *p = (const unsigned char *) ctx[i];
        *p != '\0'; ++p) {
      h = PREMIER_NUMBER * h + *p;
    }
  }
  return h;
}

int rfree(void *ptr) {
  free(ptr);
  return EXEC_CONTINUE;
}

int display_rtable(void *key, void *ctr) {
  display_table((const void **) key,
      (ctrvalref *) ctr);
  return EXEC_CONTINUE;
}

int dispose_rctr(void *h_keyword, void *key, void *ctr) {
  if (ctr != EMPTY()) {
    hashtable_remove((hashtable *) h_keyword, key);
    dispose_ctr((ctrvalref *) ctr);
  }
  free(key);
  return EXEC_CONTINUE;
}

int display_all_rword(void *ref) {
  display_all_word(ref);
  return EXEC_CONTINUE;
}
