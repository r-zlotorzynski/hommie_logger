/********************************************************************************************************************
    @author         hommie.pl
    @license        MIT
    @contributors   Rafał Złotorzyński
    
    Licencja, pozwala użytkownikom, o ile zachowane są warunki licencyjne oraz informacje
    o autorze, w nieograniczonym zakresie na używanie, kopiowanie, rozpowszechnianie, modyfikowanie
    oryginalnego lub zmodyfikowanego programu w postaci binarnej lub źródłowej.

    license that allows users, as long as the license conditions and information about the author
    are preserved, to an unlimited extent to use, copy, distribute, modify
    the original or modified program in binary or source form.

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