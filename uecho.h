#ifndef ECHO_H
#define ECHO_H

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
#define MAGIC_NUMBERU 0x7565
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
}ImageFileInfo;

// This function is used to free imageData space from memory.
void clearImageData(struct ImageFileInfo imageFileInfo)
{
    // ensure that allocated data is freed before exit.
    for (int i = 0; i < imageFileInfo.height; i++)
    {
        free(imageFileInfo.imageData[i]);
    }
    free(imageFileInfo.imageData);
}

int readInputFile(struct ImageFileInfo *imageFileInfo, char **argv)
{
    // open the input file in read mode
    FILE *inputFile = fopen(argv[1],"rb");
    // check file opened successfully
    if (!inputFile)
    { // check file pointer
        printf("ERROR: Bad File Name (%s)\n", argv[1]);
        return BAD_FILE;
    } // check file pointer

    // get first 2 characters which should be magic number
    imageFileInfo->magicNumber[0] = getc(inputFile);
    imageFileInfo->magicNumber[1] = getc(inputFile);

    // checking against the casted value due to endienness.
    if (*imageFileInfo->magicNumberValue != MAGIC_NUMBERU)
    { // check magic number
        printf("ERROR: Bad Magic Number (%s)\n", argv[1]);
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
        printf("ERROR: Bad Dimensions (%s)\n", argv[1]);
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
    // int row = 0, col = 0;
    for (int row = 0; row < imageFileInfo->height; row++)
    { // reading in
        for (int col = 0; col < imageFileInfo->width; col++)
        {
            int check = fread(&imageFileInfo->imageData[row][col],sizeof(unsigned char),1,inputFile);
            // validate that we have captured 1 pixel value
            if (check != 1)
            { // check inputted data
                clearImageData(*imageFileInfo);

                fclose(inputFile);
                printf("ERROR: Bad Data (%s)\n", argv[1]);
                return BAD_DATA;
            }

            if (imageFileInfo->imageData[row][col] < 0 || imageFileInfo->imageData[row][col] > 31)
            {
                clearImageData(*imageFileInfo);
                fclose(inputFile);
                printf("ERROR: Bad Data (%s)\n", argv[1]);
                return BAD_DATA;
            }
        }

    } // reading in
    // if (fscanf(inputFile, "%s", &imageFileInfo->imageData[row][col]) == 1)
    // {
    //     clearImageData(*imageFileInfo);
    //     fclose(inputFile);
    //     printf("ERROR: Bad Data (%s)\n", argv[1]);
    //     return BAD_DATA;
    // }

    // now we have finished using the inputFile we should close it
    fclose(inputFile);
    return SUCCESS;
}

int writeOutputFile(struct ImageFileInfo *imageFileInfo, char **argv)
{
    // open the output file in write mode
    FILE *outputFile = fopen(argv[2], "wb");
    // validate that the file has been opened correctly
    if (outputFile == NULL)
    { // validate output file
        free(imageFileInfo->imageData);
        printf("ERROR: Bad File Name (%s)\n", argv[2]);
        return BAD_FILE;
    } // validate output file

    // write the header data in one block
    int check = fprintf(outputFile, "eb\n%d %d\n", imageFileInfo->height, imageFileInfo->width);
    // and use the return from fprintf to check that we wrote.
    if (check == 0)
    { // check write
        fclose(outputFile);
        free(imageFileInfo->imageData);
        printf("ERROR: Bad Output\n");
        return BAD_OUTPUT;
    } // check write

    // iterate though the array and print out pixel values
    // int row = 0;
    for (int row = 0; row < imageFileInfo->height; row++)
    { // writing out
        for (int col = 0; col < imageFileInfo->width; col++)
        {
            // if we are at the end of a row ((current+1)%width == 0) then write a newline, otherwise a space.
            check = fwrite(&imageFileInfo->imageData[row][col],sizeof(unsigned char),1,outputFile);
            if (check == 0)
            { // check write
                fclose(outputFile);
                clearImageData(*imageFileInfo);
                printf("ERROR: Bad Output\n");
                return BAD_OUTPUT;
            } // check write
        }

    } // writing out

    clearImageData(*imageFileInfo);

    // close the output file before exit
    fclose(outputFile);

    // print final success message and return
    printf("ECHOED\n");
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
