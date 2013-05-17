Artemis
=======
Nifty prompt for searching strings.  
"Let the hunt begin... and end quickly!"

Features / Docs
===============
* small/fast/low-dependency (ncurses)
* searching is case-insensitive
* tab-completion of strings/searching
* arrow-up/down or Shift-J,K to scroll through results
* results displayed as !: <chosen strings>
* <space> resets current search
* strings from flat-file or stdin
* dumps choice to stdout or file

Building
========
gcc main.c -ncurses -o artemis

Todo
====
* dynamic memory mgmt
* command-line arguments/file-io
* search the hits after tab-list (search matches)
* demo
* ansi color?

License
=======
GNU GENERAL PUBLIC LICENSE  
Version 3, 29 June 2007  
Copyright © 2013 Greg Berenfield
