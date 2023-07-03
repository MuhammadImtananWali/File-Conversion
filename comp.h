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
#define MAGIC_NUMBER 0x6265
#define MAX_DIMENSION 262144
#define MIN_DIMENSION 1

typedef struct ImageFileInfo
{
    unsigned char magicNumber1[2];
    unsigned short *magicNumberValue1;

    // create and initialise variables used within code
    int width1, height1;
    unsigned int **imageData1;
    long numBytes1;
} ImageFileInfo;

// This function is used to free imageData space from memory.
void clearImageData(struct ImageFileInfo *imageFileInfo)
{
    for (int i = 0; i < imageFileInfo->height1; i++)
    {
        free(imageFileInfo->imageData1[i]);
    }
    free(imageFileInfo->imageData1);
}

int processInputFile(struct ImageFileInfo *imageFileInfo, char **argv)
{
    // Open the input file in Read Mode.
    FILE *inputFile1 = fopen(argv[1], "r");
    // check file opened successfully.
    if (!inputFile1)
    {
        printf("ERROR: Bad File Name (%s)\n", argv[1]);
        return BAD_FILE;
    }

    // Get first 2 characters from file which should be magic number.
    imageFileInfo->magicNumber1[0] = getc(inputFile1);
    imageFileInfo->magicNumber1[1] = getc(inputFile1);

    // checking magic number is valid or not.
    if (*imageFileInfo->magicNumberValue1 != MAGIC_NUMBER)
    {
        printf("ERROR: Bad Magic Number (%s)\n", argv[1]);
        return BAD_MAGIC_NUMBER;
    }
    
    // Capture dimensions of the image.
    int check = fscanf(inputFile1, "%d %d", &imageFileInfo->height1, &imageFileInfo->width1);
    if (check != 2 || imageFileInfo->height1 < MIN_DIMENSION || imageFileInfo->width1 < MIN_DIMENSION || imageFileInfo->height1 > MAX_DIMENSION || imageFileInfo->width1 > MAX_DIMENSION)
    {
        // close the file if error found.
        fclose(inputFile1);
        printf("ERROR: Bad Dimensions (%s)\n", argv[1]);
        return BAD_DIM;
    }

    // Caclulate total size and allocate memory for 2D array.
    imageFileInfo->numBytes1 = imageFileInfo->height1 * imageFileInfo->width1;
    imageFileInfo->imageData1 = (unsigned int **)malloc(imageFileInfo->numBytes1 * sizeof(unsigned int *));

    // if malloc is unsuccessful, it will return a null pointer.
    if (imageFileInfo->imageData1 == NULL)
    {
        fclose(inputFile1);
        printf("ERROR: Image Malloc Failed\n");
        return BAD_MALLOC;
    }
    for (int i = 0; i < imageFileInfo->height1; i++)
    {
        imageFileInfo->imageData1[i] = (unsigned int *)malloc(imageFileInfo->width1 * sizeof(unsigned int));
    }

    // Read each grey value from the file to 2D array.
    int row1 = 0, col1 = 0;
    for (int row1 = 0; row1 < imageFileInfo->height1; row1++)
    { // reading in
        for (int col1 = 0; col1 < imageFileInfo->width1; col1++)
        {
            check = fscanf(inputFile1, "%u", &imageFileInfo->imageData1[row1][col1]);
            // validate that we have captured 1 pixel value
            if (check != 1)
            {
                // ensure that allocated data is freed before exit.
                clearImageData(imageFileInfo);
                fclose(inputFile1);
                printf("ERROR: Bad Data (%s)\n", argv[1]);
                return BAD_DATA;
            }

            if (imageFileInfo->imageData1[row1][col1] < 0 || imageFileInfo->imageData1[row1][col1] > 31)
            {
                clearImageData(imageFileInfo);
                fclose(inputFile1);
                printf("ERROR: Bad Data (%s)\n", argv[1]);
                return BAD_DATA;
            }
        }

    } // reading out
    if (fscanf(inputFile1, "%u", &imageFileInfo->imageData1[row1][col1]) == 1)
    {
        clearImageData(imageFileInfo);
        fclose(inputFile1);
        printf("ERROR: Bad Data (%s)\n", argv[1]);
        return BAD_DATA;
    }

    // Now we have finished using the inputFile1 we should close it.
    fclose(inputFile1);
    return SUCCESS;  //Return success status.
}
int processOutputFile(struct ImageFileInfo *imageFileInfo2, char **argv)
{
    // Open the input file in Read Mode.
    FILE *inputFile2 = fopen(argv[2], "r");
    // Check file opened successfully.
    if (!inputFile2)
    {
        printf("ERROR: Bad File Name (%s)\n", argv[1]);
        return BAD_FILE;
    }

    // Get first 2 characters which should be magic number.
    imageFileInfo2->magicNumber1[0] = getc(inputFile2);
    imageFileInfo2->magicNumber1[1] = getc(inputFile2);

    // checking magic number.
    if (*imageFileInfo2->magicNumberValue1 != MAGIC_NUMBER)
    {
        printf("ERROR: Bad Magic Number (%s)\n", argv[1]);
        return BAD_MAGIC_NUMBER;
    }
    
    // Captures the dimensions of the image.
    int check = fscanf(inputFile2, "%d %d", &imageFileInfo2->height1, &imageFileInfo2->width1);
    if (check != 2 || imageFileInfo2->height1 < MIN_DIMENSION || imageFileInfo2->width1 < MIN_DIMENSION || imageFileInfo2->height1 > MAX_DIMENSION || imageFileInfo2->width1 > MAX_DIMENSION)
    {
        // Close the file when found error.
        fclose(inputFile2);
        printf("ERROR: Bad Dimensions (%s)\n", argv[1]);
        return BAD_DIM;
    }
    
    // Caclulate total size and allocate memory for 2D array.
    imageFileInfo2->numBytes1 = imageFileInfo2->height1 * imageFileInfo2->width1;
    imageFileInfo2->imageData1 = (unsigned int **)malloc(imageFileInfo2->numBytes1 * sizeof(unsigned int *));

    // If malloc is unsuccessful, it will return a null pointer.
    if (imageFileInfo2->imageData1 == NULL)
    {
        fclose(inputFile2);
        printf("ERROR: Image Malloc Failed\n");
        return BAD_MALLOC;
    }
    
    for (int i = 0; i < imageFileInfo2->height1; i++)
    {
        imageFileInfo2->imageData1[i] = (unsigned int *)malloc(imageFileInfo2->width1 * sizeof(unsigned int));
    }

    // Read each grey value from the file to 2D array.
    int row2 = 0, col2 = 0;
    for (int row2 = 0; row2 < imageFileInfo2->height1; row2++)
    { // reading in
        for (int col2 = 0; col2 < imageFileInfo2->width1; col2++)
        {
            check = fscanf(inputFile2, "%u", &imageFileInfo2->imageData1[row2][col2]);
            // validate that we have captured 1 pixel value
            if (check != 1)
            { // check inputted data
                // ensure that allocated data is freed before exit.
                clearImageData(imageFileInfo2);
                fclose(inputFile2);
                printf("ERROR: Bad Data (%s)\n", argv[1]);
                return BAD_DATA;
            }

            if (imageFileInfo2->imageData1[row2][col2] < 0 || imageFileInfo2->imageData1[row2][col2] > 31)
            {
                clearImageData(imageFileInfo2);
                fclose(inputFile2);
                printf("ERROR: Bad Data (%s)\n", argv[1]);
                return BAD_DATA;
            }
        }

    } // reading out
    if (fscanf(inputFile2, "%u", &imageFileInfo2->imageData1[row2][col2]) == 1)
    {
        clearImageData(imageFileInfo2);
        fclose(inputFile2);
        printf("ERROR: Bad Data (%s)\n", argv[1]);
        return BAD_DATA;
    }

    // Now we have finished using the inputFile2 we should close it.
    fclose(inputFile2);
    return SUCCESS;
}

// This function compares two structures and tell they are identical or not.
int compareData(struct ImageFileInfo *imageFileInfo, struct ImageFileInfo *imageFileInfo2)
{
    // Start with magic number values
    if (*imageFileInfo->magicNumberValue1 != *imageFileInfo2->magicNumberValue1)
    { // free and exit
        clearImageData(imageFileInfo);
        clearImageData(imageFileInfo2);
        printf("DIFFERENT\n");
        return SUCCESS;
    }
    
    // Compare dimensions
    if ((imageFileInfo->height1 != imageFileInfo2->height1) || (imageFileInfo->width1 != imageFileInfo2->width1))
    { // free and exit
        clearImageData(imageFileInfo);
        clearImageData(imageFileInfo2);
        printf("DIFFERENT\n");
        return SUCCESS;
    }
    
    // Compare the pixel values
    for (int row = 0; row < imageFileInfo->height1; row++)
    {
        for (int col = 0; col < imageFileInfo->width1; col++)
        {
            if (imageFileInfo->imageData1[row][col] != imageFileInfo2->imageData1[row][col])
            { // free and exit
                clearImageData(imageFileInfo);
                clearImageData(imageFileInfo2);
                printf("DIFFERENT\n");
                return SUCCESS;
            } 
        }
    }

    // Free allocated memory before exit.(Its a good practice.)
    clearImageData(imageFileInfo);
    clearImageData(imageFileInfo2);

    // If we have not exited till now, then the files must be identical.
    printf("IDENTICAL\n");
    return SUCCESS;
}
int run(int argc, char **argv)
{
    //  Process file 1.
    struct ImageFileInfo imageFileInfo;
    // Create a char array to hold magic number and cast to short.
    imageFileInfo.magicNumberValue1 = (unsigned short *)imageFileInfo.magicNumber1;

    // Initialization.
    imageFileInfo.width1 = imageFileInfo.height1 = 0;

    int verify = processInputFile(&imageFileInfo, argv);
    if (verify != SUCCESS)
        return verify;

    //  Process file 2.
    struct ImageFileInfo imageFileInfo2;
    // Create a char array to hold magic number and cast to short.
    imageFileInfo2.magicNumberValue1 = (unsigned short *)imageFileInfo2.magicNumber1;

    // Initialization.
    imageFileInfo2.width1 = 0;
    imageFileInfo2.height1 = 0;

    // compare the data from the two files:
    verify = processOutputFile(&imageFileInfo2, argv);
    if (verify != SUCCESS)
        return verify;

    return compareData(&imageFileInfo, &imageFileInfo2);
}

#endif
