#include "head_tracker.h"
#include <math.h>
#define M_PI 3.14159265358979323846

/*
  Just a note,
  Certain numbers represent errors where the depth can't be determined.
  These numbers will almost certainly exist in both the `background` and `current` arrays.
  too near	0x0000
  too far	0x0FFF
  unknown	0x1FFF
*/
// Error numbers for convienience ;)
#define TOO_NEAR 0x0000
#define TOO_FAR 0x0FFF
#define UNKNOWN 0x1FFF
float rotation = 0;
void locate_head(const uint16_t *background, const uint16_t *current, int width, int height, int *x, int *y, uint16_t *z){

  *x=320+320*cos(rotation);
  *y=240+240*sin(rotation);
  rotation += .005;
  if (rotation > 2 * M_PI) {
    rotation -= 2*M_PI;
  }
  *z = current[*x + *y*width];
}
