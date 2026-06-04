// Gère les options du programme randtext. Définit le type option regroupant
// les paramètres de configuration du programme, ainsi que la variable globale
// opt initialisée avec les valeurs par défaut. Fournit la fonction d'analyse
// de la ligne de commande et d'affichage des informations du programme.
//
// Limites :
// - les valeurs numériques des options doivent respecter les bornes définies
//   par les constantes MIN et MAX associées à chaque option ;
// - le nom de fichier retenu est celui du premier argument non-option
//   rencontré sur la ligne de commande ; les suivants lèvent une erreur ;
//

#include "help.h"
#include "option_ip.h"

//  get_option : parcourt les nb_arg arguments de opt_argtab et met à jour les
//    options en conséquence. Renvoie 64 si une option est invalide ou non
//    reconnue,accompagné d'un message d'erreur sur la sortie d'erreur standard.
//    Renvoie sinon zéro.
int get_option(int nb_arg, char *opt_argtab[]);

//  opt_get_limit : renvoie la valeur courante du nombre maximal de mots
//    générés.
unsigned int opt_get_limit();

//  opt_get_order : renvoie la valeur courante de l'ordre.
unsigned int opt_get_order();

//  opt_get_seed : renvoie la valeur courante de la graine utilisée pour
//    l'initialisation du générateur de nombres pseudo-aléatoires.
unsigned int opt_get_seed();

//  opt_get_size_max : renvoie la valeur courante de la taille maximale des
//    mots pris en compte lors de l'analyse du texte source.
unsigned int opt_get_size_max();

//  opt_get_width : renvoie la valeur courante de la largeur maximale en
//    caractères d'une ligne produite en sortie.
unsigned int opt_get_width();

//  opt_get_col_word : renvoie la valeur courante de la position de la colonne
//    du prochain mot à écrire sur la ligne de sortie en cours.
size_t opt_get_col_word();

//  opt_reset_col_word : remet à zéro la position de colonne du prochain mot
//    à écrire, indiquant que l'on se trouve en début de ligne.
void opt_reset_col_word();

//  opt_set_col_word : affecte la valeur k à la position de colonne du
//    prochain mot à écrire sur la ligne de sortie en cours.
void opt_set_col_word(size_t k);

//  opt_get_mode : renvoie le mode de fonctionnement courant du programme
int opt_get_mode();

//  opt_get_filename : renvoie le nom du fichier source à analyser, ou un
//    pointeur null si aucun fichier n'a été fourni sur la ligne de commande.
const char *opt_get_filename();
