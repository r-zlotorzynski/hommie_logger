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
    @file           ./src/hommie_logger.h
    @todo           Definition of the constans class
********************************************************************************************************************/

// Use enviromental class - Arduino SDK
#include <Arduino.h>


// If you have not defined constants yet...
#if !defined ( CONST_ALREADY_DECLARED )
    // Setting variable - Constants have already been declared
    #define CONST_ALREADY_DECLARED true


    // Whether the module should be debugged
    #define DEBUG_INFO_FS false


#endif