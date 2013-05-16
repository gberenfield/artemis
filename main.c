//
//  main.c
//  artemis
//
//  Created by Greg Berenfield on 5/14/13.
//
//

#include <stdio.h>
#include <string.h>
#include <ncurses.h>

#define FILENAME "/Users/mc/.mutt/tags"
#define MAX_LINE 128

struct match {
  int index;
  char *sloc;
};

char current[MAX_LINE];       // The current text being typed for search
int num_tags;                 // The file length of the tags file (# of tags)
int row,col,cursor_row,cursor_col,hit_row,hit_col;
bool showing;                 // Are we displaying the hunt_list ?
WINDOW *mywin;
char tags[1000][MAX_LINE];
struct match matches[100];
int sel_match;                // currently selected match
int num_matches;              // number of matches
char *choice;                 // choice of selection/typing

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
  return lineCount;
}

void get_file(char tags[][MAX_LINE])
{
  FILE *inFile;
  int lineCount,i;
  inFile = fopen(FILENAME, "r");

  /* lineCount = flen(FILENAME); */
  /* char tags[lineCount][MAX_LINE]; */

  fopen(FILENAME, "r");
  for(i = 0; i < num_tags; i++)
    fscanf(inFile, "%s", tags[i]);
  fclose(inFile);
}

void setup_screen() /* print the message at the center of the screen */
{
  char mesg[]="?: "; /* message to be appeared onn the screen */

  mywin=initscr();
  idcok(mywin, 1);
  idlok(mywin, 1);
  getmaxyx(stdscr,row,col);/* get the number of rows and columns */
  cursor_row = row/5;
  cursor_col = (col-strlen(mesg))/4;
  mvprintw(cursor_row,cursor_col,"%s",mesg);
}

void clear_matches()
{
  int i;

  for (i=0;i<100;i++) {
    matches[i].index=-1;
    matches[i].sloc=NULL;
  }
}

void hunt_current()
{
  int i,j=0;
  char *k;

  clear_matches();
  sel_match = -1;
  num_matches = 0;

  for (i=0;i<num_tags;i++) {
    if ((k=strcasestr(tags[i],current)) != NULL) {
      matches[j].index=i;
      matches[j].sloc=k;
      j++;
      num_matches++;
    }
  }
}

void show_hits()
{
  int i=0;
  char a[128],b[128];

  while (matches[i].index > -1) {
    memset(a,'\0',128);
    memset(b,'\0',128);
    strncpy(a,tags[matches[i].index],matches[i].sloc-tags[matches[i].index]);
    strcpy(b,&tags[matches[i].index][strlen(a)+strlen(current)]);
    mvprintw((cursor_row+2+i),cursor_col,"%s",a);
    attron(A_BOLD);
    printw("%s",current);
    attroff(A_BOLD);
    printw("%s",b);
    ++i;
  }
  showing=TRUE;
  move(cursor_row,cursor_col);
  refresh();
}

void backspace() {
  noecho();
  noraw();
  move(cursor_row,cursor_col-1);
  delch();
  raw();
  refresh();
  getsyx(cursor_row,cursor_col);
}
void tab_hits_down() {
  if (sel_match < (num_matches-1)) {
    sel_match++;
    if (sel_match==0) {
      hit_row=cursor_row+1;
      hit_col=cursor_col;
    }
    else {
      move (hit_row,hit_col);
      printw(tags[matches[sel_match-1].index]);
    }
    move (++hit_row,hit_col);
    clrtoeol();
    attron(A_REVERSE);
    printw(tags[matches[sel_match].index]);
    attroff(A_REVERSE);
    move(cursor_row,cursor_col);
  }
}
void tab_hits_up() {
  if (sel_match > 0) {
    sel_match--;
    move (hit_row,hit_col);
    printw(tags[matches[sel_match+1].index]);
    move (--hit_row,hit_col);
    clrtoeol();
    attron(A_REVERSE);
    printw(tags[matches[sel_match].index]);
    attroff(A_REVERSE);
    move(cursor_row,cursor_col);
  }
}

void get_current()
{
  int c,lc , i=0;

  raw();
  noecho();
  keypad(stdscr, TRUE);

  while ((c=getch()) != '\n')
  {
    getsyx(cursor_row,cursor_col);
    if (c == '\t') {
      if (lc == '\t') { tab_hits_down(); }
      else {
        clrtobot();
        if (strlen(current)>1) {
          hunt_current();
          show_hits();
        }
      }
    }
    else if (c == 'J') tab_hits_down();
    else if (c == 'K') tab_hits_up();
    else if (c == 127 || c == 8) { // delete or backspace
      backspace();
      i--;
      current[i] = '\0';
      clrtobot();
      if (showing && strlen(current)>1) {
        hunt_current();
        show_hits();
      }
    }
    else {
      addch(c);
      current[i]=c;
      i++;
      if (showing) clrtobot();
      showing=FALSE;
    }
    lc = c;
  }
  current[i] = '\0';
}

int main(int argc, const char * argv[])
{
  num_tags=flen(FILENAME);
  get_file(tags);

  setup_screen();

  get_current();

  if (showing && sel_match>-1) {
    choice = tags[matches[sel_match].index];
  }
  else choice = current;

  mvprintw(LINES - 4, 0, "tag: %s", tags[0]);
  mvprintw(LINES - 3, 0, "You Entered: %s", choice);
  mvprintw(LINES - 2, 0, "Number of tags: %d", num_tags);
  getch();
  endwin();

  printf("%s\n",choice);
  return 0;
}

