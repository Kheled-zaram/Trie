/** @file
 * Interfejs stosu, zawierającego wskaźniki na elementy struktury PhoneForwardPrefixes.
 *
 * @author Agnieszka Klempis
 * @copyright Uniwersytet Warszawski
 * @date 2022
 */

#ifndef PHONE_FORWARD_PREFIXES_STACK_H
#define PHONE_FORWARD_PREFIXES_STACK_H

#include <stdbool.h>

/**
 * @struct PhfwdPrefixesStack
 * @brief PhfwdPrefixesStack jest strukturą, przechowującą stos wskaźników na elementy struktury PhoneForwardPrefixes.
 */
struct PhfwdPrefixesStack;
typedef struct PhfwdPrefixesStack PhfwdPrefixesStack;

/**
 * Sprawdza, czy stos s jest pusty.
 * @param s - wskaźnik na stos.
 * @return - true, jeśli stos jest pusty,
 *           false w przeciwnym wypadku.
 */
bool prefixesEmpty(PhfwdPrefixesStack *s);

/**
 * Inicjalizuje stos na NULL.
 * @param s - wskaźnik na wskaźnik na stos.
 */
void prefixesInit(PhfwdPrefixesStack **s);

/**
 * Wstawia nowy element do stosu.
 * @param s - wskaźnik na wskaźnik na stos.
 * @param node - wskaźnik na element, który jest wstawiany do stosu.
 * @return - false, jeśli nie powiodła się alokacja pamięci,
 *           true, w pozostałych przypadkach.
 */
bool prefixesInsert(PhfwdPrefixesStack **s, PhoneForwardPrefixes *node);

/**
 * Zwraca pierwszy element zdjęty ze stosu.
 * @param *s - wskaźnik na wskaźnik na stos.
 * @return - wskaźnik na element struktury PhoneForwardPrefixes zdjęty ze stosu.
 */
PhoneForwardPrefixes *prefixesPop(PhfwdPrefixesStack **s);

#endif //PHONE_FORWARD_PREFIXES_STACK_H
