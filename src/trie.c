/** @file
 * Implementacja drzew typu trie, przechowujących przekierowania numerów telefonu.
 *
 * @author Agnieszka Klempis
 * @copyright Uniwersytet Warszawski
 * @date 2022
 */

#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "prefix.h"
#include "phone_forward_reverse_stack.h"
#include "phone_forward_prefixes_stack.h"

int charToNum(char c) {
    if (c == '*') return 10;
    if (c == '#') return 11;
    else return c - '0';
}

PhfwdPointers *PhfwdPointersNew(PhoneForwardReverse *node, Prefix *prevInList) {
    PhfwdPointers *new = (PhfwdPointers *) malloc(sizeof(PhfwdPointers));
    if (new == NULL)
        return NULL;
    new->node = node;
    new->prevInList = prevInList;
    return new;
}

PhoneForwardPrefixes *phfwdPrefixesNew() {
    PhoneForwardPrefixes *root = (PhoneForwardPrefixes *) malloc(sizeof(PhoneForwardPrefixes));

    if (root == NULL)
        return NULL;

    root->pointersToReverse = NULL;
    for (int i = 0; i < SIGNS_IN_NUMBER; i++)
        (root->children)[i] = NULL;

    return root;
}

PhoneForwardReverse *phfwdReverseNew() {
    PhoneForwardReverse *root = (PhoneForwardReverse *) malloc(sizeof(PhoneForwardReverse));

    if (root == NULL)
        return NULL;

    root->diversion = NULL;
    root->prefixes = NULL;

    for (int i = 0; i < SIGNS_IN_NUMBER; i++)
        (root->children)[i] = NULL;
    return root;
}

/**
 * Zwalnia pamięć zajmowaną przez pojedynczy węzeł drzewa PhoneForwardReverse.
 * @param node - liść drzewa PhoneForwardReverse.
 */
static void freeReverseNode(PhoneForwardReverse *node) {
    if (node == NULL)
        return;

    if (node->diversion != NULL) {
        free(node->diversion);
    }
    PrefixDelete(node->prefixes);
    free(node);
}

/**
 * Usuwa strukturę PhoneForwardReverse, nie wykorzystując przy tym dodatkowej pamięci.
 * @param rev - korzeń drzewa PhoneForwardReverse.
 */
static void phfwdReverseDeleteWithLackOfMemory(PhoneForwardReverse *rev) {
    PhoneForwardReverse *now = NULL;
    PhoneForwardReverse *parent = rev;
    int parentsIndex;
    int j = 0;

    while (j < SIGNS_IN_NUMBER) {
        if ((parent->children)[j] != NULL) {
            now = (parent->children)[j];
            parentsIndex = j;

            int i = 0;
            for (; i < SIGNS_IN_NUMBER; i++) {
                if ((now->children)[i] != NULL) {
                    parent = now;
                    now = (parent->children)[i];
                    parentsIndex = i;
                    i = 0;
                }
            }
            if (i == SIGNS_IN_NUMBER) {
                freeReverseNode(now);
                (parent->children[parentsIndex]) = NULL;
                parent = rev;
                now = (rev->children)[j];
            }
        }
        if ((rev->children)[j] == NULL)
            j++;
    }

    freeReverseNode(rev);
}

void phfwdReverseDelete(PhoneForwardReverse *rev) {
    if (rev == NULL)
        return;

    PhfwdReverseStack *stack;
    reverseInit(&stack);

    if (!reverseInsert(&stack, rev)) {
        phfwdReverseDeleteWithLackOfMemory(rev);
        return;
    }

    while (!reverseEmpty(stack)) {
        PhoneForwardReverse *node = reversePop(&stack);
        for (int i = 0; i < SIGNS_IN_NUMBER; i++) {
            if ((node->children)[i] != NULL)
                if (!reverseInsert(&stack, (node->children)[i]))
                    phfwdReverseDeleteWithLackOfMemory((node->children)[i]);
        }
        freeReverseNode(node);
    }
}

/**
 * Usuwa pojedynczy węzeł drzewa PhoneForwardPrefixes.
 * @param node - liść drzewa PhoneForwardPrefixes.
 */
static void freePrefixNode(PhoneForwardPrefixes *node) {
    if (node->pointersToReverse != NULL)
        free(node->pointersToReverse);
    free(node);
}

/**
 * Usuwa strukturę PhoneForwardPrefixes, nie wykorzystując przy tym dodatkowej pamięci.
 * @param pref - korzeń drzewa PhoneForwardPrefixes.
 */
static void phfwdPrefixesDeleteWithLackOfMemory(PhoneForwardPrefixes *pref) {
    PhoneForwardPrefixes *now = NULL;
    PhoneForwardPrefixes *parent = pref;
    int parentsIndex;
    int j = 0;

    while (j < SIGNS_IN_NUMBER) {
        if ((parent->children)[j] != NULL) {
            now = (parent->children)[j];
            parentsIndex = j;

            int i = 0;
            for (; i < SIGNS_IN_NUMBER; i++) {
                if ((now->children)[i] != NULL) {
                    parent = now;
                    now = (parent->children)[i];
                    parentsIndex = i;
                    i = -1;
                }
            }
            if (i == SIGNS_IN_NUMBER) {
                freePrefixNode(now);
                (parent->children[parentsIndex]) = NULL;
                parent = pref;
                now = (pref->children)[j];
            }
        }
        if ((pref->children)[j] == NULL)
            j++;
    }

    freePrefixNode(pref);
}

void phfwdPrefixesDelete(PhoneForwardPrefixes *pref) {
    if (pref == NULL)
        return;

    PhfwdPrefixesStack *stack;
    prefixesInit(&stack);
    if (!prefixesInsert(&stack, pref)) {
        phfwdPrefixesDeleteWithLackOfMemory(pref);
        return;
    }

    while (!prefixesEmpty(stack)) {
        PhoneForwardPrefixes *node = prefixesPop(&stack);
        for (int i = 0; i < SIGNS_IN_NUMBER; i++) {
            if ((node->children)[i] != NULL)
                if (!prefixesInsert(&stack, (node->children)[i]))
                    phfwdPrefixesDeleteWithLackOfMemory((node->children)[i]);
        }

        freePrefixNode(node);
    }
}

/**
 * Usuwa pojedyncze przekierowanie numeru.
 * @param pointers - wskaźnik na strukturę przechowującą wskaźniki węzeł drzewa PhoneForwardReverse
 *                   i element w liście poprzedzający element zawierający prefiks numeru telefonu.
 */
static void deleteDiversion(PhfwdPointers *pointers) {
    PrefixDeleteOneElement(pointers->prevInList);

    if (pointers->node->prefixes->next == NULL) {
        Prefix *tmp = pointers->node->prefixes;
        pointers->node->prefixes = NULL;
        free(tmp);

        char *tmp2 = pointers->node->diversion;
        pointers->node->diversion = NULL;
        free(tmp2);
    }

}

/**
 * Szuka węzła, przechowującego przekierowanie num. Zmienia wartość wskazywaną przez @p idxInNum tak, żeby
 * oznaczała indeks w @p num odpowiadający ostatnio odwiedzonej gałęzi w drzewie trie.
 * @param tree - wskaźnik na korzeń drzewa PhoneForwardReverse.
 * @param num - przekierowanie numeru telefonu.
 * @param idxInNum - wskaźnik, który w wyniku działania funkcji zostanie ustawiony na indeks
 *                   w @p num odpowiadający ostatnio odwiedzonej gałęzi w drzewie.
 * @return - węzeł, przechowujący przekierowanie lub ostatni odwiedzony węzeł, jeśli szukanego elementu nie ma drzewie.
 */
static PhoneForwardReverse *findNodeInReverse(PhoneForwardReverse *tree, char const num[], size_t *idxInNum) {
    size_t diversionLength = strlen(num);
    *idxInNum = 0;

    while (*idxInNum < diversionLength && (tree->children)[charToNum(num[*idxInNum])] != NULL) {
        tree = (tree->children)[charToNum(num[*idxInNum])];
        (*idxInNum)++;
    }
    return tree;
}

/**
 * Szuka węzła, przechowującego prefiks numeru telefonu @p num. Zmienia wartość wskazywaną przez @p idxInNum tak, żeby
 * oznaczała indeks w @p num odpowiadający ostatnio odwiedzonej gałęzi w drzewie trie.
 * @param tree - wskaźnik na korzeń drzewa PhoneForwardPrefixes.
 * @param num - prefiks numeru telefonu.
 * @param idxInNum - wskaźnik, który w wyniku działania funkcji zostanie ustawiony na indeks
 *                   w @p num odpowiadający ostatnio odwiedzonej gałęzi w drzewie.
 * @return - węzeł, przechowujący wskaźniki na prefiks numeru telefonu lub ostatni odwiedzony węzeł, jeśli szukanego elementu nie ma drzewie.
 */
static PhoneForwardPrefixes *findNodeInPrefixes(PhoneForwardPrefixes *tree, char const num[], size_t *idxInNum) {
    size_t diversionLength = strlen(num);
    *idxInNum = 0;

    while (*idxInNum < diversionLength && (tree->children)[charToNum(num[*idxInNum])] != NULL) {
        tree = (tree->children)[charToNum(num[*idxInNum])];
        (*idxInNum)++;
    }
    return tree;
}

/**
 * Dodaje przekierowanie do węzła @p node w drzewie PhoneForwardReverse.
 * @param node - węzeł, który będzie przechowywał przekierowanie numeru.
 * @param num1 - prefiks numeru telefonu.
 * @param num2 - numer będący przekierowaniem prefiks numeru telefonu.
 * @return - element struktury PhfwdPointers, przechowujący wskaźniki na @p node oraz
 *          element listy Prefix, poprzedzający element przechowujący prefiks numeru telefonu.
 *         - NULL, jeśli nie udało sie alokować pamięci.
 */
static PhfwdPointers *addDiversion(PhoneForwardReverse *node, char const *num1, char const *num2) {
    char *diversionCopy = NULL;
    size_t diversionLength = strlen(num2);

    if (node->diversion == NULL) {
        diversionCopy = (char *) malloc(sizeof(char) * (diversionLength + 1));

        if (diversionCopy == NULL)
            return NULL;

        strcpy(diversionCopy, num2);

        node->diversion = diversionCopy;
    }

    Prefix *prev = prefixAdd(node, num1);

    if (prev == NULL) {
        if (diversionCopy != NULL) {
            free(diversionCopy);
            node->diversion = NULL;
        }
        return NULL;
    }

    PhfwdPointers *pointers = PhfwdPointersNew(node, prev);

    if (pointers == NULL) {
        PrefixDeleteOneElement(prev);
        if (diversionCopy != NULL) {
            free(diversionCopy);
            node->diversion = NULL;
        }
        return NULL;
    }

    return pointers;
}

PhfwdPointers *addToReverse(PhoneForwardReverse *tree, char const *num1, char const *num2) {

    size_t diversionLength = strlen(num2);
    size_t idx = 0;

    tree = findNodeInReverse(tree, num2, &idx);

    size_t safetyIdx = idx;
    PhoneForwardReverse *safetyNode = tree;

    while (idx < diversionLength) {
        PhoneForwardReverse *newNode = phfwdReverseNew();

        if (newNode == NULL) {
            phfwdReverseDelete((safetyNode->children)[charToNum(num2[safetyIdx])]);
            (safetyNode->children)[charToNum(num2[safetyIdx])] = NULL;
            return NULL;
        }

        (tree->children)[charToNum(num2[idx])] = newNode;
        tree = newNode;
        idx++;
    }

    PhfwdPointers *pointers = addDiversion(tree, num1, num2);

    if (pointers == NULL) {
        if (safetyIdx < diversionLength) {
            phfwdReverseDelete((safetyNode->children)[charToNum(num2[safetyIdx])]);
            (safetyNode->children)[charToNum(num2[safetyIdx])] = NULL;
        }
        return NULL;
    }

    return pointers;
}

bool addToPrefixes(PhoneForwardPrefixes *tree, char const *num1, PhfwdPointers *pointers) {
    size_t prefixLength = strlen(num1);
    size_t idx = 0;

    tree = findNodeInPrefixes(tree, num1, &idx);

    size_t safetyIdx = idx;
    PhoneForwardPrefixes *safetyNode = tree;

    while (idx < prefixLength) {
        PhoneForwardPrefixes *newNode = phfwdPrefixesNew();

        if (newNode == NULL) {
            phfwdPrefixesDelete((safetyNode->children)[charToNum(num1[safetyIdx])]);
            (safetyNode->children)[charToNum(num1[safetyIdx])] = NULL;
            return false;
        }

        (tree->children)[charToNum(num1[idx])] = newNode;
        tree = newNode;
        idx++;
    }

    if (pointers->prevInList->num != NULL)
        addPointerToPrefixesNode(tree, pointers->prevInList);

    if (safetyIdx == prefixLength && safetyNode->pointersToReverse != NULL) {
        deleteDiversion(safetyNode->pointersToReverse);
        PhfwdPointers *tmp = safetyNode->pointersToReverse;
        safetyNode->pointersToReverse = NULL;
        free(tmp);
    }

    tree->pointersToReverse = pointers;

    return true;
}

void deleteSubtree(PhoneForwardPrefixes *tree) {
    PhfwdPrefixesStack *stack;
    prefixesInit(&stack);
    prefixesInsert(&stack, tree);

    while (!prefixesEmpty(stack)) {
        PhoneForwardPrefixes *node = prefixesPop(&stack);
        for (int i = 0; i <= 11; i++) {
            if ((node->children)[i] != NULL)
                prefixesInsert(&stack, (node->children)[i]);
        }
        if (node->pointersToReverse != NULL) {
            deleteDiversion(node->pointersToReverse);
            free(node->pointersToReverse);
        }
        free(node);
    }
}