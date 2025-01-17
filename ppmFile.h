/****************************************************************
 *
 * ppm.h
 *
 * Read and write PPM files.  Only works for "raw" format.
 *
 * AF970205
 *
 ****************************************************************/

#ifndef PPM_H
#define PPM_H

#include <sys/types.h>

typedef struct Image
{
  int width;
  int height;
  unsigned char *data;
} Image;

Image *ImageCreate(int width, int height);
Image *ImageRead(char *filename);
void   ImageWrite(Image *image, char *filename);

int    ImageWidth(Image *image);
int    ImageHeight(Image *image);

void   ImageClear(Image *image, unsigned char red, unsigned char green, unsigned char blue);
void   ImageSetPixelA(Image *image, int x, int y, unsigned char R, unsigned char G, unsigned char B);
void   ImageSetPixel(Image *image, int x, int y, int chan, unsigned char val);
unsigned char ImageGetPixel(Image *image, int x, int y, int chan);

#endif /* PPM_H */

