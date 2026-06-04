//  help.h : partie interface du module help.
//
//  Le module help regroupe les fonctions d'affichage des messages
//    informationnels du programme randtext : aide complète, aide style man,
//    valeurs courantes des options, mode d'emploi court et information de
//    version. Ces fonctions sont appelées par le module option en réaction
//    aux options correspondantes de la ligne de commande.
//
//  Fonctionnement général :
//  - chaque fonction écrit sur la sortie standard ;
//  - aucune des fonctions n'appelle exit elle-même ; la fin d'exécution est
//      déclenchée par l'appelant via la macro DISPLAY_AND_EXIT dans option.c ;
//  - les messages sont construits par concaténation de littéraux et de
//      stringifications de macros définies dans define.h, ce qui garantit
//      la cohérence entre la syntaxe des options reconnues et celle affichée
//      dans l'aide ;
//  - aucune des fonctions n'effectue d'allocation dynamique ni ne modifie
//      d'état global.

#ifndef HELP_H
#define HELP_H

#include "define.h"

//  display_help :
//    Écrit sur la sortie standard l'aide complète du programme, comprenant
//    l'usage, une description générale et la liste exhaustive des options
//    avec, pour chacune, le format court, le format long, la plage de
//    valeurs admise et la valeur par défaut. Termine par la description
//    des codes de retour.
void display_help();

//  display_man :
//    Écrit sur la sortie standard l'aide du programme dans un style
//    proche de la commande man : sections NAME, SYNOPSIS, DESCRIPTION,
//    options détaillées une par une, EXIT STATUS, AUTHOR et COPYRIGHT.
//    Les valeurs par défaut sont substituées dynamiquement par printf.
void display_man();

//  display_options :
//    Écrit sur la sortie standard les valeurs courantes des options.
void display_options();

//  display_usage :
//    Écrit sur la sortie standard un message d'utilisation concis
//    listant toutes les options disponibles sous leurs formes courte
//    et longue, sans description.
void display_usage();

//  display_version :
//    Écrit sur la sortie standard le nom du programme, sa version,
//    la mention de licence et l'identité des auteurices.
void display_version();

#endif
