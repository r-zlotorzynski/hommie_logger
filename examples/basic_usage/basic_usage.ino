/********************************************************************************************************************
    @author         hommie.pl
    @license        MIT
    @contributors   Rafał Złotorzyński
    
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

    @module         hommie_logger
    @file           ./example/basic/basic_usage.cpp
    @todo           The source of example usage library
********************************************************************************************************************/


// Use enviromental class - Arduino SDK
#include <arduino.h>

// Define library
#include "hommie_logger.h"


// Optional - copy class on variable
class_hommie_logger hommie_logger;

// Test variable
uint16_t test_counter = 0;


// Setup
void setup ( void ) {
    // Start COM for debug
    Serial.begin( 115200 );

    // Delay...
    delay( 500 );

    // Initialize lib
    hommie_logger.init();
}

// Loop
void loop ( void ) {
    // Delay for tests
    delay( 5000 );

    // Add new log
    hommie_logger.append( String( test_counter++ ) );

    // Delay for tests
    delay( 5000 );

    // Show file logs
    Serial.println( "------------ List save logs ------------" );
        hommie_logger.print_logs( Serial );
    Serial.println( "----------------------------------------" );
}