#include "helium.h"
#include "lib.h"
#include "hmc.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int opt;
    int verbose = 0;
    unsigned int cycles = 1;
    // put ':' in the starting of the
    // string so that program can
    // distinguish between '?' and ':'
    while ((opt = getopt(argc, argv, "c:v")) != -1)
    {
        switch (opt)
        {
        case 'c':
            printf("Running with %s cycles.\n", optarg);
            cycles = parse_int(optarg);
            break;
        case 'v':
            printf("Running helium in verbose mode.\n");
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
        hmc_load_file(argv[optind]);
        if(verbose)
        {
            vm_clock(cycles);
            print_hmc_program(argv[optind]);
            printf("\n");
            vm_print_registers();
            printf("\n");
            vm_print_info();
        }
    }

    return 0;
}