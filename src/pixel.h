#include <vector>


#ifndef PIXEL
struct pixel{
  int rgb[3];
};

inline pixel add(const pixel& a, const pixel& b){
  pixel c;
  
  c.rgb[0] = a.rgb[0]+b.rgb[0];
  c.rgb[1] = a.rgb[1]+b.rgb[1];
  c.rgb[2] = a.rgb[2]+b.rgb[2];

  return c;
}

inline pixel diff(const pixel& a, const pixel& b){
  pixel c;
  
  c.rgb[0] = a.rgb[0]-b.rgb[0];
  c.rgb[1] = a.rgb[1]-b.rgb[1];
  c.rgb[2] = a.rgb[2]-b.rgb[2];

  return c;
}

inline pixel mult(float k, const pixel& a){
  pixel c;
  
  c.rgb[0] = k * a.rgb[0];
  c.rgb[1] = k * a.rgb[1];
  c.rgb[2] = k * a.rgb[2];

  return c;
}


typedef std::vector< std::vector<pixel> > image_matrix;
#define PIXEL
#endif
