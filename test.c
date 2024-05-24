#include "helium.h"
#include "lib.h"
#include "hmc.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define TEST_NUM 10425
#define TEST_PATH "test.hmc"

void barrier()
{
    printf("\n");
}

int main(int argc, char** argv){
    hmc_load_file(TEST_PATH);
    // vm_clock(1400);
    print_hmc_program(TEST_PATH);
    printf("\n");
    vm_print_registers();
    printf("\n");
    vm_print_info();
    return 0;
}