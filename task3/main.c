#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <errno.h>
#include "elf.h"

int debug = 0;
int Currentfd = -1;

struct fun_desc {
    char *name;
    void (*fun)();
};
Elf32_Ehdr* openHeader();

void examineElfFile();
void toggleDebugMode();
void printSectionNames();
void printSymbols();
void quit();


int getNumberOfOptions(struct fun_desc * menu);
void getIntoLoop(struct fun_desc * menu, int numberOfOptions);
void displayPrompt (struct fun_desc * menu, int numberOfOptions);
int getOption (struct fun_desc * menu, int numberOfOptions);

int stringToDecimal(char * str);
int stringToHex(char * str);
char* unit_to_format_dec(int unit);
char* unit_to_format_hex(int unit);
int getHexa();
int getDecimal();


void toggleDebugMode()
{
    debug = !debug;
}
void examineElfFile()
{
    if(Currentfd != -1)
    {
        if (close(Currentfd) < 0)
        {

            perror("close");
        }
    }
    printf("Enter a file name\n");


    char input[100];
    fgets(input, sizeof(input), stdin);
    //Get the \n
    fgets(input, sizeof(input), stdin);
    input[strlen(input)-1] = 0;

    void *map_start; /* will point to the start of the memory mapped file */
    struct stat fd_stat; /* this is needed to  the size of the file */
    Elf32_Ehdr *header; /* this will point to the header structure */
    int num_of_section_headers;

    if( (Currentfd = open(input, O_RDWR)) < 0 ) {
        perror("error in open");
        exit(-1);
    }

    if( fstat(Currentfd, &fd_stat) != 0 ) {
        perror("stat failed");
        exit(-1);
    }

    if ( (map_start = mmap(0, fd_stat.st_size, PROT_READ | PROT_WRITE , MAP_SHARED, Currentfd, 0)) == MAP_FAILED ) {
        perror("mmap failed");
        exit(-4);
    }

    /* now, the file is mapped starting at map_start.
     * all we need to do is tell *header to point at the same address:
     */

    header = (Elf32_Ehdr *) map_start;
    /* now we can do whatever we want with header!!!!
     * for example, the number of section header can be obtained like this:
     */
    num_of_section_headers = header->e_shnum;
    printf("Magic:\n");
    printf("%c \t",header->e_ident[EI_MAG0]);
    printf("%c \t",header->e_ident[EI_MAG1]);
    printf("%c \t",header->e_ident[EI_MAG2]);
    printf("%c \t",header->e_ident[EI_MAG3]);
    printf("\n");
    printf("Num of sections:\n");

    printf("%d\n", num_of_section_headers);
    printf("Entry point:\n");
    printf("%x\n" , header->e_entry);

    printf("The file offset in which the section header table resides:\n");
    printf("%x bytes\n" , header->e_shoff);

    printf("The number of section header entries:\n");
    printf("%x bytes\n" , header->e_shnum);

    printf("The size of each section header entry:\n");
    printf("%x bytes\n" , header->e_phentsize);

    /* now, we unmap the file */
    munmap(map_start, fd_stat.st_size);


}



void printSectionNames()
{
    int fd;
    void *map_start; /* will point to the start of the memory mapped file */
    struct stat fd_stat; /* this is needed to  the size of the file */
    Elf32_Ehdr *header; /* this will point to the header structure */
    int num_of_section_headers;
    printf("Enter a file name\n");
    char input[100];
    fgets(input, sizeof(input), stdin);
    //Get the \n
    fgets(input, sizeof(input), stdin);
    input[strlen(input)-1] = 0;

    if( (fd = open(input, O_RDWR)) < 0 ) {
        perror("error in open");
        exit(-1);
    }

    if( fstat(fd, &fd_stat) != 0 ) {
        perror("stat failed");
        exit(-1);
    }

    if ( (map_start = mmap(0, fd_stat.st_size, PROT_READ | PROT_WRITE , MAP_SHARED, fd, 0)) == MAP_FAILED ) {
        perror("mmap failed");
        exit(-4);
    }

    /* now, the file is mapped starting at map_start.
     * all we need to do is tell *header to point at the same address:
     */
    int i;
    header = (Elf32_Ehdr *) map_start;
    /* now we can do whatever we want with header!!!!
     * for example, the number of section header can be obtained like this:
     */

    num_of_section_headers = header->e_shnum;
    Elf32_Shdr* shstrtabSection = (Elf32_Shdr *)(map_start  + header->e_shoff + (header->e_shstrndx * header->e_shentsize));

    printf("[index]\tsection_name\tsection_address\tsection_offset\tsection_size\tsection_type\n");
    for (i=0; i< num_of_section_headers; i++)
    {
        Elf32_Shdr * sectionHeader = (Elf32_Shdr *)(header->e_shoff + map_start + (i * header->e_shentsize));

        printf("[%d]\t",i);                            //index

        printf("%s\t\t",  (char *) (map_start + shstrtabSection->sh_offset +sectionHeader->sh_name));      //section name
        printf("%x\t\t", sectionHeader->sh_addr);      //section_address
        printf("%d\t\t", sectionHeader->sh_offset);                              //section_offset
        printf("%d\t\t", sectionHeader->sh_size);                              //section_size
        printf("%d\t\t", sectionHeader->sh_type);                              //section_type
        printf("\n");
    }

    /* now, we unmap the file */
    munmap(map_start, fd_stat.st_size);

}
void printSymbols()
{
    int fd;
    void *map_start; /* will point to the start of the memory mapped file */
    struct stat fd_stat; /* this is needed to  the size of the file */
    Elf32_Ehdr *header; /* this will point to the header structure */
    int num_of_section_headers;
    printf("Enter a file name\n");
    char input[100];
    fgets(input, sizeof(input), stdin);
    //Get the \n
    fgets(input, sizeof(input), stdin);
    input[strlen(input)-1] = 0;

    if( (fd = open(input, O_RDWR)) < 0 ) {
        perror("error in open");
        exit(-1);
    }

    if( fstat(fd, &fd_stat) != 0 ) {
        perror("stat failed");
        exit(-1);
    }

    if ( (map_start = mmap(0, fd_stat.st_size, PROT_READ | PROT_WRITE , MAP_SHARED, fd, 0)) == MAP_FAILED ) {
        perror("mmap failed");
        exit(-4);
    }

    /* now, the file is mapped starting at map_start.
     * all we need to do is tell *header to point at the same address:
     */
    int i;
    header = (Elf32_Ehdr *) map_start;
    /* now we can do whatever we want with header!!!!
     * for example, the number of section header can be obtained like this:
     */

    num_of_section_headers = header->e_shnum;
    Elf32_Shdr* strtabSection;

    printf("[index]\t\tvalue\t\tsection_index\tsection_name\tsymbol_name\n");
    for (int k = 0; k < num_of_section_headers; ++k) {
        Elf32_Shdr * sectionHeader = (Elf32_Shdr *)(header->e_shoff + map_start + (k * header->e_shentsize));
        if(sectionHeader->sh_type == SHT_STRTAB){
            strtabSection = sectionHeader;
        }
    }
    for (i=0; i< num_of_section_headers; i++)
    {
        Elf32_Shdr * sectionHeader = (Elf32_Shdr *)(header->e_shoff + map_start + (i * header->e_shentsize));
        Elf32_Shdr* shstrtabSection = (Elf32_Shdr *)(map_start  + header->e_shoff + (header->e_shstrndx * header->e_shentsize));
        if ((sectionHeader->sh_type == SHT_SYMTAB) || (sectionHeader->sh_type == SHT_DYNSYM))
        {

            Elf32_Off sh_offset = sectionHeader->sh_offset;
            Elf32_Sym * symTable = (Elf32_Sym *) (map_start + sh_offset);
            int numberOfEntries = sectionHeader->sh_size/sectionHeader->sh_entsize;



            int j;
            for (j=0; j<numberOfEntries; j++)
            {
                printf("[%d]\t\t",j);                      //index
                printf("%x\t\t", symTable[j].st_value);
                printf("%d\t\t", symTable[j].st_shndx);
                printf("%s\t\t",  (char *) (map_start + shstrtabSection->sh_offset +sectionHeader->sh_name));      //section name
                printf("%s\t\t",  (char *) (map_start + strtabSection->sh_offset + symTable[j].st_name));      //symbol name
                printf("\n");
            }





        }





    }



    /* now, we unmap the file */
    munmap(map_start, fd_stat.st_size);

}

void quit()
{
    exit(0);
}



int stringToDecimal(char * str)
{
    return atoi(str);
}

int getDecimal()
{
    char input[4];
    scanf("%s", input);
    return atoi(input);
}
int getHexa()
{
    char input[4];
    scanf("%s", input);
    return (int)strtol(input, NULL, 16);
}
int stringToHex(char * str)
{
    return (int)strtol(str, NULL, 16);
}
int getNumberOfOptions(struct fun_desc * menu)
{
    int numberOfOptions = 1;
    int j = 0;
    while (menu[j].name != NULL)
    {
        j++;
        numberOfOptions = j;
    }
    return numberOfOptions;
}

void displayPrompt (struct fun_desc * menu, int numberOfOptions)
{
    fprintf(stdout, "Choose action:\n");
    for (int i=0; i<numberOfOptions; i++)
    {
        fprintf(stdout, "%d-%s\n", i, menu[i].name);
    }
}
int getOption (struct fun_desc * menu, int numberOfOptions)
{
    int option = getDecimal();

    if((option < 0) || (numberOfOptions <= option))
    {
        fprintf(stderr, "Not within bounds. EXITING\n");
        exit(1);
    }

    return option;
}

void getIntoLoop(struct fun_desc * menu, int numberOfOptions)
{
    while (0 != 1)
    {
        displayPrompt(menu, numberOfOptions);
        int option = getOption(menu, numberOfOptions);
        (menu[option].fun)();

        if (option == 1000) // never gets to here
        {
            return;
        }
    }
}

int main(int argc, char **argv)
{
    struct fun_desc menu[] =
            {
                    { "Toggle Debug Mode", &toggleDebugMode },
                    { "Examine ELF File", &examineElfFile },
                    { "Print Section Names", &printSectionNames},
                    { "Print Symbols", &printSymbols},
                    { "quit", &quit},
                    { NULL, NULL }
            };

    getNumberOfOptions(menu);

    //Currentfd

    getIntoLoop(menu, getNumberOfOptions(menu));

    return 0;
}

