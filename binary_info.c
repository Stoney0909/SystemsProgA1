#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "elf.h"

#define ElfW(type) Elf64_##type

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
    // ElfW(Sym) symtab;

    if (in_file)
    {
        fseek(in_file, 0, SEEK_END);
        long fsize = ftell(in_file);
        fseek(in_file, 0, SEEK_SET);

        char *buffer = malloc(fsize + 1);
        fread(buffer, 1, fsize, in_file);

        memcpy(&header, buffer, sizeof(header));

        // read the header
        // fread(&header, sizeof(header), 1, in_file);

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
                char *SectNames = NULL;
                fseek(in_file, header.e_shoff + header.e_shstrndx * header.e_shentsize, SEEK_SET);
                fread(&sectHdr, 1, sizeof(sectHdr), in_file);

                // memcpy(&sectHdr, buffer+ header.e_shoff , sizeof(sectHdr)- header.e_shoff);
                
                

                



                SectNames = malloc(sectHdr.sh_size);

                // fseek(in_file, sectHdr.sh_offset, SEEK_SET);
                // fread(SectNames, 1, sectHdr.sh_size, in_file);
                memcpy(&SectNames, buffer + sectHdr.sh_offset,sectHdr.sh_size );

                

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

            else if (strcmp(dash, "--sections") == 0)
            {
                char *SectNames = NULL;
                fseek(in_file, header.e_shoff + header.e_shstrndx * header.e_shentsize, SEEK_SET);
                fread(&sectHdr, 1, sizeof(sectHdr), in_file);
                SectNames = malloc(sectHdr.sh_size);
                fseek(in_file, sectHdr.sh_offset, SEEK_SET);
                fread(SectNames, 1, sectHdr.sh_size, in_file);
                printf("There are %" PRIu16 " section headers, starting at offset 0x%" PRIx64 ":\n", header.e_shnum, header.e_shoff);
                for (int idx = 0; idx < header.e_shnum; idx++)
                {
                    char *name = "";

                    fseek(in_file, header.e_shoff + idx * sizeof(sectHdr), SEEK_SET);
                    fread(&sectHdr, 1, sizeof(sectHdr), in_file);

                    // print section name

                    name = SectNames + sectHdr.sh_name;
                    if (!sectHdr.sh_name)
                    {
                        name = "Null";
                    }

                    printf("%s", name);

                    switch (sectHdr.sh_type)
                    {
                    case SHT_NULL:
                        printf(" ");
                        break;
                    case SHT_PROGBITS:
                        printf(" PROGBITS ");
                        break;
                    case SHT_SYMTAB:
                        printf(" Symbol Table ");

                    case SHT_STRTAB:
                        printf(" SYMTAB ");
                        break;
                    case SHT_RELA:
                        printf(" RELA ");
                        break;
                    case SHT_HASH:
                        printf(" HASH ");
                        break;
                    case SHT_DYNAMIC:
                        printf(" DYNAMIC ");
                        break;

                    case SHT_NOTE:
                        printf(" NOTE ");
                        break;

                    case SHT_NOBITS:
                        printf(" NOBITS ");
                        break;

                    case SHT_REL:
                        printf(" REL ");
                        break;
                    case SHT_SHLIB:
                        printf(" SHLIB ");
                        break;
                    case SHT_DYNSYM:
                        printf(" DYNSYM ");
                        break;
                    case SHT_INIT_ARRAY:
                        printf(" INIT_ARRAY ");
                        break;
                    case SHT_FINI_ARRAY:
                        printf(" FINI_ARRAY ");
                        break;
                    case SHT_PREINIT_ARRAY:
                        printf(" PREINIT_ARRAY ");
                        break;
                    case SHT_GROUP:
                        printf(" GROUP ");
                        break;
                    case SHT_SYMTAB_SHNDX:
                        printf(" SYMTAB_SHNDX ");
                        break;
                    case SHT_NUM:
                        printf(" NUM ");
                        break;
                    case SHT_LOOS:
                        printf(" LOOS ");
                        break;
                    case SHT_GNU_ATTRIBUTES:
                        printf(" GNU_ATTRIBUTES ");
                        break;
                    case SHT_GNU_HASH:
                        printf(" GNU_HASH ");
                        break;
                    case SHT_GNU_LIBLIST:
                        printf(" GNU_LIBLIST ");
                        break;
                    case SHT_CHECKSUM:
                        printf(" CHECKSUM ");
                        break;
                    case SHT_LOSUNW:
                        printf(" LOSUNW ");
                        break;
                    case SHT_SUNW_COMDAT:
                        printf(" SUNW_COMDAT ");
                        break;
                    case SHT_SUNW_syminfo:
                        printf(" SUNW_syminfo ");
                        break;
                    case SHT_GNU_verdef:
                        printf(" GNU_verdef ");
                        break;
                    case SHT_GNU_verneed:
                        printf(" GNU_verneed ");
                        break;
                    case SHT_GNU_versym:
                        printf(" SHT_GNU_versym ");
                        break;
                    case SHT_LOPROC:
                        printf(" LOPROC ");
                        break;
                    case SHT_HIPROC:
                        printf(" HIPROC ");
                        break;
                    case SHT_LOUSER:
                        printf(" LOUSER ");
                        break;
                    case SHT_HIUSER:
                        printf(" HIUSER ");
                        break;
                    default:
                        break;
                    }

                    printf(" %" PRIx64 " ", sectHdr.sh_addr);
                    printf(" %" PRIx64 " ", sectHdr.sh_offset);
                    printf(" %" PRIx64 "\n", sectHdr.sh_size);
                }
            }
            
            else if (strcmp(dash, "--section_flags") == 0)
            {
                char *SectNames = NULL;
                fseek(in_file, header.e_shoff + header.e_shstrndx * header.e_shentsize, SEEK_SET);
                fread(&sectHdr, 1, sizeof(sectHdr), in_file);
                SectNames = malloc(sectHdr.sh_size);
                fseek(in_file, sectHdr.sh_offset, SEEK_SET);
                fread(SectNames, 1, sectHdr.sh_size, in_file);
                for (int idx = 0; idx < header.e_shnum; idx++)
                {
                    char *name = "";

                    fseek(in_file, header.e_shoff + idx * sizeof(sectHdr), SEEK_SET);
                    fread(&sectHdr, 1, sizeof(sectHdr), in_file);

                    // print section name
                    if (sectHdr.sh_name)
                        ;
                    name = SectNames + sectHdr.sh_name;

                    printf("%s ", name);

                    int flags, i;

                    flags = sectHdr.sh_flags;


                    int bin[12];
                    for (i = 0; flags > 0; i++)
                    {
                        bin[i]= flags%2;
                        flags = flags/2;                         
                    }

                    for (int j = 0; j <= i-1; j++)
                    {
                        // printf("%d-%d\n", j, bin[j]);
                        if (j == 0)
                        {
                            if (bin[j] == 1)
                            {
                                printf(" Writeable ");
                            }
                            
                        }
                         if (j == 1)
                        {
                            if (bin[j] == 1)
                            {
                                printf(" Alloc ");
                            }
                            
                        }
                         if (j == 2)
                        {
                            if (bin[j] == 1)
                            {
                                printf(" Exec ");
                            }
                            
                        }
                         if (j == 4)
                        {
                            if (bin[j] == 1)
                            {
                                printf(" Merge ");
                            }
                            
                        }
                         if (j == 5)
                        {
                            if (bin[j] == 1)
                            {
                                printf(" Strings ");
                            }
                            
                        }
                         if (j == 6)
                        {
                            if (bin[j] == 1)
                            {
                                printf(" Info_Link ");
                            }
                            
                        }
                         if (j == 7)
                        {
                            if (bin[j] == 1)
                            {
                                printf(" Link_Order ");
                            }
                            
                        }
                         if (j == 8)
                        {
                            if (bin[j] == 1)
                            {
                                printf(" OS_Nonconforming ");
                            }
                            
                        }
                         if (j == 9)
                        {
                            if (bin[j] == 1)
                            {
                                printf(" Group ");
                            }
                            
                        }
                         if (j == 10)
                        {
                            if (bin[j] == 1)
                            {
                                printf(" TLS ");
                            }
                            
                        }
                         if (j == 11)
                        {
                            if (bin[j] == 1)
                            {
                                printf(" Compressed ");
                            }
                            
                        }
                        
                    }
                    
                    printf("\n");

                    
                }
            }
            
            else if (strcmp(dash, "--symtab_names"))
            {
                


            }
            
            else if (strcmp(dash, "--dynsym_names"))
            {
            }
            
            else if (strcmp(dash, "--dynamic"))
            {
            }
            
            else if (strcmp(dash, "--program_headers"))
            {
            }
            
            else if (strcmp(dash, "--segment..rodata"))
            {
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
