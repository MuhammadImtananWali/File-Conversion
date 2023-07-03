#ifndef COMP_H
#define COMP_H

#include <stdio.h>
#include <stdlib.h>

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

typedef struct ImageFileInfo
{
    unsigned char magicNumber[2];
    unsigned short *magicNumberValue;

    // create and initialise variables used within code
    int width, height;
    unsigned char **imageData;
    long numBytes;
} ImageFileInfo;

void clearImageData(struct ImageFileInfo *imageFileInfo)
{
    for (int i = 0; i < imageFileInfo->height; i++)
    {
        free(imageFileInfo->imageData[i]);
    }
    free(imageFileInfo->imageData);
}

// This function copy file data in structure 2D array imageData.
int processInputFile(struct ImageFileInfo *imageFileInfo, char argv[])
{
    // open the input file in read mode
    FILE *inputFile = fopen(argv, "rb");
    // check file opened successfully
    if (!inputFile)
    { // check file pointer
        printf("ERROR: Bad File Name (%s)\n", argv);
        return BAD_FILE;
    } // check file pointer

    // get first 2 characters which should be magic number
    imageFileInfo->magicNumber[0] = getc(inputFile);
    imageFileInfo->magicNumber[1] = getc(inputFile);

    // checking against the casted value due to endienness.
    if (*imageFileInfo->magicNumberValue != MAGIC_NUMBER)
    { // check magic number
        printf("ERROR: Bad Magic Number (%s)\n", argv);
        return BAD_MAGIC_NUMBER;
    } // check magic number

    // scan for the dimensions
    // and capture fscanfs return to ensure we got 2 values.
    int check = fscanf(inputFile, "%d %d", &imageFileInfo->height, &imageFileInfo->width);

    if (check != 2 || imageFileInfo->height < MIN_DIMENSION || imageFileInfo->width < MIN_DIMENSION || imageFileInfo->height > MAX_DIMENSION || imageFileInfo->width > MAX_DIMENSION)
    { // check dimensions
        // close the file as soon as an error is found
        fclose(inputFile);
        // print appropriate error message and return
        printf("ERROR: Bad Dimensions (%s)\n", argv);
        return BAD_DIM;
    } // check dimensions

    // caclulate total size and allocate memory for array
    imageFileInfo->numBytes = imageFileInfo->height * imageFileInfo->width;
    imageFileInfo->imageData = (unsigned char **)malloc(imageFileInfo->numBytes * sizeof(unsigned char *));

    // if malloc is unsuccessful, it will return a null pointer
    if (imageFileInfo->imageData == NULL)
    { // check malloc
        fclose(inputFile);
        printf("ERROR: Image Malloc Failed\n");
        return BAD_MALLOC;
    } // check malloc

    for (int i = 0; i < imageFileInfo->height; i++)
    {
        imageFileInfo->imageData[i] = (unsigned char *)malloc(imageFileInfo->width * sizeof(unsigned char));
    }

    // read in each grey value from the file
    for (int row1 = 0; row1 < imageFileInfo->height; row1++)
    { // reading in
        for (int col1 = 0; col1 < imageFileInfo->width; col1++)
        {
            check = fread(&imageFileInfo->imageData[row1][col1],sizeof(char),1,inputFile);
            // validate that we have captured 1 pixel value
            if (check != 1)
            { // check inputted data
                // ensure that allocated data is freed before exit.
                clearImageData(imageFileInfo);
                fclose(inputFile);
                printf("ERROR: Bad Data (%s)\n", argv);
                return BAD_DATA;
            }
            if (imageFileInfo->imageData[row1][col1] < 0 || imageFileInfo->imageData[row1][col1] > 31)
            {
                clearImageData(imageFileInfo);
                fclose(inputFile);
                printf("ERROR: Bad Data (%s)\n", argv);
                return BAD_DATA;
            }
        }

    } // reading in

    // now we have finished using the inputFile we should close it
    fclose(inputFile);
    return SUCCESS;
}

// Compare two files are IDENTICAL or not
int compareData(struct ImageFileInfo *imageFileInfo, struct ImageFileInfo *imageFileInfo2)
{
    // start with magic number values
    if (*imageFileInfo->magicNumberValue != *imageFileInfo2->magicNumberValue)
    { // free and exit
        clearImageData(imageFileInfo);
        clearImageData(imageFileInfo2);
        printf("DIFFERENT\n");
        return SUCCESS;
    } // free and exit

    // check dimensions
    if ((imageFileInfo->height != imageFileInfo2->height) || (imageFileInfo->width != imageFileInfo2->width))
    { // free and exit
        clearImageData(imageFileInfo);
        clearImageData(imageFileInfo2);
        printf("DIFFERENT\n");
        return SUCCESS;
    }

    // and check the pixel values
    for (int row = 0; row < imageFileInfo->height; row++)
    {
        for (int col = 0; col < imageFileInfo->width; col++)
        {
            if (imageFileInfo->imageData[row][col] != imageFileInfo2->imageData[row][col])
            { // free and exit
                clearImageData(imageFileInfo);
                clearImageData(imageFileInfo2);
                printf("DIFFERENT\n");
                return SUCCESS;
            }
        }
    }

    // free allocated memory before exit
    clearImageData(imageFileInfo);
    clearImageData(imageFileInfo2);

    // if we have not exited on different data, must be identical
    printf("IDENTICAL\n");
    return SUCCESS;
}

int runC(int argc, char **argv)
{
    struct ImageFileInfo imageFileInfo1, imageFileInfo2;
    // create a char array to hold magic number
    // and cast to short
    imageFileInfo1.magicNumberValue = (unsigned short *)imageFileInfo1.magicNumber;
    imageFileInfo2.magicNumberValue = (unsigned short *)imageFileInfo2.magicNumber;

    // create and initialise variables used within code
    imageFileInfo1.width = imageFileInfo2.width = 0;
    imageFileInfo1.height = imageFileInfo2.height = 0;

    int verify = processInputFile(&imageFileInfo1, argv[1]);
    if (verify != SUCCESS)
        return verify;

    // compare the data from the two files:
    verify = processInputFile(&imageFileInfo2, argv[2]);
    if (verify != SUCCESS)
        return verify;

    return compareData(&imageFileInfo1, &imageFileInfo2);
}

#endif
