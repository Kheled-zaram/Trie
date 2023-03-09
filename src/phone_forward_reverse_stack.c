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

struct PhfwdReverseStack {
    PhoneForwardReverse *node; ///< Wskaźnika na węzeł struktury PhoneForwardReverse.
    struct PhfwdReverseStack *next; ///< Wskaźnik na następny element stosu.
};
typedef struct PhfwdReverseStack PhfwdReverseStack;

bool reverseEmpty(PhfwdReverseStack *s) {
    return s == NULL;
}

void reverseInit(PhfwdReverseStack **s) {
    *s = NULL;
}

bool reverseInsert(PhfwdReverseStack **s, PhoneForwardReverse *node) {
    PhfwdReverseStack *newElement = (PhfwdReverseStack *) malloc(sizeof(PhfwdReverseStack));
    if (newElement == NULL)
        return false;

    newElement->node = node;
    newElement->next = *s;
    *s = newElement;

    return true;
}

PhoneForwardReverse *reversePop(PhfwdReverseStack **s) {
    PhoneForwardReverse *result = (*s)->node;
    PhfwdReverseStack *tmp = (*s);
    *s = (*s)->next;
    free(tmp);
    return result;
}

