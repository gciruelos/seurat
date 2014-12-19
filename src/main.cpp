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



void print_repr_ncurses (const color_matrix& m, options* opt, bool one_more) {
  int row = m.size(), col = m[0].size();
  int colors = opt->colors;
  std::string imgsize = std::to_string(opt->width) + " x " + std::to_string(opt->height);

  init_pair(0, COLOR_BLACK, COLOR_BLACK); init_pair(1, COLOR_RED, COLOR_BLACK); 
  init_pair(2, COLOR_GREEN, COLOR_BLACK); init_pair(3, COLOR_YELLOW, COLOR_BLACK); 
  init_pair(4, COLOR_BLUE, COLOR_BLACK);  init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(6, COLOR_CYAN, COLOR_BLACK);  init_pair(7, COLOR_WHITE, COLOR_BLACK);
  
  for(int i = 0; i<row; i++) {
    if(one_more){attron(COLOR_PAIR(0)); addch(' '); attroff(COLOR_PAIR(0));}       
    
    for(int j = 0; j<col; j++) {
      color c = colors == 16? m[i][j] : (m[i][j] == 0 ? 0 : m[i][j] + 8); // horrible hack  
      if(c > 7)
        attron(COLOR_PAIR(c-8) | A_REVERSE);
      else if(c > 0)
        attron(COLOR_PAIR(c) | A_BOLD | A_REVERSE); // A_BOLD makes the color lighter
      else 
        attron(COLOR_PAIR(c));
    
      /* here I print what has to be printed, be it nothing or some file info */
      if(opt->info && i == 0){
        int sz = opt->filename.size();
        if (2*j < sz) addch(opt->filename[2*j]); else addch(' ');
        if (2*j+1 < sz) addch(opt->filename[2*j+1]); else addch(' ');
      }
      else if(opt->info && i == 1){
        int sz = imgsize.size();
        if (2*j < sz) addch(imgsize[2*j]); else addch(' ');
        if (2*j+1 < sz) addch(imgsize[2*j+1]); else addch(' ');
      }else {
        addch(' '); addch(' ');
      }
      
      
      
      attrset(A_NORMAL);
    }
  }
}

int input_action(options* opt){
  /**
   * <Reads input char and does expected action>
   * @param opt: the options struct to be potentially modified
   * @return Returns 0 if 'q' was pressed, 2 if it failed to read input, 1 otherwise
   */

  char ch = getch();
  if(opt->debug)
    opt->logs << "key pressed -> " << ch << "  (ascii "<< (int) ch << ")" <<std::endl;


  if (ch == ERR) return 2;
  else if(ch == 'q') return 0;
  else if((ch == 'Z' || ch == '+') && 
             opt->delta_x * ZOOM_IN > 15 && opt->delta_y * ZOOM_IN > 15){
    opt->x_i += ((1-ZOOM_IN) * opt->delta_x)/2; // This is because I want to mantain the middle,
    opt->y_i += ((1-ZOOM_IN) * opt->delta_y)/2; // so if     m_x = (x_i + delta_x)/2,    then
                                                // m_x = (x_i + 0.1 * delta_x + 0.9 * delta_x)/2 
    opt->delta_x *= ZOOM_IN;
    opt->delta_y *= ZOOM_IN;
  }
  else if(ch == 'z' || ch=='-') {
    if(opt->delta_x * ZOOM_OUT >= opt->width || opt->delta_y * ZOOM_OUT >= opt->height){
      opt->delta_x = opt->width; opt->delta_y = opt->height; opt->x_i = 0; opt->y_i = 0;
    } else{
      opt->x_i += ((1-ZOOM_OUT) * opt->delta_x)/2;
      opt->y_i += ((1-ZOOM_OUT) * opt->delta_y)/2;

      opt->delta_x *= ZOOM_OUT;
      opt->delta_y *= ZOOM_OUT;
    }
  }
  else if(ch == 5)  opt->x_i += opt->delta_x * 0.1;
  else if(ch == 4)  opt->x_i -= opt->delta_x * 0.1;
  else if(ch == 3)  opt->y_i -= opt->delta_y * 0.1;
  else if(ch == 2)  opt->y_i += opt->delta_y * 0.1;
  
  
  else if(ch == 'i')  opt->info = !opt->info;
 
  else if(ch == 'c') opt->colors == 2? opt->colors = 8 :
                       opt->colors == 8? opt->colors = 16 : opt->colors = 2;
  else if(ch == 'C') opt->colors == 16? opt->colors = 8 :
                       opt->colors == 8? opt->colors = 2 : opt->colors = 16;
  else if(ch == 'd') opt->dithering_option < 4 ? opt->dithering_option++ :
                                            opt->dithering_option = 0;

  // various corrections
  if (opt->x_i < 0) opt->x_i = 0;
  if (opt->y_i < 0) opt->y_i = 0;
  if (opt->x_i + opt->delta_x > opt->width)  opt->x_i = opt->width - opt->delta_x;
  if (opt->y_i + opt->delta_y > opt->height) opt->y_i = opt->height - opt->delta_y;

  return 1; // everyting went fine
}


static void show_usage(std::string name){
  std::cerr << "Usage: " << name << " [--debug] [-h | --help] filename" << std::endl
            << "Options:\n"
            << "\t-h,--help\t\t\tShow this message\n"
            << "\t--debug\tDebug mode"
            << std::endl;
}

int parse_args(int argc, char* argv[], options* opt){   
  /**
   * <Parses command-line arguments>
   * @param argc: number of arguments
   * @param argv: array of argument values
   * @return Returns 1 if it fails (printing the reason), 0 if everything went ok
   */

  /* initialize opt to default values */
  opt->colors = 8;
  opt->dithering_option = 0;

  opt->debug = false;
  opt->info = false;

  /* read args and change values if necessary */
  if(argc == 1){
    std::cout << "needs file" << std::endl;  
    show_usage(argv[0]);
    return 1;
  }
  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];
    if ((arg == "-h") || (arg == "--help")) {
      show_usage(argv[0]);
      return 1;
    } else if ((arg == "--debug")) {
      opt->debug = true;
      opt->logs.open ("logs.txt");
    } else {
      opt->filename = argv[i];
    }
  }
  return 0;
}

int main(int argc, char* argv[]){
  options* opt = new options;
  if(parse_args(argc, argv, opt)) return 1;

  Image img(opt->filename, opt->opened);

  if(!opt->opened) {
    std::cout << "could not open file" << std::endl;
    return 1;
  }

  start_curses();
  
  opt->width = img.width();
  opt->height = img.height();

  opt->x_i = 0, opt->delta_x = opt->width;
  opt->y_i = 0, opt->delta_y = opt->height; 

  if(opt->debug) {
    opt->logs << "Logs...\n"
              << "key right: " << (char) KEY_RIGHT << "\n"
              << "key left: " << (char) KEY_LEFT << "\n"
              << "key up: " << (char) KEY_UP << "\n"
              << "key down: " << (char) KEY_DOWN << "\n";
  }
  
  /* main program loop */
  while (true) {
    clear();
    int row,col;
    getmaxyx(stdscr,row,col); 
    
    color_matrix img_repr = img.generate_representation(col/2, row, opt);
    print_repr_ncurses(img_repr, opt, col%2);
    
    refresh();
    int result = input_action(opt);
    if(!result) break;
    if(opt->debug)
      opt->logs << "delta_x = " << opt->delta_x << "\tdelta_y = " << opt->delta_y <<
            "\tx_i = " << opt->x_i << "\ty_i = " << opt->y_i << "\n";
  }

  if (opt->debug){opt->logs << "Closing...\n"; opt->logs.close();}

  end_curses();
  delete opt;
}

