/*! \file    get_switch.cpp
    \brief   Functions to read command line switches.
    \author  Allen I Holub

(C) Copyright 1985, 1987 Allen I Holub. All rights reserved. This program may be copied for
personal, non-profit use only. Information on this program can be found in the book "C chest and
Other C Treasures" by Allen Holub. (C) 1987. M & T Books. Pages 41-51.

I have made extensive modifications to this code. Most of my modifications are stylistic, but
some are more significant. In particular, I converted the code to C++.

Peter C. Chapin <PChapin@vtc.vsc.edu>
*/

#include <cctype>
#include <iostream>
#include <cstdlib>
#include "get_switch.hpp"

using namespace std;

const char switch_character = '-';

/*=========================================*/
/*           Function Prototypes           */
/*=========================================*/

static const char *get_value( SwitchInfo *, const char * );
static SwitchInfo *find_switch( char, SwitchInfo *, int );


static const char *get_value( SwitchInfo *switch_info, const char *line )
{
    ++line;
    switch( switch_info->type ) {

    case int_switch:
        *switch_info->value = atoi( line );
        while( isdigit( *line ) ) line++;
        break;

    case bin_switch:
        *switch_info->value = 1;
        break;

    case chr_switch:
        *switch_info->value = static_cast< int >( *line++ );
        break;

    case str_switch:
        *( switch_info->pvalue ) = line;
        line = "";
        break;

    default:
        cerr << "INTERNAL ERROR: get_switchs() is using a bad switch type!" << endl;
        break;
    }
    return line;
}


static SwitchInfo *find_switch( char c, SwitchInfo *switch_table, int table_size )
{
    for( ; --table_size >= 0; switch_table++ )
        if( switch_table->name == c )
            return switch_table;
    return NULL;
}


void print_usage( SwitchInfo *switch_table, int table_size, std::ostream &output_file )
{
    for( ; --table_size >= 0; switch_table++ ) {
        switch( switch_table->type ) {

        case int_switch:
            output_file << "-" << switch_table->name
                        << "<num> " << switch_table->help_message << endl;
            break;

        case bin_switch:
            output_file << "-" << switch_table->name
                        << "      " << switch_table->help_message << endl;
            break;

        case chr_switch:
            output_file << "-" << switch_table->name
                        << "<c>   " << switch_table->help_message << endl;
            break;

        case str_switch:
            output_file << "-" << switch_table->name
                        << "<str> " << switch_table->help_message << endl;
            break;

        default:
            cerr << "INTERNAL ERROR: Bad switch type passed to print_usage()." << endl;
            break;
        }
    }
    return;
}


int get_switchs( int argc, char **argv, SwitchInfo *switch_table, int table_size )
{
    int        new_argc;
    char     **new_argv;
    const char *p;
    SwitchInfo *switch_info;

    new_argc = 1;
    for( new_argv = ++argv; --argc > 0; argv++ ) {
        if( **argv != switch_character ) {
            *new_argv++ = *argv;
            new_argc++;
        }
        else {
            p = *argv + 1;
            while( *p ) {
                if( ( switch_info = find_switch( *p, switch_table, table_size ) ) != NULL )
                    p = get_value( switch_info, p );
                else {
                    cerr << "Illegal switch <" << *p << ">. Legal switchs are:\n\n";
                    print_usage( switch_table, table_size, cerr );
                    exit( bad_switch );
                }
            }
        }
    }
    *new_argv = NULL;
    return new_argc;
}
