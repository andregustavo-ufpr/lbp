#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

FILE* open_file(char* filepath){
    FILE *file = fopen(filepath, "r");

    if(file == NULL){
        printf("Failed to open file %s", filepath);
        return NULL;
    }

    return file;
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
    
    FILE* file_to_analize = open_file(reference_image);

    if(file_to_analize != NULL){
        while (!feof(file_to_analize))
        {
            char buffer[40];
            fgets(buffer, 40, file_to_analize);

            printf("%s", buffer);
        }
        
        
    }
    
    fclose(file_to_analize);
    return 0;
}
