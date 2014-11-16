#include <vector>


#ifndef PIXEL
struct pixel{
  int r;
  int g;
  int b;
};

typedef std::vector< std::vector<pixel> > image_matrix;
#define PIXEL
#endif
