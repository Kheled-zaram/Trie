/** @file
 * Implementacja klasy przechowującej przekierowania numerów telefonicznych.
 *
 * @author Agnieszka Klempis
 * @copyright Uniwersytet Warszawski
 * @date 2022
 */

#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "trie.h"

struct PhoneForward {
    PhoneForwardReverse *reverse; ///< Wskaźnik na drzewo trie przechowujące przekierowania numerów telefonów.
    ///< Gałęzie drzewa reverse oznaczają kolejne cyfry przekierowania numeru.
    PhoneForwardPrefixes *prefixes; ///< Wskaźnik na drzewo trie przechowujące wskaźniki na przekierowania numerów telefonu.
    ///< Gałęzie drzewa prefixes oznaczają kolejne cyfry prefiksu numeru telefonu.
};
typedef struct PhoneForward PhoneForward;

struct PhoneNumbers {
    char **num; ///< Tablica numerów telefonu.
    size_t elements; ///< Ilość przechowywanych numerów.
    size_t size; ///< Rozmiar tablicy num.
};
typedef struct PhoneNumbers PhoneNumbers;

char const *phnumGet(PhoneNumbers const *pnum, size_t idx) {
    if (pnum == NULL || idx >= pnum->elements)
        return NULL;

    return (pnum->num)[idx];
}

void phnumDelete(PhoneNumbers *pnum) {
    if (pnum == NULL)
        return;
    if (pnum->num != NULL) {
        for (size_t i = 0; i < pnum->elements; i++) {
            char *tmp = (pnum->num)[i];
            if (tmp != NULL)
                free(tmp);
        }
        free(pnum->num);
    }
    free(pnum);
}

/**
 * Tworzy nową strukturę, alokując pamięć do przechowywania numerów telefonu.
 * @param howManyNumbers - ile numerów telefonu może pomieścić (rozmiar struktury można potem dynamicznie powiększać).
 * @return stworzoną strukturę,
 *         NULL - jeśli alokacja pamięci się nie powiodła.
 */
static PhoneNumbers *phnumNew(size_t howManyNumbers) {
    PhoneNumbers *phnum = (PhoneNumbers *) malloc(sizeof(PhoneNumbers));
    if (phnum == NULL)
        return NULL;
    phnum->elements = 0;
    if (howManyNumbers == 0) {
        phnum->num = NULL;
        phnum->size = 0;
        return phnum;
    }
    char **nums = (char **) malloc(sizeof(char *) * howManyNumbers); //< tablica wskaźników na numery.
    if (nums == NULL) {
        free(phnum);
        return NULL;
    }
    phnum->num = nums;
    phnum->size = howManyNumbers;
    for (size_t i = 0; i < howManyNumbers; i++) {
        nums[i] = NULL;  //< inicjalizuje wszystkie elementy tablicy na NULL.
    }
    return phnum;
}

/**
 * Sprawdza, czy w tablicy zmieści się słowo długości @p newLength. Jeśli nie, to zwiększa rozmiar tablicy.
 * @param A - wskaźnik na wskaźnik na tablicę.
 * @param size - aktualny rozmiar tablicy.
 * @param newLength - długość słowa, które powinno zmieścić się w tablicy.
 * @return - false, jeśli nie powiodła się alokacja pamięci,
 *           true, w pozostałych przypadkach.
 */
static bool adjustSizeOfString(char **A, size_t *size, size_t newLength) {
    if (*size == 0) {
        // Tworzy nową tablicę.
        (*A) = (char *) malloc(sizeof(char) * (newLength + 1));
        if (*A == NULL) return false;
        (*A)[0] = '\0'; //< Umieszcza w tablicy pusty string.
        *size = newLength + 1;
    } else if (newLength + 1 > *size) {
        // Zwiększa rozmiar tablicy.
        (*size) = newLength + 1;
        char *newString = (char *) realloc(*A, sizeof(char) * (*size));

        if (newString == NULL) {
            free(*A);
            return false;
        }
    }
    return true;
}

/**
 * Umieszcza numer telefonu w strukturze.
 * @param phnum - wskaźnik na strukturę.
 * @param newNumber - numer telefonu do umieszczenia w strukturze.
 * @return true - jeśli udało się dodać numer.
 *         false - jeśli nie powiodła się alokacja pamięci.
 */
static bool phnumAddNumber(PhoneNumbers *phnum, char newNumber[]) {
    if (phnum == NULL)
        return false;
    if (phnum->elements >= phnum->size) {
        // Trzeba zwiększyć rozmiar tablicy.
        if (phnum->size == 0) {
            // Tworzy nową tablicę.
            phnum->num = (char **) malloc(sizeof(char *));
            if (phnum->num == NULL)
                return false;
            phnum->size = 1;
        } else {
            // Zwiększa rozmiar tablicy.
            char **tmp = phnum->num;
            tmp = realloc(tmp, (phnum->size) * 2 * sizeof(char *));
            if (tmp == NULL) {
                return false;
            }
            phnum->num = tmp;
            (phnum->size) *= 2;
        }
    }

    (phnum->num)[phnum->elements] = newNumber;
    (phnum->elements)++;
    return true;
}

/**
 * Sprawdza, czy napis jest numerem telefonu.
 * @param string - wskaźnik na napis.
 * @return true, jeśli napis jest numerem telefonu.
 *         false, jeśli wskaźnik na napis ma wartość NULL lub napis nie jest numerem telefonu.
 */
static bool isStringAPhoneNumber(char const *string) {
    if (string == NULL)
        return false;
    int i = 0;
    while (isdigit(string[i]) || string[i] == '*' || string[i] == '#') i++;
    if (string[i] != '\0' || i == 0)
        return false;
    return true;
}

PhoneForward *phfwdNew() {
    // 1
    PhoneForward *new = (PhoneForward *) malloc(sizeof(PhoneForward));

    if (new == NULL)
        return NULL;

    new->prefixes = phfwdPrefixesNew(); // Struktura drzewa prefiksowego po prefiksach numerów telefonu.

    if (new->prefixes == NULL) {
        free(new);
        return NULL;
    }

    new->reverse = phfwdReverseNew(); // Struktura drzewa prefiksowego po przekierowaniach numerów telefonu.

    if (new->reverse == NULL) {
        free(new->prefixes);
        free(new);
        return NULL;
    }

    return new;
}

void phfwdDelete(PhoneForward *pf) {
    if (pf == NULL)
        return;

    phfwdReverseDelete(pf->reverse);
    phfwdPrefixesDelete(pf->prefixes);
    free(pf);
}

bool phfwdAdd(PhoneForward *pf, char const *num1, char const *num2) {

    if (!isStringAPhoneNumber(num1) || !isStringAPhoneNumber(num2) || !strcmp(num1, num2))
        return false;
    if (pf == NULL)
        return false;
    if (pf->prefixes == NULL || pf->reverse == NULL)
        return false;

    PhfwdPointers *pointers = addToReverse(pf->reverse, num1, num2);

    if (pointers == NULL)
        return false;

    if (addToPrefixes(pf->prefixes, num1, pointers))
        return true;

    free(pointers);
    return false;
}

/**
 * Znajduje najdłuższy możliwy prefiks, do którego istnieje przekierowanie, przechowywane w strukturze @p pf.
 * @p *length przyjmuje wartość długości znalezionego prefiksu.
 * @param tree  – wskaźnik na strukturę przechowującą wskaźniki na przekierowania numerów;
 * @param num – wskaźnik na napis reprezentujący numer.
 * @param length - wskaźnik na zmienną, która będzie przechowywać długość odnalezionego prefiksu.
 * @return - wskaźnik na napis reprezentujący przekierowanie numeru.
 *         - NULL, jeśli w strukturze @p pf nie ma żadnego pasującego prefiksu.
 */
static const char *findOnePrefix(PhoneForwardPrefixes *tree, char const *num, size_t *length) {
    size_t prefixLength = strlen(num);
    size_t idx = 0;
    char *diversion = NULL;

    while (idx < prefixLength && (tree->children)[charToNum(num[idx])] != NULL) {
        tree = (tree->children)[charToNum(num[idx])];
        idx++;
        if (tree->pointersToReverse != NULL) {
            *length = idx;
            diversion = tree->pointersToReverse->node->diversion;
        }
    }
    if (diversion == NULL) {
        *length = 0;
    }
    return diversion;
}

PhoneNumbers *phfwdGet(PhoneForward const *pf, char const *num) {
    if (pf == NULL)
        return NULL;
    if (!isStringAPhoneNumber(num))
        return phnumNew(0);

    size_t resultSize = 0; //< rozmiar tablicy, w której będzie przechowywane przekierowanie num.
    char *result = NULL; //< wskaźnik na numer, będący przekierowaniem num.
    size_t num_length = 0;  //< długość znalezionego prefiksu, do którego istnieje przekierowanie.
    char const *tmp = findOnePrefix(pf->prefixes, num, &num_length);

    size_t tmpLength = (tmp == NULL) ? 0 : strlen(tmp);
    size_t afterPrefix =
            strlen(num) - num_length; //< indeks w num, od którego numer nie zmienia się na jego przekierowanie.
    size_t newLength = afterPrefix + tmpLength;  //< całkowita długość wynikowego przekierowania.

    if (!adjustSizeOfString(&result, &resultSize, newLength)) {
        return NULL;
    }

    if (num_length != 0) { //< Jeśli istnieje przekierowanie prefiksu num.
        strcat(result, tmp);
    }

    size_t j = num_length; //< indeks w tablicy num.
    // Kopiuje resztę num, która nie zamieniła się na przekierowanie.
    for (size_t i = tmpLength; i <= newLength; i++) {
        result[i] = num[j];
        j++;
    }

    PhoneNumbers *diversion = phnumNew(1);
    if (diversion == NULL) {
        free(result);
        return NULL;
    }

    if (!phnumAddNumber(diversion, result)) {
        phnumDelete(diversion);
        return NULL;
    }

    return diversion;
}

void phfwdRemove(PhoneForward *pf, char const *num) {
    if (pf == NULL || !isStringAPhoneNumber(num))
        return;
    PhoneForwardPrefixes *node = pf->prefixes;
    PhoneForwardPrefixes *parent = node;
    size_t prefixLength = strlen(num);
    size_t idx = 0;


    while (idx < prefixLength && (node->children)[charToNum(num[idx])] != NULL) {
        parent = node;
        node = (node->children)[charToNum(num[idx])];
        idx++;
    }

    if (idx != prefixLength)
        return;

    idx--;
    (parent->children)[charToNum(num[idx])] = NULL;
    deleteSubtree(node);
}

/**
 * Zwraca numery z jednego węzła @p node, których przekierowaniem mógłby być numer @p num.
 * @param node - węzeł drzewa PhoneForwardReverse, zawierający listę przekierowywanych prefiksów numeru telefonu.
 * @param num - numer telefonu.
 * @param result - tablica, przechowująca wynikowe numery.
 * @return - true - jeśli udało się dodać wszystkie przekierowywane numery.
 *          - false - jeśli nie powiodła się alokacja pamięci.
 */
static bool reverseOneNode(PhoneForwardReverse *node, char const *num, PhoneNumbers *result) {
    Prefix *list = node->prefixes;

    if (list == NULL)
        return true;

    list = list->next;
    size_t diversionLength = strlen(node->diversion);
    size_t rest = strlen(num) - diversionLength;

    while (list != NULL) {
        size_t prefixLength = strlen(list->num);
        char *rev = (char *) malloc(sizeof(char) * (prefixLength + rest + 1));

        if (rev == NULL)
            return false;

        strcpy(rev, list->num);
        strcat(rev, num + diversionLength);

        if (!phnumAddNumber(result, rev)) {
            free(rev);
            return false;
        }

        list = list->next;
    }

    return true;
}

/**
 * Porównuje dwa znaki, oznaczające cyfry numeru telefonu.
 * @param a - pierwszy znak.
 * @param b - drugi znak.
 * @return - dodatnią liczbę - jeśli a > b.
 *         - ujemną liczbę - jeśli a < b.
 *         - 0 - jeśli a == b.
 */
static int compareCharacters(char a, char b) {
    if (a >= '0' && b >= '0')
        return a - b;
    if (a < '0' && b >= '0')
        return 1;
    if (b < '0' && a >= '0')
        return -1;
    return b - a;
}

/**
 * Porównuje dwa numery telefonów.
 * @param a - pierwszy numer telefonu.u
 * @param b - drugi numer telefonu.
 * @return - dodatnią liczbę - jeśli numer @p a jest wyższy w porządku leksykograficznym.
 *         - ujemną liczbę - jeśli @p a jest niższy w porządku leksykograficznym.
 *         - 0 - jeśli numery są takie same.
 */
static int compare(const void *a, const void *b) {
    const char **str_a = (const char **) a;
    const char **str_b = (const char **) b;
    size_t i = 0;
    while ((*str_a)[i] != '\0' && (*str_b)[i] != '\0') {
        if ((*str_a)[i] == (*str_b)[i])
            i++;
        else return compareCharacters((*str_a)[i], (*str_b)[i]);
    }
    if ((*str_a)[i] != '\0')
        return 1;
    if ((*str_b)[i] != '\0')
        return -1;
    return 0;
}

/**
 * Usuwa element ze środka tablicy @p phnum.
 * @param phnum - wskaźnik na tablicę.
 * @param idx - indeks usuwanego elementu.
 */
static void phnumRemoveElement(PhoneNumbers *phnum, size_t idx) {
    if (phnum == NULL || idx >= phnum->elements)
        return;

    free((phnum->num)[idx]);

    for (size_t j = idx + 1; j < phnum->elements; j++) {
        (phnum->num)[j - 1] = (phnum->num)[j];
    }
    (phnum->elements)--;
}

/**
 * Usuwa powtarzające się elementy w posortowanej niemalejąca względem porządku leksykograficznego tablicy.
 * @param phnum - wskaźnik na tablicę.
 */
static void phnumRemoveRepetitions(PhoneNumbers *phnum) {
    char const *prev = phnumGet(phnum, 0);

    for (size_t i = 1; i < phnum->elements; i++) {

        if (!strcmp(prev, phnumGet(phnum, i)))
            phnumRemoveElement(phnum, i);
        else
            prev = phnumGet(phnum, i);
    }
}

/**
 * Usuwa ze struktury @p phnum numery, dla których num nie jest wynikiem wywołania funkcji phfwdGet.
 * @param pf - struktura przechowująca przekierowania prefiksów numerów telefonu.
 * @param num - numer, który powinien być wynikiem funkcji phfwdGet z numerami z @p phnum.
 * @param phnum - struktura, z której zostaną usunięte niespełniające powyższego warunku numery.
 * @return true - jeśli alokacja pamięci się powiodła, false - w przeciwnym wypadku.
 */
static bool phnumDeleteIncorrectReverseResults(PhoneForward const *pf, char const *num, PhoneNumbers *phnum) {
    if (phnum == NULL || phnum->elements == 0)
        return true;

    for (size_t i = 0; i < phnum->elements; i++) {
        PhoneNumbers *phfwdGetResult = phfwdGet(pf, phnumGet(phnum, i));

        if (phfwdGetResult == NULL)
            return false;

        if (strcmp(phnumGet(phfwdGetResult, 0), num)) {
            phnumRemoveElement(phnum, i);
            i--;
        }
        phnumDelete(phfwdGetResult);
    }
    return true;
}

PhoneNumbers *phfwdReverse(PhoneForward const *pf, char const *num) {
    if (pf == NULL)
        return NULL;
    if (!isStringAPhoneNumber(num)) {
        return phnumNew(0);
    }

    size_t prefixLength = strlen(num);
    PhoneForwardReverse *node = pf->reverse;
    size_t idx = 0;
    PhoneNumbers *result = phnumNew(0);
    if (result == NULL)
        return NULL; //1

    while (idx <= prefixLength && node != NULL) {
        if (node->diversion != NULL && !reverseOneNode(node, num, result)) {
            phnumDelete(result);
            return NULL;
        }
        if (idx < prefixLength)
            node = (node->children)[charToNum(num[idx])];
        idx++;
    }

    char *copy = (char *) malloc(sizeof(char) * (prefixLength + 1));

    if (copy == NULL) {
        phnumDelete(result);
        return NULL; //3
    }

    strcpy(copy, num);

    if (!phnumAddNumber(result, copy)) {
        phnumDelete(result);
        free(copy);
        return NULL;
    }
    qsort(result->num, result->elements, sizeof(char *), compare);
    phnumRemoveRepetitions(result);
    return result;
}

PhoneNumbers *phfwdGetReverse(PhoneForward const *pf, char const *num) {
    PhoneNumbers *pnum = phfwdReverse(pf, num);

    if (pnum == NULL)
        return NULL;

    if (pnum->elements == 0)
        return pnum;

    if (!phnumDeleteIncorrectReverseResults(pf, num, pnum)) {
        phnumDelete(pnum);
        return NULL;
    }

    return pnum;
}