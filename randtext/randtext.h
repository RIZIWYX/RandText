//  randtext.h : partie interface d'un module mutatif polymorphe pour la
//    génération de texte aléatoire à partir d'un texte donné.

#ifndef RANDTEXT_H
#define RANDTEXT_H

#include <stdlib.h>
#include <stdio.h>
#include "define.h"
#include "randtext_ip.h"

//  Fonctionnement général :
//  - le module manipule des références génériques de type « const void * » ;
//  - aucune copie des objets pointés n'est réalisée ;
//  - les allocations dynamiques concernent uniquement les structures internes
//      du module (la structure ctrvalref et les tableaux de clés) ;
//  - toute fonction recevant un pointeur invalide a un comportement
//      indéterminé ;
//  - aucune fonction n'ajoute de pointeur nul dans les structures ;
//  - les fonctions retournant « void * » renvoient nullptr en cas d'échec
//      d'allocation. En cas de succès, elles renvoient une référence
//      actuellement ou auparavant stockée par la structure.
//
//  - Le module randtext est fonctionnellement couplé au module option :
//    toute utilisation du module randtext suppose que les options soient
//    initialisée avant tout appel par get_option ou équivalent.

//  struct ctrvalref, ctrvalref : type et nom de type d'une structure
//    qui représente la liste des successeurs d'une clé.
typedef struct ctrvalref ctrvalref;

//  empty_ctrvalref :
//    Tente d'allouer et d'initialiser une structure ctrvalref vide.
//    Renvoie nullptr en cas d'échec d'allocation.
//    Renvoie sinon un pointeur vers une structure initialisée.
extern ctrvalref *empty_ctrvalref();

//  array_empty :
//    Tente d'allouer un tableau de n références, où n est l'ordre choisi
//    par l'utilisateur (par defaut 1).
//    Initialise toutes ses cases avec la chaîne vide "" (mot vide ε).
//    Renvoie nullptr en cas d'échec d'allocation.
//    Renvoie sinon le tableau alloué.
extern void *array_empty();

//  new_key :
//    Tente de créer une nouvelle clé à partir d'un tableau keyarray de taille
//    l'ordre choisie. La nouvelle clé est obtenue par décalage à gauche des
//    éléments puis ajout de ref en dernière position.
//    Renvoie nullptr en cas d'échec d'allocation.
//    Renvoie sinon la nouvelle clé allouée.
extern void *new_key(const void **keyarray, const void *ref);

//  new_val :
//    Ajoute la référence ref dans ctr.
//    Renvoie nullptr si ref est un pointeur nul ou en cas d'échec
//    d'allocation. Renvoie sinon ctr après insertion.
extern ctrvalref *new_val(ctrvalref *ctr, const void *ref);

//  rand_val :
//    Sélectionne aléatoirement une référence parmi celles contenues dans ctr
//    en utilisant la fonction standard rand avec un germe fixé par
//    l'utilisateur ou par defaut 1. Renvoie la référence sélectionnée.
extern void *rand_val(ctrvalref *ctr);

//  rand_key :
//    Met à jour en place le tableau array de taille l'ordre courant
//    en effectuant un décalage à gauche puis en ajoutant ref en
//    dernière position.
extern void rand_key(const void **array, const void *ref);

//  ctrvalref_is_empty :
//    Renvoie une valeur non nulle si ctr ne contient aucune référence,
//    zéro sinon.
extern int ctrvalref_is_empty(ctrvalref *ctr);

//  display_table :
//    Affiche sur la sortie standard la clé key puis tous les successeurs
//    contenus dans ctr, séparés par des tabulations, suivis d'un
//    retour à la ligne.
extern void display_table(const void **key, ctrvalref *ctr);

//  display_all_word :
//    Affiche sur la sortie standard la chaîne pointée par ref, suivie
//    d'un retour à la ligne.
extern void display_all_word(const void *ref);

//  display_randtext :
//    Tente de générer et d'afficher un mot à partir de val_ref. Si val_ref
//    est un pointeur nul ou ne contient aucune valeur, aucun affichage n'a
//    lieu et la fonction renvoie une valeur non nulle.
//    Sélectionne sinon une référence aléatoire dans val_ref ; si la chaîne
//    sélectionnée est vide, aucun affichage n'a lieu et la fonction renvoie
//    une valeur non nulle. Sinon, affiche le mot sur la sortie standard en
//    respectant la contrainte de largeur de l'option width.
//    Met à jour le contexte array avec le mot sélectionné et
//    incrémente la valeur pointée par count.
//    Renvoie zéro en cas de succès.
extern int display_randtext(size_t *count, ctrvalref *val_ref,
    const void **array);

//  open_file :
//    Si un nom de fichier est passé comme argument sur la ligne de commande,
//    open_file tente d'ouvrir le fichier correspondant en
//    lecture et renvoie le FILE * associé. En cas d'échec d'ouverture, émet
//    un message sur la sortie d'erreur standard et termine le programme
//    par exit(EXIT_FAILURE).
//    Sinon renvoie nullptr.
extern FILE *open_file();

//  size, limit, order, mode :
//    Accesseurs vers les champs correspondants des options.
extern size_t size();
extern size_t limit();
extern size_t order();
extern int mode();

//  getoption :
//    parcourt les nb_arg arguments de opt_argtab et met à jour les
//    options en conséquence. Renvoie 64 si une option est invalide ou non
//    reconnue,accompagné d'un message d'erreur sur la sortie d'erreur standard.
//    Zéro sinon.
extern int getoption(int nb_arg, char *opt_argtab[]);

//  dispose_ctr :
//    Libère les ressources allouées pour ctr.
extern void dispose_ctr(ctrvalref *ctr);

#endif
