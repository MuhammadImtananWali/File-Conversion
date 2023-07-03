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

    // Variable initialization.
    int width, height;
    unsigned char **imageData;
    long numBytes;
} ImageFileInfo;

// This function is used to free imageData space from memory.
void clearImageData(struct ImageFileInfo imageFileInfo)
{
    for (int i = 0; i < imageFileInfo.height; i++)
    {
        free(imageFileInfo.imageData[i]);
    }
    free(imageFileInfo.imageData);
}

int readInputFile(struct ImageFileInfo *imageFileInfo, char **argv)
{
    // Open the input file in Read mode.
    FILE *inputFile = fopen(argv[1], "rb");
    // Check file opened successfully.
    if (!inputFile)
    {
        printf("ERROR: Bad File Name (%s)\n", argv[1]);
        return BAD_FILE;
    }

    // Get first 2 characters which should be magic number.
    imageFileInfo->magicNumber[0] = getc(inputFile);
    imageFileInfo->magicNumber[1] = getc(inputFile);

    // Checking magic number.
    if (*imageFileInfo->magicNumberValue != MAGIC_NUMBERU)
    {
        printf("ERROR: Bad Magic Number (%s)\n", argv[1]);
        return BAD_MAGIC_NUMBER;
    }
    
    // Capture the dimensions of image.
    int check = fscanf(inputFile, "%d %d", &imageFileInfo->height, &imageFileInfo->width);
    if (check != 2 || imageFileInfo->height < MIN_DIMENSION || imageFileInfo->width < MIN_DIMENSION || imageFileInfo->height > MAX_DIMENSION || imageFileInfo->width > MAX_DIMENSION)
    {
        // Close the file when error found.
        fclose(inputFile);
        printf("ERROR: Bad Dimensions (%s)\n", argv[1]);
        return BAD_DIM;
    }

    // Caclulate total size and allocate memory for 2D array.
    imageFileInfo->numBytes = imageFileInfo->height * imageFileInfo->width;
    imageFileInfo->imageData = (unsigned char **)malloc(imageFileInfo->numBytes * sizeof(unsigned char *));

    // If malloc is unsuccessful, it will return a null pointer.
    if (imageFileInfo->imageData == NULL)
    {
        fclose(inputFile);
        printf("ERROR: Image Malloc Failed\n");
        return BAD_MALLOC;
    }
    
    for (int i = 0; i < imageFileInfo->height; i++)
    {
        imageFileInfo->imageData[i] = (unsigned char *)malloc(imageFileInfo->width * sizeof(unsigned char));
    }

    // Read each grey value from the file and store it in a 2D array.
    for (int row = 0; row < imageFileInfo->height; row++)
    { // reading in
        for (int col = 0; col < imageFileInfo->width; col++)
        {
             int check = fread(&imageFileInfo->imageData[row][col],sizeof(unsigned char),1,inputFile);
            // validate that we have captured 1 pixel value
            if (check != 1)
            {
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

    } // reading out

    // Now we have finished using the inputFile we should close it.
    fclose(inputFile);
    return SUCCESS;
}

int writeOutputFile(struct ImageFileInfo *imageFileInfo, char **argv)
{
    // Open the output file in Write mode.
    FILE *outputFile = fopen(argv[2], "w");
    // Validate that the file has been opened correctly.
    if (outputFile == NULL)
    {
        free(imageFileInfo->imageData);
        printf("ERROR: Bad File Name (%s)\n", argv[2]);
        return BAD_FILE;
    }
    
    // Write the header data first 2 rows of file.
    int check = fprintf(outputFile, "eb\n%d %d\n", imageFileInfo->height, imageFileInfo->width);
    if (check == 0)
    {
        fclose(outputFile);
        free(imageFileInfo->imageData);
        printf("ERROR: Bad Output\n");
        return BAD_OUTPUT;
    }
    
    // Iterate though the array and print out pixel values in the file.
    for (int row = 0; row < imageFileInfo->height; row++)
    { // writing in
        for (int col = 0; col < imageFileInfo->width; col++)
        {
            	// If we are at the end of a row ((current+1)%width == 0) then write a newline, otherwise a space.
                if (col==0)
                {
                    col++;
                }

                if (row!=imageFileInfo->height-1 || col!=imageFileInfo->width-1)
                {
                    if (col == imageFileInfo->width-1){
                        fprintf(outputFile, "%u", imageFileInfo->imageData[row][col]);
                        fprintf(outputFile, "%c", ' ');
                        fprintf(outputFile, "%u\n", imageFileInfo->imageData[row+1][0]);
                    }
                    else
                    {
                        fprintf(outputFile, "%u", imageFileInfo->imageData[row][col]);
                        fprintf(outputFile, "%c", ' ');
                    }                   
                }
                else
                {
                    fprintf(outputFile, "%u", imageFileInfo->imageData[row][col]);
                    fprintf(outputFile, "%c", ' ');
                    check = fprintf(outputFile, "%u", imageFileInfo->imageData[row][0]);    
                }

            if (check == 0)
            {
                fclose(outputFile);
                clearImageData(*imageFileInfo);
                printf("ERROR: Bad Output\n");
                return BAD_OUTPUT;
            }
        }

    } // writing out
    
    // Close the output file and free memory space before exit.
    clearImageData(*imageFileInfo);
    fclose(outputFile);

    // Print final success message and return.
    printf("CONVERTED\n");
    return SUCCESS;
}

int run(int argc, char **argv)
{
    struct ImageFileInfo imageFileInfo;
    // Create a char array to hold magic number and cast to short.
    imageFileInfo.magicNumberValue = (unsigned short *)imageFileInfo.magicNumber;
    imageFileInfo.width = 0;

    int flag = readInputFile(&imageFileInfo, argv);
    if (flag != SUCCESS)
        return flag;

    return writeOutputFile(&imageFileInfo, argv);
}

#endif
