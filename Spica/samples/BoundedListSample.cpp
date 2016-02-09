/*! \file   main.cpp
 *  \brief  BoundedList demonstration program.
 *  \author Peter C. Chapin <PChapin@vtc.vsc.edu>
 */

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "BoundedList.hpp"

using namespace std;
using namespace spica;

string trim_leading_spaces( const string &s )
{
    string::size_type nonwhite = s.find_first_not_of( " \t\r\n" );
    if( nonwhite != string::npos ) {
        return s.substr( nonwhite );
    }
    return s;
}


int main( )
{
    int sentence_count = 0;
    ifstream input( "sample.txt" );
    string   line;
    BoundedList<string> lines( 128 );  // No more than 128 lines supported.

    string stuff[] = { "hello", "there", "world" };
    lines.push_back( stuff, stuff + 3 );

    vector<int> vector_stuff = { 1, 2, 3, 4 };
    lines.push_back( vector_stuff.begin(), vector_stuff.end() );

    while( getline( input, line ) ) {
        lines.push_back( line );
    }
    cout << "Read complete: " << lines.size( ) << " lines found.\n";

    BoundedList<string>::iterator current = lines.begin( );
    while( current != lines.end( ) ) {
        // Is there a period on the current line?
        string::size_type dot = current->find( '.' );
        if( dot == string::npos ) {
            ++current;
        }
        else {
            // There is (at least one) period on this line.
            string head = current->substr( 0, dot + 1 );
            string tail = trim_leading_spaces( current->substr( dot + 1 ) );

            BoundedList<string>::iterator old = current;
            ++current;
            lines.erase( old );
            lines.insert( current, head );
            lines.insert( current, "" );
            ostringstream formatter;
            formatter << "^Sentence #" << ++sentence_count;
            lines.insert( current, formatter.str( ) );
            lines.insert( current, "*****" );
            lines.insert( current, tail );
            --current;
        }
    }

    // Output the processed text.
    for( auto item : lines ) {
        cout << item << "\n";
    }

    return EXIT_SUCCESS;
}
