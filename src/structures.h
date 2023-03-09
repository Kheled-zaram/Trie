/** @file
 * Interfejs struktur pomocniczych do przechowywania i wykonywania operacji na przekierowaniach numerów telefonu.
 *
 * @author Agnieszka Klempis
 * @copyright Uniwersytet Warszawski
 * @date 2022
 */

#ifndef STRUCTURES_H
#define STRUCTURES_H

/**
 * Ilość wszystkich możliwych znaków, które mogą wystąpić w numerze telefonu.
 */
#define SIGNS_IN_NUMBER 12

/**
 * @struct PhoneForwardPrefixes
 * @brief PhoneForwardPrefixes jest strukturą przechowującą wskaźniki na elementy drzewa PhoneForwardReverse.
 * Drzewo PhoneForwardPrefixes jest drzewem typu trie, gdzie każda gałąź oznacza kolejną cyfrę w prefiksie numeru telefonu.
 */
struct PhoneForwardPrefixes {
    struct PhfwdPointers *pointersToReverse; ///< Struktura przechowująca informacje o lokalizacji numeru telefonu
    ///< oraz jego przekierowania w drzewie PhoneForwardReverse.
    struct PhoneForwardPrefixes *children[SIGNS_IN_NUMBER]; ///< Tablica wskaźników na dzieci węzła PhoneForwardPrefixes.
};
typedef struct PhoneForwardPrefixes PhoneForwardPrefixes;

/**
 * @struct Prefix
 * @brief Prefix jest strukturą będącą listą jednokierunkową.
 */
struct Prefix {
    char *num; ///< Prefiks numeru telefonu.
    PhoneForwardPrefixes *nodeInPrefixes; ///< Jest ustawiony na NULL, chyba, że lista przechowuje więcej niż jeden prefiks.
    ///< Wtedy każdy następny element listy zawiera także wskaźnik na odpowiadający
    ///< mu węzeł w drzewie phoneForwardPrefixes.
    struct Prefix *next; ///< Wskaźnik na kolejny element listy.
};
typedef struct Prefix Prefix;

/**
 * @struct PhoneForwardReverse
 * @brief PhoneForwardReverse jest strukturą przechowującą przekierowania numerów telefonu.
 * Drzewo PhoneForwardReverse jest drzewem trie, w którym kolejne gałęzie są oznaczone cyframi numeru będącego
 * przekierowaniem prefiksu numeru telefonu.
 */
struct PhoneForwardReverse {
    char *diversion; ///< Przekierowaniu prefiksu numeru telefonu.
    Prefix *prefixes; ///< Lista przechowująca prefiksy numerów telefonu, których diversion jest przekierowaniem.
    struct PhoneForwardReverse *children[SIGNS_IN_NUMBER]; ///< Tablica wskaźników na dzieci węzła PhoneForwardReverse.
};
typedef struct PhoneForwardReverse PhoneForwardReverse;

/**
 * @struct PhfwdPointers
 * @brief PhfwdPointers jest strukturą przechowującą wskaźniki na węzeł drzewa PhoneForwardReverse oraz element listy Prefix.
 */
struct PhfwdPointers {
    PhoneForwardReverse *node; ///< Węzeł drzewa PhoneForwardReverse, zawierający przekierowanie numeru telefonu.
    Prefix *prevInList; ///< Element w liście poprzedzający element, w którym zapisany jest prefiks numeru telefonu.
};
typedef struct PhfwdPointers PhfwdPointers;

#endif //STRUCTURES_H
