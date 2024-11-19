/*
 * dictionary.c
 *
 * Funtionality related to processing words from the dictionary.
 *
 * Author: Tze Kheng Goh
 */ 
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include "dictionary.h"

/* createsubstringdict() 
 * --------------------- 
 * Creates a substring dictionary that holds all the words that have
 * the starter word as a substring. 
 *
 * ptr: File pointer that has been opned and points to a dictionary file 
 * that contains words
 * starterstring: A starter word chosen to start the game 
 * words: Dictionary that will hold all the words that have the starter 
 * word as a substring. 
 * 
 * Returns: Void
 *
 **/
void createsubstringdict(FILE* ptr, char* starterstring, Dictionary* words) { 
    char buffer[INITIAL_BUFFER_SIZE]; 
    char* word = NULL; 
    while ((word = fgets(buffer, INITIAL_BUFFER_SIZE, ptr))) { 
        word = touppercase(word);
        int length = 0;
        char* substring = strstr(word, starterstring); 
        //remove extra space character and replace it with null terminator. 
        word[strlen(word) - 1] = '\0'; 
        if ((processword(word) && substring != NULL)) { 
            char* toaddword = strdup(word);   
            words->listofwords[words->sizeoflist] = toaddword;  
            words->sizeoflist += 1;  
            length = strlen(toaddword); 
        }
        if (words->sizeoflist > 0 && words->sizeoflist % INITIAL_DICT_SIZE 
                == 0) { 
            //makes sure size of list is above zero before allocating. 
            words->listofwords = realloc(words->listofwords, sizeof(char*) * 
                    (words->sizeoflist * 2));   
            //doubles the size of the array each time its reallocated
        }
        if (length > words->longestword) { 
            words->longestword = length; 
            //keeps track of longest work possible. 
        }

    } 
    fclose(ptr);
}

/* longestpossiblewords()
 * ----------------------
 * Creates a new dictionary that hold the longest words possible depending
 * on the stater string. 
 *
 * words: Its a pointer to a dictionary (struct) that holds all the words 
 * available givenn the starter substring. 
 * 
 * Returns: A pointer to a dictionary (struct) 
 * Error: if words is null, a seg fault will occur.
 **/
Dictionary* longestpossiblewords(Dictionary* words) { 
    Dictionary* longestwords = init_dictionary(); 
    char* longword = NULL; 
    for (int index = 0; index < words->sizeoflist; index++) { 
        if (strlen(words->listofwords[index]) == words->longestword) {  
            //checks if word has the same length as longest word possible
            longword = strdup(words->listofwords[index]); 
            longestwords->listofwords[longestwords->sizeoflist] = longword; 
            longestwords->sizeoflist += 1; 
        }
        if ((longestwords->sizeoflist > 0) && (longestwords->sizeoflist % 
                INITIAL_DICT_SIZE == 0)) {  
            longestwords->listofwords = realloc(longestwords->listofwords, 
                    sizeof(char*) * (longestwords->sizeoflist * 2)); 
            //Doubles the size of malloc'ed array when mem needs 
            //to be rellocated
        }
    }
    return longestwords; 
}

/* touppercase() 
 * -------------
 * This fucnction turns a lower letter string into 
 * an upper level string. 
 *
 * word: Pointer to an array of characters (string) to be changed. 
 *
 * Returns: The same pointer that was given in parameter. 
 *
 **/
char* touppercase(char* word) { 
    int length = strlen(word); 
    for (int index = 0; index < length; index++) { 
        word[index] = toupper(word[index]); 
    }
    return word; 
}

/* init_dictionary() 
 * -----------------
 * Initialises a new dictionary (struct) and the variables it has. 
 * The dictionary struct (malloc'ed) holds: 
 * possiblewords = malloc'ed array of strings (character pointers) 
 * sizeoflist = integer value that represents the size of the 
 * malloc'ed array. 
 * longestword = length of longest word that can be made, given
 * the starter string.
 *
 * Params: Void
 * 
 * Returns: Void
 *
 **/
Dictionary* init_dictionary(void) { 
    Dictionary* possiblewords = NULL; 
    possiblewords = malloc(sizeof(Dictionary) * 1); 
    possiblewords->listofwords = malloc(sizeof(char*) * INITIAL_DICT_SIZE); 
    possiblewords->sizeoflist = 0; 
    possiblewords->longestword = 0;
    return possiblewords; 
}

/* freedictionary()
 * ----------------
 * Frees a dictionary (struct) and everything that was malloced.
 * (the array of strings)
 *
 * dictpoint: A pointer to a dictionary (struct). 
 *
 * Returns: void
 * Error: Heap corruption. Undefined behaviour for freeing memory not malloc'
 * ed, calloc'ed or realloc'ed
 *
 **/ 
void freedictionary(Dictionary* dictpoint) { 
    for (int i = 0; i < dictpoint->sizeoflist; i++) { 
        free(dictpoint->listofwords[i]); 
    }
    free(dictpoint->listofwords); 
    free(dictpoint); 
}

/* processword()
 * -------------
 *
 * Function checks if given word is upper case and has no special characters. 
 *
 * checkword: pointer to array of charcters (string). 
 *
 * Returns: a boolean value. True, if the character is all upper case 
 * and has no special characters. Else false. 
 * Error: Seg Fault if checkword is NULL.
 *
 **/ 
bool processword(char* checkword) { 
    for (int check = 0; checkword[check] != '\0'; check++) { 
        char character = checkword[check]; 
        if (character < 'A' || character > 'Z') { 
            return false; 
        }
    } 
    return true; 
}


