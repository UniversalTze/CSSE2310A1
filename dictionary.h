#ifndef DICTIONARY_H_

#define DICTIONARY_H_

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>


#define INITIAL_DICT_SIZE 10
#define INITIAL_BUFFER_SIZE 80

//struct for dictionary that will keep track of the list of words 
//that have the substring of the starterword. 
// Dictionary will also hold the length of the longest word
typedef struct PossibleWords { 
    int sizeoflist; 
    char** listofwords;  
    int longestword;
} Dictionary;  

//Function to convert all words of dictionary to capital words. 
char* touppercase(char* word); 
 
void createsubstringdict(FILE* ptr, char* starterstring, Dictionary* words); 
//will need to be changed. 
//creates a dictionary that will hold the list of words that include 
//the substring "staterstring" in it.

//Returns a pointer to a dictionary that will hold the longest words. 
Dictionary* longestpossiblewords(Dictionary* possibilities); 

//Initialises a dictionary struct, with all variables.
Dictionary* init_dictionary(void);

//function used to free all heap memory allocated. 
void freedictionary(Dictionary* dictpoint); 

bool processword(char* word);

#endif
