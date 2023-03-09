/** @file
 * Interfejs operacji na strukturze Prefix.
 *
 * @author Agnieszka Klempis
 * @copyright Uniwersytet Warszawski
 * @date 2022
 */

#ifndef PHONE_NUMBERS_PREFIX_H
#define PHONE_NUMBERS_PREFIX_H

#include <stdlib.h>
#include <stdbool.h>
#include "structures.h"
#include "structures.h"

/**
 * Tworzy nowy, pusty element struktury Prefix.
 * @return wskaźnik na utworzoną strukturę lub NULL, jeśli nie powiodła się alokacja pamięci.
 */
Prefix *prefixNew();

/**
 * Dodaje nowy element na końcu listy.
 * Jeśli węzeł @p node nie zawiera listy prefiksów, to tworzy nową listę. Nowa lista zawiera pierwszy pusty element
 * i dopiero następny przechowuje @p prefixNum.
 * @param node - wskaźnik na węzeł PhoneForwardReverse, który przechowuje listę prefiksów numerów telefonu.
 * @param prefixNum - prefiks numeru telefonu.
 * @return - element poprzedzający w liście Prefix ten zawierający nowo dodany numer lub
 *           NULL, jeśli nie powiodła alokacja pamięci.
 */
Prefix *prefixAdd(PhoneForwardReverse *node, char const prefixNum[]);

/**
 * Usuwa listę i przechowywane przez nią numery (nie zwalnia żadnych węzłów PhoneForwardPrefixes).
 * @param prefix - wskaźnik na listę.
 */
void PrefixDelete(Prefix *prefix);

/**
 * Usuwa jeden element z listy.
 * @param prev - wskaźnik na element poprzedzający usuwany element listy.
 */
void PrefixDeleteOneElement(Prefix *prev);

/**
 * Dodaje do elementu listy wskaźnik na odpowiadający mu węzeł drzewa PhoneForwardPrefixes.
 * @param node - węzeł drzewa PhoneForwardPrefixes.
 * @param prevInList - element listy poprzedzający ten, do którego dodany zostanie wskaźnik.
 */
void addPointerToPrefixesNode(PhoneForwardPrefixes *node, Prefix *prevInList);

#endif //PHONE_NUMBERS_PREFIX_H
