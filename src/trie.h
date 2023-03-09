/** @file
 * Interfejs drzew typu trie, przechowujących przekierowania numerów telefonu.
 *
 * @author Agnieszka Klempis
 * @copyright Uniwersytet Warszawski
 * @date 2022
 */

#ifndef PHONE_NUMBERS_TRIE_H
#define PHONE_NUMBERS_TRIE_H

#include "structures.h"
#include "phone_forward.h"

/**
 * Tworzy nową strukturę typu PhfwdPointers.
 * @param parentNode - węzeł drzewa Revers, zawierający przekierowanie prefiksu numeru.
 * @param prevInList - element poprzedzający w liście element z prefiksem numeru telefonu.
 * @return - wskaźnik na utworzoną strukturę lub NULL, jeśli alokacja pamięci się nie powiodła.
 */
PhfwdPointers *PhfwdPointersNew(PhoneForwardReverse *parentNode, Prefix *prevInList);

/**
 * Tworzy nową, pustą strukturę typu PhoneForwardPrefixes.
 * @return - wskaźnik na utworzoną strukturę lub NULL, jeśli alokacja pamięci się nie powiodła.
 */
PhoneForwardPrefixes *phfwdPrefixesNew();

/**
 * Tworzy nową, pustą strukturę typu PhoneForwardReverse.
 * @return - wskaźnik na utworzoną strukturę lub NULL, jeśli alokacja pamięci się nie powiodła.
 */
PhoneForwardReverse *phfwdReverseNew();

/**
 * Usuwa drzewo PhoneForwardReverse.
 * @param rev - wskaźnik na korzeń drzewa.
 */
void phfwdReverseDelete(PhoneForwardReverse *rev);

/**
 * Usuwa drzewo PhoneForwardPrefixes.
 * @param pref - wskaźnik na korzeń drzewa.
 */
void phfwdPrefixesDelete(PhoneForwardPrefixes *pref);

/**
 * @param c - znak będący cyfrą lub '#' lub '*'.
 * @return - odpowiadającą znakowi liczbę w numerze telefonu.
 */
int charToNum(char c);

/**
 * Dodaje przekierowanie w drzewie PhoneForwardReverse.
 * @param tree - korzeń drzewa PhoneForwardReverse.
 * @param num1 - prefiks numeru telefonu.
 * @param num2 - numer będący przekierowaniem prefiks numeru telefonu.
 * @return - element struktury PhfwdPointers, przechowujący wskaźniki na węzeł z przekierowaniem oraz
 *          element listy Prefix, poprzedzający element przechowujący prefiks numeru telefonu.
 *         - NULL, jeśli nie udało sie alokować pamięci.
 */
PhfwdPointers *addToReverse(PhoneForwardReverse *tree, char const *num1, char const *num2);

/**
 * Dodaje przekierowanie w drzewie PhoneForwardPrefixes.
 * @param tree - korzeń drzewa PhoneForwardPrefixes.
 * @param num1 - prefiks numeru telefonu.
 * @param pointers - element struktury PhfwdPointers, przechowujący wskaźniki na węzeł z drzewa PhoneForwardReverse
 *                   z przekierowaniem oraz element listy Prefix, poprzedzający element przechowujący prefiks numeru telefonu.
 * @return true - jeśli udało się dodać przekierowanie.
 *         false - jeśli nie powiodła się alokacja pamięci.
 */
bool addToPrefixes(PhoneForwardPrefixes *tree, char const *num1, PhfwdPointers *pointers);

/**
 * Usuwa poddrzewo drzewa PhoneForwardPrefixes zaczynające się od węzła @p tree.
 * Usuwa także odpowiednie przekierowania i prefiksy numerów telefonów z drzewa PhoneForwardReverse.
 * @param tree - korzeń poddrzewa.
 */
void deleteSubtree(PhoneForwardPrefixes *tree);

#endif //PHONE_NUMBERS_TRIE_H
