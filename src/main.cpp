#include "main.h"

#define BLACK 0
#define DARK_BLUE 1
#define DARK_GREEN 2
#define DARK_CYAN 3
#define DARK_RED 4
#define DARK_PURPLE 5
#define DARK_YELLOW 6
#define LIGHT_GREY 7
#define DARK_GREY 8
#define LIGHT_BLUE 9
#define LIGHT_GREEN 10
#define LIGHT_CYAN 11
#define LIGHT_RED 12
#define LIGHT_PURPLE 13
#define LIGHT_YELLOW 14
#define WHITE 15


int rgbto256(pixel p) {
  int r = p.rgb[0];
  int g = p.rgb[1];
  int b = p.rgb[2];

  int code256 = (int) (36 * ((r/255.0) * 5) + 6 * ((g/255.0) * 5) + ((b/255.0) * 5) + 16);
  return code256;

}

int rgbto8(pixel p) {
  int r = p.rgb[0];
  int g = p.rgb[1];
  int b = p.rgb[2];

  if (r < 128) {
    if(g < 128) {
      if(b < 128) return 0; //black
      else        return 4; //blue
    } else {
      if(b < 128) return 2; //green
      else        return 6; //cyan
    }
  } else {
    if(g < 128) {
      if(b < 128) return 1; //red
      else        return 5; //magenta
    } else {
      if(b < 128) return 3; //yellow
      else        return 7; //white
    }
  }
}



int rgbto16(pixel p) {
  int r = p.rgb[0];
  int g = p.rgb[1];
  int b = p.rgb[2];

  if (r < 128) {
    if(g < 128) {
      if(b < 128) return 0; //black
      else        return 4; //blue
    } else {
      if(b < 128) return 2; //green
      else        return 6; //cyan
    }
  } else {
    if(g < 128) {
      if(b < 128) return 1; //red
      else        return 5; //magenta
    } else {
      if(b < 128) return 3; //yellow
      else        return 7; //white
    }
  }
}


bool curses_started = false;

void end_curses() {
  if (curses_started && !isendwin())
    endwin();
}

void start_curses() {
  if (curses_started) {
    refresh();
  }
  else {  
    initscr();
    start_color(); 
    cbreak();
    noecho();
    intrflush(stdscr, false);
    keypad(stdscr, true);
    atexit(end_curses);
    curses_started = true;
    }
}

//void floyd_steinberg_dithering(image_matrix& m)

void print_repr_ncurses (const image_matrix& m, int colors) {
  int row = m.size(), col = m[0].size();

  for(int i = 0; i<row; i++) {
    for(int j = 0; j<col; j++) {
      pixel p = m[i][j];
      if(colors == 256) {
        int code256 = rgbto256(p);
        init_color(code256, p.rgb[0], p.rgb[1], p.rgb[2]);

        attron(COLOR_PAIR(code256));
        addstr("  ");
        attroff(COLOR_PAIR(code256)); 
      }
      else if(colors == 8) {
        int code8 = rgbto8(p);
       
        init_pair(0, COLOR_WHITE, COLOR_BLACK); init_pair(1, COLOR_WHITE, COLOR_RED); 
        init_pair(2, COLOR_WHITE, COLOR_GREEN); init_pair(3, COLOR_WHITE, COLOR_YELLOW); 
        init_pair(4, COLOR_WHITE, COLOR_BLUE);  init_pair(5, COLOR_WHITE, COLOR_MAGENTA);
        init_pair(6, COLOR_WHITE, COLOR_CYAN);  init_pair(7, COLOR_WHITE, COLOR_WHITE);
        
        attron(COLOR_PAIR(code8));
        addstr("  ");
        attroff(COLOR_PAIR(code8));
      }
    }
  }
}

int main(int argc, char* argv[]) {
  if(argc == 1) {
    std::cout << "needs moar commands" << std::endl;
    return 1;
  }
  Image img(argv[argc-1]);

  start_curses();
  
  std::ofstream logs;
  logs.open ("logs.txt");
  logs << "Logs...\n";

  int colors = 8;

  int width = img.width();
  int height = img.height();

  int x_i = 0, delta_x = width;
  int y_i = 0, delta_y = height;

  while (true) {
    clear();
    int row,col;
    getmaxyx(stdscr,row,col); 
    
    image_matrix img_repr = img.generate_representation(col/2, row, x_i, delta_x, y_i, delta_y);
    print_repr_ncurses(img_repr, 8);
    
    refresh();
    int ch = getch();
    if (ch == ERR) continue;
    else if(ch == 'q') break;
    else if((ch == 'Z' || ch == '+') && delta_x * ZOOM_IN > 10 && delta_y * ZOOM_IN > 10){
      x_i += ((1-ZOOM_IN) * delta_x)/2; // This is because I want to mantain the middle,
      y_i += ((1-ZOOM_IN) * delta_y)/2; // so if     m_x = (x_i + delta_x)/2,    then
                                        // m_x = (x_i + 0.1 * delta_x + 0.9 * delta_x)/2 
      delta_x *= ZOOM_IN;
      delta_y *= ZOOM_IN;
    }
    else if(ch == 'z' || ch=='-') {
      if(delta_x * ZOOM_OUT >= width || delta_y * ZOOM_OUT >= height){
        delta_x = width; delta_y = height; x_i = 0; y_i = 0;
      } else{
        x_i += ((1-ZOOM_OUT) * delta_x)/2;
        y_i += ((1-ZOOM_OUT) * delta_y)/2;

        delta_x *= ZOOM_OUT;
        delta_y *= ZOOM_OUT;
      }
    }
    else if(ch == KEY_RIGHT) x_i+=delta_x*0.1;
    else if(ch == KEY_LEFT) x_i-=delta_x*0.1;
    else if(ch == KEY_UP) y_i-=delta_y*0.1;
    else if(ch == KEY_DOWN) y_i+=delta_y*0.1;

    // various corrections
    if (x_i < 0) x_i = 0;
    if (y_i < 0) y_i = 0;
    if (x_i + delta_x > width)  x_i = width - delta_x;
    if (y_i + delta_y > height) y_i = height - delta_y;



    logs << "delta_x -> " << delta_x << "\tdelta_y -> " << delta_y << "\tx_i -> " << x_i << "\ty_i -> " << y_i << "\n";
  }

  logs << "Closing...\n";
  logs.close();
  end_curses();
}

