/*
    ONID: yamamsat
    email: yamamsat@oregonstate.edu

    NOTE: I avoid to use single-line comment because this was created in cpp
    If I remember correctly, some of old compiler display error message for this.

    Here is the links where I checked while I was making Programming memo

    reference:
    - for some information
    cs344 Explorations

**********************************************************************************************/
/*  TODO
    Thread 1, called the Input Thread, reads in lines of characters from the standard input.

    Thread 2, called the Line Separator Thread, replaces every line separator in the input by a space.
    '\n' to space
    Thread 3, called the Plus Sign thread, replaces every pair of plus signs, i.e., "++", by a "^".
    replace ++ to ^
    --count
    Thread 4, called the Output Thread, write this processed data to standard output as lines of exactly 80 characters.
    count the charactors and display
**********************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int main(int argc, char *argv[])
{

    return EXIT_SUCCESS;
}
