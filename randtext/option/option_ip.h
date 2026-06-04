//  option_ip.h : précisions sur l'implantation du module option.
//
//
//  Lorsqu'ils ne sont pas constants, les coûts sont exprimés en fonction
//    de N, le nombre d'arguments sur la ligne de commande (nb_arg),
//    de M, la longueur en octets du plus long argument à analyser,
//    et de K, le nombre fixe d'options reconnues par get_option (constante
//    déterminée à la compilation, indépendante de l'entrée).
//
//  get_option         : temps en O(N * M) ; espace constant.
//                       Pour chaque argument, jusqu'à K motifs sont testés,
//                       chacun par un strcmp ou strncmp en O(M). Comme K
//                       est une constante du code source, le coût réel est
//                       linéaire en N * M.
//  opt_get_*          : temps constant ; espace constant.
//
//  opt_reset_col_word : temps constant ; espace constant.
//
//  opt_set_col_word   : temps constant ; espace constant.
//
