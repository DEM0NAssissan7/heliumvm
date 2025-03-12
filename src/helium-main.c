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
    int show_mem = 0;
    unsigned int cycles = 1000;
    // put ':' in the starting of the
    // string so that program can
    // distinguish between '?' and ':'
    while ((opt = getopt(argc, argv, "c:vm")) != -1)
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
        case 'm':
            printf("Displaying memory after execution\n");
            show_mem = 1;
            break;
        case '?':
            printf("unknown option : %c\n", optopt);
            return 1;
            break;
        }
    }

    // optind is for the extra arguments
    // which are not parsed
    for (; optind < argc; optind++)
    {
        Ramdisk ramdisk = parse_file_ramdisk(argv[optind]);
        load_ramdisk(ramdisk.data, ramdisk.byte_size);
        vm_clock(cycles);
        if (show_mem)
        {
            vm_print_memory();
            printf("\n");
        }
        if (verbose)
        {
            printf("\n");
            vm_print_registers();
            printf("\n");
            vm_print_info();
        }
    }

    return 0;
}