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
    @file           ./src/hommie_logger.cpp
    @todo           The source of the logger program
********************************************************************************************************************/


// Use enviromental class - Arduino SDK
#include <Arduino.h>

// Filesystem library
#include <FS.h>

// Support for the internal file system
#include <SPIFFS.h>

// hommie logger library - constans
#include "hommie_logger_const.h"

// hommie logger library
#include "hommie_logger.h"


/**
    * TODO: File system initialization
    *
    * @param        {boolean}                                       auto_start                                  Automatically initialize the module using the default settings
    * @return       {boolean}                                                                                   Successfully completed or not
    * @public
*/
bool class_hommie_logger::init( bool auto_start ) {
    // System logs
    #if DEBUG_INFO_FS
        Serial.println( "hommie logger -> Initialization..." );
    #endif

    // Domyślnie ustaw wewnętrzny system plików
    if ( auto_start ) {
        // Próba inicjalizacji wewnętrznego systemu plików
        if ( !SPIFFS.begin( true ) ) {
            // System logs
            Serial.println( "hommie logger -> OK!" );
        } else {
            // System logs
            Serial.println( "hommie logger -> FAILED!" );
        }
    } 

    // Zakończ powodzeniem
    return true;
}

/**
    * TODO: Set controller
    *
    * @param        {class}                                         fs                                          File controller
    * @public
*/
void class_hommie_logger::set_controller( fs::FS &fs ) {
    // Set controller
    this->fs_controller = fs;
}

/**
    * TODO: Set max logs
    *
    * @param        {uint16}                                        max_logs                                    Max. logs count
    * @public
*/
void class_hommie_logger::set_max_logs( uint16_t max_logs ) {
    this->max_logs_line = max_logs;
}

/**
    * TODO: Set usage timestamp inline
    *
    * @param        {bool}                                          use_timestamp                               Usage timestamp
    * @public
*/
void class_hommie_logger::set_usage_timestamp( bool use_timestamp ) {
    this->is_use_uptime = use_timestamp;
}

/**
    * TODO: Creates a new record in the file using the circular buffer algorithm
    *
    * @return           {string}                                    path_to_file                                Path to the file being tested
    * @return           {boolean}                                                                               Success or not
    * @public
*/
bool class_hommie_logger::append( String message ) {
    // Czy system plików jest dostępny
    if ( this->is_busy ) return false;

    // Plik jest aktualnie zajęty
    this->is_busy = true;

    // Sprawdź czy katalog istnieje, jeśli nie - UTWÓRZ
    if ( !this->is_dir_exists( this->log_dir ) ) this->create_dir( this->log_dir );

    // Jeśli nie określono stanu pliku
    if ( this->save_logs_count == -1 ) {
        // Otwieram plik testowy
        File file = this->get_handler_loc( log_file , 0 );
        
        // Plik jest nowy - zapisujemy od 0
        if ( file && file.size() ) {
            // Ilość logów
            this->save_logs_count = this->get_line_count( file );

            // Zamknij plik
            file.close();
        } else {
            this->save_logs_count = 0;
        }

        // System logs
        #if DEBUG_INFO_FS
            Serial.println( "hommie logger -> Określono aktualną ilość logów w pliku -> " + String( this->save_logs_count ) );
        #endif
    }

    // Timestamp
    if ( this->is_use_uptime ) message = String( millis() ) + " - " + message;

    // Dopisanie do pliku
    if ( this->save_logs_count < this->max_logs_line ) {
        // Otwieram plik
        File file = this->get_handler_loc( this->log_file , 2 );

        // Błąd uchwytu pliku - PRAWDOPODOBNIE KARTA ZOSTAŁA WYJĘTA
        if ( !file ) {
            // Udostępnij zasób ponownie
            this->is_busy = false;

            // Koniec procedury
            return false;
        }

        //Dopisanie się nie powiodło     
        if ( !file.println( message ) ) {
            // System logs
            #if DEBUG_INFO_FS
                Serial.println( "hommie logger -> Nie udało się dopisać logów!" );
            #endif

            // Udostępnij zasób ponownie
            this->is_busy = false;

            // Zamknij plik
            file.close();

            // Koniec procedury
            return false;
        }

        // Zamknij plik
        file.close();

        // Inkrementuj ilośc logów
        this->save_logs_count++;

        // System logs
        #if DEBUG_INFO_FS
            Serial.println( "hommie logger -> Dopisano log -> " + String( this->save_logs_count ) );
        #endif

    // Bufor kołowy
    } else {
        // Otwieram plik - w celu wyszukania najstarszego logu
        File file = this->get_handler_loc( this->log_file , 0 );

        // Błąd uchwytu pliku - PRAWDOPODOBNIE KARTA ZOSTAŁA WYJĘTA
        if ( !file ) {
            // Udostępnij zasób ponownie
            this->is_busy = false;

            // Koniec procedury
            return false;
        }

        // Wskaźnik od którego należy przepisać plik
        uint32_t pointer_seek = 0;

        // Ostatni odczyt - rozmiar
        size_t length = 0;

        // Bufory
        uint8_t buffer[ 256 ];
        uint8_t _buffer[ 1 ];

        // Weryfikuj plik
        do {
            // Pobierz 1 znak
            length = file.read( _buffer , 1 );

            if ( length && _buffer[ 0 ] == 13 ) {
                // Pobierz 2 znak
                length = file.read( _buffer , 1 );

                if ( length && _buffer[ 0 ] == 10 ) {
                    // Znaleziono najstarszą linię
                    pointer_seek = file.position();
                    
                    // Koniec wyszukiwania
                    break;
                }
            }

        // Wykonuj do końca pliku
        } while ( length );

        // System logs
        #if DEBUG_INFO_FS
            Serial.println( "hommie logger -> (C-BUFFER) znalazłem koniec linii -> " + String( pointer_seek ) );
        #endif

        // Tworzę plik tymczasowy do przepisania
        File _file = this->get_handler_loc( this->_log_file , 1 );

        // Ustaw pointer odczytu
        file.seek( pointer_seek );

        // Weryfikuj plik
        do {
            // Pobierz 2 znaki
            length = file.read( buffer , sizeof( buffer ) );

            // System logs
            #if DEBUG_INFO_FS
                Serial.println( "hommie logger -> (C-BUFFER) czytam stary pakiet -> " + String( length ) );
            #endif

            // Zapisz w nowym pliku
            if ( length ) _file.write( buffer , length );

        // Wykonuj do końca pliku
        } while ( length );

        // System logs
        #if DEBUG_INFO_FS
            Serial.println( "hommie logger -> (C-BUFFER) przepisałem plik na nowy" );
        #endif

        // Zamknij plik do odczytu
        file.close();

        // Dopisz do nowego pliku log
        _file.println( message );

        // Zamknij nowy plik
        _file.close();

        // Usuń stary plik
        this->delete_file( this->log_file );

        // Zmień nazwę nowego pliku na stary plik
        this->rename( this->_log_file , this->log_file );

        // System logs
        #if DEBUG_INFO_FS
            Serial.println( "hommie logger -> C-BUFFER -> Nadpisano najstarszy log" );
        #endif
    }

    // Udostępnij zasób ponownie
    this->is_busy = false;

    // Powodzenie
    return true;
}

/**
    * TODO: Gets the file handle to read
    *
    * @return           {handler}                                                                               File handler
    * @public
*/
File class_hommie_logger::get_read_handler() {
    // Otwieram plik
    return this->get_handler_loc( this->log_file );
}

/**
    * TODO: Download the number of text file lines
    *
    * @param            {handler}                                   path_to_file                                File handler
    * @return           {int}                                       is_used_sd                                  The number of lines in the file
    * @public
*/
int class_hommie_logger::get_line_count( File file ) {
    // Ilość linii
    int line_count = 0;

    // Ostatni odczyt
    size_t length = 0;

    // Bufor 2 znaków
    uint8_t inputChar[ 1 ];

    // Weryfikuj plik
    do {
        // Pobierz 1 znak
        length = file.read( inputChar , 1 );

        // Znak końca linii
        if ( length && inputChar[ 0 ] == 13 ) {
            // Pobierz 2 znak
            length = file.read( inputChar , 1 );

            // Znalazłem znak enter
            if ( length && inputChar[ 0 ] == 10 ) line_count++;
        }

    // Wykonuj do końca pliku
    } while ( length );

    // Zwróć ilość linii
    return line_count;
}

/**
    * TODO: Print logs on port COM
    *
*/
void class_hommie_logger::print_logs( HardwareSerial &COM ) {
    // Buffer
    String buffer;

    // Otwieram plik - w celu wyszukania najstarszego logu
    File file = this->get_handler_loc( this->log_file , 0 );

    // Pobierz 1 znak
    buffer = file.readString();

    // Print
    COM.println( buffer );
}


/****************************************************************************** Obsługa plików i katalogów *******************************************************************************/

/**
    * TODO: Gets the file handle in the file system
    *
    * @param            {string}                                    path_to_file                                Path to the file being tested
    * @param            {boolean}                                   is_used_sd                                  Whether the file should be examined in the internal or external file system
    * @param            {boolean}                                   method_access                               Is access to the file includes reading, writing, adding
    * @example
    *   0 - read only
    *   1 - write
    *   2 - append
    * @return           {handler}                                                                               File handler
    * @public
*/
File class_hommie_logger::get_handler_loc( String path_to_file , uint8_t method_access ) {
    // Uchwyt pliku
    File file;

    // System logs
    #if DEBUG_INFO_FS
        Serial.println( "hommie logger -> Otwieram lokalizację: " + path_to_file );
    #endif

    /**
        * TODO: Otwórz kanał z plikiem...
        * 
        * ? Dostępne flagi: FILE_READ , FILE_WRITE , FILE_APPEND
    */
    file = this->fs_controller.open( path_to_file , ( !method_access ? FILE_READ : method_access == 1 ? FILE_WRITE : FILE_APPEND ) );

    // Zwróć rezultat
    return file;
}

/**
    * TODO: Checks whether a given directory exists
    *
    * @param            {string}                                    path_to_dir                                 Path to the directory tree
    * @param            {boolean}                                   is_used_sd                                  Whether the file should be examined in the internal or external file system
    * @return           {boolean}                                                                               Does the directory exist
    * @public
*/
bool class_hommie_logger::is_dir_exists( String path_to_dir ) {
    // Połącz z katalogiem
    File root_dir = get_handler_loc( path_to_dir , 0 );

    // Nie udało się połączyć
    if ( !root_dir ) {
        // Zamknij połączenie
        root_dir.close();

        // Katalog nie istnieje
        return false;
    }

    // Ścieżka nie jest katalogiem
    if ( !root_dir.isDirectory() ) {
        // Zamknij połączenie
        root_dir.close();

        // Katalog nie istnieje
        return false;
    }

    // Zamknij połączenie
    root_dir.close();

    // Katalog istnieje
    return true;
}

/**
    * TODO: Recursively creates a directory tree
    *
    * @param            {string}                                    path_to_dir                                 Path to the directory tree
    * @param            {boolean}                                   is_used_sd                                  Whether the file should be examined in the internal or external file system
    * @return           {boolean}                                                                               Has the procedure been carried out correctly
    * @public
*/
bool class_hommie_logger::create_dir( String path_to_dir ) {
    // System logs
    #if DEBUG_INFO_FS
        Serial.println( "hommie logger -> Tworzę katalog: " + path_to_dir );
    #endif

    // Sprawdzenie czy plik istnieje w systemie plików...
    return this->fs_controller.mkdir( path_to_dir );
}

/**
    * TODO: Rename file
    *
    * @param            {string}                                    path_to_file                                Current name
    * param             {string}                                    new_path_to_file                            New name
    * @param            {boolean}                                   is_used_sd                                  Whether the file should be examined in the internal or external file system
    * @return           {boolean}                                                                               Czy plik istnieje
    * @public
*/
bool class_hommie_logger::rename( String path_to_file , String new_path_to_file ) {
    // System logs
    #if DEBUG_INFO_FS
        Serial.println( "hommie logger -> Zmieniam nazwę: " + path_to_file + " na " + new_path_to_file );
    #endif

    // Sprawdzenie czy plik istnieje w systemie plików...
    return this->fs_controller.rename( path_to_file , new_path_to_file );
}

/**
    * TODO: Delete file
    *
    * @param            {string}                                    path_to_file                                File path
    * @param            {boolean}                                   is_used_sd                                  Whether the file should be examined in the internal or external file system
    * @return           {boolean}                                                                               Has the procedure been carried out correctly
    * @public
*/
bool class_hommie_logger::delete_file( String path_to_file ) {
    // System logs
    #if DEBUG_INFO_FS
        Serial.println( "hommie logger -> Usuwam plik: " + path_to_file );
    #endif

    // Sprawdzenie czy plik istnieje w systemie plików...
    return this->fs_controller.remove( path_to_file );
}