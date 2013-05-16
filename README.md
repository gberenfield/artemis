Artemis
=======
Nifty prompt for searching strings.

Features
========
* small/fast/low-dependency (ncurses)
* strings from flat-file or stdin
* dumps choice to stdout or file
* tab-completion of strings/searching
* searching case-insensitive
* arrow-up/down or Shift-J,K to scroll through results

Building
========
gcc main.c -ncurses -o artemis

Todo
====
Make enter of selecting string put it in 'selection buffer' and offer
new blank current prompt

License
=======
GNU GENERAL PUBLIC LICENSE  
Version 3, 29 June 2007  
Copyright © 2013 Greg Berenfield
