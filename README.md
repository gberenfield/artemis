# Artemis

Nifty prompt for searching strings.  
"Let the hunt begin... and end quickly!"

## Features / Docs

* small/fast/low-dependency (ncurses)
* searching is case-insensitive
* tab-completion of strings/searching
* arrow-up/down, Tab/Shift-Tab, or Shift-J,K to scroll through results
* results displayed as !: <chosen strings>
* <space> resets current search
* strings from flat-file or stdin
* dumps choice to stdout or file
* ESC 2-times to cancel search with no selection

## Building

    gcc main.c -lncurses -o artemis

## Demo / Usage

    ./artemis -f demo.txt
Then try typing `ge<return><return`

    ./artemis -o output.txt "ashen benign caustic finite zodiac"
N.B. The word-list has to be both space-separated, and the last argument

## Todo

* search the hits after tab-list (search matches)
* ansi color?

## License

GNU GENERAL PUBLIC LICENSE, Version 3, 29 June 2007  
Copyright © 2013 Greg Berenfield
