//
// Created by Nico on 16/05/2019.
//

#ifndef JODA_CURSESFWD_H
#define JODA_CURSESFWD_H

#define JODA_JSON_OUTPUT_COLOR_NUMBER 1
#define JODA_JSON_OUTPUT_COLOR_STRING 2
#define JODA_JSON_OUTPUT_COLOR_BOOL 3
#define JODA_JSON_OUTPUT_COLOR_NULL 4
#define JODA_JSON_OUTPUT_COLOR_KEY 5

#ifdef JODA_CURSES_SIMPLE_HEADER
#include <curses.h>
#include <ncurses.h>
#else
#include <ncursesw/curses.h>
#include <ncursesw/ncurses.h>
#endif

#endif  // JODA_CURSESFWD_H
