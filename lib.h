#ifndef LIB_HEADER
#define LIB_HEADER

#define OPTION_SIZE 4
#define BINARY_SIZE OPTION_SIZE * 8
#define NUM_INSTRUCTIONS 15

char* int_to_binary(unsigned int num);

unsigned int binary_to_int(char* string, int length);

unsigned char* split_int(unsigned int number);

unsigned int combine_int(unsigned char parts[4]);

unsigned int string_to_int(char* string);

char* parse_opcode(char opcode);

char parse_operation(char* string);

unsigned int parse_int(char* string);

int string_is_equal(char* string1, char* string2, int length);

#endif