Artemis
=======
Nifty prompt for searching strings.
"Let the hunt begin... and end quickly!"

Features
========
* small/fast/low-dependency (ncurses)
* searching is case-insensitive
* results displayed as !: <chosen strings>
* strings from flat-file or stdin
* dumps choice to stdout or file
* tab-completion of strings/searching
* arrow-up/down or Shift-J,K to scroll through results

Building
========
gcc main.c -ncurses -o artemis

Todo
====
* ESC-some key to reset search quickly
* dynamic memory mgmt
* command-line arguments/file-io

License
=======
GNU GENERAL PUBLIC LICENSE  
Version 3, 29 June 2007  
Copyright © 2013 Greg Berenfield
