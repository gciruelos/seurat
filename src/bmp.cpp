#include "bmp.h"



void read_from_bmp(char* filename, std::vector< std::vector<pixel> >& m  ) {
  //http://stackoverflow.com/questions/9296059/read-pixel-value-in-bmp-file

  FILE* f = fopen(filename, "rb");

  unsigned char info[54];
  fread(info, sizeof(unsigned char), 54, f);

  int width = *(int*)&info[18];
  int height = *(int*)&info[22];

  int row_pad = (width*3 + 3) & (~3);
  unsigned char* data = new unsigned char[row_pad];

  for(int i = 0; i < height; i++) {
    std::vector<pixel> row;
    fread(data, sizeof(unsigned char), row_pad, f);
    for(int j = 0; j < width*3; j += 3) {
      pixel p;
      p.rgb[0] = data[j+2];
      p.rgb[1] = data[j+1];
      p.rgb[2] = data[j];

      row.push_back(p);
    }
    m.insert(m.begin(), row);
  }
}

