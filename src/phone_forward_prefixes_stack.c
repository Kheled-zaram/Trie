/** @file
 * Implementacja stosu, zawierającego wskaźniki na węzły drzewa PhoneForwardPrefixes.
 *
 * @author Agnieszka Klempis
 * @copyright Uniwersytet Warszawski
 * @date 2022
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "structures.h"

struct PhfwdPrefixesStack {
    PhoneForwardPrefixes *node; ///< Węzeł struktury PhoneForwardPrefixes.
    struct PhfwdPrefixesStack *next; ///< Wskaźnik na następny element stosu.
};
typedef struct PhfwdPrefixesStack PhfwdPrefixesStack;

bool prefixesEmpty(PhfwdPrefixesStack *s) {
    return s == NULL;
}

void prefixesInit(PhfwdPrefixesStack **s) {
    *s = NULL;
}

bool prefixesInsert(PhfwdPrefixesStack **s, PhoneForwardPrefixes *node) {
    PhfwdPrefixesStack *newElement = (PhfwdPrefixesStack *) malloc(sizeof(PhfwdPrefixesStack));
    if (newElement == NULL)
        return false;

    newElement->node = node;
    newElement->next = *s;
    *s = newElement;

    return true;
}

PhoneForwardPrefixes *prefixesPop(PhfwdPrefixesStack **s) {
    PhoneForwardPrefixes *result = (*s)->node;
    PhfwdPrefixesStack *tmp = (*s);
    *s = (*s)->next;
    free(tmp);
    return result;
}