N.B. This project is still in development; many core features are not yet complete or may be implemented differently in the first 'user-ready' release.

# Mantra

This is an ncurses-based terminal application to browse and bookmark man pages.

## Why?

Two main reasons:<br/>
1:<br/>
```
$ man bash | wc -l
3408
```

2:<br/>
"Ah, that looks like a cool utility! I'll have to remember to read up on that later."
*Promptly forgets*

## Requirements

libgdbm-dev  
libncursesw5-dev  

## Installation

```
$ make
$ sudo make install
```

For those interested, the specific mantra referred to by the name is 'RTFM', which of course stands for 'Read the Full Manual' ;)
