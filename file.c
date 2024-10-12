#include "file.h"

// in file
void ignoreComments(FILE* fp)
{
    int ch;
    char line[100];
 
    // Ignore any blank lines
    while ((ch = fgetc(fp)) != EOF && isspace(ch));
 
    // Recursively ignore comments
    // in a PGM image commented lines
    // start with a '#'
    if (ch == '#') {
        fgets(line, sizeof(line), fp);
        ignoreComments(fp);
    }
    else
        fseek(fp, -1, SEEK_CUR);
}

// Function to open the input a PGM
// file and process it
bool openPGM(PGMImage* pgm, const char* filename)
{
    // Open the image file in the
    // 'read binary' mode
    FILE* pgm_file = fopen(filename, "rb");
 
    // If file does not exist,
    // then return
    if (pgm_file == NULL) {
        printf("File does not exist\n");
        return false;
    }
 
    ignoreComments(pgm_file);
    fscanf(pgm_file, "%s", pgm->pgmType);
 
    // Check for correct PGM Binary
    // file type
    if (strcmp(pgm->pgmType, "P5")) {
        fprintf(stderr, "Wrong file type!\n");
        exit(EXIT_FAILURE);
    }
 
    ignoreComments(pgm_file);
 
    // Read the image dimensions
    fscanf(pgm_file, "%d %d", &(pgm->width), &(pgm->height));
 
    ignoreComments(pgm_file);
 
    // Read maximum gray value
    fscanf(pgm_file, "%d", &(pgm->max_value));
    ignoreComments(pgm_file);
 
    // Allocating memory to store
    // img info in defined struct
    pgm->data = malloc(pgm->height * sizeof(unsigned char*));
 
    // Storing the pixel info in
    // the struct
    if (pgm->pgmType[1] == '5') {
 
        fgetc(pgm_file);
 
        for (int i = 0;
             i < pgm->height; i++) {
            pgm->data[i] = malloc(pgm->width * sizeof(unsigned char));
 
            // If memory allocation
            // is failed
            if (pgm->data[i] == NULL) {
                fprintf(stderr, "malloc failed\n");
                exit(1);
            }
 
            // Read the gray values and
            // write on allocated memory
            fread(pgm->data[i], sizeof(unsigned char), pgm->width, pgm_file);
        }
    }
 
    // Close the file
    fclose(pgm_file);
 
    return true;
}

unsigned char** allocateImage(int width, int height) {
    unsigned char **image = (unsigned char **)malloc(height * sizeof(unsigned char *));
    for (int i = 0; i < height; i++) {
        image[i] = (unsigned char *)malloc(width * sizeof(unsigned char));
        if (image[i] == NULL) {
            printf("Memory allocation failed at row %d!\n", i);
            break;
        }
    }

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            image[i][j] = 0;
        }
    }

    return image;
}

void freeImage(unsigned char** image, int height) {
    for (int i = 0; i < height; i++) {
        free(image[i]);
    }
    free(image);
}

int saveToFile(char *file_name, PGMImage *base_image, unsigned char **lbp){
    FILE* file = fopen(file_name, "w");
    if (file == NULL) {
        printf("Error opening file for writing!\n");
        
        return 1;
    }

    fprintf(file, "%s \n", base_image->pgmType);
    fprintf(file, "%d %d\n", base_image->width, base_image->height);
    fprintf(file, "%d \n", base_image->max_value);

    // Write the LBP image to the file
    for (int y = 0; y < base_image->height; y++) {
        for (int x = 0; x < base_image->width; x++) {
            fprintf(file, "%3d ", lbp[y][x]);
        }
        fprintf(file, "\n");
    }

    // Close the file
    fclose(file);

    return 0;
}