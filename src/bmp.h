#include <vector>
#include <stdio.h>

#ifndef PIXEL
#include "pixel.h"
#define PIXEL
#endif 

void read_from_bmp(const char*, std::vector< std::vector<pixel> >& );
