#include "lib.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

char* token_map[] = {
    "nul",
    "add",
    "sub",
    "or",
    "and",
    "sl",
    "sr",
    "lt",
    "jmp",
    "cjp",
    "set",
    "mv",
    "ld",
    "str",
    "hlt"
};

int string_is_equal(char* string1, char* string2, int length)
{
    if(strlen(string1) != strlen(string2)) return 0;
    for(int i = 0; i < length; i++) {
        if(string1[i] == '\0' || string2[i] == '\0')
            return 0;
        if(string1[i] != string2[i])
            return 0;
    }
    return 1;
}

int index_of_string(char** array, int array_length, char* string, int string_length)
{
    for(int i = 0; i < array_length; i++)
    {
        if(string_is_equal(array[i], string, string_length))
            return i;
    }
    return -1;
}

char parse_operation(char* string)
{
    return (char) index_of_string(token_map, NUM_INSTRUCTIONS, string, strlen(string));
}

char* parse_opcode(char opcode)
{
    return (char*) token_map[opcode];
}

char* reverse_string(char* str, int length)
{
    char* buffer = malloc(length);
    for(int i = 0; i < length; i++) // Invert character organization
        buffer[length - 1 - i] = str[i];
    return buffer;
}

char* int_to_binary(unsigned int num)
{
    char* str = malloc(BINARY_SIZE);
    int i = 0;
    while(num){
        if(num & 1) // 1
            str[i] = '1';
        else // 0
            str[i] = '0';
        num >>= 1; // Right Shift by 1
        i++;
    }
    for(;i < BINARY_SIZE; i++) // Fill the rest of the string
        str[i] = '0';
    char* result = reverse_string(str, BINARY_SIZE);
    free(str);
    return result;
}

char* char_to_binary(unsigned char num)
{
    char* str = malloc(BINARY_SIZE);
    int i = 0;
    while(num){
        if(num & 1) // 1
            str[i] = '1';
        else // 0
            str[i] = '0';
        num >>= 1; // Right Shift by 1
        i++;
    }
    for(;i < 8; i++) // Fill the rest of the string
        str[i] = '0';
    char* result = reverse_string(str, 8);
    free(str);
    return result;
}

unsigned int binary_to_int(char* string, int length) {
    unsigned int retval = 0;
    for(int i = length - 1; i >= 0; i--) {
        int num = string[i] - '0';
        retval += num * pow(2, length - i - 1);
    }
    return retval;
}

unsigned char* split_int(unsigned int number){ // Split an integer into 4 8-bit chars
    unsigned char* retval = malloc(4);

    char* binary = int_to_binary(number); // Convert to binary
    char* split_binary[4];
    for(int i = 0; i < 4; i++) {
        split_binary[i] = malloc(8);
        for(int j = 0; j < 8; j++)
            split_binary[i][j] = binary[i * 8 + j]; // Split binary into 4 strings
    }
    free(binary);

    for(int i = 0; i < 4; i++) // Turn all binary back into decimal
        retval[i] = binary_to_int(split_binary[i], 8);
    return retval;
};

unsigned int combine_int(unsigned char parts[4]) // Combine the 4 byte-long numbers back into an integer
{
    char combined_binary[BINARY_SIZE];

    for(int i = 0; i < 4; i++)
    {
        char* binary = char_to_binary(parts[i]); // Get char 8-bit binary
        for(int k = 0; k < 8; k++) // Append binary to combined string
            combined_binary[i * 8 + k] = binary[k];
        free(binary);
    }

    return binary_to_int(combined_binary, BINARY_SIZE);
}

unsigned int parse_int(char* string)
{
    return atoi(string);
    unsigned int retval = 0;
    int length = strlen(string);
    for(int i = 0; i < length; i++) {
        char c = string[length - i - 1];
        retval += (c - '0') * pow(10, i);
    }
    return retval;
}