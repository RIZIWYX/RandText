//  Partie implantation du module malloccp.

//  AUCUNE MODIFICATION DE CE SOURCE N'EST AUTORISÉE.

#include <time.h>
#include "malloccp.h"

#if __CH_VERSION_MALLOCCP_H__ != 202602L
#error Bad 'malloccp' version.
#else

//  _malloccp_ : état du planificateur. Au premier appel à l'une quelconque des
//    huit macrofonctions, la valeur du champ status vaut TO_BE_INITIALIZED ; ce
//    premier appel tente de faire enregistrer par la fonction standard atexit
//    la fonction locale malloccp__atexit ; en cas de succès, la valeur du
//    champ status est fixée à SET_BY_THE_USER ou CHOSEN_AT_RANDOM selon que la
//    valeur de MALLOCCP est (strictement) positive ou (strictement) négative.
//    Le champ count mémorise le nombre nombre d'appels aux macrofonctions, le
//    champ limit, la limite, le champ randommax, l'opposé de la valeur de
//    MALLOCCP lorsque la valeur du champ status est CHOSEN_AT_RANDOM.

static struct {
  enum {
    TO_BE_INITIALIZED,
    SET_BY_THE_USER,
    CHOSEN_AT_RANDOM,
  } status;
  unsigned long long int count;
  unsigned long long int limit;
  unsigned long long int randommax;
} _malloccp_ = {
  .status = TO_BE_INITIALIZED,
};

//  malloccp__atexit : affiche les messages d'informations de fin d'exécution.
static void malloccp__atexit();

//  malloccp__internal_error : affiche un message d'erreur signifiant l'échec de
//    l'opération signifiée par la chaine pointée par cause. Puis met fin à
//    l'exécution du programme.
[[noreturn]] static void malloccp__internal_error(const char *cause);

//  malloccp__prisection : procédure d'affichage.
static void malloccp__prisection(const char *name);

//  PRI, PRE_ERR, PRE_HEAD, PRE_NEXT : macros utilisées dans les divers
//    affichages.

#define PRI(pre, format)  pre "%40s" " " format "\n"

#define PRE_ERR   "###"
#define PRE_HEAD  "---"
#define PRE_NEXT  "   "

bool malloccp__crash(
    long long int n,
    const char *file, int line, const char *func,
    const char *allocfunccalled) {
  if (_malloccp_.status == TO_BE_INITIALIZED) {
    if (atexit(malloccp__atexit) != 0) {
      malloccp__internal_error("atexit");
    }
    _malloccp_.count = 0;
    if (n >= 0) {
      _malloccp_.status = SET_BY_THE_USER;
      _malloccp_.limit = (unsigned long long int) n;
    } else {
      _malloccp_.status = CHOSEN_AT_RANDOM;
      _malloccp_.randommax = 1 + (unsigned long long int) ~n;
      struct timespec ts;
      timespec_get(&ts, TIME_UTC);
      unsigned long long int x
        = (unsigned long long int) ts.tv_sec
          + (unsigned long long int) ts.tv_nsec * 7;
      x *= 2'971'215'073;           //  nombre de Fibonacci premier F_{47}
      x *= 99'194'853'094'755'497;  //  nombre de Fibonacci premier F_{83}
      _malloccp_.limit = 1 + x % _malloccp_.randommax;
    }
  }
  ++_malloccp_.count;
  if (_malloccp_.count != _malloccp_.limit) {
    return false;
  }
  malloccp__prisection("SIMULATED MEMORY ALLOCATION ERROR");
  fprintf(stderr,
      PRI(PRE_HEAD, "%s")
      PRI(PRE_NEXT, "%d")
      PRI(PRE_NEXT, "%s")
      PRI(PRE_NEXT, "%s"),
      "call to", allocfunccalled,
      "at line", line,
      "in function", func,
      "from source", file);
  return true;
}

void malloccp__atexit() {
  if (_malloccp_.count < _malloccp_.limit) {
    malloccp__prisection("NO MEMORY ALLOCATION ERRORS SIMULATED");
    fprintf(stderr,
        PRI(PRE_HEAD, "%llu")
        PRI(PRE_NEXT, "%llu"),
        "with a calls number of", _malloccp_.count,
        "out of a limit of", _malloccp_.limit);
  } else {
    malloccp__prisection("ONE MEMORY ALLOCATION ERROR SIMULATED");
    fprintf(stderr,
        PRI(PRE_HEAD, "%llu"),
        "at call number", _malloccp_.limit);
  }
  if (_malloccp_.status == CHOSEN_AT_RANDOM) {
    fprintf(stderr,
        PRI(PRE_NEXT, "%llu"),
        "fixed at random in the range 1 to", _malloccp_.randommax);
  }
}

void malloccp__internal_error(const char *cause) {
  fprintf(stderr,
      PRE_ERR " " "'malloccp' internal call to '%s' failed\n",
      cause);
  exit(EXIT_FAILURE);
}

void malloccp__prisection(const char *name) {
  fprintf(stderr, "\nMALLOCCP: %s\n", name);
}

#endif
