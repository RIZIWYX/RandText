#!/bin/bash
# test_randtext.sh — Script de test complet pour le projet randtext.
#
# Usage : ./test_randtext.sh [OPTIONS]
#   -r REF   chemin vers l'exécutable de référence  (défaut : ./randtext_ref)
#   -e EXE   chemin vers votre exécutable            (défaut : ./randtext)
#   -f FILE  fichier texte source pour les tests     (défaut : knock.txt généré)
#   -v       mode verbeux (affiche les diff complets)
#   -k       continue même après un échec
#   -c       affiche uniquement le résumé (mode CI)

RED='\033[0;31m'; GREEN='\033[0;32m'; YELLOW='\033[1;33m'
BLUE='\033[0;34m'; CYAN='\033[0;36m'; BOLD='\033[1m'; NC='\033[0m'

REF="./randtext_ref"; EXE="./randtext"; TXTFILE=""
VERBOSE=0; KEEP_GOING=0; CI_MODE=0
PASS=0; FAIL=0; SKIP=0; TOTAL=0

while getopts "r:e:f:vkc" opt; do
  case $opt in
    r) REF="$OPTARG" ;; e) EXE="$OPTARG" ;; f) TXTFILE="$OPTARG" ;;
    v) VERBOSE=1 ;; k) KEEP_GOING=1 ;; c) CI_MODE=1 ;;
    *) echo "Usage: $0 [-r ref] [-e exe] [-f file] [-v] [-k] [-c]"; exit 1 ;;
  esac
done

TMPDIR_TEST=$(mktemp -d /tmp/randtext_test.XXXXXX)
trap 'rm -rf "$TMPDIR_TEST"' EXIT

if [ -z "$TXTFILE" ]; then
  TXTFILE="$TMPDIR_TEST/knock.txt"
  cat > "$TXTFILE" << 'EOF'
Knock: De quoi souffrez-vous?
Le Tambour: Attendez que je réfléchisse! Voilà. Quand j'ai dîné,
il y a des fois que je sens une espèce de démangeaison ici.
Ça me chatouille, ou plutôt, ça me grattouille.
Knock: Attention. Ne confondons pas.
Est-ce que ça vous chatouille, ou est-ce que ça vous grattouille?
Le Tambour: Ça me grattouille. Mais ça me chatouille bien un peu aussi.
Knock: Désignez-moi exactement l'endroit.
Le Tambour: Par ici.
Knock: Par ici... où cela, par ici?
Le Tambour: Là. Ou peut-être là... Entre les deux.
Knock: Juste entre les deux? Est-ce que ça ne serait pas plutôt un rien à gauche?
Le Tambour: Il me semble bien.
Knock: Ça vous fait mal quand j'enfonce le doigt?
Le Tambour: Oui, on dirait que ça me fait mal.
Knock: Ah! ah! Est-ce que ça ne vous grattouille pas davantage quand vous avez mangé de la tête de veau à la vinaigrette?
Le Tambour: Je n'en mange jamais. Mais il me semble que si j'en mangeais, effectivement, ça me grattouillerait plus.
EOF
  echo -e "${YELLOW}[INFO]${NC} Fichier de test généré : $TXTFILE"
fi

SIMPLE_FILE="$TMPDIR_TEST/simple.txt";  echo "a b a a b a b" > "$SIMPLE_FILE"
EMPTY_FILE="$TMPDIR_TEST/empty.txt";    touch "$EMPTY_FILE"
ONE_FILE="$TMPDIR_TEST/one.txt";        echo "bonjour" > "$ONE_FILE"
TWO_FILE="$TMPDIR_TEST/two.txt";        echo "hello world" > "$TWO_FILE"
THREE_FILE="$TMPDIR_TEST/three.txt";    echo "a b c" > "$THREE_FILE"
REP_FILE="$TMPDIR_TEST/rep.txt";        echo "a a a a a" > "$REP_FILE"
LONG_FILE="$TMPDIR_TEST/long.txt"
python3 -c "print('a' * 100 + ' b ' + 'c' * 200 + ' d normal')" > "$LONG_FILE"

print_header() {
  echo ""
  echo -e "${BOLD}${BLUE}══════════════════════════════════════════════════════${NC}"
  echo -e "${BOLD}${BLUE}  $1${NC}"
  echo -e "${BOLD}${BLUE}══════════════════════════════════════════════════════${NC}"
}
print_section() { echo ""; echo -e "${CYAN}── $1 ──${NC}"; }

compare_outputs() {
  local name="$1"; shift; local args=("$@"); TOTAL=$((TOTAL + 1))
  local ref_out="$TMPDIR_TEST/ref_out_$$" exe_out="$TMPDIR_TEST/exe_out_$$"
  local ref_err="$TMPDIR_TEST/ref_err_$$" exe_err="$TMPDIR_TEST/exe_err_$$"
  "$REF" "${args[@]}" >"$ref_out" 2>"$ref_err"; local ref_code=$?
  "$EXE" "${args[@]}" >"$exe_out" 2>"$exe_err"; local exe_code=$?
  local stdout_ok=0 code_ok=0
  diff -q "$ref_out" "$exe_out" >/dev/null 2>&1 && stdout_ok=1
  [ "$ref_code" -eq "$exe_code" ] && code_ok=1
  if [ $stdout_ok -eq 1 ] && [ $code_ok -eq 1 ]; then
    PASS=$((PASS + 1)); [ $CI_MODE -eq 0 ] && echo -e "  ${GREEN}✓${NC} $name"
  else
    FAIL=$((FAIL + 1)); echo -e "  ${RED}✗${NC} $name"
    [ $code_ok -eq 0 ] && echo -e "    ${YELLOW}code${NC}: ref=$ref_code exe=$exe_code"
    if [ $stdout_ok -eq 0 ]; then
      echo -e "    ${YELLOW}diff${NC}:"; diff "$ref_out" "$exe_out" | head -20 | sed 's/^/      /'
    fi
    [ $KEEP_GOING -eq 0 ] && { rm -f "$ref_out" "$exe_out" "$ref_err" "$exe_err"; return 1; }
  fi
  rm -f "$ref_out" "$exe_out" "$ref_err" "$exe_err"; return 0
}

compare_stdin() {
  local name="$1" stdin_file="$2"; shift 2; local args=("$@"); TOTAL=$((TOTAL + 1))
  local ref_out="$TMPDIR_TEST/ref_out_$$" exe_out="$TMPDIR_TEST/exe_out_$$"
  "$REF" "${args[@]}" <"$stdin_file" >"$ref_out" 2>/dev/null; local ref_code=$?
  "$EXE" "${args[@]}" <"$stdin_file" >"$exe_out" 2>/dev/null; local exe_code=$?
  if diff -q "$ref_out" "$exe_out" >/dev/null 2>&1 && [ "$ref_code" -eq "$exe_code" ]; then
    PASS=$((PASS + 1)); [ $CI_MODE -eq 0 ] && echo -e "  ${GREEN}✓${NC} $name"
  else
    FAIL=$((FAIL + 1)); echo -e "  ${RED}✗${NC} $name"
    [ "$ref_code" -ne "$exe_code" ] && echo -e "    code: ref=$ref_code exe=$exe_code"
    diff "$ref_out" "$exe_out" | head -15 | sed 's/^/      /'
  fi
  rm -f "$ref_out" "$exe_out"
}

check_exit_code() {
  local name="$1" expected="$2"; shift 2; TOTAL=$((TOTAL + 1))
  "$EXE" "$@" >/dev/null 2>&1; local code=$?
  if [ "$code" -eq "$expected" ]; then
    PASS=$((PASS + 1)); [ $CI_MODE -eq 0 ] && echo -e "  ${GREEN}✓${NC} $name (exit=$code)"
  else
    FAIL=$((FAIL + 1)); echo -e "  ${RED}✗${NC} $name — attendu exit=$expected, obtenu exit=$code"
  fi
}

check_stderr_nonempty() {
  local name="$1"; shift; TOTAL=$((TOTAL + 1))
  local err_out="$TMPDIR_TEST/err_$$"
  "$EXE" "$@" >/dev/null 2>"$err_out"
  if [ -s "$err_out" ]; then
    PASS=$((PASS + 1)); [ $CI_MODE -eq 0 ] && echo -e "  ${GREEN}✓${NC} $name"
  else
    FAIL=$((FAIL + 1)); echo -e "  ${RED}✗${NC} $name — aucun message sur stderr"
  fi
  rm -f "$err_out"
}

# compare_help : compare stdout en ignorant le nom de l'exécutable (ligne 1)
compare_help() {
  local name="$1"; shift; TOTAL=$((TOTAL + 1))
  local ref_out="$TMPDIR_TEST/ref_h_$$" exe_out="$TMPDIR_TEST/exe_h_$$"
  "$REF" "$@" 2>/dev/null | tail -n +2 >"$ref_out"
  "$EXE" "$@" 2>/dev/null | tail -n +2 >"$exe_out"
  if diff -q "$ref_out" "$exe_out" >/dev/null 2>&1; then
    PASS=$((PASS + 1)); [ $CI_MODE -eq 0 ] && echo -e "  ${GREEN}✓${NC} $name (hors ligne 1)"
  else
    FAIL=$((FAIL + 1)); echo -e "  ${RED}✗${NC} $name"
    diff "$ref_out" "$exe_out" | head -20 | sed 's/^/      /'
  fi
  rm -f "$ref_out" "$exe_out"
}

# ── Vérification des exécutables ─────────────────────────────────────────────
echo -e "${BOLD}Vérification des exécutables...${NC}"
[ ! -x "$REF" ] && { echo -e "${RED}[ERREUR]${NC} Référence introuvable : $REF"; exit 1; }
[ ! -x "$EXE" ] && { echo -e "${RED}[ERREUR]${NC} Exécutable introuvable : $EXE"; exit 1; }
echo -e "  ref : ${GREEN}$REF${NC}"; echo -e "  exe : ${GREEN}$EXE${NC}"; echo -e "  txt : ${GREEN}$TXTFILE${NC}"

# ════════════════════════════════════════════════════════════════════════════
print_header "1. ORDRES 1 À 20 — texte normal"
# ════════════════════════════════════════════════════════════════════════════
for order in $(seq 1 20); do
  compare_outputs "ordre $order graine 1"   -o "$order" -s 1   -l 50  "$TXTFILE"
  compare_outputs "ordre $order graine 42"  -o "$order" -s 42  -l 50  "$TXTFILE"
  compare_outputs "ordre $order graine 999" -o "$order" -s 999 -l 100 "$TXTFILE"
done

# ════════════════════════════════════════════════════════════════════════════
print_header "2. ORDRES 1 À 20 — affichage table (-t)"
# ════════════════════════════════════════════════════════════════════════════
for order in $(seq 1 20); do
  compare_outputs "table ordre $order" -t -o "$order" "$TXTFILE"
done
compare_outputs "table simple ordre 1" -t          "$SIMPLE_FILE"
compare_outputs "table simple ordre 2" -t -o 2     "$SIMPLE_FILE"
compare_outputs "table simple ordre 3" -t -o 3     "$SIMPLE_FILE"
compare_outputs "table two mots ordre 1" -t        "$TWO_FILE"
compare_outputs "table two mots ordre 2" -t -o 2   "$TWO_FILE"
compare_outputs "table repetitions"    -t           "$REP_FILE"
compare_outputs "table un mot"         -t           "$ONE_FILE"
compare_outputs "table fichier vide"   -t           "$EMPTY_FILE"
compare_outputs "table three ordre 1"  -t           "$THREE_FILE"
compare_outputs "table three ordre 2"  -t -o 2      "$THREE_FILE"
compare_outputs "table three ordre 3"  -t -o 3      "$THREE_FILE"

# ════════════════════════════════════════════════════════════════════════════
print_header "3. ORDRES 1 À 20 — affichage mots (-w)"
# ════════════════════════════════════════════════════════════════════════════
for order in $(seq 1 20); do
  compare_outputs "mots ordre $order" -w -o "$order" "$TXTFILE"
done
compare_outputs "mots simple"      -w "$SIMPLE_FILE"
compare_outputs "mots repetitions" -w "$REP_FILE"
compare_outputs "mots un mot"      -w "$ONE_FILE"
compare_outputs "mots fichier vide" -w "$EMPTY_FILE"

# ════════════════════════════════════════════════════════════════════════════
print_header "4. ALIAS -r table / -r words"
# ════════════════════════════════════════════════════════════════════════════
compare_outputs "-r table ordre 1"  -r table -o 1 "$TXTFILE"
compare_outputs "-r table ordre 2"  -r table -o 2 "$TXTFILE"
compare_outputs "-r words ordre 1"  -r words       "$TXTFILE"
compare_outputs "--reveal=table o1" --reveal=table -o 1 "$TXTFILE"
compare_outputs "--reveal=words"    --reveal=words       "$TXTFILE"
compare_outputs "-r table == -t"    -r table -o 3 -s 1 "$SIMPLE_FILE"
compare_outputs "-r words == -w"    -r words             "$SIMPLE_FILE"

# ════════════════════════════════════════════════════════════════════════════
print_header "5. VALEURS D'ORDRE HORS INTERVALLE"
# ════════════════════════════════════════════════════════════════════════════
print_section "Ordre 0 (invalide)"
check_exit_code "ordre 0 → exit 64" 64 -o 0 "$TXTFILE"
check_stderr_nonempty "ordre 0 → message stderr" -o 0 "$TXTFILE"
print_section "Ordres très grands"
for order in 50 100 200 1000; do
  compare_outputs "ordre $order vs ref" -o "$order" -s 1 -l 20 "$TXTFILE"
done

# ════════════════════════════════════════════════════════════════════════════
print_header "6. OPTION -l / --limit"
# ════════════════════════════════════════════════════════════════════════════
compare_outputs "limit 0"             -l 0    -s 1 "$TXTFILE"
compare_outputs "limit 1"             -l 1    -s 1 "$TXTFILE"
compare_outputs "limit 5"             -l 5    -s 1 "$TXTFILE"
compare_outputs "limit 10"            -l 10   -s 1 "$TXTFILE"
compare_outputs "limit 100"           -l 100  -s 1 "$TXTFILE"
compare_outputs "limit 200"           -l 200  -s 1 "$TXTFILE"
compare_outputs "--limit=50 long"     --limit=50 -s 1 "$TXTFILE"
compare_outputs "limit 0 table"       -t -l 0    -s 1 "$TXTFILE"
compare_outputs "limit 0 mots"        -w -l 0         "$TXTFILE"
compare_outputs "limit 1 simple"      -l 1    -s 1 "$SIMPLE_FILE"
compare_outputs "limit 3 simple"      -l 3    -s 1 "$SIMPLE_FILE"

# ════════════════════════════════════════════════════════════════════════════
print_header "7. OPTION -s / --seed"
# ════════════════════════════════════════════════════════════════════════════
for seed in 0 1 2 7 42 100 999 12345 65535; do
  compare_outputs "seed $seed"            -s "$seed" -l 50 "$TXTFILE"
  compare_outputs "--seed=$seed long"     --seed="$seed" -l 50 "$TXTFILE"
done
print_section "Reproductibilité"
T1="$TMPDIR_TEST/r1.txt"; T2="$TMPDIR_TEST/r2.txt"
"$EXE" -s 42 -l 50 "$TXTFILE" >"$T1" 2>/dev/null
"$EXE" -s 42 -l 50 "$TXTFILE" >"$T2" 2>/dev/null
TOTAL=$((TOTAL + 1))
if diff -q "$T1" "$T2" >/dev/null 2>&1; then
  PASS=$((PASS + 1)); [ $CI_MODE -eq 0 ] && echo -e "  ${GREEN}✓${NC} reproductibilité seed=42"
else
  FAIL=$((FAIL + 1)); echo -e "  ${RED}✗${NC} reproductibilité seed=42"
fi
rm -f "$T1" "$T2"
print_section "Graines différentes → sorties différentes (probabiliste)"
T1="$TMPDIR_TEST/s1.txt"; T2="$TMPDIR_TEST/s2.txt"
"$EXE" -s 1  -l 50 "$TXTFILE" >"$T1" 2>/dev/null
"$EXE" -s 99 -l 50 "$TXTFILE" >"$T2" 2>/dev/null
TOTAL=$((TOTAL + 1))
if ! diff -q "$T1" "$T2" >/dev/null 2>&1; then
  PASS=$((PASS + 1)); [ $CI_MODE -eq 0 ] && echo -e "  ${GREEN}✓${NC} seed=1 ≠ seed=99"
else
  FAIL=$((FAIL + 1)); echo -e "  ${RED}✗${NC} seed=1 et seed=99 donnent la même sortie"
fi
rm -f "$T1" "$T2"

# ════════════════════════════════════════════════════════════════════════════
print_header "8. OPTION -S / --size (taille max des mots)"
# ════════════════════════════════════════════════════════════════════════════
compare_outputs "size défaut (63)"    -s 1 -l 50 "$TXTFILE"
compare_outputs "size 5"    -S 5  -s 1 -l 50 "$TXTFILE"
compare_outputs "size 10"   -S 10 -s 1 -l 50 "$TXTFILE"
compare_outputs "size 31"   -S 31 -s 1 -l 50 "$TXTFILE"
compare_outputs "size 63"   -S 63 -s 1 -l 50 "$TXTFILE"
compare_outputs "size 1"    -S 1  -s 1 -l 20 "$TXTFILE"
compare_outputs "--size=20 long"   --size=20 -s 1 -l 50 "$TXTFILE"
compare_outputs "size 5 table"  -t -S 5 "$TXTFILE"
compare_outputs "size 5 mots"   -w -S 5 "$TXTFILE"
check_exit_code "size 0 → exit 64" 64 -S 0 "$TXTFILE"
check_stderr_nonempty "size 0 → stderr" -S 0 "$TXTFILE"
print_section "Avertissement mots trop longs"
TOTAL=$((TOTAL + 1))
"$EXE" -S 10 -l 5 "$LONG_FILE" >/dev/null 2>"$TMPDIR_TEST/lerr.txt"
"$REF" -S 10 -l 5 "$LONG_FILE" >/dev/null 2>"$TMPDIR_TEST/rerr.txt"
if [ -s "$TMPDIR_TEST/rerr.txt" ] && [ ! -s "$TMPDIR_TEST/lerr.txt" ]; then
  FAIL=$((FAIL + 1)); echo -e "  ${RED}✗${NC} pas d'avertissement pour mots trop longs"
else
  PASS=$((PASS + 1)); [ $CI_MODE -eq 0 ] && echo -e "  ${GREEN}✓${NC} avertissement mots trop longs"
fi

# ════════════════════════════════════════════════════════════════════════════
print_header "9. OPTION -W / --width (largeur affichage)"
# ════════════════════════════════════════════════════════════════════════════
compare_outputs "width 0 (défaut)"   -W 0   -s 1 -l 50 "$TXTFILE"
compare_outputs "width ignoré par -t" -t -W 80 -s 1 "$TXTFILE"
compare_outputs "width ignoré par -w" -w -W 80 "$TXTFILE"
compare_outputs "width 1 (très étroit)" -W 1 -s 1 -l 20 "$TXTFILE"

# ════════════════════════════════════════════════════════════════════════════
print_header "10. OPTION -o / --order (formes longues)"
# ════════════════════════════════════════════════════════════════════════════
for o in 1 2 3 5 10 15 20; do
  compare_outputs "--order=$o long" --order="$o" -s 1 -l 50 "$TXTFILE"
done

# ════════════════════════════════════════════════════════════════════════════
print_header "11. OPTION DERNIÈRE VALEUR GAGNE"
# ════════════════════════════════════════════════════════════════════════════
compare_outputs "seed répétée (dernière gagne)"   -s 1 -s 42       -l 50 "$TXTFILE"
compare_outputs "order répété (dernier gagne)"    -o 1 -o 3   -s 1 -l 50 "$TXTFILE"
compare_outputs "limit répétée (dernière gagne)"  -l 10 -l 50 -s 1       "$TXTFILE"
compare_outputs "size répété (dernier gagne)"     -S 5 -S 31  -s 1 -l 50 "$TXTFILE"
compare_outputs "mode répété (dernier gagne) t+w" -t -w                  "$TXTFILE"
compare_outputs "mode répété (dernier gagne) w+t" -w -t                  "$TXTFILE"
compare_outputs "seed 3 fois"   -s 1 -s 2 -s 42  -l 50 "$TXTFILE"
compare_outputs "order 3 fois"  -o 1 -o 2 -o 3   -s 1 -l 50 "$TXTFILE"

# ════════════════════════════════════════════════════════════════════════════
print_header "12. LECTURE STDIN"
# ════════════════════════════════════════════════════════════════════════════
compare_stdin "stdin sans option"       "$TXTFILE"
compare_stdin "stdin avec -s 1"         "$TXTFILE"   -s 1 -l 50
compare_stdin "stdin avec -o 2"         "$TXTFILE"   -o 2 -s 1 -l 50
compare_stdin "stdin table"             "$TXTFILE"   -t -o 2
compare_stdin "stdin mots"              "$TXTFILE"   -w
compare_stdin "stdin simple table"      "$SIMPLE_FILE" -t
compare_stdin "stdin simple texte"      "$SIMPLE_FILE" -s 1 -l 10
compare_stdin "stdin vide"              "$EMPTY_FILE"
compare_stdin "stdin vide table"        "$EMPTY_FILE"  -t
compare_stdin "stdin option -"          "$TXTFILE"   - -s 1 -l 50

# ════════════════════════════════════════════════════════════════════════════
print_header "13. CAS LIMITES — entrées particulières"
# ════════════════════════════════════════════════════════════════════════════
compare_outputs "fichier vide texte"    -s 1 -l 50 "$EMPTY_FILE"
compare_outputs "fichier vide table"    -t          "$EMPTY_FILE"
compare_outputs "fichier vide mots"     -w          "$EMPTY_FILE"
compare_outputs "un seul mot texte"     -s 1 -l 10  "$ONE_FILE"
compare_outputs "un seul mot table"     -t           "$ONE_FILE"
compare_outputs "un seul mot mots"      -w           "$ONE_FILE"
compare_outputs "deux mots texte"       -s 1 -l 10  "$TWO_FILE"
compare_outputs "deux mots table"       -t           "$TWO_FILE"
compare_outputs "deux mots mots"        -w           "$TWO_FILE"
compare_outputs "répétitions texte"     -s 1 -l 10  "$REP_FILE"
compare_outputs "répétitions table"     -t           "$REP_FILE"
compare_outputs "ordre > nb mots o=5"   -o 5 -s 1 -l 20 "$ONE_FILE"
compare_outputs "ordre > nb mots o=10"  -o 10 -s 1 -l 20 "$THREE_FILE"
compare_outputs "simple s=1 l=3"        -s 1 -l 3  "$SIMPLE_FILE"
compare_outputs "simple s=1 l=1"        -s 1 -l 1  "$SIMPLE_FILE"
compare_outputs "two s=1 l=1"           -s 1 -l 1  "$TWO_FILE"
compare_outputs "two s=1 l=2"           -s 1 -l 2  "$TWO_FILE"

# ════════════════════════════════════════════════════════════════════════════
print_header "14. MESSAGES D'AIDE — -U / --usage"
# ════════════════════════════════════════════════════════════════════════════
print_section "-U et --usage (comparaison brute)"
print_section "-U et --usage (hors ligne 1 = nom exe)"

# ════════════════════════════════════════════════════════════════════════════
print_header "15. MESSAGES D'AIDE — --help / -?"
# ════════════════════════════════════════════════════════════════════════════

# ════════════════════════════════════════════════════════════════════════════
print_header "16. MESSAGES D'AIDE — --man / --version / --options"
# ════════════════════════════════════════════════════════════════════════════

# ════════════════════════════════════════════════════════════════════════════
print_header "17. OPTIONS INVALIDES / NON RECONNUES"
# ════════════════════════════════════════════════════════════════════════════
check_exit_code "option -z → exit 64"    64 -z "$TXTFILE"
check_stderr_nonempty "option -z → stderr"  -z "$TXTFILE"
check_exit_code "option --foo → exit 64" 64 --foo "$TXTFILE"
check_stderr_nonempty "option --foo → stderr" --foo "$TXTFILE"
check_exit_code "option -Z → exit 64"    64 -Z "$TXTFILE"
check_exit_code "-r invalide → exit 64"  64 -r invalid "$TXTFILE"
check_stderr_nonempty "-r invalide → stderr" -r invalid "$TXTFILE"
check_exit_code "option --bar → exit 64" 64 --bar "$TXTFILE"

# ════════════════════════════════════════════════════════════════════════════
print_header "18. SÉPARATEUR '--' (fin des options)"
# ════════════════════════════════════════════════════════════════════════════
compare_outputs "-- puis fichier"          -s 1 -l 50 -- "$TXTFILE"
NEG_FILE="$TMPDIR_TEST/-strange.txt"; cp "$SIMPLE_FILE" "$NEG_FILE"
compare_outputs "-- avec fichier -strange" -s 1 -l 20 -- "$NEG_FILE"
compare_outputs "-- avec table"            -t -- "$TXTFILE"
compare_outputs "-- avec mots"             -w -- "$TXTFILE"
compare_outputs "-- o=2 fichier"           -o 2 -s 1 -l 30 -- "$TXTFILE"

# ════════════════════════════════════════════════════════════════════════════
print_header "19. OPTIONS COLLÉES (valeur sans espace)"
# ════════════════════════════════════════════════════════════════════════════
compare_outputs "-o2 collé"    -o2   -s 1 -l 50 "$TXTFILE"
compare_outputs "-o2 espace"   -o 2  -s 1 -l 50 "$TXTFILE"
compare_outputs "-s1 collé"    -s1   -l 50 "$TXTFILE"
compare_outputs "-s42 collé"   -s42  -l 50 "$TXTFILE"
compare_outputs "-l50 collé"   -l50  -s 1 "$TXTFILE"
compare_outputs "-l0 collé"    -l0   -s 1 "$TXTFILE"
compare_outputs "-S63 collé"   -S63  -s 1 -l 50 "$TXTFILE"
compare_outputs "-S10 collé"   -S10  -s 1 -l 50 "$TXTFILE"

# ════════════════════════════════════════════════════════════════════════════
print_header "20. ORDRES 1-7 — fichier simple (tous modes)"
# ════════════════════════════════════════════════════════════════════════════
for order in $(seq 1 7); do
  compare_outputs "simple texte o=$order s=1"  -o "$order" -s 1  -l 30 "$SIMPLE_FILE"
  compare_outputs "simple texte o=$order s=42" -o "$order" -s 42 -l 30 "$SIMPLE_FILE"
  compare_outputs "simple table o=$order"      -t -o "$order"          "$SIMPLE_FILE"
  compare_outputs "simple mots  o=$order"      -w -o "$order"          "$SIMPLE_FILE"
done

# ════════════════════════════════════════════════════════════════════════════
print_header "21. COMBINAISONS D'OPTIONS — texte normal"
# ════════════════════════════════════════════════════════════════════════════
compare_outputs "o=2 s=42 l=30 S=20"       -o 2 -s 42  -l 30 -S 20      "$TXTFILE"
compare_outputs "o=1 s=1  l=0"             -o 1 -s 1   -l 0              "$TXTFILE"
compare_outputs "o=2 s=1  l=1"             -o 2 -s 1   -l 1              "$TXTFILE"
compare_outputs "o=5 s=99 l=50"            -o 5 -s 99  -l 50             "$TXTFILE"
compare_outputs "o=1 s=7  l=100 W=0"       -o 1 -s 7   -l 100 -W 0      "$TXTFILE"
compare_outputs "o=5 s=1 l=50 simple"      -o 5 -s 1   -l 50            "$SIMPLE_FILE"
compare_outputs "o=3 s=7 l=10 two"         -o 3 -s 7   -l 10            "$TWO_FILE"

print_section "Combinaisons avec table (-t)"
compare_outputs "t + o=2 + s=42"           -t -o 2 -s 42                "$TXTFILE"
compare_outputs "t + o=3 + S=10"           -t -o 3 -S 10                "$TXTFILE"
compare_outputs "t + o=1 simple"           -t -o 1                      "$SIMPLE_FILE"
compare_outputs "t + o=2 two"              -t -o 2                      "$TWO_FILE"
compare_outputs "t + o=5 knock"            -t -o 5                      "$TXTFILE"

print_section "Combinaisons avec mots (-w)"
compare_outputs "w + o=3"                  -w -o 3                      "$TXTFILE"
compare_outputs "w + S=5"                  -w -S 5                      "$TXTFILE"
compare_outputs "w + S=10 simple"          -w -S 10                     "$SIMPLE_FILE"
compare_outputs "w + o=2 S=5"             -w -o 2 -S 5                  "$TXTFILE"

print_section "Combinaisons avec -r"
compare_outputs "-r table o=2 s=1"         -r table -o 2 -s 1           "$TXTFILE"
compare_outputs "-r words S=5"             -r words -S 5                 "$TXTFILE"
compare_outputs "--reveal=table o=3"       --reveal=table -o 3           "$TXTFILE"
compare_outputs "--reveal=words"           --reveal=words                 "$TXTFILE"

# ════════════════════════════════════════════════════════════════════════════
print_header "22. VALEURS LIMITES DES OPTIONS NUMÉRIQUES"
# ════════════════════════════════════════════════════════════════════════════
compare_outputs "limit MAX (9999)"    -l 9999 -s 1 "$TXTFILE"
compare_outputs "order 10"            -o 10   -s 1 -l 50 "$TXTFILE"
compare_outputs "order 20"            -o 20   -s 1 -l 50 "$TXTFILE"
check_exit_code "limit -1 → exit 64"  64 -l -1 "$TXTFILE"
check_exit_code "order 0 → exit 64"   64 -o 0  "$TXTFILE"
check_exit_code "size 0 → exit 64"    64 -S 0  "$TXTFILE"
check_exit_code "width -1 → exit 64"  64 -W -1 "$TXTFILE" 2>/dev/null || true

# ════════════════════════════════════════════════════════════════════════════
print_header "23. FICHIER INEXISTANT / ERREURS I/O"
# ════════════════════════════════════════════════════════════════════════════
check_exit_code "fichier inexistant → 1" 1 "$TMPDIR_TEST/nope.txt"
check_stderr_nonempty "fichier inexistant → stderr" "$TMPDIR_TEST/nope.txt"
check_exit_code "fichier inexistant avec opts → 1" 1 -o 2 -s 1 "$TMPDIR_TEST/nope.txt"

# ════════════════════════════════════════════════════════════════════════════
print_header "24. PLUSIEURS FICHIERS (seul le premier compte)"
# ════════════════════════════════════════════════════════════════════════════
compare_outputs "2 fichiers → seul le 1er"      -s 1 -l 30 "$TXTFILE" "$SIMPLE_FILE"
compare_outputs "2 fichiers table → seul le 1er" -t "$TXTFILE" "$SIMPLE_FILE"

# ════════════════════════════════════════════════════════════════════════════
print_header "25. FORMAT DE SORTIE NORMALE"
# ════════════════════════════════════════════════════════════════════════════
print_section "Dernier caractère = newline unique"
TOTAL=$((TOTAL + 1))
"$EXE" -s 1 -l 10 "$TXTFILE" >"$TMPDIR_TEST/fmt.txt" 2>/dev/null
LAST=$(tail -c 1 "$TMPDIR_TEST/fmt.txt" | xxd | head -1)
if echo "$LAST" | grep -q "0a"; then
  PASS=$((PASS + 1)); [ $CI_MODE -eq 0 ] && echo -e "  ${GREEN}✓${NC} dernier caractère newline (0x0a)"
else
  FAIL=$((FAIL + 1)); echo -e "  ${RED}✗${NC} dernier caractère n'est pas newline : $LAST"
fi

print_section "Pas d'espace avant le premier mot"
TOTAL=$((TOTAL + 1))
FIRST_CHAR=$("$EXE" -s 1 -l 10 "$TXTFILE" 2>/dev/null | head -c 1)
if [ "$FIRST_CHAR" != " " ]; then
  PASS=$((PASS + 1)); [ $CI_MODE -eq 0 ] && echo -e "  ${GREEN}✓${NC} pas d'espace avant le premier mot"
else
  FAIL=$((FAIL + 1)); echo -e "  ${RED}✗${NC} espace détecté avant le premier mot"
fi

print_section "Pas de double espace entre deux mots (width=0)"
TOTAL=$((TOTAL + 1))
if "$EXE" -s 1 -l 50 "$TXTFILE" 2>/dev/null | grep -q "  "; then
  FAIL=$((FAIL + 1)); echo -e "  ${RED}✗${NC} double espace détecté"
else
  PASS=$((PASS + 1)); [ $CI_MODE -eq 0 ] && echo -e "  ${GREEN}✓${NC} pas de double espace"
fi

print_section "Sortie vide si limit=0"
TOTAL=$((TOTAL + 1))
OUT=$("$EXE" -l 0 "$TXTFILE" 2>/dev/null)
if [ -z "$OUT" ]; then
  PASS=$((PASS + 1)); [ $CI_MODE -eq 0 ] && echo -e "  ${GREEN}✓${NC} sortie vide avec limit=0"
else
  FAIL=$((FAIL + 1)); echo -e "  ${RED}✗${NC} sortie non vide avec limit=0"
fi

# ════════════════════════════════════════════════════════════════════════════
print_header "26. TESTS DE STRESS (grands ordres + texte réel)"
# ════════════════════════════════════════════════════════════════════════════
LES_MIS=""
for candidate in ./lesmiserables.txt ../lesmiserables.txt ~/lesmiserables.txt; do
  [ -f "$candidate" ] && LES_MIS="$candidate" && break
done

if [ -n "$LES_MIS" ]; then
  echo -e "  ${CYAN}lesmiserables.txt trouvé : $LES_MIS${NC}"
  for o in 1 2 3; do
    print_section "Les Misérables ordre $o"
    START=$(date +%s%3N)
    compare_outputs "miserables o=$o s=1 l=100"  -o "$o" -s 1  -l 100 "$LES_MIS"
    compare_outputs "miserables o=$o s=42 l=100" -o "$o" -s 42 -l 100 "$LES_MIS"
    compare_outputs "miserables o=$o table"       -t -o "$o"           "$LES_MIS"
    compare_outputs "miserables o=$o mots"        -w -o "$o"           "$LES_MIS"
    END=$(date +%s%3N); echo -e "    temps : $((END - START))ms"
  done
  print_section "Contraintes de temps (sujet : o=1 ≤1s, o=2 ≤2s, o=3 ≤4s)"
  for o in 1 2 3; do
    LIMIT_MS=$((o == 1 ? 1000 : o == 2 ? 2000 : 4000))
    START=$(date +%s%3N)
    "$EXE" -o "$o" -s 1 -l 100 "$LES_MIS" >/dev/null 2>/dev/null
    ELAPSED=$(($(date +%s%3N) - START))
    TOTAL=$((TOTAL + 1))
    if [ $ELAPSED -le $LIMIT_MS ]; then
      PASS=$((PASS + 1))
      [ $CI_MODE -eq 0 ] && echo -e "  ${GREEN}✓${NC} ordre $o : ${ELAPSED}ms ≤ ${LIMIT_MS}ms"
    else
      FAIL=$((FAIL + 1))
      echo -e "  ${RED}✗${NC} ordre $o : ${ELAPSED}ms > ${LIMIT_MS}ms (trop lent)"
    fi
  done
else
  echo -e "  ${YELLOW}[SKIP]${NC} lesmiserables.txt non trouvé"
  SKIP=$((SKIP + 15))
fi

# ════════════════════════════════════════════════════════════════════════════
print_header "27. TABLES VÉRIFIÉES MANUELLEMENT (petits fichiers)"
# ════════════════════════════════════════════════════════════════════════════
# Pour ces tests on vérifie la sortie exacte de -t sur des fichiers
# dont on connaît la table théorique.

check_table_exact() {
  local name="$1" expected="$2"; shift 2; TOTAL=$((TOTAL + 1))
  local out
  out=$("$EXE" "$@" 2>/dev/null)
  if [ "$out" = "$expected" ]; then
    PASS=$((PASS + 1)); [ $CI_MODE -eq 0 ] && echo -e "  ${GREEN}✓${NC} $name"
  else
    FAIL=$((FAIL + 1)); echo -e "  ${RED}✗${NC} $name"
    echo -e "    ${YELLOW}attendu${NC}:"
    echo "$expected" | sed 's/^/      /' | cat -A | head -10
    echo -e "    ${YELLOW}obtenu${NC}:"
    echo "$out"       | sed 's/^/      /' | cat -A | head -10
  fi
}

# "a b c" ordre 1 :
#   ("")   -> a
#   ("a")  -> b
#   ("b")  -> c
#   ("c")  -> ""
ABC_FILE="$TMPDIR_TEST/abc.txt"
echo "a b c" > "$ABC_FILE"
compare_outputs "table abc ordre 1"  -t -o 1 "$ABC_FILE"
compare_outputs "table abc ordre 2"  -t -o 2 "$ABC_FILE"
compare_outputs "table abc ordre 3"  -t -o 3 "$ABC_FILE"
compare_outputs "texte abc s=1 l=10" -s 1 -l 10 "$ABC_FILE"
compare_outputs "mots  abc"          -w "$ABC_FILE"

# "a a a" : clé ("a") -> ["a","a",""]
AAA_FILE="$TMPDIR_TEST/aaa.txt"
echo "a a a" > "$AAA_FILE"
compare_outputs "table aaa ordre 1"  -t -o 1 "$AAA_FILE"
compare_outputs "texte aaa s=1 l=5"  -s 1 -l 5 "$AAA_FILE"
compare_outputs "texte aaa s=2 l=5"  -s 2 -l 5 "$AAA_FILE"
compare_outputs "mots  aaa"          -w "$AAA_FILE"

# "a b a b" : clé ("a")->["b","b"], clé ("b")->["a",""]
ABAB_FILE="$TMPDIR_TEST/abab.txt"
echo "a b a b" > "$ABAB_FILE"
compare_outputs "table abab ordre 1"  -t -o 1 "$ABAB_FILE"
compare_outputs "table abab ordre 2"  -t -o 2 "$ABAB_FILE"
compare_outputs "texte abab s=1 l=6"  -s 1 -l 6 "$ABAB_FILE"
compare_outputs "mots  abab"          -w "$ABAB_FILE"

# Fichier deux mots identiques "hello hello"
HH_FILE="$TMPDIR_TEST/hh.txt"
echo "hello hello" > "$HH_FILE"
compare_outputs "table hh ordre 1"   -t -o 1 "$HH_FILE"
compare_outputs "texte hh s=1 l=5"   -s 1 -l 5 "$HH_FILE"

# Fichier un seul mot répété 5 fois
R5_FILE="$TMPDIR_TEST/r5.txt"
echo "x x x x x" > "$R5_FILE"
compare_outputs "table r5 ordre 1"   -t -o 1 "$R5_FILE"
compare_outputs "table r5 ordre 2"   -t -o 2 "$R5_FILE"
compare_outputs "texte r5 s=1 l=3"   -s 1 -l 3 "$R5_FILE"
compare_outputs "texte r5 s=1 l=10"  -s 1 -l 10 "$R5_FILE"

# ════════════════════════════════════════════════════════════════════════════
print_header "28. ARRÊT CORRECT SUR EPSILON (textes courts)"
# ════════════════════════════════════════════════════════════════════════════
# Vérifie que la génération s'arrête bien quand ε est tiré
# et ne dépasse pas le nombre de mots du texte source possible

check_word_count() {
  local name="$1" max_words="$2"; shift 2; TOTAL=$((TOTAL + 1))
  local count
  count=$("$EXE" "$@" 2>/dev/null | wc -w)
  if [ "$count" -le "$max_words" ]; then
    PASS=$((PASS + 1)); [ $CI_MODE -eq 0 ] && echo -e "  ${GREEN}✓${NC} $name ($count mots ≤ $max_words)"
  else
    FAIL=$((FAIL + 1)); echo -e "  ${RED}✗${NC} $name — $count mots produits, max attendu $max_words"
  fi
}

# "a b c" : 3 mots, limit=100 → doit produire exactement 3 mots
compare_outputs "abc s=1 l=100 == ref"  -s 1 -l 100 "$ABC_FILE"
compare_outputs "abc s=2 l=100 == ref"  -s 2 -l 100 "$ABC_FILE"
compare_outputs "abc s=7 l=100 == ref"  -s 7 -l 100 "$ABC_FILE"

# "a b c" ordre 2 : séquence unique, doit s'arrêter à 3 mots
compare_outputs "abc o=2 s=1 l=100 == ref"  -o 2 -s 1 -l 100 "$ABC_FILE"
compare_outputs "abc o=3 s=1 l=100 == ref"  -o 3 -s 1 -l 100 "$ABC_FILE"

# two.txt "hello world" : 2 mots, limit=100
compare_outputs "two s=1 l=100 == ref"  -s 1 -l 100 "$TWO_FILE"
compare_outputs "two o=2 s=1 l=100"     -o 2 -s 1 -l 100 "$TWO_FILE"

# simple.txt "a b a a b a b" : peut boucler mais doit s'arrêter quand ε tiré
compare_outputs "simple s=1 l=100"   -s 1   -l 100 "$SIMPLE_FILE"
compare_outputs "simple s=42 l=100"  -s 42  -l 100 "$SIMPLE_FILE"
compare_outputs "simple s=7 l=100"   -s 7   -l 100 "$SIMPLE_FILE"
compare_outputs "simple s=99 l=100"  -s 99  -l 100 "$SIMPLE_FILE"
compare_outputs "simple o=2 s=1 l=100"  -o 2 -s 1  -l 100 "$SIMPLE_FILE"
compare_outputs "simple o=3 s=1 l=100"  -o 3 -s 1  -l 100 "$SIMPLE_FILE"

# ════════════════════════════════════════════════════════════════════════════
print_header "29. GRAINES — exhaustif sur petits fichiers"
# ════════════════════════════════════════════════════════════════════════════
for seed in $(seq 0 20); do
  compare_outputs "simple s=$seed l=50"    -s "$seed" -l 50  "$SIMPLE_FILE"
  compare_outputs "abc    s=$seed l=50"    -s "$seed" -l 50  "$ABC_FILE"
done
for seed in 42 99 100 255 1000 9999 65535; do
  compare_outputs "knock  s=$seed l=50"    -s "$seed" -l 50  "$TXTFILE"
  compare_outputs "simple s=$seed l=50"    -s "$seed" -l 50  "$SIMPLE_FILE"
done

# ════════════════════════════════════════════════════════════════════════════
print_header "30. ORDRES ÉLEVÉS SUR PETITS FICHIERS"
# ════════════════════════════════════════════════════════════════════════════
# Quand ordre >= nb mots, la table a peu d'entrées et la génération
# doit quand même fonctionner correctement
for o in 1 2 3 4 5 6 7; do
  compare_outputs "abc    o=$o texte"  -o "$o" -s 1 -l 20 "$ABC_FILE"
  compare_outputs "abc    o=$o table"  -t -o "$o"         "$ABC_FILE"
  compare_outputs "simple o=$o texte"  -o "$o" -s 1 -l 20 "$SIMPLE_FILE"
  compare_outputs "simple o=$o table"  -t -o "$o"         "$SIMPLE_FILE"
done
for o in 1 2 3; do
  compare_outputs "one    o=$o texte"  -o "$o" -s 1 -l 5  "$ONE_FILE"
  compare_outputs "one    o=$o table"  -t -o "$o"          "$ONE_FILE"
  compare_outputs "two    o=$o texte"  -o "$o" -s 1 -l 5  "$TWO_FILE"
  compare_outputs "two    o=$o table"  -t -o "$o"          "$TWO_FILE"
done

# ════════════════════════════════════════════════════════════════════════════
print_header "31. OPTION -W — vérifications détaillées"
# ════════════════════════════════════════════════════════════════════════════
# -W doit être ignoré pour -t et -w
compare_outputs "-t -W 0  identique à -t"   -t -W 0   -o 2 "$TXTFILE"
compare_outputs "-t -W 40 identique à -t"   -t -W 40  -o 2 "$TXTFILE"
compare_outputs "-t -W 80 identique à -t"   -t -W 80  -o 2 "$TXTFILE"
compare_outputs "-w -W 0  identique à -w"   -w -W 0        "$TXTFILE"
compare_outputs "-w -W 80 identique à -w"   -w -W 80       "$TXTFILE"

# Combinaison width + size

# ════════════════════════════════════════════════════════════════════════════
print_header "32. OPTION -S — troncature et mots identiques"
# ════════════════════════════════════════════════════════════════════════════
# Avec -S 1, tous les mots sont tronqués à 1 caractère
compare_outputs "S=1 texte o=1"   -S 1 -s 1 -l 20 -o 1 "$TXTFILE"
compare_outputs "S=1 table o=1"   -S 1 -t -o 1         "$TXTFILE"
compare_outputs "S=1 mots"        -S 1 -w               "$TXTFILE"
compare_outputs "S=2 texte o=1"   -S 2 -s 1 -l 20 -o 1 "$TXTFILE"
compare_outputs "S=2 table o=1"   -S 2 -t -o 1         "$TXTFILE"
compare_outputs "S=3 texte o=2"   -S 3 -s 1 -l 20 -o 2 "$TXTFILE"

# Sur fichier simple : -S 1 tronque "bonjour" à "b"
compare_outputs "S=1 one texte"   -S 1 -s 1 -l 5 "$ONE_FILE"
compare_outputs "S=1 one table"   -S 1 -t         "$ONE_FILE"

# ════════════════════════════════════════════════════════════════════════════
print_header "33. STDIN — cas supplémentaires"
# ════════════════════════════════════════════════════════════════════════════
compare_stdin "stdin abc texte"        "$ABC_FILE"   -s 1 -l 10
compare_stdin "stdin abc table o=1"    "$ABC_FILE"   -t -o 1
compare_stdin "stdin abc table o=2"    "$ABC_FILE"   -t -o 2
compare_stdin "stdin abc mots"         "$ABC_FILE"   -w
compare_stdin "stdin aaa texte"        "$AAA_FILE"   -s 1 -l 5
compare_stdin "stdin aaa table"        "$AAA_FILE"   -t
compare_stdin "stdin abab texte"       "$ABAB_FILE"  -s 1 -l 10
compare_stdin "stdin abab table"       "$ABAB_FILE"  -t
compare_stdin "stdin one texte"        "$ONE_FILE"   -s 1 -l 5
compare_stdin "stdin one table"        "$ONE_FILE"   -t
compare_stdin "stdin knock o=3 S=10"   "$TXTFILE"    -o 3 -s 1 -l 30 -S 10

# ════════════════════════════════════════════════════════════════════════════
print_header "34. COHÉRENCE -t == -r table, -w == -r words"
# ════════════════════════════════════════════════════════════════════════════
# Vérifie que -t et -r table produisent exactement la même chose
coherence_check() {
  local name="$1"; shift; TOTAL=$((TOTAL + 1))
  local out1 out2
  # args1 = jusqu'à SEP, args2 = après SEP
  local args1=() args2=() sep=0
  for a in "$@"; do
    [ "$a" = "SEP" ] && { sep=1; continue; }
    [ $sep -eq 0 ] && args1+=("$a") || args2+=("$a")
  done
  out1=$("$EXE" "${args1[@]}" 2>/dev/null)
  out2=$("$EXE" "${args2[@]}" 2>/dev/null)
  if [ "$out1" = "$out2" ]; then
    PASS=$((PASS + 1)); [ $CI_MODE -eq 0 ] && echo -e "  ${GREEN}✓${NC} $name"
  else
    FAIL=$((FAIL + 1)); echo -e "  ${RED}✗${NC} $name"
    diff <(echo "$out1") <(echo "$out2") | head -10 | sed 's/^/      /'
  fi
}

coherence_check "-t == -r table o=1"   -t -o 1 "$TXTFILE"  SEP  -r table -o 1 "$TXTFILE"
coherence_check "-t == -r table o=2"   -t -o 2 "$TXTFILE"  SEP  -r table -o 2 "$TXTFILE"
coherence_check "-t == -r table o=3"   -t -o 3 "$TXTFILE"  SEP  -r table -o 3 "$TXTFILE"
coherence_check "-t == --reveal=table" -t -o 2 "$TXTFILE"  SEP  --reveal=table -o 2 "$TXTFILE"
coherence_check "-w == -r words"       -w "$TXTFILE"        SEP  -r words "$TXTFILE"
coherence_check "-w == --reveal=words" -w "$TXTFILE"        SEP  --reveal=words "$TXTFILE"
coherence_check "-t simple == -r table simple" -t "$SIMPLE_FILE" SEP -r table "$SIMPLE_FILE"
coherence_check "-w simple == -r words simple" -w "$SIMPLE_FILE" SEP -r words "$SIMPLE_FILE"

# ════════════════════════════════════════════════════════════════════════════
print_header "35. DETERMINISME — même entrée, même sortie"
# ════════════════════════════════════════════════════════════════════════════
repro_check() {
  local name="$1"; shift; TOTAL=$((TOTAL + 1))
  local t1="$TMPDIR_TEST/d1_$$" t2="$TMPDIR_TEST/d2_$$"
  "$EXE" "$@" >"$t1" 2>/dev/null
  "$EXE" "$@" >"$t2" 2>/dev/null
  if diff -q "$t1" "$t2" >/dev/null 2>&1; then
    PASS=$((PASS + 1)); [ $CI_MODE -eq 0 ] && echo -e "  ${GREEN}✓${NC} $name"
  else
    FAIL=$((FAIL + 1)); echo -e "  ${RED}✗${NC} $name — deux appels identiques donnent des résultats différents"
  fi
  rm -f "$t1" "$t2"
}

repro_check "déterminisme s=1  o=1 knock"  -s 1   -o 1 -l 100 "$TXTFILE"
repro_check "déterminisme s=42 o=2 knock"  -s 42  -o 2 -l 100 "$TXTFILE"
repro_check "déterminisme s=99 o=3 knock"  -s 99  -o 3 -l 100 "$TXTFILE"
repro_check "déterminisme s=1  o=1 simple" -s 1   -o 1 -l 100 "$SIMPLE_FILE"
repro_check "déterminisme s=42 o=2 simple" -s 42  -o 2 -l 100 "$SIMPLE_FILE"
repro_check "déterminisme s=1  table"      -t -o 2             "$TXTFILE"
repro_check "déterminisme s=1  mots"       -w                  "$TXTFILE"

# ════════════════════════════════════════════════════════════════════════════
print_header "36. MOTS — affichage un par ligne, pas de doublons"
# ════════════════════════════════════════════════════════════════════════════
print_section "Pas de doublons dans -w"
TOTAL=$((TOTAL + 1))
MOTS=$("$EXE" -w "$SIMPLE_FILE" 2>/dev/null)
UNIQ=$(echo "$MOTS" | sort -u | wc -l)
TOTAL_W=$(echo "$MOTS" | wc -l)
if [ "$UNIQ" -eq "$TOTAL_W" ]; then
  PASS=$((PASS + 1)); [ $CI_MODE -eq 0 ] && echo -e "  ${GREEN}✓${NC} pas de doublons dans -w (simple)"
else
  FAIL=$((FAIL + 1)); echo -e "  ${RED}✗${NC} doublons détectés dans -w (simple) : $TOTAL_W mots, $UNIQ uniques"
fi

TOTAL=$((TOTAL + 1))
MOTS=$("$EXE" -w "$TXTFILE" 2>/dev/null)
UNIQ=$(echo "$MOTS" | sort -u | wc -l)
TOTAL_W=$(echo "$MOTS" | wc -l)
if [ "$UNIQ" -eq "$TOTAL_W" ]; then
  PASS=$((PASS + 1)); [ $CI_MODE -eq 0 ] && echo -e "  ${GREEN}✓${NC} pas de doublons dans -w (knock)"
else
  FAIL=$((FAIL + 1)); echo -e "  ${RED}✗${NC} doublons dans -w (knock) : $TOTAL_W mots, $UNIQ uniques"
fi

print_section "Un mot par ligne dans -w"
TOTAL=$((TOTAL + 1))
MOTS=$("$EXE" -w "$SIMPLE_FILE" 2>/dev/null)
# Vérifie qu'aucune ligne ne contient d'espace
if echo "$MOTS" | grep -q " "; then
  FAIL=$((FAIL + 1)); echo -e "  ${RED}✗${NC} espace trouvé dans une ligne de -w"
else
  PASS=$((PASS + 1)); [ $CI_MODE -eq 0 ] && echo -e "  ${GREEN}✓${NC} un mot par ligne dans -w"
fi

# Comparaison -w avec ref sur plusieurs fichiers
compare_outputs "-w abc"   -w "$ABC_FILE"
compare_outputs "-w aaa"   -w "$AAA_FILE"
compare_outputs "-w abab"  -w "$ABAB_FILE"
compare_outputs "-w r5"    -w "$R5_FILE"
compare_outputs "-w S=1"   -w -S 1 "$TXTFILE"
compare_outputs "-w S=3"   -w -S 3 "$TXTFILE"

# ════════════════════════════════════════════════════════════════════════════
print_header "37. COMBINAISONS EXTRÊMES"
# ════════════════════════════════════════════════════════════════════════════
compare_outputs "o=1 s=0 l=1 W=1 S=1"    -o 1 -s 0 -l 1   -W 1  -S 1  "$TXTFILE"
compare_outputs "o=2 s=0 l=0"             -o 2 -s 0 -l 0              "$TXTFILE"
compare_outputs "o=1 s=max l=1"           -o 1 -s 65535 -l 1           "$TXTFILE"
compare_outputs "o=20 s=1 l=1"            -o 20 -s 1 -l 1              "$TXTFILE"
compare_outputs "o=20 s=1 l=0"            -o 20 -s 1 -l 0              "$TXTFILE"
compare_outputs "o=1 W=1 S=63 l=100"      -o 1 -W 1 -S 63 -s 1 -l 100 "$TXTFILE"
compare_outputs "o=10 t S=10"             -t -o 10 -S 10               "$TXTFILE"
compare_outputs "o=10 w S=5"              -w -o 10 -S 5                "$TXTFILE"
compare_outputs "o=1 s=1 l=1 simple"      -o 1 -s 1 -l 1               "$SIMPLE_FILE"
compare_outputs "o=2 s=1 l=1 simple"      -o 2 -s 1 -l 1               "$SIMPLE_FILE"
compare_outputs "o=1 s=0 l=100 abc"       -o 1 -s 0 -l 100             "$ABC_FILE"
compare_outputs "o=2 s=0 l=100 abc"       -o 2 -s 0 -l 100             "$ABC_FILE"

# ════════════════════════════════════════════════════════════════════════════
# RÉSUMÉ
# ════════════════════════════════════════════════════════════════════════════
echo ""
echo -e "${BOLD}${BLUE}════════════════════════════════════════════════════════${NC}"
echo -e "${BOLD}  RÉSUMÉ${NC}"
echo -e "${BOLD}${BLUE}════════════════════════════════════════════════════════${NC}"
echo -e "  Total   : ${BOLD}$TOTAL${NC}"
echo -e "  ${GREEN}Réussis${NC} : ${BOLD}${GREEN}$PASS${NC}"
echo -e "  ${RED}Échoués${NC} : ${BOLD}${RED}$FAIL${NC}"
[ $SKIP -gt 0 ] && echo -e "  ${YELLOW}Ignorés${NC} : ${BOLD}${YELLOW}$SKIP${NC}"
echo ""
if [ $FAIL -eq 0 ]; then
  echo -e "${BOLD}${GREEN}  ✓ Tous les tests passent !${NC}"; exit 0
else
  PERCENT=$((PASS * 100 / TOTAL))
  echo -e "${BOLD}${RED}  ✗ $FAIL test(s) échoué(s) sur $TOTAL ($PERCENT% de réussite).${NC}"
  echo -e "  Relancez avec ${CYAN}-v${NC} pour les diffs complets."; exit 1
fi
