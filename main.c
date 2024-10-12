#include "lbp.h"
#include "file.h"

#include <math.h>
#include <dirent.h>
#include <getopt.h>

#define SPACE_AS_OPTARG (((!optarg) && (optind < argc) && (argv[optind][0] != '-')) \
    ? (optarg = argv[optind++]): 0)

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
    double min_euclidian_dist = 10000;
    unsigned int saved = 0;
    char min_file_name[100];

    if (dp == NULL) {
        perror("Unable to open directory");
        return;
    }

    chdir(directory);

    while ((entry = readdir(dp))) {
        // Skip current and parent directory entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
            continue;
        }
                
        if (hasExtension(entry->d_name, "lbp")) {
            int i = 0;
            char *line_buffer = malloc(sizeof(char)*100);
            int hist_values[LINES_HISTOGRAM];
            double euclidian_dist = 0;

            FILE *file = fopen(entry->d_name, "r");

            if(file != NULL){
                while(
                    (i < LINES_HISTOGRAM) && 
                    (fgets(line_buffer, sizeof(line_buffer), file) != NULL)
                ) {
                    // Convert the line to a int and store it in the array
                    hist_values[i] = atoi(line_buffer); 
                    i++;
                }

                fclose(file);
                free(line_buffer);

                euclidian_dist = squaredEuclidianDistance(hist_values, reference_histogram);

                if((euclidian_dist <= min_euclidian_dist) || (saved == 0)){
                    saved=1;
                    min_euclidian_dist = euclidian_dist;
                    strcpy(min_file_name, entry->d_name);
                }
            }
        }
    }

    closedir(dp);
    if(saved == 1){
        printf("Imagem mais similar: %s %.6f\n", 
            min_file_name, 
            sqrt(min_euclidian_dist)
        );
        return;
    }

    printf("Não foram encontrados nenhum arquivo com extensão .lbp na pasta de destino");
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
    char *name_without_extension = removeExtension(file_name);

    FILE* histogram_file = fopen(strcat(name_without_extension, ".lbp"), "w");
    
    for (int i = 0; i < LINES_HISTOGRAM; i++) {
        fprintf(histogram_file, "%d\n", histogram[i]);
    }

    fclose(histogram_file);
    free(name_without_extension);
}
   
int main(int argc, char **argv){
    char next_option;
    char *reference_image = NULL, *output_image_name = NULL, *reference_base_path = NULL;
    
    while((next_option = getopt(argc, argv, "i:d::o::")) != -1){
        switch(next_option){
            case 'i':
                reference_image = optarg;
                break;
            case 'o':
                if(!SPACE_AS_OPTARG) exit(1);
                output_image_name = optarg;
                break;
            case 'd':
                if(!SPACE_AS_OPTARG) exit(1);
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
        unsigned char** lbp_image= allocateImage(pgm->width, pgm->height);
        int *histogram= (int *) malloc(sizeof(int)*LINES_HISTOGRAM);

        applyLBP(pgm->data, lbp_image, pgm->width, pgm->height);
        computeLBPHistogram(lbp_image, pgm->width, pgm->height, histogram);
        
        if(output_image_name != NULL){
            saveToFile(output_image_name, pgm, lbp_image);
            saveHistogramFile(output_image_name, histogram);
        }
        else if(reference_base_path != NULL){
            searchFilesInDirectoryAndCompare(reference_base_path, histogram);
        }
        
        free(histogram);
        freeImage(lbp_image, pgm->height);
        freeImage(pgm->data, pgm->height);
        free(pgm);
    }
    
    return 0;
}
