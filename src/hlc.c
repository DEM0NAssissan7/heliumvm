#include "helium.h"

#include <stdio.h>


void end_of_command() {

}
VMProgram hcc_parse_file(char* filename)
{
    FILE* fptr = fopen(filename, "r");
    char c;
    char *token = malloc(128);
    int token_index = 0;
    while((c=fgetc(fptr)) != EOF) {
        switch(c) {
            case ';':
                break;
            case ' ':
                token_index = 0;
                break;
            default:
                token[token_index] = c;
                token_index++;
                break;
        }
    }
}