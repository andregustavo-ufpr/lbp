#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


typedef struct PGMImage {
    char pgmType[3];
    unsigned char** data;
    unsigned int width;
    unsigned int height;
    unsigned int max_value;
} PGMImage;

// Auxiliar function to ignore commented lines in the file opened
void ignoreComments(FILE* fp);

// Function that open a file "file_name" and process its information into the
// struct PGMImage "pgm"
bool openPGM(PGMImage* pgm, const char* file_name);

// Function that saves the information contained in "lbp" to a file of name
// "file_name" and uses the "base_image" info
int saveToFile(char *file_name, PGMImage *base_image, unsigned char **lbp);

// Allocate the necessary memory space for a image of size
// "height" and "width"
unsigned char** allocateImage(int width, int height);

// Disposes of allocated memory
void freeImage(unsigned char** image, int height);

#endif