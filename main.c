//
//  main.c
//  artemis
//
//  Copyright 2013 Greg Berenfield

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#define MAX_LINE 128

struct match {
  int index;
  char *sloc;
};

char infile[128],outfile[128];     // input and output filenames
char current[MAX_LINE];            // The current text being typed for search
int num_tags=0;                    // The file length of the tags file (# of tags)
int row,col,cursor_row,cursor_col,hit_row,hit_col;
bool showing;                      // Are we displaying the hunt_list ?
WINDOW *mywin;                     // curses window
char **tags;                       // set of tags to search
struct match *matches;             // set of matched tags from current search
int sel_match;                     // currently selected match
int num_matches=0;                 // number of matches
char items[MAX_LINE*2];            // all choices, space-delimited

int flen(char fname[])
{
  FILE *inFile;
  int lineCount,inputError;
  inFile = fopen(fname, "r");
  char word[MAX_LINE];

  lineCount = 0;
  while(inputError != EOF) {
    inputError = fscanf(inFile, "%s\n", word);
    lineCount++;
  }
  fclose(inFile);
  return (lineCount-1);
}

void get_file()
{
  FILE *inFile;
  char str[MAX_LINE];
  size_t i;

  inFile = fopen(infile, "r");
  tags = malloc(sizeof(*tags) * num_tags);
  for(i = 0; i < num_tags; i++)
  {
    fscanf(inFile, "%s", str);
    tags[i]=(char *)malloc(sizeof(char *) * (strlen(str)+1));
    strcpy(tags[i],str);
  }
  fclose(inFile);
}

void put_file()
{
  FILE *File;
  File = fopen(outfile, "w");

  fprintf(File, "%s\n", items);
  fclose(File);
}

void setup_screen() /* print the message at the center of the screen */
{
  char mesg[]="?: "; /* message to be appeared onn the screen */

  mywin=initscr();
  clear();
  idcok(mywin, 1);
  idlok(mywin, 1);
  mvprintw(LINES - 2, 0, "!: %s", items);
  getmaxyx(stdscr,row,col);/* get the number of rows and columns */
  cursor_row = row/5;
  cursor_col = (col-strlen(mesg))/4;
  mvprintw(cursor_row,cursor_col,"%s",mesg);
}

void clear_matches()
{
  size_t i;

  sel_match = -1;
  num_matches = 0;
  for (i=0;i<num_matches;i++) {
    matches[i].index=-1;
    matches[i].sloc=NULL;
  }
}

void hunt_current()
{
  size_t i,j=0;
  char str[MAX_LINE];
  char *k;

  clear_matches();
  for (i=0;i<num_tags;i++) {
    if ((k=strcasestr(tags[i],current)) != NULL) {
      matches[j].index=i;
      matches[j].sloc=k;
      j++;
      num_matches++;
    }
  }
}

void print_hit_highlight_kw(size_t i)
{
  // i is the relative row position from the first (0) hit item
  char front[128],back[128];

  memset(front,'\0',128);
  memset(back,'\0',128);
  strncpy(front,tags[matches[i].index],matches[i].sloc-tags[matches[i].index]);
  strcpy(back,&tags[matches[i].index][strlen(front)+strlen(current)]);
  mvprintw((cursor_row+2+i),cursor_col,"%s",front);
  attron(A_UNDERLINE);
  printw("%s",current);
  attroff(A_UNDERLINE);
  printw("%s",back);
}

void show_hits()
{
  size_t i=0;

  clrtobot();
  while (i<num_matches) {
    print_hit_highlight_kw(i);
    ++i;
  }
  showing=TRUE;
  mvprintw(LINES - 2, 0, "!: %s", items);
  move(cursor_row,cursor_col);
  refresh();
}

void backspace()
{
  noecho();
  noraw();
  move(cursor_row,cursor_col-1);
  delch();
  raw();
  refresh();
  getsyx(cursor_row,cursor_col);
  move(cursor_row,cursor_col);
}

void tab_hits_down()
{
  if (sel_match < (num_matches-1)) {
    sel_match++;
    if (sel_match==0) {
      hit_row=cursor_row+1;
      hit_col=cursor_col;
    }
    else {
      move (hit_row,hit_col);
      print_hit_highlight_kw(sel_match-1);
    }
    move (++hit_row,hit_col);
    clrtoeol();
    attron(A_REVERSE);
    printw(tags[matches[sel_match].index]);
    attroff(A_REVERSE);
    move(cursor_row,cursor_col);
  }
}

void tab_hits_up()
{
  if (sel_match > 0) {
    sel_match--;
    move (hit_row,hit_col);
    print_hit_highlight_kw(sel_match+1);
    move (--hit_row,hit_col);
    clrtoeol();
    attron(A_REVERSE);
    printw(tags[matches[sel_match].index]);
    attroff(A_REVERSE);
    move(cursor_row,cursor_col);
  }
}

void diag(int c)
{
  getsyx(cursor_row,cursor_col);
  mvprintw(LINES - 8, 0, "# tags: %d", num_tags);
  mvprintw(LINES - 7, 0, "# matches: %d", num_matches);
  mvprintw(LINES - 6, 0, "sizeof matches: %d", sizeof(matches));
  mvprintw(LINES - 5, 0, "current: %s", current);
  mvprintw(LINES - 4, 0, "c: %d", c);
  move(cursor_row,cursor_col);
}

void get_current()
{
  int c,lc,i=0;

  raw();
  noecho();
  keypad(stdscr, TRUE);

  while ((c=getch()))
  {
    getsyx(cursor_row,cursor_col);
    /* diag(c); */
    if (c == '\t')                                // Tab
    {
      if (showing) tab_hits_down();
      else {
        clrtobot();
        if (strlen(current)>1) {
          hunt_current();
          show_hits();
          tab_hits_down();
        }
      }
    }
    else if (c == '\n' && lc == '\n') break;      // two-returns to quit
    else if (c == 'J' || c == 258)                // J | d-arrow down hit list
      tab_hits_down();
    else if (c == 'K' || c == 353 || c == 259)    // K | <shift-TAB> | u-arrow up hit list
      tab_hits_up(); 
    else if (c == 32)                             // SPACE to reset search
    {
      memset(current,'\0',MAX_LINE);
      clear_matches();
      setup_screen();
      mvprintw(LINES - 2, 0, "!: %s", items);
      move(cursor_row,cursor_col+3);
      i=0;
    }
    else if (c == 127 || c == 8)                  // DELETE or BACKSPACE
    {
      backspace();
      i--;
      current[i] = '\0';
      clrtobot();
      mvprintw(LINES - 2, 0, "!: %s", items);
      move(cursor_row,cursor_col);
      if (showing && strlen(current)>1) {
        hunt_current();
        show_hits();
        tab_hits_down();
      }
    }
    else if (c=='\n')                             // SELECT hightlighted hit or typed word
    {
      if (showing && sel_match>-1) strcat(items,tags[matches[sel_match].index]);
      else strcat(items,current);
      memset(current,'\0',MAX_LINE);
      i=0;
      setup_screen();
      showing=FALSE;
      strcat(items," ");
      mvprintw(LINES - 2, 0, "!: %s", items);
      move(cursor_row,cursor_col+3);
    }
    else                                          // regular key-typed
    {
      addch(c);
      current[i]=c;
      i++;
      if (showing && strlen(current)>1) {
        noecho();
        noraw();
        move(cursor_row,cursor_col+1);
        delch();
        raw();
        refresh();
        getsyx(cursor_row,cursor_col);
        hunt_current();
        show_hits();
        tab_hits_down();
      }
      else showing=FALSE;
    }
    lc = c;
  }
  current[i] = '\0';
}

void show_basics()
{
  printf("artemis - (C) 2013 Greg Berenfield\nReleased under the GNU GPL.\n\nUsage: artemis [-o <output>] [-f <word-file> | \"string of space-separated words\"]\n");
}

int main(int argc, const char * argv[])
{
  size_t i;
  if (argc==1)                  // no arguments
  {
    show_basics();
    return 0;
  }
  for (i = 1; i < argc; i++)    // parse arguments
  {
    if (argv[i][0] == '-') {
      switch (argv[i][1]) {
        case 'f':strcpy(infile,argv[++i]); break;
        case 'o':strcpy(outfile,argv[++i]); break;
      }
    }
  }

  if (strlen(infile)>0)         // read file for tags
  {
    num_tags=flen(infile);
    get_file();
  }
  else                          // read string from stdin for tags
  {
    i=0;
    char *s,t[MAX_LINE*2];
    strcpy(t,argv[argc-1]);
    s=strtok(t," ");
    while (s != NULL)
    {
      strcpy(tags[i],s);
      s = strtok (NULL, " ");
      ++i;
    }
    num_tags=i;
  }

  matches = malloc(num_tags * sizeof(struct match));

  setup_screen();
  get_current();
  items[strlen(items)-1]='\0';  // remove trailing space-separation char
  endwin();

  if (strlen(outfile)>0) put_file();
  else printf("%s\n",items);

  // be kind and free up allocations
  for(i = 0; i < num_tags; i++) free(tags[i]); // free tags
  free(tags);
  free(matches);

  return 0;
}

