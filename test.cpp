#include "src/stb_image.h"

int main(){
  int x,y,n;

  //FILE * f = fopen("test/Untitled.jpeg", "rb");
  stbi_load("a", &x, &y, &n, 3);

}
