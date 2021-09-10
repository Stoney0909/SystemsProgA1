#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
// #include "ELFHeader.h"
#include "elf.h"
///usr/include/

// #if defined(__LP64__)
#define ElfW(type) Elf64_##type
// #else
// #define ElfW(type) Elf32_##type
// #endif

int main(int argc, char **argv)
{
    // printf("%X , %s , %s, %s \n", argc, argv[0], argv[1], argv[2]);

    char filename[30] = "./";
    strcat(filename, argv[2]);
    FILE *in_file = fopen(filename, "rb");

    // if (in_file == NULL)
    // {
    //     printf("Error! Could not open file\n");
    // }
    ElfW(Ehdr) header;
    ElfW(Shdr) sectHdr;
    if (in_file)
    {
        // read the header
        fread(&header, sizeof(header), 1, in_file);

        // check so its really an elf file
        if (memcmp(header.e_ident, ELFMAG, SELFMAG) == 0)
        {
            char dash[10];
            sprintf(dash, "%s", argv[1]);

            printf("File: %s\n", argv[2]); //output

            if (strcmp(dash, "-h") == 0)
            {
                printf("ELF Header:\n");
                printf("Magic:");
                for (int i = 0; i < 16; i++) //Print Magic Numbers
                {
                    printf("%X ", header.e_ident[i]);
                }

                switch (header.e_ident[4]) //Print class
                {
                case ELFCLASS32:
                    printf("\nClass: ELF32\n");
                    break;

                case ELFCLASS64:
                    printf("\nClass: ELF64\n");
                    break;
                default:
                    printf("\nClass: Invalid Class\n");
                    break;
                }

                switch (header.e_ident[5]) //print Data
                {
                case ELFDATA2LSB:
                    printf("Data: 2's complement, little endian\n");
                    break;
                case ELFDATA2MSB:
                    printf("Data: 2's complement, big endian\n");
                    break;
                default: //program in erro
                    break;
                }

                switch (header.e_type) //print Type of file
                {
                case ET_NONE:
                    printf("Type: No file type\n");
                    break;
                case ET_REL:
                    printf("Type: Relocatable file\n");
                    break;
                case ET_EXEC:
                    printf("Type: Executable file\n");
                    break;
                case ET_DYN:
                    printf("Type: Shared object file\n");
                    break;
                case ET_CORE:
                    printf("Type: Core file\n");
                    break;
                case ET_NUM:
                    printf("Type: Number of defined types\n");
                    break;
                case ET_LOOS:
                    printf("Type: OS-specific range start\n");
                    break;
                case ET_HIOS:
                    printf("Type: Processor-specific range start\n");
                    break;
                case ET_LOPROC:
                    printf("Type: OS-specific range end\n");
                    break;
                case ET_HIPROC:
                    printf("Type: Processor-specific range end\n");
                    break;
                default:
                    //add the error
                    break;
                }

                printf("Version: %" PRIu16 "\n", header.e_version);
                printf("Entry point address: %" PRIu64 "\n", header.e_entry);
                printf("Start of program headers: %" PRIu64 " (bytes into file)\n", header.e_phoff);
                printf("Start of section headers: %" PRIu64 " (bytes into file)\n", header.e_shoff);
                printf("Flags: %" PRIu32 "\n", header.e_flags);
                printf("Size of this header: %" PRIu16 "(bytes)\n", header.e_ehsize);
                printf("Size of program headers: %" PRIu16 "(bytes)\n", header.e_phentsize);
                printf("Number of program headers: %" PRIu16 "\n", header.e_phnum);
                printf("Size of program headers: %" PRIu16 "(bytes)\n", header.e_phentsize);
                printf("Size of section headers: %" PRIu16 "(bytes)\n", header.e_shentsize);
                printf("Number of section headers: %" PRIu16 "\n", header.e_shnum);
                printf("Section header string table index: %" PRIu16 "\n", header.e_shstrndx);
            }

            else if (strcmp(dash, "--section_names") == 0)
            {
                char* SectNames = NULL;
                fseek(in_file, header.e_shoff + header.e_shstrndx * header.e_shentsize, SEEK_SET);
                fread(&sectHdr, 1, sizeof(sectHdr), in_file);
                SectNames = malloc(sectHdr.sh_size);
                fseek(in_file, sectHdr.sh_offset, SEEK_SET);
                fread(SectNames, 1, sectHdr.sh_size, in_file);
                printf("Section names from '.shstrtab':\n");
                for (int idx = 0; idx < header.e_shnum; idx++)
                {
                    char *name = "";

                    fseek(in_file, header.e_shoff + idx * sizeof(sectHdr), SEEK_SET);
                    fread(&sectHdr, 1, sizeof(sectHdr), in_file);

                    // print section name
                    if (sectHdr.sh_name)
                        ;
                    name = SectNames + sectHdr.sh_name;

                    printf("%s\n", name);
                }
            }
            else
            {
                printf("Incorect"); //need stderr
            }
            // finally close the file
            fclose(in_file);
        }

        return 0;
    }
}
