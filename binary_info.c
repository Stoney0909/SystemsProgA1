#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "elf.h"
#include <sys/mman.h>

#define ElfW(type) Elf64_##type

int main(int argc, char **argv)
{

    char filename[30] = "./";
    if (argc > 3)//error handler
    {
        fprintf(stderr, "Multiple options or files not allowed\n");
        exit(0);
    }

    strcat(filename, argv[2]);
    FILE *in_file = fopen(filename, "rb");

    if (in_file == NULL)
    {
        fprintf(stderr, "File does not exist\n");
        fclose(in_file);
        exit(0);
    }

    ElfW(Ehdr) header;
    ElfW(Shdr) sectHdr;
    ElfW(Phdr) phHdr;

    if (in_file)
    {
        fseek(in_file, 0, SEEK_END);//get file and put it in buffer
        long fsize = ftell(in_file);
        fseek(in_file, 0, SEEK_SET);

        char *buffer = malloc(fsize + 1);
        fread(buffer, 1, fsize, in_file);
        fclose(in_file);

        memcpy(&header, buffer, sizeof(header));
        char elf[8];

        for (int i = 1; i < 4; i++) //error handling for if it is a elf file
        {
            elf[i] = header.e_ident[i];
        }
        if (!(elf[1] == 'E' && elf[2] == 'L' && elf[3] == 'F'))
        {
            fprintf(stderr, "Not and Elf file\n");
            exit(0);
        }

    

        memcpy(&sectHdr, buffer +  header.e_shoff + header.e_shstrndx * header.e_shentsize, sizeof(sectHdr));//get section with section names


        char *SectNames = malloc(sectHdr.sh_size);


        memcpy(SectNames, buffer + sectHdr.sh_offset, sectHdr.sh_size);//copy section names

        char *str = NULL;

        for (int idx = 0; idx < header.e_shnum; idx++) //get .strtab names
        {
            char *name = "";

            memcpy(&sectHdr, buffer + header.e_shentsize * idx + header.e_shoff, sizeof(sectHdr));

            // print section name
            if (sectHdr.sh_name)
                ;
            name = SectNames + sectHdr.sh_name;

            if (strcmp(name, ".strtab") == 0)
            {

                str = (buffer + sectHdr.sh_offset);
            }
        }

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

                printf("Section names from '.shstrtab':\n");
                for (int idx = 0; idx < header.e_shnum; idx++)//iterate through sections
                {
                    char *name = "";

                    memcpy(&sectHdr, buffer + header.e_shentsize * idx + header.e_shoff, sizeof(sectHdr));

                    // print section name
                    if (sectHdr.sh_name)
                        ;
                    name = SectNames + sectHdr.sh_name;

                    printf("%s\n", name);
                }
            }

            else if (strcmp(dash, "--sections") == 0)
            {
                printf("There are %" PRIu16 " section headers, starting at offset 0x%" PRIx64 ":\n", header.e_shnum, header.e_shoff);
                for (int idx = 0; idx < header.e_shnum; idx++)
                {
                    char *name = "";

                    memcpy(&sectHdr, buffer + header.e_shentsize * idx + header.e_shoff, sizeof(sectHdr));

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
                for (int idx = 0; idx < header.e_shnum; idx++)
                {
                    char *name = "";

                    memcpy(&sectHdr, buffer + header.e_shentsize * idx + header.e_shoff, sizeof(sectHdr));

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
                        bin[i] = flags % 2;
                        flags = flags / 2;
                    }

                    for (int j = 0; j <= i - 1; j++)
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

            else if (strcmp(dash, "--symtab_names") == 0)
            {
                ElfW(Sym) symtab;

                for (int idx = 0; idx < header.e_shnum; idx++)
                {

                    // char *sectBuffer = malloc(header.e_shentsize+1);

                    char *name = "";

                    memcpy(&sectHdr, buffer + header.e_shentsize * idx + header.e_shoff, sizeof(sectHdr));

                    // print section name
                    if (sectHdr.sh_name)
                        ;
                    name = SectNames + sectHdr.sh_name;

                    if (strcmp(name, ".symtab") == 0)
                    {

                        int symbol_num = sectHdr.sh_size / sectHdr.sh_entsize;
                        printf("Symbol table '.symtab' contains %d entries:\n", symbol_num);
                        printf("Num:     Value                 Size     Type   Bind      Vis     Ndx    Name\n");
                        for (int i = 0; i < symbol_num; i++)
                        {
                            printf("%d: ", i);
                            memcpy(&symtab, buffer + sectHdr.sh_entsize * i + sectHdr.sh_offset, sizeof(symtab));
                            printf(" %" PRIx64 " ", symtab.st_value);
                            printf(" %" PRIu64 " ", symtab.st_size);

                            switch (ELF64_ST_TYPE(symtab.st_info))
                            {
                            case 0:
                                printf(" NOTYPE ");
                                break;
                            case STT_OBJECT:
                                printf(" OBJECT ");
                                break;

                            case STT_FUNC:
                                printf(" FUNC ");
                                break;

                            case STT_SECTION:
                                printf(" SECTION ");
                                break;
                            case STT_FILE:
                                printf(" FILE ");
                                break;
                            case STT_COMMON:
                                printf(" COMMON ");
                                break;

                            case STT_TLS:
                                printf(" TLS ");
                                break;

                            case STT_NUM:
                                printf(" NUM ");
                                break;
                            case STT_LOOS:
                                printf(" LOOS ");
                                break;

                            case STT_HIOS:
                                printf(" HIOS ");
                                break;

                            case STT_LOPROC:
                                printf(" LOPROC ");
                                break;

                            case STT_HIPROC:
                                printf(" HIPROC ");
                                break;

                            default:
                                break;
                            }

                            switch (ELF64_ST_BIND(symtab.st_info))
                            {
                            case STB_LOCAL:
                                printf(" LOCAL ");
                                break;
                            case STB_GLOBAL:
                                printf(" GLOBAL");
                                break;

                            case STB_WEAK:
                                printf(" WEAK ");
                                break;
                            case STB_NUM:
                                printf(" NUM ");
                                break;
                            case STB_LOOS:
                                printf(" LOOS ");
                                break;

                            case STB_HIOS:
                                printf(" HIOS ");
                                break;
                            case STB_LOPROC:
                                printf(" LOPROC ");
                                break;
                            case STB_HIPROC:
                                printf(" STB_HIPROC ");
                                break;
                            default:
                                break;
                            }

                            switch (ELF64_ST_VISIBILITY(symtab.st_other))
                            {
                            case STV_DEFAULT:
                                printf(" DEFAULT ");
                                break;
                            case STV_INTERNAL:
                                printf(" INTERNAL ");
                                break;
                            case STV_HIDDEN:
                                printf(" HIDDEN ");
                                break;
                            case STV_PROTECTED:
                                printf(" PROTECTED ");
                                break;

                            default:
                                break;
                            }

                            if (symtab.st_shndx == 0)
                            {
                                printf(" UND ");
                            }
                            else if (symtab.st_shndx == 65521)
                            {
                                printf(" ABS ");
                            }
                            else
                            {
                                printf(" %" PRIu16 " ", symtab.st_shndx);
                            }

                            char *symbName = "";
                            symbName = str + symtab.st_name;
                            printf(" %s ", symbName);
                            printf("\n");
                        }
                    }
                }
            }

            else if (strcmp(dash, "--dynsym_names") == 0)
            {
                ElfW(Sym) dynsymtab;
                int dynsymExists = 0;

                for (int idx = 0; idx < header.e_shnum; idx++)
                {

                    // char *sectBuffer = malloc(header.e_shentsize+1);

                    char *name = "";

                    memcpy(&sectHdr, buffer + header.e_shentsize * idx + header.e_shoff, sizeof(sectHdr));

                    // print section name
                    if (sectHdr.sh_name)
                        ;
                    name = SectNames + sectHdr.sh_name;

                    if (strcmp(name, ".dynsym") == 0)
                    {
                        dynsymExists = 1;
                        int symbol_num = sectHdr.sh_size / sectHdr.sh_entsize;
                        printf("Symbol table '.dynsym' contains %d entries:\n", symbol_num);
                        printf("Num:     Value                 Size     Type   Bind      Vis     Ndx    Name\n");
                        for (int i = 0; i < symbol_num; i++)
                        {
                            printf("%d: ", i);
                            memcpy(&dynsymtab, buffer + sectHdr.sh_entsize * i + sectHdr.sh_offset, sizeof(dynsymtab));
                            printf(" %" PRIx64 " ", dynsymtab.st_value);
                            printf(" %" PRIu64 " ", dynsymtab.st_size);

                            switch (ELF64_ST_TYPE(dynsymtab.st_info))
                            {
                            case 0:
                                printf(" NOTYPE ");
                                break;
                            case STT_OBJECT:
                                printf(" OBJECT ");
                                break;

                            case STT_FUNC:
                                printf(" FUNC ");
                                break;

                            case STT_SECTION:
                                printf(" SECTION ");
                                break;
                            case STT_FILE:
                                printf(" FILE ");
                                break;
                            case STT_COMMON:
                                printf(" COMMON ");
                                break;

                            case STT_TLS:
                                printf(" TLS ");
                                break;

                            case STT_NUM:
                                printf(" NUM ");
                                break;
                            case STT_LOOS:
                                printf(" LOOS ");
                                break;

                            case STT_HIOS:
                                printf(" HIOS ");
                                break;

                            case STT_LOPROC:
                                printf(" LOPROC ");
                                break;

                            case STT_HIPROC:
                                printf(" HIPROC ");
                                break;

                            default:
                                break;
                            }

                            switch (ELF64_ST_BIND(dynsymtab.st_info))
                            {
                            case STB_LOCAL:
                                printf(" LOCAL ");
                                break;
                            case STB_GLOBAL:
                                printf(" GLOBAL");
                                break;

                            case STB_WEAK:
                                printf(" WEAK ");
                                break;
                            case STB_NUM:
                                printf(" NUM ");
                                break;
                            case STB_LOOS:
                                printf(" LOOS ");
                                break;

                            case STB_HIOS:
                                printf(" HIOS ");
                                break;
                            case STB_LOPROC:
                                printf(" LOPROC ");
                                break;
                            case STB_HIPROC:
                                printf(" STB_HIPROC ");
                                break;
                            default:
                                break;
                            }

                            switch (ELF64_ST_VISIBILITY(dynsymtab.st_other))
                            {
                            case STV_DEFAULT:
                                printf(" DEFAULT ");
                                break;
                            case STV_INTERNAL:
                                printf(" INTERNAL ");
                                break;
                            case STV_HIDDEN:
                                printf(" HIDDEN ");
                                break;
                            case STV_PROTECTED:
                                printf(" PROTECTED ");
                                break;

                            default:
                                break;
                            }

                            if (dynsymtab.st_shndx == 0)
                            {
                                printf(" UND ");
                            }
                            else if (dynsymtab.st_shndx == 65521)
                            {
                                printf(" ABS ");
                            }
                            else
                            {
                                printf(" %" PRIu16 " ", dynsymtab.st_shndx);
                            }

                            char *symbName = "";
                            symbName = str + dynsymtab.st_name;
                            printf(" %s ", symbName);
                            printf("\n");
                        }
                    }
                }
                if (dynsymExists == 0)
                {
                    printf("No .dynsym table.\n");
                }
            }

            else if (strcmp(dash, "--dynamic") == 0)
            {
                ElfW(Dyn) dyntab;
                int dynExists = 0;

                for (int idx = 0; idx < header.e_shnum; idx++)
                {

                    char *name = "";

                    memcpy(&sectHdr, buffer + header.e_shentsize * idx + header.e_shoff, sizeof(sectHdr));

                    // print section name
                    if (sectHdr.sh_name)
                        ;
                    name = SectNames + sectHdr.sh_name;

                    if (strcmp(name, ".dynamic") == 0)
                    {
                        dynExists = 1;

                        int symbol_num = sectHdr.sh_size / sectHdr.sh_entsize;
                        printf("Dynamic Section at offset %" PRIx64 " contains %d \n", sectHdr.sh_offset, symbol_num);
                        printf("Tag  Type                 Name/Value\n");
                        for (int i = 0; i < symbol_num; i++)
                        {
                            printf("%d: ", i);
                            memcpy(&dyntab, buffer + sectHdr.sh_entsize * i + sectHdr.sh_offset, sizeof(dyntab));
                            printf(" %" PRIX64 " ", dyntab.d_tag);
                            switch (dyntab.d_tag)
                            {
                            case DT_NULL:
                                printf("(Null)");
                                break;
                            case DT_NEEDED:
                                printf("(NEEDED)");
                                break;
                            case DT_PLTRELSZ:
                                printf("(PLTRELSZ)");
                                break;
                            case DT_PLTGOT:
                                printf("(PLTGOT)");
                                break;
                            case DT_HASH:
                                printf("(HASH)");
                                break;
                            case DT_STRTAB:
                                printf("(STRTAB)");
                                break;
                            case DT_SYMTAB:
                                printf("(SYMTAB)");
                                break;
                            case DT_RELA:
                                printf("(RELA)");
                                break;
                            case DT_RELASZ:
                                printf("(RELASZ)");
                                break;
                            case DT_RELAENT:
                                printf("(RELAENT)");
                                break;
                            case DT_STRSZ:
                                printf("(STRSZ)");
                                break;
                            case DT_SYMENT:
                                printf("(SYMENT)");
                                break;
                            case DT_INIT:
                                printf("(INIT)");
                                break;
                            case DT_FINI:
                                printf("(FINI)");
                                break;
                            case DT_SONAME:
                                printf("(SONAME)");
                                break;
                            case DT_RPATH:
                                printf("(DT_RPATH)");
                                break;
                            case DT_SYMBOLIC:
                                printf("(SYMBOLIC)");
                                break;
                            case DT_REL:
                                printf("(REL)");
                                break;
                            case DT_RELSZ:
                                printf("(RELSZ)");
                                break;
                            case DT_RELENT:
                                printf("(RELENT)");
                                break;
                            case DT_PLTREL:
                                printf("(PLTREL)");
                                break;
                            case DT_DEBUG:
                                printf("(DEBUG)");
                                break;
                            case DT_TEXTREL:
                                printf("(TEXTREL)");
                                break;
                            case DT_JMPREL:
                                printf("(JMPREL)");
                                break;
                            case DT_BIND_NOW:
                                printf("(BIND_NOW)");
                                break;
                            case DT_INIT_ARRAY:
                                printf("(INIT_ARRAY)");
                                break;
                            case DT_FINI_ARRAY:
                                printf("(FINI_ARRAY)");
                                break;
                            case DT_INIT_ARRAYSZ:
                                printf("(INIT_ARRAYSZ)");
                                break;
                            case DT_FINI_ARRAYSZ:
                                printf("(FINI_ARRAYSZ)");
                                break;
                            case DT_RUNPATH:
                                printf("(RUNPATH)");
                                break;
                            case DT_FLAGS:
                                printf("(FLAGS)");
                                break;
                            case DT_ENCODING:
                                printf("(ENCODING)");
                                break;
                            case DT_PREINIT_ARRAYSZ:
                                printf("(PREINIT_ARRAYSZ)");
                                break;
                            case DT_SYMTAB_SHNDX:
                                printf("(SYMTAB_SHNDX)");
                                break;
                            case DT_NUM:
                                printf("(NUM)");
                                break;
                            case DT_LOOS:
                                printf("(LOOS)");
                                break;
                            case DT_HIOS:
                                printf("(HIOS)");
                                break;
                            case DT_LOPROC:
                                printf("(LOPROC)");
                                break;
                            case DT_HIPROC:
                                printf("(HIPROC)");
                                break;

                            default:
                                break;
                            }

                            char *dynname = str + dyntab.d_un.d_val;
                            printf("%s %" PRIx64, dynname, dyntab.d_un.d_ptr);

                            printf("\n");
                        }
                    }
                }
                if (dynExists == 0)
                {
                    printf("No .dynamic table.\n");
                }
            }

            else if (strcmp(dash, "--program_headers") == 0)
            {
                if (header.e_phnum > 0)
                {
                    printf("Entry point at 0x%" PRIx64 "\n", header.e_entry);
                    printf("There are %" PRIu16 " program headers starting at offset %" PRIu64, header.e_phnum, header.e_phoff);
                    printf("Program Headers:\n");
                    printf("Type      Offset      VirtAddr      PhysAddr       FileSiz    MemSiz    Flg Allign");

                    for (int idx = 0; idx < header.e_phnum; idx++)
                    {
                        memcpy(&phHdr, buffer + header.e_phentsize * idx + header.e_phoff, sizeof(phHdr));

                        switch (phHdr.p_type)
                        {
                        case PT_NULL:
                            printf("NULL ");
                            break;
                        case PT_LOAD:
                            printf("LOAD ");
                            break;
                        case PT_DYNAMIC:
                            printf("DYNAMIC ");
                            break;
                        case PT_INTERP:
                            printf("INTERP ");
                            break;
                        case PT_NOTE:
                            printf("NOTE ");
                            break;
                        case PT_SHLIB:
                            printf("SHLIB ");
                            break;
                        case PT_PHDR:
                            printf("PHDR ");
                            break;
                        case PT_TLS:
                            printf("TLS ");
                            break;
                        case PT_NUM:
                            printf("NUM ");
                            break;
                        case PT_LOOS:
                            printf("LOOS ");
                            break;
                        case PT_GNU_EH_FRAME:
                            printf("GNU_EH_FRAME ");
                            break;
                        case PT_GNU_STACK:
                            printf("GNU_STACK ");
                            break;
                        case PT_GNU_RELRO:
                            printf("GNU_RELRO ");
                            break;
                        case PT_LOSUNW:
                            printf("LOSUNW ");
                            break;
                        case PT_SUNWSTACK:
                            printf("UNWSTACK ");
                            break;

                        case PT_HIOS:
                            printf("HIOS ");
                            break;
                        case PT_LOPROC:
                            printf("LOPROC ");
                            break;
                        case PT_HIPROC:
                            printf("HIPROC ");
                            break;
                        default:
                            break;
                        }

                        printf(" 0x%" PRIx64 " ", phHdr.p_offset);
                        printf(" 0x%" PRIx64 " ", phHdr.p_vaddr);
                        printf(" 0x%" PRIx64 " ", phHdr.p_paddr);
                        printf(" 0x%" PRIx64 " ", phHdr.p_filesz);
                        printf(" 0x%" PRIx64 " ", phHdr.p_memsz);
                        printf(" %" PRId32 " ", phHdr.p_flags);

                        // switch (p_flags(phHdr.p_flags))
                        // {
                        // case PF_X:
                        //     printf("X");
                        //     break;
                        // case PF_W:
                        //     printf("W");
                        //     break;
                        // case PF_R:
                        //     printf("R");
                        //     break;
                        // default:
                        //     break;
                        // }

                        printf(" 0x%" PRIx64 " \n", phHdr.p_align);
                    }
                }
                else
                {
                    printf("There are no program headers.\n");
                }
            }

            else if (strcmp(dash, "--segment..rodata") == 0)
            {
                int rodata = 0;

                for (int idx = 0; idx < header.e_shnum; idx++)
                {

                    // char *sectBuffer = malloc(header.e_shentsize+1);

                    char *name = "";

                    memcpy(&sectHdr, buffer + header.e_shentsize * idx + header.e_shoff, sizeof(sectHdr));

                    // print section name
                    if (sectHdr.sh_name)
                        ;
                    name = SectNames + sectHdr.sh_name;

                    if (strcmp(name, ".rodata") == 0)
                    {
                        rodata = 1;
                        printf("Hex dump of section '.rodata':\n");

                        char rodataHex[sectHdr.sh_size + 1];

                        memcpy(&rodataHex, buffer + sectHdr.sh_offset, sectHdr.sh_size);
                        // for (int i = 0; i < sizeof(rodataHex); i++)
                        // {
                        //     printf("%02hhX", (rodataHex[i]);
                        // }
                        // printf("%s", rodataHex);
                        // unsigned char hexholder[20];
                        int counter = 0;
                        int hexnumCounter = 0;

                        for (int i = 0; i < sizeof(rodataHex); i++)
                        {

                            printf("%02hhx", rodataHex[i]);
                            hexnumCounter++;

                            // sprintf(hexholder[counter],  "%s", rodataHex[i]);
                            if (i % 4 == 3)
                            {
                                printf(" ");
                            }
                            if (i % 20 == 3)
                            {
                                printf("    ");

                                for (int j = counter; j < hexnumCounter; j++)
                                {
                                    printf("%c", rodataHex[j]);
                                }
                                printf("\n");

                                counter = i + 1;
                            }
                        }
                        printf("\n");
                    }
                }
                if (rodata == 0)
                {
                    printf("No .rodata table.\n");
                }
            }

            else
            {
                fprintf(stderr, "Incorect - Input\n");
                exit(0);
            }
            // finally close the file
            
        }

        return 0;
    }
}
