#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "image_manip.h"
#include "ppm_io.h"
#include <stdbool.h>

/* HELPER for grayscale:
 * convert a RGB pixel to a single grayscale intensity;
 * uses NTSC standard conversion
 */
unsigned char pixel_to_gray(const Pixel *p) {
  return (unsigned char)( (0.3 * (double)p->r) +
                          (0.59 * (double)p->g) +
                          (0.11 * (double)p->b) );
}

/* ______grayscale______
 * convert an image to grayscale (NOTE: pixels are still
 * RGB, but the three values will be equal)
 */
void grayscale(Image *im) {
  if (!im || !im->data) {
    fprintf(stderr, "Error:image_manip - grayscale given a bad image pointer\n");
    return;
  }

  for (int r=0; r<im->rows; r++){
    for (int c=0; c<im->cols; c++){
      // find grayscale intensity
      unsigned char grayLevel = pixel_to_gray(&(im->data[(r*im->cols)+c]));

      // set all chanels to it
      im->data[(r*im->cols)+c].r = grayLevel;
      im->data[(r*im->cols)+c].g = grayLevel;
      im->data[(r*im->cols)+c].b = grayLevel;

    }
  }
}

/* ______swap______
 * swap color channels of an image
 */

 Image* swap(Image *im, Image *im_out) {
  im_out = make_copy(im);

  for (int r=0; r<im->rows; r++){
    for (int c=0; c<im->cols; c++){
      // find grayscale intensity
      unsigned char temp = im->data[(r*im->cols)+c].r;

      // set all chanels to it
      im_out->data[(r*im->cols)+c].r = im_out->data[(r*im->cols)+c].g;
      im_out->data[(r*im->cols)+c].g = im_out->data[(r*im->cols)+c].b;
      im_out->data[(r*im->cols)+c].b = temp;

    }
  }
  return im_out;

 }

/* ______invert______
 * invert the intensity of each channel
 */

 Image* invert(Image *im, Image *im_out) {
  im_out = make_image(im->rows, im->cols); 

  for (int r=0; r<im->rows; r++){
    for (int c=0; c<im->cols; c++){

      // set all chanels to it
      im_out->data[(r*im->cols)+c].r = (unsigned char) 255 - (im->data[(r*im->cols)+c].r);
      im_out->data[(r*im->cols)+c].g = (unsigned char) 255 - (im->data[(r*im->cols)+c].g);
      im_out->data[(r*im->cols)+c].b = (unsigned char) 255 - (im->data[(r*im->cols)+c].b);

    }
  }
  return im_out;
 }


/* ______zoom_out______
 * "zoom out" an image, by taking a 2x2 square of pixels and averaging
 * each of the three color channels to make a single pixel. If an odd
 * number of rows in original image, we lose info about the bottom row.
 * If an odd number of columns in original image, we lose info about the
 * rightmost column.
 */

 Image* zoom_out(Image *im, Image *im_out) {
  im_out = make_image((im->rows)/2, (im->cols)/2);
  int sum_r = 0;
  int sum_g = 0;
  int sum_b = 0;
  int index = 0;
  for (int i = 0; i < im->rows; i += 2) {
    if (i+1 > im->rows) {
        break;
      }
    for (int j=0; j < im->cols; j += 2) {
      if (j+1 > im->cols) {
        break;
      }
      
      sum_r = 0;
      sum_g = 0;
      sum_b = 0;

      for (int x=0; x < 2; x++) {
        sum_r += im->data[(im->cols*i)+j+x].r;
        sum_r += im->data[(im->cols*(i+1))+j+x].r;

        sum_g += im->data[(im->cols*i)+j+x].g;
        sum_g += im->data[(im->cols*(i+1))+j+x].g;

        sum_b += im->data[(im->cols*i)+j+x].b;
        sum_b += im->data[(im->cols*(i+1))+j+x].b;
      }
      im_out->data[index].r = sum_r /= 4;
      im_out->data[index].g = sum_g /= 4;
      im_out->data[index].b = sum_b /= 4;
      index++;
    }
  }
  return im_out;
      
  
 }

/* _______rotate-right________
 * rotate the input image clockwise 90 degrees
 */

Image* rotate_right(Image *im, Image *im_out) {
    im_out = make_image(im->cols, im->rows);
    int index = 0;
    for (int i = 0; i < im_out->rows; i++) {
      index = 0;
        for (int j=im_out->cols - 1; j > -1; j--) {
            im_out->data[(im_out->cols*i)+j] = im->data[(im_out->rows*index)+i];
            index++;
        }
    }
    return im_out;
    
 }

/* ________Swirl effect_________
 * Create a whirlpool effect!
 */
Image* swirl(Image *im, Image *im_out, double cx, double cy, double s) { 
  // make output image pointer
  im_out = make_image(im->rows,im->cols);

  // check to see it is made correctly
  assert(im_out);

  // if centers are negative set them equal to center location of image for that coordinate
  if (cx < 0) cx = im->cols / 2.;
  if (cy < 0) cy = im->rows / 2.;
  
  // implement swirl effect
  for (int i = 0; i < (*im_out).rows; i++) {
    for (int j = 0; j < (*im_out).cols; j++) {
      double alpha = (sqrt(pow(j - cx, 2) + pow(i - cy, 2))) / s;
      int pixelYCoordinate = (j - cx) * sin(alpha) + (i - cy) * cos(alpha) + cy;
      int pixelXCoordinate = (j - cx) * cos(alpha) - (i - cy) * sin(alpha) + cx;
      if ((int)(((*im_out).cols * pixelYCoordinate) + pixelXCoordinate) < 0) {
        (*im_out).data[((*im_out).cols * i) + j].r = 0;
        (*im_out).data[((*im_out).cols * i) + j].g = 0;
        (*im_out).data[((*im_out).cols * i) + j].b = 0;

      // check if pixelYCoordinate is >=0 and less than rows
      } else if (pixelYCoordinate < 0 || pixelYCoordinate >= (*im_out).rows) {
        // pixelYCoordinate not in range
      }

      // check if pixelXCoordinate is >=0 and less than cols
      else if (pixelXCoordinate < 0 || pixelXCoordinate >= (*im_out).cols) {
        // pixelXCoordinate not in range
      
      // if both coordinates pass tests, set pixel locations to swirled locations
      } else {
        (*im_out).data[((*im_out).cols * i) + j] = (*im).data[(int)(((*im_out).cols * pixelYCoordinate) + pixelXCoordinate)];
      }
    }
  }

  // check if image created properly and if output pointer pointing to that image
  if (!im_out) printf("null\n");

  // return output image pointer
  return im_out;
}


/* _______edges________
 * apply edge detection as a grayscale conversion
 * followed by an intensity gradient computation and
 * thresholding
 */
Image* edge_detection(Image *im, Image *im_out, double threshold) {
  im_out = make_copy(im);
  grayscale(im);
  grayscale(im_out);
  double intensityX = 0.00;
  double intensityY = 0.00;
  double intensityMagnitude = 0.00;
  bool edge = false;

  for (int r = 0; r < im->rows; r++) {
    for (int c = 0; c < im->cols; c++) {
      edge = false;
      if (c+1 == im->cols || (c - 1 < 0)) {
        edge = true;
      } else {
        
        intensityX = (im->data[(r*im->cols)+c+1].r - im->data[(r*im->cols)+c-1].r) / 2.0;
      }

      if (r+1 == im->rows || (r - 1 < 0)) {
        edge = true;
      } else {
        intensityY = (im->data[((r+1)*im->cols)+c].r - im->data[((r-1)*im->cols)+c].r) / 2.0;
      }
      
      if (edge) {
        continue;
      } else {
        intensityMagnitude = sqrt(sq(intensityX) + sq(intensityY));

        if (intensityMagnitude > threshold) {
          im_out->data[(r*im_out->cols)+c].r = 0;
          im_out->data[(r*im_out->cols)+c].g = 0;
          im_out->data[(r*im_out->cols)+c].b = 0;
        }
        else {
          im_out->data[(r*im_out->cols)+c].r = 255;
          im_out->data[(r*im_out->cols)+c].g = 255;
          im_out->data[(r*im_out->cols)+c].b = 255;
        }
      }

      
    }
  }
  return im_out;
}
  
