#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>

#include <ncurses.h>

#include "image.h"

#ifndef PIXEL
#include "pixel.h"
#define PIXEL
#endif 

#define ZOOM_IN 0.9
#define ZOOM_OUT 1./ZOOM_IN


