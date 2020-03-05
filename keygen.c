/*filename: keygen.c
 *
 * SYNOPSIS:
 *
 * DESCRIPTION:
 *
 * AUTHOR: Gerson Lindor Jr. (lindorg@oregonstate.edu)
 * DATE CREATED: March 3, 2020
 * DATE LAST MODIFIED: March 5, 2020 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

const int MIN = 64;
const int MAX = 90;
const int SPACE = 32;
const int MODULUS = 27;


int verifyInput(char *input, int size);
char *generateKey(int size);


int main(int argc, char *argv[]) {
    char *key = NULL;
    int flag, keySize;

    srand(time(NULL));
    // verify input
    flag = verifyInput(argv[1], argc);
    if (flag == 1) {
        // change input to int
        keySize = atoi(argv[1]) + 1;
        // generate random key
        key = generateKey(keySize);
        // print key
        printf("%s\n", key);
    } else if (flag == -1) {
        fprintf(stderr, "Error: Check that argument 1 is a number\n");
    } else {
        fprintf(stderr, "Error: Check that argument count is greater than 1 and less than 5\n");
    }
    if (key) { free(key); key = NULL; }
    return 0;
}


/* Verifies that the input is a number and the command line argument is between 2 and 4
 * @return: -1 - the first argument after the command 'keygen' is not a digit
 *          0 - The input does not meet argument size limits
 *          1 - The input is correct.
 */
int verifyInput(char *input, int size) {
    int min = 2, max = 4, flag = 0, strSize;

    if (!input) { return flag; }
    strSize = strlen(input);
    // verify that size is between local min and max (exculsive)
    if ( min <= size && size <= max) {
        flag = 1;
    }
    // verify that input is a number
    for (int i = 0; i < strSize; ++i) {
        if (!isdigit(input[i])) {
            flag = -1;
            break;
       }
   }
    return flag;
}


/* Generates a string characters which its length is determined by the 
 * integer value stored in the parameter, size.
 * @precondition: size must be greater than 0
 * @postcondition: Returns a string of length 'size' which consist of randomly selected characters
 *                  from A - Z including a space character.
 */
char *generateKey(int size) {
    int value, range;
    char str[size];
    char *key = NULL;

    memset(str, '\0', size);
    // allocate memory for key
    key = (char *)malloc(size * sizeof(char));
    assert(key != 0);
    // randomly generate a number between 64 - 90
    range = MAX - MIN;
    // let i = 1, to make sure you leave space for '\0'
    for (int i = 1; i < size; ++i) {
        value = MIN + rand() % range;
        // 64 - 90 are ascii values from @ - Z, if value == 64, change 64 to 32 (the space value)
        if (value == 64) { value = 32; }
        // place a character in each index of key
        str[i - 1] = value;
    }
    strcpy(key, str);
    return key;
}
