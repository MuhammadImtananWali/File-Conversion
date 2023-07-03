#include "Cecho.h"

int main(int argc, char **argv)
{
    // main
    if (argc == 1)
    {
        printf("Usage: ebcEcho file1 file2");
        return SUCCESS;
    } 
    // validate that user has enter 2 arguments (plus the executable name)
    if (argc != 3) // check arg count 
    { 
        printf("ERROR: Bad Arguments\n");
        return BAD_ARGS;
    }
    return runE(argc, argv);
    
} // main()