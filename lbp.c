#include "lbp.h"

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

void computeLBPHistogram(unsigned char** lbp_image, int width, int height, int* histogram) {
    // Initialize the histogram array (256 bins for 8-bit LBP values)
    for (int i = 0; i < LINES_HISTOGRAM; i++) {
        histogram[i] = 0;
    }

    // Count occurrences of each LBP value
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            unsigned char lbp_value = lbp_image[y][x];
            histogram[lbp_value] += 1;
        }
    }
}

void applyLBP(unsigned char** image, unsigned char** lbp_image, int width, int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            lbp_image[y][x] = computeLBP(image, x, y, width, height);
        }
    }
}
