//  randtext_ip.h : précisions sur l'implantation du module randtext.

//  Lorsqu'ils ne sont pas constants, les couts sont exprimés en fonction
//    de n = opt.order, de la longueur du mot affiché L et, pour
//    new_val et display_randtext, du nombre d'éléments présents.
//  Les couts en temps de new_val sont amortis sous l'hypothèse usuelle
//    d'un agrandissement du tableau interne (facteur CAP_MUL).
//
//  empty_ctrvalref      : temps constant ; espace constant.
//  array_empty          : temps en O(n) ; espace en O(n).
//  new_key              : temps en O(n) ; espace en O(n).
//  new_val              : temps amorti constant ; espace amorti constant.
//  rand_val             : temps constant ; espace constant.
//  rand_key             : temps en O(n) ; espace constant.
//  ctrvalref_is_empty   : temps constant ; espace constant.
//  display_table        : temps en O(n + p) où p est le nombre de
//                         successeurs stockés dans ctr ; espace constant.
//  display_all_word     : temps en O(L) ; espace constant.
//  display_randtext     : temps en O(L + n + W) où W = opt.width
//                         (le terme W vient du padding éventuel de la
//                         fenêtre courante par des espaces) ;
//                         espace constant.
//  open_file            : temps constant ; espace constant.
//  size, limit, order   : temps constant ; espace constant.
//  mode                 : temps constant ; espace constant.
//  getoption            : reporte au cout de get_option du module option.
//  dispose_ctr          : temps constant ; espace constant.
