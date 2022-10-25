//project.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ppm_io.h"
#include "image_manip.h"
#include <ctype.h>
#include <assert.h>

// Return (exit) codes
#define RC_SUCCESS            0
#define RC_MISSING_FILENAME   1
#define RC_OPEN_FAILED        2
#define RC_INVALID_PPM        3
#define RC_INVALID_OPERATION  4
#define RC_INVALID_OP_ARGS    5
#define RC_OP_ARGS_RANGE_ERR  6
#define RC_WRITE_FAILED       7
#define RC_UNSPECIFIED_ERR    8

void print_usage();


int main(int argc, char* argv[]) {

  // less than 2 command line args means that input or output filename
  // wasn't specified
  if (argc < 4) {
    fprintf(stderr, "Missing mandatory arguments\n");
    print_usage();
    return RC_MISSING_FILENAME;
  }
  

  FILE *input = fopen(argv[1],"rb");
  if (input == NULL) {
    fprintf(stderr, "ERROR: could not open input file\n");
    fclose(input);
    return RC_OPEN_FAILED;
  }

  FILE *output = fopen(argv[2], "wb");
  if (output == NULL) {
    fprintf(stderr, "ERROR: could not write on output file\n");
    fclose(input);
    fclose(output);
    return RC_OPEN_FAILED;
  }

  Image * myImage = read_ppm(input);
  if (myImage == NULL) {
    fprintf(stderr, "ERROR: could not read input file as a PPM file\n");
    fclose(input);
    fclose(output);
    free_image(&myImage);
    return RC_INVALID_PPM;
  }

  Image * myImage_out = NULL;

  float x;
  float y;
  float strength = -1;
  if (strcmp(argv[3], "swap") == 0) {
    if (argc != 4) {
      fprintf(stderr, "Incorrect number of arguments for the specified operation.\n");
      fclose(input);
      fclose(output);
      free_image(&myImage);
      free(myImage_out);
      return RC_INVALID_OP_ARGS;
    }
    myImage_out = swap(myImage, myImage_out);
  } 
  else if (strcmp(argv[3], "invert") == 0) {
    if (argc != 4) {
      fprintf(stderr, "Incorrect number of arguments for the specified operation.\n");
      fclose(input);
      fclose(output);
      free_image(&myImage);
      free(myImage_out);
      return RC_INVALID_OP_ARGS;
    }
    myImage_out = invert(myImage, myImage_out);
  } 
  else if (strcmp(argv[3], "zoom-out") == 0) {
    if (argc != 4) {
      fprintf(stderr, "Incorrect number of arguments for the specified operation.\n");
      fclose(input);
      fclose(output);
      free_image(&myImage);
      free(myImage_out);
      return RC_INVALID_OP_ARGS;
    }
    myImage_out = zoom_out(myImage, myImage_out);
  }
  else if (strcmp(argv[3], "rotate-right") == 0) {
    if (argc != 4) {
      fprintf(stderr, "Incorrect number of arguments for the specified operation.\n");
      fclose(input);
      fclose(output);
      free_image(&myImage);
      free(myImage_out);
      return RC_INVALID_OP_ARGS;
    }
    myImage_out = rotate_right(myImage, myImage_out);
  }
  else if (strcmp(argv[3], "swirl") == 0) {
    if (argc != 7) {
      fprintf(stderr, "Incorrect number of arguments for the specified operation.\n");
      fclose(input);
      fclose(output);
      free_image(&myImage);
      free(myImage_out);
      return RC_INVALID_OP_ARGS;
    }

    x = atof(argv[4]);
    y = atof(argv[5]);
    strength = atoi(argv[6]);
    if (((!x || !y) && ((strcmp(argv[4], "0") != 0) || (strcmp(argv[5], "0") != 0))) || strength < 1) {
      fprintf(stderr, "Invalid arguments.\n");
      fclose(input);
      fclose(output);
      free_image(&myImage);
      free(myImage_out);
      return RC_OP_ARGS_RANGE_ERR;
    }
    
    myImage_out = swirl(myImage, myImage_out, x, y, strength);
  }
  else if (strcmp(argv[3], "edge-detection") == 0) {
    if (argc != 5) {
      fprintf(stderr, "Incorrect number of arguments for the specified operation.\n");
      fclose(input);
      fclose(output);
      free_image(&myImage);
      free(myImage_out);
      return RC_INVALID_OP_ARGS;
    }
    
    double arg4 = atof(argv[4]);
    if (((!arg4) && ((strcmp(argv[4], "0") != 0))) || arg4 < 0) {
      fprintf(stderr, "Invalid arguments.\n");
      fclose(input);
      fclose(output);
      free_image(&myImage);
      free(myImage_out);
      return RC_OP_ARGS_RANGE_ERR;
    }

    myImage_out = edge_detection(myImage, myImage_out, arg4);
  } else {
    fprintf(stderr, "ERROR: Unsupported image processing operations.\n");
    fclose(input);
    fclose(output);
    free_image(&myImage);
    free(myImage_out);
    return RC_INVALID_OPERATION;
  }

  if (myImage_out == NULL) {
    fprintf(stderr, "ERROR: output file is not correctly written.\n");
    fclose(input);
    fclose(output);
    free_image(&myImage);
    free(myImage_out);
    return RC_WRITE_FAILED;
  }
  
  
  int numPixels = write_ppm(output, myImage_out);

  //check to see if fwrite failed
  if (numPixels == -1) {
    fprintf(stderr, "ERROR: output file is not correctly written.\n");
    fclose(input);
    fclose(output);
    free_image(&myImage);
    free_image(&myImage_out);
    return RC_WRITE_FAILED;
  } 

  free_image(&myImage);
  free_image(&myImage_out);


  fclose(input);
  fclose(output);

  return RC_SUCCESS;
}

void print_usage() {
  printf("USAGE: ./project <input-image> <output-image> <command-name> <command-args>\n");
  printf("SUPPORTED COMMANDS:\n");
  printf("   swap\n");
  printf("   invert\n");
  printf("   zoom-out\n");
  printf("   rotate-right\n");
  printf("   swirl <cx> <cy> <strength>\n");
  printf("   edge-detection <threshold>\n");
}
