#include "main.h"

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

void print_repr_ncurses (const color_matrix& m, int colors, bool one_more) {
  int row = m.size(), col = m[0].size();

       
  init_pair(0, COLOR_BLACK, COLOR_BLACK); init_pair(1, COLOR_RED, COLOR_BLACK); 
  init_pair(2, COLOR_GREEN, COLOR_BLACK); init_pair(3, COLOR_YELLOW, COLOR_BLACK); 
  init_pair(4, COLOR_BLUE, COLOR_BLACK);  init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(6, COLOR_CYAN, COLOR_BLACK);  init_pair(7, COLOR_WHITE, COLOR_BLACK);
        
  for(int i = 0; i<row; i++) { 
    for(int j = 0; j<col; j++) {
      color c = m[i][j];
      if(colors == 8 || colors == 2) {
        if(c != 0){
        attron(COLOR_PAIR(c) | A_REVERSE);
        addstr("  ");
        attroff(COLOR_PAIR(c) | A_REVERSE);
        }
        else{attron(COLOR_PAIR(c));
        addstr("  ");
        attroff(COLOR_PAIR(c));} 
      }
      if(colors == 16) {
        if(c > 7){
          attron(COLOR_PAIR(c-8) |A_REVERSE);
          addstr("  ");
          attroff(COLOR_PAIR(c-8)| A_REVERSE);
        } else if(c > 0){
          attron(COLOR_PAIR(c)|A_BOLD| A_REVERSE);
          addstr("  ");
          attroff(COLOR_PAIR(c) |A_BOLD| A_REVERSE);
        } else{
          attron(COLOR_PAIR(c));
          addstr("  ");
          attroff(COLOR_PAIR(c));} 
      }
    }
  if(one_more){attron(COLOR_PAIR(0)); addstr(" "); attroff(COLOR_PAIR(0));}
  }
}

int main(int argc, char* argv[]) {
  if(argc == 1) {
    std::cout << "needs moar commands" << std::endl;
    return 1;
  }
  Image img(argv[argc-1]);

  std::ofstream logs;
  logs.open ("logs.txt");
  logs << "Logs...\n";

  start_curses();
  
  int colors = 16;

  int width = img.width();
  int height = img.height();

  int x_i = 0, delta_x = width;
  int y_i = 0, delta_y = height;

  while (true) {
    clear();
    int row,col;
    getmaxyx(stdscr,row,col); 
    
    color_matrix img_repr = img.generate_representation(col/2, row, x_i, delta_x, y_i, delta_y, colors);
    print_repr_ncurses(img_repr, colors, col%2);
    
    refresh();
    int ch = getch();
    if (ch == ERR) continue;
    else if(ch == 'q') break;
    else if((ch == 'Z' || ch == '+') && 
                delta_x * ZOOM_IN > 10 && delta_y * ZOOM_IN > 10){
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
    else if(ch == 'c') colors == 2? colors = 8 :
                         colors == 8? colors = 16 : colors = 2;

    // various corrections
    if (x_i < 0) x_i = 0;
    if (y_i < 0) y_i = 0;
    if (x_i + delta_x > width)  x_i = width - delta_x;
    if (y_i + delta_y > height) y_i = height - delta_y;

    logs << "delta_x -> " << delta_x << "\tdelta_y -> " << delta_y <<
            "\tx_i -> " << x_i << "\ty_i -> " << y_i << "\n";
  }

  logs << "Closing...\n";
  logs.close();
  end_curses();
}

