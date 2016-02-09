/*! \file    main.cpp
 *  \brief   Date demonstration program.
 *  \author  Peter C. Chapin <PChapin@vtc.vsc.edu>
 *
 * This program shows off some features of class Date. It is not a detailed test program!
 */

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include "Date.hpp"

using namespace std;
using namespace spica;

void date_subtraction( )
{
    Date birthdate;
    Date today;

    cout << "What is your birthdate (yyyy-mm-dd)? ";
    cin  >> birthdate;
    cout << "What is today's date? ";
    cin  >> today;
    cout << "You are " << today - birthdate << " days old!\n";
    cout << workday_difference( today, birthdate ) << " workdays have elapsed in your life.\n";
}


void date_loops( )
{
    Date start_date;
    Date end_date;
    const vector<string> day_names {
        "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

    cout << "What start date? ";
    cin  >> start_date;
    cout << "What end date? ";
    cin  >> end_date;

    if( start_date > end_date ) {
        cout << "The end date must be the same or later than the start date!\n";
    }
    else {
        for( Date current = start_date; current <= end_date; current.advance( ) ) {
            cout << current << ": " << day_names.at( current.day_of_week( ) ) << "\n";
        }
    }
}


void date_skipping( )
{
    Date start_date;
    int  skip_interval;
    int  skip_count;

    cout << "What start date? ";
    cin  >> start_date;
    cout << "What skip interval? ";
    cin  >> skip_interval;
    cout << "How many skips? ";
    cin  >> skip_count;

    // Compute all the dates.
    vector<Date> stepping_stones { start_date };
    for( int i = 0; i < skip_count; ++i ) {
        Date latest = stepping_stones.back( );
        latest.advance( skip_interval );
        stepping_stones.push_back( latest );
    }

    // Display the results.
    for( auto stone : stepping_stones ) {
        cout << stone << "\n";
    }
}


int main( )
{
    int  choice;
    bool done = false;

    while( !done ) {
        cout << "\n"
                "0) Quit\n"
                "1) Date subtraction\n"
                "2) Date loops\n"
                "3) Date skipping\n"
                "\n"
                "choice: ";

        cin >> choice;
        switch( choice ) {
        case 0:
            done = true;
            break;
        case 1:
            date_subtraction( );
            break;
        case 2:
            date_loops( );
            break;
        case 3:
            date_skipping( );
            break;
        default:
            cout << "Unknown choice!\n";
            break;
        }
    }

    return EXIT_SUCCESS;
}
