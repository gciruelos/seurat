#include <vector>

#ifndef PIXEL
struct pixel {
  int rgb[3];
};

typedef std::vector< std::vector<pixel> > image_matrix;
#define PIXEL
#endif
