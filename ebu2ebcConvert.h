#ifndef ECHO_H
#define ECHO_H

#include <stdio.h>
#include <stdlib.h>
#include "Ccomp.h"

#define SUCCESS 0
#define BAD_ARGS 1
#define BAD_FILE 2
#define BAD_MAGIC_NUMBER 3
#define BAD_DIM 4
#define BAD_MALLOC 5
#define BAD_DATA 6
#define BAD_OUTPUT 7
#define MAGIC_NUMBER 0x6365
#define MAX_DIMENSION 262144
#define MIN_DIMENSION 1

int readInputFile(struct ImageFileInfo *imageFileInfo, char **argv)
{
    // Function is from Ccomp.h that copy file data in structure 2D array imageData.
    return processInputFile(imageFileInfo, argv[1]);
}

int writeOutputFile(struct ImageFileInfo *imageFileInfo, char **argv)
{
    // open the output file in write mode
    FILE *outputFile = fopen(argv[2], "wb");
    // validate that the file has been opened correctly
    if (outputFile == NULL)
    { // validate output file
        clearImageData(imageFileInfo);
        printf("ERROR: Bad File Name (%s)\n", argv[2]);
        return BAD_FILE;
    } // validate output file

    // write the header data in one block
    int check = fprintf(outputFile, "ec\n%d %d\n", imageFileInfo->height, imageFileInfo->width);
    // and use the return from fprintf to check that we wrote.
    if (check == 0)
    { // check write
        fclose(outputFile);
        clearImageData(imageFileInfo);
        printf("ERROR: Bad Output\n");
        return BAD_OUTPUT;
    }

    // iterate though the array and print out pixel values
    for (int row = 0; row < imageFileInfo->height; row++)
    { // writing in
        for (int col = 0; col < imageFileInfo->width; col++)
        {
            // if we are at the end of a row ((current+1)%width == 0) then write a newline, otherwise a space.
            unsigned char pixel_val = 255 - (imageFileInfo->imageData[row][col]);	//Converting EBU to EBC
            check = fwrite(&pixel_val,sizeof(unsigned char),1,outputFile);
            if (check == 0)
            { // check write
                fclose(outputFile);
                clearImageData(imageFileInfo);
                printf("ERROR: Bad Output\n");
                return BAD_OUTPUT;
            }
        }

    } // writing out

    clearImageData(imageFileInfo);

    // close the output file before exit
    fclose(outputFile);

    // print final success message and return
    printf("CONVERTED\n");
    return SUCCESS;
}

int run(int argc, char **argv)
{
    // create a char array to hold magic number
    // and cast to short
    struct ImageFileInfo imageFileInfo;
    imageFileInfo.magicNumberValue = (unsigned short *)imageFileInfo.magicNumber;
    imageFileInfo.width = 0;

    int flag = readInputFile(&imageFileInfo, argv);
    if (flag != SUCCESS)
        return flag;

    return writeOutputFile(&imageFileInfo, argv);
}
#endif
