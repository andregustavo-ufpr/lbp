#ifndef LBP_H
#define LBP_H

// How many lines the histogram has
#define LINES_HISTOGRAM 256

// Function that calculates the LBP algorithm for one pixel 
// from the image "image", that has height "height" and width "width"
// located in the position "x" and "y"
unsigned char computeLBP(unsigned char** image, int x, int y, int width, int height);

// Function that, given the lbp image "lbp_image", of dimensions "height" and "width"
// calculates the histogram
void computeLBPHistogram(unsigned char** lbp_image, int width, int height, int* histogram);

// Function that loops through "image", get the LBP result and saves to "lbp_image"
void applyLBP(unsigned char** image, unsigned char** lbp_image, int width, int height);

#endif