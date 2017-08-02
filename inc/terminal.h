// -*-c++-*-
#pragma once

#include <stdio.h>
#include <unistd.h>

// Support for ANSI escape codes for setting terminal colors and the like.
//
// There are four functions that take 1,2,3, or 4 attributes, with an optional
// file stream to write to (default is stdout).
//
//    eg> term_mode(TERM_BG_RED); // sets stdout background to red
//    eg> term_mode(TERM_BG_RED, TERM_FG_WHITE, stderr); 
//
// term_reset() removes all formatting from the device
//

typedef enum {
    TERM_NOOP       = -1,
    TERM_RESET      =  0,
    TERM_BOLD       =  1,
    TERM_FAINT      =  2, 
    TERM_ITALIC     =  3, // limited support
    TERM_UNDERLINE  =  4,
    TERM_BLINK      =  5,
    TERM_REVERSE    =  7, // reverse video
    TERM_STRIKE     =  9,

    // foreground colors
    TERM_FG_BLACK   = 30,
    TERM_FG_RED     = 31,
    TERM_FG_GREEN   = 32,
    TERM_FG_YELLOW  = 33,
    TERM_FG_BLUE    = 34,
    TERM_FG_MAGENTA = 35,
    TERM_FG_CYAN    = 36,
    TERM_FG_WHITE   = 37,

    // background colors
    TERM_BG_BLACK   = 40,
    TERM_BG_RED     = 41,
    TERM_BG_GREEN   = 42,
    TERM_BG_YELLOW  = 43,
    TERM_BG_BLUE    = 44,
    TERM_BG_MAGENTA = 45,
    TERM_BG_CYAN    = 46,    
    TERM_BG_WHITE   = 47,
    
    TERM_FRAMED     = 51,
    TERM_ENCIRCLED  = 52,
    TERM_OVERLINED  = 53,
} term_attrib_t;



/******************************************************************************
 * Set terminal attributes from lists above.  Attributes are _all_ applied at once
 * and override and previously set attributes.
 *******************************************************************************/
static inline void term_mode(
    term_attrib_t attrib0, term_attrib_t attrib1,
    term_attrib_t attrib2, term_attrib_t attrib3,  FILE *fd=stdout
) { 
    if (!isatty(fileno(fd))) return;

    if (TERM_NOOP != attrib3) fprintf(fd, "\e[" "%i;%i;%i;%i" "m", (int)attrib0, (int)attrib1, (int)attrib2, (int)attrib3);
    if (TERM_NOOP != attrib2) fprintf(fd, "\e[" "%i;%i;%i"    "m", (int)attrib0, (int)attrib1, (int)attrib2);
    if (TERM_NOOP != attrib1) fprintf(fd, "\e[" "%i;%i"       "m", (int)attrib0, (int)attrib1);
    if (TERM_NOOP != attrib0) fprintf(fd, "\e[" "%i"          "m", (int)attrib0);
}


/*******************************************************************************
 * Helper funtions to eliminate needing to always specify either fd or the attributes
 * if we did this with one function.  These can used to pass 1,2,3 or 4 attributes
 * to term_mode and have a default fd output value.
 *******************************************************************************/
// helper functions for passing up to 4 terminal attributes
static inline void term_mode(
    term_attrib_t aa,                   FILE *fd=stdout) { term_mode(aa, TERM_NOOP, TERM_NOOP, TERM_NOOP, fd); }

static inline void term_mode(
    term_attrib_t aa, term_attrib_t bb, FILE *fd=stdout) { term_mode(aa, bb, TERM_NOOP, TERM_NOOP, fd); }
     
static inline void term_mode(
    term_attrib_t aa, term_attrib_t bb,
    term_attrib_t cc,                   FILE *fd=stdout) { term_mode(aa, bb, cc, TERM_NOOP, fd); }

// reset terminal to defaults
static inline void term_reset(FILE* fd=stdout) {
    term_mode(TERM_RESET, fd);
}





 
