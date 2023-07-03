#include "ebfConvert.h"

int main(int argc, char **argv)
{
    // main
    if (argc == 1)
    {
        printf("Usage: ebf2ebu file1 file2");
        return SUCCESS;
    } 
    // validate that user has enter 2 arguments (plus the executable name)
    if (argc != 3) // check arg count 
    { 
        printf("ERROR: Bad Arguments\n");
        return BAD_ARGS;
    }
    return run(argc, argv);
    
} // main()