#include <stdio.h>
#include "/usr/include/elf.h"
#include "ELFHeader.h"

static void interpretValues(char blob[10000][100]){
    if (blob[1][EI_CLASS] == ELFCLASS32)
    {
        printf("Class: ELF32");
    }
    else if (blob[1][EI_CLASS] == ELFCLASS64)
    {
        printf("Class: ELF64");
    }
        
};