#include "hmc.h"
#include "helium.h"

#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    int opt;
    int verbose = 0;
    char* output_file = "program";
    // put ':' in the starting of the
    // string so that program can
    // distinguish between '?' and ':'
    while ((opt = getopt(argc, argv, "o:v")) != -1)
    {
        switch (opt)
        {
        case 'o':
            output_file = optarg;
            break;
        case 'v':
            printf("Running hmc in verbose mode.\n");
            verbose = 1;
            break;
        case '?':
            printf("unknown option : %c\n", optopt);
            break;
        }
    }

    // optind is for the extra arguments
    // which are not parsed
    for (; optind < argc; optind++)
    {
        VMProgram* program = hmc_parse_file(argv[optind]);
        create_machine_code_file(program, output_file);
        if(verbose)
        {
            VMProgram output_program = parse_file(output_file);
            printf("Program:\n");
            print_hmc_program(&output_program);
        }
    }
    return 0;
}