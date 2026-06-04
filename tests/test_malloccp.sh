#!/bin/bash
# test_malloccp.sh — Test malloccp + valgrind sur randtext.
#
# Pour chaque valeur k de 1 à MAX_ALLOC :
#   - Met MALLOCCP = k dans le makefile
#   - Recompile
#   - Lance valgrind sur les 3 modes (text, table, words) avec knock.txt
#   - Vérifie qu'il n'y a pas de fuite mémoire ni d'erreur valgrind
#   - Vérifie que le programme retourne EXIT_FAILURE (et non un crash)
#
# Usage :
#   ./test_malloccp.sh [OPTIONS]
#   -m MAKEFILE   chemin vers le makefile          (défaut : ./makefile)
#   -f FILE       fichier texte source             (défaut : knock.txt)
#   -n MAX        nombre max d'allocations à tester (défaut : 1043)
#   -o ORDER      ordre pour les tests             (défaut : 1)
#   -k            continue même si valgrind râle
#   -q            mode silencieux (n'affiche que les erreurs)
#   -j JOBS       nombre de jobs make parallèles   (défaut : 1)

RED='\033[0;31m'; GREEN='\033[0;32m'; YELLOW='\033[1;33m'
BLUE='\033[0;34m'; CYAN='\033[0;36m'; BOLD='\033[1m'; NC='\033[0m'

MAKEFILE="./makefile"
TXTFILE="knock.txt"
MAX_ALLOC=1043
ORDER=1
KEEP_GOING=0
QUIET=0
JOBS=1

while getopts "m:f:n:o:kqj:" opt; do
  case $opt in
    m) MAKEFILE="$OPTARG" ;;
    f) TXTFILE="$OPTARG" ;;
    n) MAX_ALLOC="$OPTARG" ;;
    o) ORDER="$OPTARG" ;;
    k) KEEP_GOING=1 ;;
    q) QUIET=1 ;;
    j) JOBS="$OPTARG" ;;
    *) echo "Usage: $0 [-m makefile] [-f file] [-n max] [-o order] [-k] [-q] [-j jobs]"
       exit 1 ;;
  esac
done

# ── Vérifications préalables ─────────────────────────────────────────────────
if [ ! -f "$MAKEFILE" ]; then
  echo -e "${RED}[ERREUR]${NC} Makefile introuvable : $MAKEFILE"; exit 1
fi
if [ ! -f "$TXTFILE" ]; then
  echo -e "${RED}[ERREUR]${NC} Fichier texte introuvable : $TXTFILE"; exit 1
fi
if ! command -v valgrind >/dev/null 2>&1; then
  echo -e "${RED}[ERREUR]${NC} valgrind non installé"; exit 1
fi

# Vérifie que la première ligne est bien MALLOCCP = ...
FIRST_LINE=$(head -1 "$MAKEFILE")
if ! echo "$FIRST_LINE" | grep -q "MALLOCCP"; then
  echo -e "${RED}[ERREUR]${NC} La première ligne du makefile ne contient pas MALLOCCP"
  echo -e "  Ligne actuelle : $FIRST_LINE"
  exit 1
fi

# Récupère le nom de l'exécutable depuis le makefile
EXE=$(grep -E "^(randtext|EXE|TARGET)\s*[:=]" "$MAKEFILE" | head -1 | \
      sed 's/.*[:=]\s*//' | awk '{print $1}')
[ -z "$EXE" ] && EXE="randtext"
# Cherche l'exe dans le dossier courant
[ ! -f "./$EXE" ] && EXE=$(find . -maxdepth 2 -name "randtext" -type f | head -1)
echo -e "${BOLD}Exécutable détecté : ${GREEN}$EXE${NC}"

TMPDIR_MCP=$(mktemp -d /tmp/malloccp_test.XXXXXX)
trap 'restore_makefile; rm -rf "$TMPDIR_MCP"' EXIT

ORIGINAL_FIRST=$(head -1 "$MAKEFILE")

restore_makefile() {
  sed -i "1s/.*/$(echo "$ORIGINAL_FIRST" | sed 's/[\/&]/\\&/g')/" "$MAKEFILE"
}

set_malloccp() {
  local k="$1"
  sed -i "1s/.*/MALLOCCP = $k/" "$MAKEFILE"
}

recompile() {
  make -f "$MAKEFILE" -j"$JOBS" randtext >/dev/null 2>&1
  return $?
}

# ── Compteurs ────────────────────────────────────────────────────────────────
PASS=0; FAIL=0; TOTAL=0
CRASH_LIST=()
LEAK_LIST=()
NOERR_LIST=()

VALGRIND_OPTS=(
  --leak-check=full
  --show-leak-kinds=all
  --errors-for-leak-kinds=all
  --error-exitcode=42
  --quiet
)

MODES=("text" "table" "words")
MODE_ARGS=("" "-t" "-w")

echo ""
echo -e "${BOLD}${BLUE}══════════════════════════════════════════════════════${NC}"
echo -e "${BOLD}${BLUE}  TEST MALLOCCP + VALGRIND${NC}"
echo -e "${BOLD}${BLUE}══════════════════════════════════════════════════════${NC}"
echo -e "  Makefile   : ${GREEN}$MAKEFILE${NC}"
echo -e "  Fichier    : ${GREEN}$TXTFILE${NC}"
echo -e "  Allocations: ${GREEN}1 → $MAX_ALLOC${NC}"
echo -e "  Ordre      : ${GREEN}$ORDER${NC}"
echo -e "  Modes      : ${GREEN}text, table, words${NC}"
echo ""
echo -e "${YELLOW}[INFO]${NC} Compilation initiale avec MALLOCCP=0..."
set_malloccp 0
if ! recompile; then
  echo -e "${RED}[ERREUR]${NC} Compilation échouée avec MALLOCCP=0"; exit 1
fi
echo -e "${GREEN}[OK]${NC} Compilation réussie"
echo ""

# ── Boucle principale ─────────────────────────────────────────────────────────
START_TOTAL=$(date +%s)

for k in $(seq 1 "$MAX_ALLOC"); do
  # Affichage progression
  if [ $((k % 50)) -eq 0 ] || [ "$k" -eq 1 ]; then
    echo -e "${CYAN}[PROGRES]${NC} Allocation k=$k/$MAX_ALLOC — pass=$PASS fail=$FAIL"
  fi

  # Mise à jour makefile + recompilation
  set_malloccp "$k"
  if ! recompile; then
    [ $QUIET -eq 0 ] && \
      echo -e "  ${RED}✗${NC} k=$k — erreur de compilation"
    FAIL=$((FAIL + 1))
    NOERR_LIST+=("k=$k:compilation")
    [ $KEEP_GOING -eq 0 ] && break
    continue
  fi

  ok_this_k=1

  for idx in 0 1 2; do
    mode="${MODES[$idx]}"
    args="${MODE_ARGS[$idx]}"
    TOTAL=$((TOTAL + 1))

    vg_out="$TMPDIR_MCP/vg_k${k}_${mode}.txt"
    prog_out="$TMPDIR_MCP/out_k${k}_${mode}.txt"

    # Construction de la commande
    if [ -z "$args" ]; then
      CMD=("valgrind" "${VALGRIND_OPTS[@]}" \
           "--log-file=$vg_out" \
           "./$EXE" -o "$ORDER" -s 1 -l 50 "$TXTFILE")
    else
      CMD=("valgrind" "${VALGRIND_OPTS[@]}" \
           "--log-file=$vg_out" \
           "./$EXE" $args -o "$ORDER" "$TXTFILE")
    fi

    "${CMD[@]}" >"$prog_out" 2>/dev/null
    prog_code=$?
    vg_code=$?

    # Analyse valgrind
    has_leak=0
    has_error=0
    has_crash=0

    # Valgrind retourne 42 si erreurs détectées
    if [ "$prog_code" -eq 42 ]; then
      has_error=1
    fi

    # Vérification supplémentaire dans le log valgrind
    if grep -q "definitely lost\|indirectly lost\|invalid read\|invalid write\|Conditional jump\|Use of uninitialised\|LEAK SUMMARY" "$vg_out" 2>/dev/null; then
      if grep -qv "0 bytes in 0 blocks\|ERROR SUMMARY: 0 errors" "$vg_out" 2>/dev/null; then
        has_leak=1
      fi
    fi

    # Crash = signal (segfault, abort...)
    if [ "$prog_code" -gt 128 ] && [ "$prog_code" -ne 42 ]; then
      has_crash=1
    fi

    # Code de retour inattendu :
    # attendu : 0 (succès), 1 (EXIT_FAILURE), 64 (erreur option)
    # interdit : tout autre code sauf 42 (valgrind) et >128 (crash)
    has_bad_code=0
    if [ "$prog_code" -ne 0 ] && [ "$prog_code" -ne 1 ] && \
       [ "$prog_code" -ne 42 ] && [ "$prog_code" -ne 64 ] && \
       [ "$prog_code" -le 128 ]; then
      has_bad_code=1
    fi

    # Le programme doit retourner EXIT_FAILURE (1) ou EXIT_SUCCESS (0)
    # quand une allocation échoue — jamais un crash (>128)
    if [ $has_crash -eq 1 ]; then
      FAIL=$((FAIL + 1)); ok_this_k=0
      echo -e "  ${RED}✗${NC} k=$k mode=$mode — CRASH (signal $((prog_code - 128)))"
      CRASH_LIST+=("k=$k:$mode")
      grep -v "^==" "$vg_out" | head -5 | sed 's/^/    /'
      [ $KEEP_GOING -eq 0 ] && { restore_makefile; exit 1; }
    elif [ $has_bad_code -eq 1 ]; then
      FAIL=$((FAIL + 1)); ok_this_k=0
      echo -e "  ${RED}✗${NC} k=$k mode=$mode — CODE RETOUR INATTENDU : $prog_code"
      NOERR_LIST+=("k=$k:$mode:code=$prog_code")
      [ $KEEP_GOING -eq 0 ] && { restore_makefile; exit 1; }
    elif [ $has_error -eq 1 ] || [ $has_leak -eq 1 ]; then
      FAIL=$((FAIL + 1)); ok_this_k=0
      echo -e "  ${RED}✗${NC} k=$k mode=$mode — FUITE/ERREUR valgrind"
      LEAK_LIST+=("k=$k:$mode")
      # Affiche les lignes pertinentes du log valgrind
      grep -v "^$\|Memcheck\|Copyright\|Using Valgrind\|For lists of" "$vg_out" \
        | grep -v "^==[0-9]*== $" | head -15 | sed 's/^/    /'
      [ $KEEP_GOING -eq 0 ] && { restore_makefile; exit 1; }
    else
      PASS=$((PASS + 1))
      [ $QUIET -eq 0 ] && [ $((k % 100)) -eq 0 ] && \
        echo -e "  ${GREEN}✓${NC} k=$k mode=$mode — OK"
    fi
  done

  [ $ok_this_k -eq 1 ] && [ $QUIET -eq 0 ] && [ $((k % 100)) -eq 1 ] && \
    echo -e "  ${GREEN}✓${NC} k=$k — tous les modes OK"
done

# Restore makefile
restore_makefile
set_malloccp 0
recompile >/dev/null 2>&1

END_TOTAL=$(date +%s)
ELAPSED=$((END_TOTAL - START_TOTAL))

# ── Résumé ───────────────────────────────────────────────────────────────────
echo ""
echo -e "${BOLD}${BLUE}════════════════════════════════════════════════════════${NC}"
echo -e "${BOLD}  RÉSUMÉ MALLOCCP + VALGRIND${NC}"
echo -e "${BOLD}${BLUE}════════════════════════════════════════════════════════${NC}"
echo -e "  Allocations testées : ${BOLD}$MAX_ALLOC${NC} × 3 modes = ${BOLD}$TOTAL${NC} tests"
echo -e "  ${GREEN}Réussis${NC}  : ${BOLD}${GREEN}$PASS${NC}"
echo -e "  ${RED}Échoués${NC}  : ${BOLD}${RED}$FAIL${NC}"
echo -e "  Temps total : ${BOLD}${ELAPSED}s${NC}"
echo ""

if [ ${#CRASH_LIST[@]} -gt 0 ]; then
  echo -e "${RED}  CRASHES détectés :${NC}"
  for c in "${CRASH_LIST[@]}"; do echo -e "    - $c"; done
  echo ""
fi

if [ ${#LEAK_LIST[@]} -gt 0 ]; then
  echo -e "${RED}  FUITES/ERREURS valgrind détectées :${NC}"
  for l in "${LEAK_LIST[@]}"; do echo -e "    - $l"; done
  echo ""
fi

if [ $FAIL -eq 0 ]; then
  echo -e "${BOLD}${GREEN}  ✓ Toutes les allocations gérées correctement !${NC}"
  exit 0
else
  PERCENT=$((PASS * 100 / TOTAL))
  echo -e "${BOLD}${RED}  ✗ $FAIL erreur(s) sur $TOTAL tests ($PERCENT% OK).${NC}"
  exit 1
fi
