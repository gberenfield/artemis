//
//  main.c
//  ngoo
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

char current[MAX_LINE]; // The current text being typed for search
int num_tags;           // The file length of the tags file (# of tags)
int row,col,cursor_row,cursor_col;
bool showing; // Are we displaying the hunt_list ?
WINDOW *mywin;
char tags[1000][MAX_LINE];
struct match matches[100];

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

  for (i=0;i<num_tags;i++) {
    if ((k=strcasestr(tags[i],current)) != NULL) {
      matches[j].index=i;
      matches[j].sloc=k;
      j++;
    }
  }
}

void show_hits()
{
  int i=0;

  while (matches[i].index > -1) {
    mvprintw((cursor_row+2+i),cursor_col,"%s",tags[matches[i].index]);
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

void get_current()
{
  int c, i=0;

  raw();
  noecho();
  keypad(stdscr, TRUE);

  while ((c=getch()) != '\n')
  {
    getsyx(cursor_row,cursor_col);
    if (c=='\t') {
      clrtobot();
      if (strlen(current)>1) {
        hunt_current();
        show_hits();
      }
    }
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
  }
  current[i] = '\0';
}

int main(int argc, const char * argv[])
{
  num_tags=flen(FILENAME);
  /* char tags[num_tags][MAX_LINE]; */
  get_file(tags);

  setup_screen();

  get_current();

  mvprintw(LINES - 4, 0, "tag: %s", tags[0]);
  mvprintw(LINES - 3, 0, "You Entered: %s", current);
  mvprintw(LINES - 2, 0, "Number of tags: %d", num_tags);
  getch();
  endwin();

  return 0;
}

