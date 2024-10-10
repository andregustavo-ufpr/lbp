#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <dirent.h>

#define LINES_HISTOGRAM 256

typedef struct PGMImage {
    char pgmType[3];
    unsigned char** data;
    unsigned int width;
    unsigned int height;
    unsigned int maxValue;
} PGMImage;

// Function to compute the LBP value for a pixel
unsigned char computeLBP(unsigned char** image, int x, int y, int width, int height) {
    unsigned char center = image[y][x];  // Center pixel
    unsigned char lbp = 0;               // LBP value
    int bitPos = 0;                      // Bit position to set

    if(y == 0 || x == 0 || x == width -1 || y == height - 1){
        return lbp;
    }
    
    // Iterate over 3x3 neighborhood
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue;  // Skip center pixel
            
            // Get neighboring pixel value
            unsigned char neighbor = image[y + i][x + j];

            // Shift bit position and set LBP bit
            lbp |= (neighbor >= center) << bitPos;  // Set bit if neighbor >= center
            
            bitPos++;  // Move to the next bit
        }
    }
    
    return lbp;
}

void computeLBPHistogram(unsigned char** lbpImage, int width, int height, int* histogram) {
    // Initialize the histogram array (256 bins for 8-bit LBP values)
    for (int i = 0; i < LINES_HISTOGRAM; i++) {
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

double squaredEuclidianDistance(int vector_a[LINES_HISTOGRAM], int vector_b[LINES_HISTOGRAM]){
    double distance = 0;

    for(int i = 0; i < LINES_HISTOGRAM; i++){
        distance += (vector_a[i] - vector_b[i])*(vector_a[i] - vector_b[i]);
    }

    return distance;
}

// Function to check if a file has the desired extension
int hasExtension(const char *filename, const char *extension) {
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) return 0; // No extension found
    return strcmp(dot + 1, extension) == 0;
}

// Function to search files with a specific extension
void searchFilesInDirectoryAndCompare(const char *directory, int reference_histogram[LINES_HISTOGRAM]) {
    struct dirent *entry;
    DIR *dp = opendir(directory);
    double min_euclidian_dist = 9999999999999;
    char min_file_name[100];

    if (dp == NULL) {
        perror("Unable to open directory");
        return;
    }

    while ((entry = readdir(dp))) {
        // Skip current and parent directory entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        if (hasExtension(entry->d_name, "lbp")) {
            int i = 0;
            char line_buffer[100];
            int hist_values[LINES_HISTOGRAM];
            double euclidian_dist = 0;

            FILE *file = fopen(entry->d_name, "r");

            while (i < LINES_HISTOGRAM && fgets(line_buffer, sizeof(line_buffer), file) != NULL) {
                // Convert the line to a float (or int) and store it in the array
                hist_values[i] = atoi(line_buffer); 
                i++;
            }

            fclose(file);

            euclidian_dist = squaredEuclidianDistance(hist_values, reference_histogram);

            if(euclidian_dist <= min_euclidian_dist){
                min_euclidian_dist = euclidian_dist;
                strcpy(min_file_name, entry->d_name);
            }
        }
    }

    closedir(dp);
    printf("Imagem mais similar: %s %.6f", min_file_name, min_euclidian_dist);
}

char *removeExtension(char* myStr) {
    char *retStr;
    char *lastExt;
    if (myStr == NULL) return NULL;
    if ((retStr = malloc (strlen (myStr) + 1)) == NULL) return NULL;
    strcpy (retStr, myStr);
    lastExt = strrchr (retStr, '.');
    if (lastExt != NULL)
        *lastExt = '\0';
    return retStr;
}

void saveHistogramFile(char *file_name, int *histogram){

    FILE* histogram_file = fopen(strcat(removeExtension(file_name), ".lbp"), "w");
    
    for (int i = 0; i < LINES_HISTOGRAM; i++) {
        fprintf(histogram_file, "%d\n", histogram[i]);
    }

    fclose(histogram_file);
}
   
int main(int argc, char **argv){
    char next_option;
    char *reference_image, *output_image_name, *reference_base_path;
    
    while((next_option = getopt(argc, argv, "i:d::o::")) != -1){
        switch(next_option){
            case 'i':
                reference_image = optarg;
                break;
            case 'o':
                output_image_name = optarg;
                break;
            case 'd':
                reference_base_path = optarg;
                break;
            default:
                perror("%Argumentos: -i [Imagem a ser analisada] -d [Diretorio de imagens base] -o [Nome da imagem LBP gerada]");
                return 1;
        }
    }

    PGMImage* pgm = malloc(sizeof(PGMImage));
    
    bool opened = openPGM(pgm, reference_image);

    if(opened){
        unsigned char** lbpImage = allocateImage(pgm->width, pgm->height);
        int histogram[LINES_HISTOGRAM];

        applyLBP(pgm->data, lbpImage, pgm->width, pgm->height);
        computeLBPHistogram(lbpImage, pgm->width, pgm->height, histogram);
        
        if(output_image_name != NULL){
            saveToFile(output_image_name, pgm, lbpImage);
            saveHistogramFile(reference_image, histogram);
        }
        else if(reference_base_path != NULL){
            searchFilesInDirectoryAndCompare(reference_base_path, histogram);
            saveHistogramFile(reference_image, histogram);
        }
        
        

        freeImage(lbpImage, pgm->height);
        freeImage(pgm->data, pgm->height);
    }
    
    return 0;
}
