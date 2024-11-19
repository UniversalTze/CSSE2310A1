//Author: Tze Kheng Goh 
//A1 file, Lets work
#include <stdio.h>
#include <stdbool.h> 
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <csse2310a1.h>
#include "dictionary.h"


bool check_starter_args(int numofargs, char** arguments);
void check_starter_word(char** params, int numargs); 
FILE* check_dictionary(char** commands, int argsnum, char* startstr); 
void print_message(int count, char* starterword); 
char* process_starter_word(int lengthofword); 
char* process_start_or_length(int numofargs, char** arguments); 
bool is_valid_guess(char* submission, char* starterstring, Dictionary* dict, 
        Dictionary* guesses); 
void print_longest_words(Dictionary* longestwords, Dictionary* dict); 
void handle_end_game(Dictionary* userAttempts); 
void free_all_mem(Dictionary* allwords, Dictionary* longwords, 
        Dictionary* userTries); 
char* read_line(FILE* stream, int count, char* start, Dictionary* all, 
        Dictionary* longest, Dictionary* userTries, bool* bptr); 
void change_eof_flag(bool* ptr); 
void process_game(Dictionary* entiredict, Dictionary* uGuess, 
        Dictionary* longestwords, char* startWord, bool* eofFlag); 

/* 
 * main function() 
 * ---------------
 * control flow of the game. 
 *
 **/
int main(int argc, char* argv[]) { 
    char* starterstring = NULL; 
    char dictcom[] = "--dictionary";  
    FILE* fpointer = NULL;  
    char defaultpath[] = "/usr/share/dict/words";    
    bool eof_flag = false; 
    bool* eof_ptr = &eof_flag;  
    if (argc > 1) {   
        if (!check_starter_args(argc, argv)) {   
            fprintf(stderr, "Usage: uqwordiply [--start starter-word | --len " 
                    "length] [--dictionary filename]\n");  
            exit(1);  
        }  
        starterstring = process_start_or_length(argc, argv); 
        for (int index = 0; index < argc; index++) { 
            if (strcmp(dictcom, argv[index]) == 0) { 
                fpointer = check_dictionary(argv, argc, starterstring); 
                if (argc == 3) { //no other present command then --dictionary
                    starterstring = process_starter_word(0); 
                }
            }
        }
        if (fpointer == NULL) { //set pointer to default path if none entered 
            fpointer = fopen(defaultpath, "r"); 
        }
    } else { 
        starterstring = process_starter_word(0);  
        fpointer = fopen(defaultpath, "r"); 
    } 
    Dictionary* allwords = init_dictionary(); 
    createsubstringdict(fpointer, starterstring, allwords);  
    Dictionary* longwords = longestpossiblewords(allwords);  
    Dictionary* userGuess = init_dictionary();  
    //will print welcome message as count is 0. 
    print_message(userGuess->sizeoflist, starterstring); 
    process_game(allwords, userGuess, longwords, starterstring, eof_ptr); 
}

/* process_game() 
 * --------------
 * 
 * This function will control the game flow of wordiply. 
 * It will process the guess from standard input and will print the required 
 * messaages, depending on the validity of the guess. 
 * It will also free all allocated memory through malloc. 
 * 
 * entiredict: Dictionary pointer that points to a dictionary struct that 
 * holds all words that can be guessed, given start word. 
 * uGuess: Dictionary pointer to a dictionary struct that holds the words
 * that user has guessed. 
 * longestwords: Dictionary pointer to a dictionary struct that holds the 
 * longest words possible given the starter string. 
 * eofFlag: A boolean pointer that points to a boolean value to detect if 
 * Control D has been pressed. 
 *
 * Returns: Void. 
 * Errors: Seg fault if most of the parameters are NULL
 **/ 
void process_game(Dictionary* entiredict, Dictionary* uGuess,
        Dictionary* longestwords, char* startWord, bool* eofFlag) {  
    int count = 0;
    count++; 
    while (count != 6) { //Loop terminates when count reaches 5.(5th guess)   
       // print_message(count, startWord); 
        print_message(count, startWord); 
        char* attemptstring = read_line(stdin, uGuess->sizeoflist, 
                startWord, entiredict, longestwords, uGuess, eofFlag); 
        attemptstring = touppercase(attemptstring); 
        if (*eofFlag == true) { //handle when stdin is closed prematurely
            if (is_valid_guess(attemptstring, startWord, entiredict,  
                    uGuess)) { 
                uGuess->listofwords[uGuess->sizeoflist] = attemptstring; 
                uGuess->sizeoflist += 1; //process final guess before exit
                count++;  
                if (count == 6) { 
                    break;
                }
                print_message(count, startWord);  
                break; 
            } else { 
                print_message(count, startWord); 
                free(attemptstring); //free memory when string is invalid. 
                break; 
            }
        } //If game is still active (Control D has not been pressed)
        if (is_valid_guess(attemptstring, startWord, entiredict, 
                uGuess)) {  
            uGuess->listofwords[uGuess->sizeoflist] = attemptstring; 
            uGuess->sizeoflist += 1; 
            count++; 
            continue; 
        } 
        free(attemptstring); //frees malloc'ed mem if guess is invalid. 
    } 
    handle_end_game(uGuess); 
    print_longest_words(longestwords, entiredict); 
    free(startWord);  
    free_all_mem(entiredict, longestwords, uGuess); 
}

/* change_eof_flag()
 * ------------------------- 
 * 
 * Function changes boolean value when Control D has been detected.
 *
 * ptr: boolean pointer that is currently holds a false boolean value. 
 *
 * Return: void
 * Error: Seg fault if ptr is null
 **/
void change_eof_flag(bool* ptr) {  
    *ptr = true; 
}

/* check_starter_args() 
 * --------------------
 *
 * Function checks user's input on the command line and makes sure it is 
 * valid (based of the spec). 
 * Valid commands: 
 * --start, --len, --dictionary
 * Will return False when: 
 * --start and --len entered together.
 * First argument is not --start, --len and --dictionary
 * num of args != 1 || 3 || 5
 * if 2 commands are entered with values and don't match commands stated 
 * above. 
 * A valid option argument is given (i.e. --start, --len or --dictionary)
 * but it is not followed by an associated value argument 
 * The value associated with the --len argument is neither 3 nor 4. 
 * Any of the option arguments are listed more than once.  
 * An unexpected argument is present. 
 *
 * numofargs: numuber of arguments entered in on command line. 
 * arguments: array of arguments entered on the command line. 
 *
 * Return: True if start arguments are valid. Else return False. 
 *
 **/
bool check_starter_args(int numofargs, char** arguments) {  
    char start[] = "--start"; 
    char len[] = "--len"; 
    char dict[] = "--dictionary"; 
    if (numofargs != 1 && numofargs != 3 && numofargs != 5) { 
        return false; //checks if the number of commands entered is correct
    }    
    if ((strcmp(start, arguments[1]) != 0) && (strcmp(len, arguments[1]) != 0) 
            && (strcmp(dict, arguments[1]) != 0)) { 
        return false; //checks if command is one of the valid commands
    }  
    if (numofargs == 5 && (strcmp(start, arguments[3]) != 0) && 
            (strcmp(len, arguments[3]) != 0) && //checks for correct commands
            (strcmp(dict, arguments[3]) != 0)) { //when 2 commands are entered 
        return false; 
    }
    if (numofargs == 5 && (((strcmp(start, arguments[1]) == 0) && 
            strcmp(len, arguments[3]) == 0) || (strcmp(len, arguments[1]) == 0 
            && strcmp(start, arguments[3]) == 0))) { 
        //checks if length and start are entered together.
        return false; 
    }
    if ((strcmp(len, arguments[1]) == 0) && numofargs == 3) { 
        int num = atoi(arguments[2]); 
        if (num != 3 && num != 4) {  
            return false; //checks for valid length when only --len specified
        }
    }
    if (numofargs == 5 && ((strcmp(len, arguments[1]) == 0) || 
            strcmp(len, arguments[3]) == 0)) {  
        int lenIndex; 
        for (int index = 0; index < numofargs; index++) {  
            if (strcmp(len, arguments[index]) == 0) { 
                lenIndex = index + 1; //find the length specified. 
                break;
            }
        }
        int num = atoi(arguments[lenIndex]); 
        if (num != 3 && num != 4) { 
            return false; //checks if correct length has been entered
        }
    }     
    if (numofargs == 5 && ((strcmp(start, arguments[1]) == 0 && 
            strcmp(start, arguments[3]) == 0) || 
            (strcmp(dict, arguments[1]) == 0 && strcmp(dict, arguments[3]) ==
            0) || (strcmp(len, arguments[1]) == 0 && strcmp(len, arguments[3])
            == 0))) {  //checks if same command has been entered twice
        return false; 
    }
    return true; 
} 

/* check_starter_word() 
 * --------------------
 * This checks if a correct starter word has been entered. 
 * A word is valid if: 
 * There are no special characters. 
 * They are of length 3 or 4
 * 
 * params: Array of arguements entered in on command line 
 * numargs: Number of commands entered in on command line
 * 
 * It will print the message "uqwordiply: invalid starter word\n" 
 * to standard error.
 * Will exit with code 2 if word is invalid. 
 * Returns: Void 
 *
 **/
void check_starter_word(char** params, int numargs) { 
    int wordindex = 0; 
    char startcom[] = "--start"; 
    for (int pindex = 0; pindex < numargs; pindex++) { 
        if (strcmp(startcom, params[pindex]) == 0) { 
            wordindex = pindex + 1; //finds index of start word specified
            break; 
        }
    }
    char* starterword = params[wordindex]; 
    int lenofsword; 
    bool status = false; 
    for (lenofsword = 0; starterword[lenofsword] != '\0'; lenofsword++) { 
        int schar = toupper(starterword[lenofsword]); 
        if (schar < 'A' || schar > 'Z') { //checks for special characters
            status = true; 
        }
    }
    if (lenofsword != 3 && lenofsword != 4) { 
        status = true; //checks one of the condition for valid start word
    } 
    if (status) {
        fprintf(stderr, "uqwordiply: invalid starter word\n");  
        exit(2); 
    }
} 

/* check_dictionary() 
 * ------------------
 * Checks if the file provided is valid or not. 
 * A file is invalid if: 
 * The file cannot be opened for reading 
 * File does not exist. 
 *
 * commands: Array of commands given by the user. 
 * argsnum: Number of inputs given by user 
 * startstr: Starter string used to start thge game
 *
 * If invalid: Will print this to standard error: 
 * "uqwordiply: dictionary file "filename" cannot be opened\n"
 * Will also exit with code 2
 *
 *
 * Returns: File pointer that points to the file to read from 
 * Error: Seg fault is commands is null. 
 **/
FILE* check_dictionary(char** commands, int argsnum, char* startstr) { 
    int dictionaryindex = 0;  
    char dictcommand[] = "--dictionary"; 
    for (int index = 0; index < argsnum; index++) { 
        if (strcmp(dictcommand, commands[index]) == 0) { 
            //find index of specified path to file
            dictionaryindex = index + 1; 
            break; 
        }
    }
    FILE* dictionary = fopen(commands[dictionaryindex], "r"); 
    if (dictionary == NULL) { 
        fprintf(stderr, "uqwordiply: dictionary file "); 
        fprintf(stderr, "\"%s\"", commands[dictionaryindex]);
        fprintf(stderr, " cannot be opened\n"); 
        if (startstr != NULL) { 
            free(startstr);
        }
        exit(3); 
    }
    //returns file pointer is program does not exit
    return dictionary;
}

/* print_message() 
 * ---------------
 * Function will print the desired message depending on where user is in 
 * the game. 
 * If count = 0, user will print starter message.
 * Else it will print: 
 * "Enter Guess N:\n" 
 * Where N represents guess number. 
 *
 * count: integer that represents where user is in the game. 
 * starterword: array of characters (string) that is used to start the game.
 * 
 * Returns: Void
 * Errors: Seg fault when starterword or count is null. 
 **/
void print_message(int count, char* starterword) { 
    switch (count) { 
        case 0: 
            fprintf(stdout, "Welcome to UQWordiply!\n"); 
            fprintf(stdout, "The starter word is: %s\n", starterword); 
            fprintf(stdout, "Enter words containing this word.\n"); 
            break; 
        default: 
            fprintf(stdout, "Enter guess %d:\n", count);
    }
}

/* process_start_word() 
 * --------------------
 * Function is used to malloc space for the starter string with strdup. 
 * 
 * lengtharg: is the lenght specified by the user on the command line. 
 * Valid inputs: 0, 3, 4
 *
 * Returns: character pointer to an array. Represents the starterestring. 
 *
 **/
char* process_starter_word(int lengtharg) {  
    const char* wordtobedup = get_wordiply_starter_word(lengtharg); 
    char* starterstring = strdup(wordtobedup); 
    return starterstring;
} 

/* process_start_or_length() 
 * -------------------------
 * Function is used to process valid inputs when given valid commands: 
 * --start, --len. 
 * If --start command is entered, it will malloc space for the starter 
 * word entered in by the user. 
 * If --len is entered, function will call process_starter_word to get
 * startword. 
 *
 * numofargs:integer that represents number of inputs by user. 
 * arguments: Array of inputs on the command line. 
 *
 * Returns: Character pointer to array that holds the characters of the 
 * starter word.
 *
 **/
char* process_start_or_length(int numofargs, char** arguments) {  
    char startcom[] = "--start";  
    char lengthcom[] = "--len"; 
    char* startword = NULL; 
    for (int count = 0; count < numofargs; count++) { 
        if (strcmp(startcom, arguments[count]) == 0) {  
            check_starter_word(arguments, numofargs); 
            //mallocs memory for starter word with strdup()
            startword = strdup(touppercase(arguments[count + 1])); 
        } 
        if (strcmp(lengthcom, arguments[count]) == 0) { 
            startword = process_starter_word(atoi(arguments[count + 1]));
        }
    }
    return startword;
} 

/* is_valid_guess() 
 * ----------------
 * This function will check if user input into standard input 
 * is a valid guess 
 * A guess is valid when: 
 * Guess has all letters (no special characters or numbers)
 * Starter word is a substring of guess 
 * Guess is not the starter word
 * Guess is found in the dictionary 
 * Guess has not been guessed before 
 *
 * submission: character pointer to an array of characters that the user 
 * has guessed from standard input 
 * starterstring: character pointer to an array of characters used to start
 * the game 
 * dict: Dictionary pointer thats points to a dictionary struct that holds all
 * possible words that can be guessed by the user. 
 * guesses: A pointer to dictionary struct that holds an array that keep 
 * track of user guesses. 
 *
 * If guess contains letters it will print: 
 * "Guesses must contain only letters - try again.\n" 
 * If guess does not contin starterword: 
 * "Guesses must contain the starter word - try again.\n" 
 * If guess not found in dictionary: 
 * "Guess not found in dictionary - try again.\n" 
 * If guess has already been guessed: 
 * "You've already guessed that word - try again.\n"
 *
 * Return: True, if guess is valid, else False. 
 * Error: Seg fault, is any parameters are Null.
 **/
bool is_valid_guess(char* submission, char* starterstring, Dictionary* dict, 
        Dictionary* guesses) {  
    if (!(processword(submission))) {   
        printf("Guesses must contain only letters - try again.\n");  
        return false; 
    } 
    char* substring = strstr(submission, starterstring);  
    if (substring == NULL) {  
        printf("Guesses must contain the starter word - try again.\n");  
        return false; 
    } 
    if (strcmp(submission, starterstring) == 0) { 
        printf("Guesses can't be the starter word - try again.\n"); 
        return false; 
    }
    int validDictSearch = 0; 
    for (int index = 0; index < dict->sizeoflist; index++) { 
        if (strcmp(submission, dict->listofwords[index]) == 0) {  
            //if word is in dictionary set variable to true boolean value
            validDictSearch = 1; 
        } 
    }
    if (!validDictSearch) { 
        printf("Guess not found in dictionary - try again.\n"); 
        return false; 
    } 

    if (guesses->sizeoflist == 0) { //nothing in guessed array
        return true; 
    } 
    for (int index = 0; index < guesses->sizeoflist; index++) { 
        if (strcmp(submission, guesses->listofwords[index]) == 0) { 
            printf("You've already guessed that word - try again.\n"); 
            return false; 
        }
    }
    return true; 
}

/* print_longest_words() 
 * ---------------------
 * This function will print the longest possible words. 
 * longestwords: Dictionary pointer that holds all the longest words possible
 * given the starter substring. 
 * dict: Is a pointer to a dictionary struct that holds the longest possible 
 * length of words that can be made given starter string.  
 *
 * Returns: Void 
 * Errors: Seg fault is parameters are NULL. 
 **/
void print_longest_words(Dictionary* longestwords, Dictionary* dict) {  
    printf("Longest word(s) possible:\n"); 
    for (int i = 0; i < longestwords->sizeoflist; i++) {  
        printf("%s (%d)\n", longestwords->listofwords[i], dict->longestword);
    }
}

/* handle_end_game() 
 * -----------------
 * This function will print all the messages required to finish the game.
 * This will be the format: 
 * Total length of words found: N1 
 * Longest word(s) found: 
 * LONGWORD (N2)  
 * Longest word(s) possible: 
 * BESTWORD (N3)
 * N1 represents the sum of all word length guessed. 
 * N2 represents the length of LONGWORD 
 * N3 represents the length of BESTWORD
 * 
 * userAttempts: A dictionary pointer that holds the array used to store
 * user valid guesses. 
 *
 * Returns: Void 
 * Error: Seg fault if userAttemps is NULL
 *
 **/
void handle_end_game(Dictionary* userAttempts) { 
    int totalLength = 0; 
    int wordLength = 0; 
    for (int iterate = 0; iterate < userAttempts->sizeoflist; iterate++) { 
        totalLength += strlen(userAttempts->listofwords[iterate]); 
        if (strlen(userAttempts->listofwords[iterate]) > wordLength) { 
            //find longest word stored in User guessed array
            wordLength = strlen(userAttempts->listofwords[iterate]); 
        }
    }
    printf("\n");
    printf("Total length of words found: %d\n", totalLength); 
    printf("Longest word(s) found:\n"); 
    for (int loop = 0; loop < userAttempts->sizeoflist; loop++) { 
        if (strlen(userAttempts->listofwords[loop]) == wordLength) {  
            char* word = userAttempts->listofwords[loop]; 
            printf("%s (%d)\n", word, wordLength);  
        }
    }

}

/* free_all_mem()
 * --------------
 * Function will free all malloc memory for all dictionary structs. 
 *
 * allwords: A pointer to a dictionary struct that holds all possible words 
 * user can enter. 
 * longwords: A pointer to a dictionary struct that holds the 
 * longest words possible given the starter string. 
 * userTries: A pointer to a dictionary struct that keep track of what the 
 * user has guessed. 
 *
 * Return: Void
 *
 **/
void free_all_mem(Dictionary* allwords, Dictionary* longwords, 
        Dictionary* userTries) { 
    freedictionary(allwords); 
    freedictionary(longwords); 
    freedictionary(userTries); 
}

/* read_line()
 * -----------
 * Fuction will read each line from standard input
 *
 * stream: File pointer that reads from a file stream. 
 * count: length of User's list of guessed strings 
 * start: Character pointer that points to character array of the starter word
 * all: Dictionary struct that points to a dictionary that holds all 
 * possible words, given starter string. 
 * longest: Dictionary pointer that points to a dictionary struct that holds
 * the longest words possible given starter word.
 * userTries: Dictionary pointer that points to a dictionary struct that holds
 * chacracter pointers to attempts made by user. 
 * bptr: A pointer to a boolean value that detects if Control D has 
 * been pressed. 
 *
 * If control D has been pressed and no valid guesses has been made 
 * (user guess list is 0), it will free all malloc'ed memory and exit 
 * with code 4. 
 * If control D has been pressed and guesses has been made, it will terminate 
 * the game and print out final message, longest words made and longest 
 * strings possible, just like the spec. 
 *
 * Returns: character pointer to a character array (string)
 * which has been malloc'ed 
 * Error: Seg fault, if any paramets is null.
 *
 **/
char* read_line(FILE* stream, int count, char* start, Dictionary* all, 
        Dictionary* longest, Dictionary* userTries, bool* bptr) { 
    int buffer = INITIAL_BUFFER_SIZE; 
    char* word = malloc(sizeof(char) * buffer); 
    int lettersRead = 0; 
    int next; 

    while (1) { 
        next = fgetc(stream); 
        if (next == EOF && lettersRead == 0 && count == 0) { 
            //exit program and free memory if no valid guesses made 
            free(word); 
            free(start); 
            free_all_mem(all, longest, userTries); 
            exit(4);
        }
        if (next == EOF && lettersRead == 0 && count != 0) { 
            //properly handle exit when more than one valid guess has been 
            //made but no guess was made before Control D was pressed
            free(word); 
            free(start); 
            handle_end_game(userTries); 
            print_longest_words(longest, all); 
            free_all_mem(all, longest, userTries); 
            exit(0); 
        }
        if (next == EOF && lettersRead != 0) { 
            word[lettersRead] = '\0'; 
            change_eof_flag(bptr); 
            break; 
        }
        if (next == '\n') {  
            word[lettersRead] = '\0'; 
            break; 
        }
        word[lettersRead++] = next; 
    } 
    return word; 
}
