#define _GNU_SOURCE MINGW64_NT-10.0-22631 DESKTOP-O941M1R 3.4.10-bc1f6498.x86_64 2024-09-15 15:08 UTC x86_64 Msys

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <dirent.h>

typedef struct PGMImage {
    char pgmType[3];
    unsigned char** data;
    unsigned int width;
    unsigned int height;
    unsigned int maxValue;
} PGMImage;

// Function to compute the LBP value for a pixel
unsigned char computeLBP(unsigned char** image, int x, int y, int width, int height) {
    unsigned char lbp = 0;
    unsigned char center = image[y][x];
    
    // Compare the center pixel to its 8 neighbors
    if (x > 0 && y > 0)              lbp |= (image[y-1][x-1] >= center) << 7;
    if (y > 0)                       lbp |= (image[y-1][x]   >= center) << 6;
    if (x < width-1 && y > 0)        lbp |= (image[y-1][x+1] >= center) << 5;
    if (x < width-1)                 lbp |= (image[y][x+1]   >= center) << 4;
    if (x < width-1 && y < height-1) lbp |= (image[y+1][x+1] >= center) << 3;
    if (y < height-1)                lbp |= (image[y+1][x]   >= center) << 2;
    if (x > 0 && y < height-1)       lbp |= (image[y+1][x-1] >= center) << 1;
    if (x > 0)                       lbp |= (image[y][x-1]   >= center) << 0;

    return lbp;
}

void computeLBPHistogram(unsigned char** lbpImage, int width, int height, int* histogram) {
    // Initialize the histogram array (256 bins for 8-bit LBP values)
    for (int i = 0; i < 256; i++) {
        histogram[i] = 0;
    }

    // Count occurrences of each LBP value
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            unsigned char lbpValue = lbpImage[y][x];
            histogram[lbpValue]++;
        }
    }
}

void applyLBP(unsigned char** image, unsigned char** lbpImage, int width, int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            lbpImage[y][x] = computeLBP(image, x, y, width, height);
        }
    }
}

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
    FILE* pgmfile = fopen(filename, "rb");
 
    // If file does not exist,
    // then return
    if (pgmfile == NULL) {
        printf("File does not exist\n");
        return false;
    }
 
    ignoreComments(pgmfile);
    fscanf(pgmfile, "%s", pgm->pgmType);
 
    // Check for correct PGM Binary
    // file type
    if (strcmp(pgm->pgmType, "P5")) {
        fprintf(stderr, "Wrong file type!\n");
        exit(EXIT_FAILURE);
    }
 
    ignoreComments(pgmfile);
 
    // Read the image dimensions
    fscanf(pgmfile, "%d %d", &(pgm->width), &(pgm->height));
 
    ignoreComments(pgmfile);
 
    // Read maximum gray value
    fscanf(pgmfile, "%d", &(pgm->maxValue));
    ignoreComments(pgmfile);
 
    // Allocating memory to store
    // img info in defined struct
    pgm->data = malloc(pgm->height * sizeof(unsigned char*));
 
    // Storing the pixel info in
    // the struct
    if (pgm->pgmType[1] == '5') {
 
        fgetc(pgmfile);
 
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
            fread(pgm->data[i], sizeof(unsigned char), pgm->width, pgmfile);
        }
    }
 
    // Close the file
    fclose(pgmfile);
 
    return true;
}

unsigned char** allocateImage(int width, int height) {
    unsigned char** image = (unsigned char**)malloc(height * sizeof(unsigned char*));
    for (int i = 0; i < height; i++) {
        image[i] = (unsigned char*)malloc(width * sizeof(unsigned char));
    }
    return image;
}

void freeImage(unsigned char** image, int height) {
    for (int i = 0; i < height; i++) {
        free(image[i]);
    }
    free(image);
}

int save_to_file(char *file_name, PGMImage *base_image, char **lbp){
    FILE* file = fopen(file_name, "w");
    if (file == NULL) {
        printf("Error opening file for writing!\n");
        
        return 1;
    }

    fprintf(file, "%s \n", base_image->pgmType);
    fprintf(file, "%d %d\n", base_image->width, base_image->height);
    fprintf(file, "%d \n", base_image->maxValue);

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

double euclidian_distance(int vector_a[256], int vector_b[256]){
    double distance = 0;

    for(int i = 0; i < 256; i++){
        distance += pow(vector_a[i] - vector_b[i], 2);
    }

    return sqrt(distance);
}

// Function to check if a file has the desired extension
int has_extension(const char *filename, const char *extension) {
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) return 0; // No extension found
    return strcmp(dot + 1, extension) == 0;
}

// Function to search files with a specific extension
void search_files_with_extension(const char *directory) {
    struct dirent *entry;
    DIR *dp = opendir(directory);

    if (dp == NULL) {
        perror("Unable to open directory");
        return;
    }

    while ((entry = readdir(dp))) {
        // Skip current and parent directory entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        if (has_extension(entry->d_name, "lbp")) {

            //Compare euclidian distance between lbp file and reference image
            printf("Found file: %s\n", entry->d_name);
        }
    }

    closedir(dp);
}
   
int main(int argc, char **argv){
    search_files_with_extension("./base");

    // char next_option;
    // char *reference_image, *output_image_name, *reference_base_path;
    
    // while((next_option = getopt(argc, argv, "i:d::o::")) != -1){
    //     switch(next_option){
    //         case 'i':
    //             reference_image = optarg;
    //             break;
    //         case 'o':
    //             output_image_name = optarg;
    //             break;
    //         case 'd':
    //             reference_base_path = optarg;
    //             break;
    //         default:
    //             perror("%Argumentos: -i [Imagem a ser analisada] -d [Diretorio de imagens base] -o [Nome da imagem LBP gerada]");
    //             return 1;
    //     }
    // }

    // PGMImage* pgm = malloc(sizeof(PGMImage));
    
    // bool opened = openPGM(pgm, reference_image);

    // if(opened){
    //     unsigned char** lbpImage = allocateImage(pgm->width, pgm->height);

    //     applyLBP(pgm->data, lbpImage, pgm->width, pgm->height);
        
    //     if(output_image_name != NULL){
            
    //         if(save_to_file(output_image_name, pgm, lbpImage) != 1){
    //             return 1;
    //         }
    //     }

        
    //     // Inserir aqui o algoritmo de medir distancia euclidiana
    //     // Percorrer toda a base, achar todos os files .lbp
    //     // Medir distancia euclidiana e mostrar a menor distancia
    
    //     int hist[256];
    //     computeLBPHistogram(lbpImage, pgm->width, pgm->height, hist);
        
    //     // Write the histogram to the file
    //     FILE* histogram_file = fopen(strcat(reference_image, ".lbp"), "w");
        
    //     for (int i = 0; i < 256; i++) {
    //         fprintf(histogram_file, "%d\n", hist[i]);
    //     }

        

    //     freeImage(lbpImage, pgm->height);
    //     freeImage(pgm->data, pgm->height);
    // }
    
    return 0;
}
