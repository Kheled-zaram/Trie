/** @file
 * Implementacja operacji na strukturze Prefix.
 *
 * @author Agnieszka Klempis
 * @copyright Uniwersytet Warszawski
 * @date 2022
 */

#include <stdlib.h>
#include <string.h>
#include "structures.h"

Prefix *prefixNew() {
    Prefix *start = (Prefix *) malloc(sizeof(Prefix));
    if (start == NULL)
        return NULL;
    start->nodeInPrefixes = NULL;
    start->num = NULL;
    start->next = NULL;
    return start;
}

Prefix *prefixAdd(PhoneForwardReverse *node, char const prefixNum[]) {
    Prefix *new = prefixNew();
    if (new == NULL)
        return NULL;
    char *prefixNumCopy = (char *) malloc(sizeof(char) * (strlen(prefixNum) + 1));
    if (prefixNumCopy == NULL) {
        free(new);
        return NULL;
    }
    strcpy(prefixNumCopy, prefixNum);
    prefixNumCopy[strlen(prefixNum)] = '\0';
    new->num = prefixNumCopy;

    if (node->prefixes == NULL) {
        Prefix *newPrev = prefixNew();
        if (newPrev == NULL) {
            free(prefixNumCopy);
            free(new);
            return NULL;
        }
        newPrev->next = new;
        node->prefixes = newPrev;
        return node->prefixes;
    }
    Prefix *prev = node->prefixes;
    while (prev->next != NULL)
        prev = prev->next;

    new->next = prev->next;
    prev->next = new;
    return prev;
}

void PrefixDelete(Prefix *prefix) {
    while (prefix != NULL) {
        Prefix *tmp = prefix;
        prefix = prefix->next;

        if (tmp->num != NULL)
            free(tmp->num);

        free(tmp);
    }
}

void PrefixDeleteOneElement(Prefix *prev) {
    Prefix *tmp = prev->next;
    prev->next = tmp->next;
    if (tmp->next != NULL) {
        PhoneForwardPrefixes *node = tmp->next->nodeInPrefixes;
        node->pointersToReverse->prevInList = prev;
    }
    free(tmp->num);
    free(tmp);
}


void addPointerToPrefixesNode(PhoneForwardPrefixes *node, Prefix *prevInList) {
    Prefix *p = prevInList->next;
    if (p != NULL)
        p->nodeInPrefixes = node;
}