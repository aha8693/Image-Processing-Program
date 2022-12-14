
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "ppm_io.h"



/* helper function for read_ppm, takes a filehandle
 * and reads a number, but detects and skips comment lines
 */
int read_num(FILE *fp) {
  assert(fp);

  int ch;
  while((ch = fgetc(fp)) == '#') { // # marks a comment line
    while( ((ch = fgetc(fp)) != '\n') && ch != EOF ) {
      /* discard characters til end of line */
    }
  }
  ungetc(ch, fp); // put back the last thing we found

  int val;
  if (fscanf(fp, "%d", &val) == 1) { // try to get an int
    while(isspace(ch = fgetc(fp))) {
      // drop trailing whitespace
    }
    ungetc(ch, fp);
    return val; // we got a value, so return it
  } else {
    fprintf(stderr, "Error:ppm_io - failed to read number from file\n");
    return -1;
  }
}



Image * read_ppm(FILE *fp) {

  /* confirm that we received a good file handle */
  assert(fp != NULL);

  /* allocate image (but not space to hold pixels -- yet) */
  Image *im = malloc(sizeof(Image));
  if (!im) {
    fprintf(stderr, "Error:ppm_io - failed to allocate memory for image!\n");
    return NULL;
  }

  /* initialize fields to error codes, in case we have to bail out early */
  im->rows = im->cols = -1;

  /* read in tag; fail if not P6 */
  char tag[20];
  tag[19]='\0';
  fscanf(fp, "%19s\n", tag);
  if (strncmp(tag, "P6", 20)) {
    fprintf(stderr, "Error:ppm_io - not a PPM (bad tag)\n");
    free(im);
    return NULL;
  }


  /* read image dimensions */
 
  //read in columns
  im->cols = read_num(fp); // NOTE: cols, then rows (i.e. X size followed by Y size)
  //read in rows
  im->rows = read_num(fp);

  //read in colors; fail if not 255
  int colors = read_num(fp);
  if (colors != 255) {
    fprintf(stderr, "Error:ppm_io - PPM file with colors different from 255\n");
    free(im);
    return NULL;
  }

  //confirm that dimensions are positive
  if (im->cols <= 0 || im->rows <= 0) {
    fprintf(stderr, "Error:ppm_io - PPM file with non-positive dimensions\n");
    free(im);
    return NULL;
  }

  /* finally, read in Pixels */

  /* allocate the right amount of space for the Pixels */
  im->data = malloc(sizeof(Pixel) * (im->rows) * (im->cols));

  if (!im->data) {
    fprintf(stderr, "Error:ppm_io - failed to allocate memory for image pixels!\n");
    free(im);
    return NULL;
  }

  /* read in the binary Pixel data */
  if (fread(im->data, sizeof(Pixel), (im->rows) * (im->cols), fp) != 
      (size_t)((im->rows) * (im->cols))) {
    fprintf(stderr, "Error:ppm_io - failed to read data from file!\n");
    free(im);
    return NULL;
  }

  //return the image struct pointer
  return im;
}



/* Write given image to disk as a PPM.
 * Return -1 if any failure occurs, otherwise return the number of pixels written.
 */
int write_ppm(FILE *fp, const Image *im) {
  if (!fp) printf("fp\n");
  if(!im) printf("im\n");
  
  //check to make sure file pointer and image pointer are passed correctly
  if (!fp || !im) {
    fprintf(stderr, "Error:write_ppm!\n");
    return -1;
  }
  
  //write necessary values to file pointer pointing to .ppm file
  fprintf(fp, "P6\n");
  fprintf(fp, "%d %d\n", im->cols, im->rows);
  fprintf(fp, "%d\n", 255);
  
  //write image to file
  size_t numPixels = fwrite(im->data, sizeof(Pixel), im->rows * im->cols, fp);

  //check to see if fwrite failed
  if (numPixels != (size_t)(im->rows * im->cols)) {
    fprintf(stderr, "Error:ppm_write - fwrite failed to write data properly!\n");
    return -1;
  }
  //return number of pixels of image
  return numPixels; 
}



/* allocate a new image of the specified size;
 * doesn't initialize pixel values */
Image * make_image (int rows, int cols) {
  // allocate space
  Image *im = malloc(sizeof(Image));
  if (!im) {
    return NULL;
  }
  // set size 
  im->rows = rows;
  im->cols = cols;

  // allocate pixel array
  im->data = malloc((im->rows * im->cols) * sizeof(Pixel));
  if (!im->data) {
    free(im);
    return NULL;
  }
  return im;
}



/* output dimensions of the image to stdout */
void output_dims(Image *im) {
  printf("cols = %d, rows = %d", im->cols, im->rows);
}



/* free_image - utility function to free inner and outer pointers, 
 * and set to null 
 */
void free_image(Image **im) {
  //deallocate data of image
  free((*im)->data);

  //deallocate pointer to image
  free(*im);
}



/* resize_image - utility function to reallocate an image to the specified size;
 * doesn't initialize pixel values 
 * return -1 if error, otherwise 0
 */
int resize_image(Image **im, int rows, int cols) {
  //reallocate image to new number or rows and cols of pixels
  (*im)->data = realloc((*im)->data, rows * cols * sizeof(Pixel));

  //if realloc fails return -1
  if (!((*im)->data)) {
    fprintf(stderr, "Error:ppm_io - failed to reallocate the image!\n");
    return -1;
  }

  //return 0 if realloc succeeds
  return 0;
}

/* copy_image
 * utility function to  allocate and fill a new image to be 
 * a copy of the image given as a parameter 
 */
Image* make_copy (Image *orig) {
  // allocate space
  Image *copy = make_image(orig->rows, orig->cols);

  // if we got space, copy pixel values
  if (copy) {
    memcpy(copy->data, orig->data, (copy->rows * copy->cols) * sizeof(Pixel));
  }

  return copy;
}

