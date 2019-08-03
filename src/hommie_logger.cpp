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
    @file           ./src/hommie_logger.cpp
    @todo           The source of the logger program
********************************************************************************************************************/


// UŻYJ KLASY ŚRODOWISKOWEJ - Arduino SDK
#include <Arduino.h>

// OBSŁUGA SYSTEMU PLIKÓW
#include <FS.h>

// OBSŁUGA KART SD
#include <SD.h>

// Obsługa wewnętrznego systemu plików
#include <SPIFFS.h>

// Obsługa wewnętrznego systemu plików
#include "hommie_logger_const.h"

// Obsługa wewnętrznego systemu plików
#include "hommie_logger.h"


/**
    * TODO: File system initialization
    *
    * @param        {boolean}                                       auto_start                                  Automatically initialize the module using the default settings
    * @return       {boolean}                                                                                   Successfully completed or not
    * @public
*/
bool class_hommie_logger::init( bool auto_start ) {
    #if DEBUG_INFO_FS
        // Logi systemowe
        Serial.println( "hommie logger -> Initialization..." );
    #endif

    // Domyślnie ustaw wewnętrzny system plików
    if ( auto_start ) {
        // Próba inicjalizacji wewnętrznego systemu plików
        if ( !SPIFFS.begin( true ) ) {
            // Logi systemowe
            Serial.println( "hommie logger -> OK!" );
        } else {
            // Logi systemowe
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
    * TODO: Creates a new record in the file using the circular buffer algorithm
    *
    * @return           {string}                                    path_to_file                                Ścieżka do badanego pliku
    * @return           {boolean}                                                                               Powodzenie lub nie
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

        // Logi systemowe
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
            // Logi systemowe
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

        // Logi systemowe
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

        // Logi systemowe
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

            // Logi systemowe
            #if DEBUG_INFO_FS
                Serial.println( "hommie logger -> (C-BUFFER) czytam stary pakiet -> " + String( length ) );
            #endif

            // Zapisz w nowym pliku
            if ( length ) _file.write( buffer , length );

        // Wykonuj do końca pliku
        } while ( length );

        // Logi systemowe
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

        // Logi systemowe
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
    * TODO: Download the number of text file lines
    *
    * @param            {handler}                               path_to_file                            Uchwyt do pliku
    * @return           {int}                                   is_used_sd                              Ilość linii w pliku
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
    * TODO: Pobiera uchwyt do pliku w systemie plików
    *
    * @param            {string}                                path_to_file                            Ścieżka do badanego pliku
    * @param            {boolean}                               is_used_sd                              Czy plik ma zostać zbadany w wewnętrznym czy zewnętrznym systemie plików
    * @param            {boolean}                               method_access                           Czy dostęp do pliku obejmuje odczyt, zapis, dopisanie
    * @example
    *   0 - tylko odczyt
    *   1 - zapis
    *   2 - dopisanie do pliku
    * @return           {handler}                                                                       Uchwyt do pliku
    * @public
*/
File class_hommie_logger::get_handler_loc( String path_to_file , uint8_t method_access ) {
    // Uchwyt pliku
    File file;

    // Logi systemowe
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
    * TODO: Sprawdza czy dany katalog istnieje
    *
    * @param            {string}                                path_to_dir                             Ścieżka do drzewa katalogów
    * @param            {boolean}                               is_used_sd                              Czy plik ma zostać zbadany w wewnętrznym czy zewnętrznym systemie plików
    * @return           {boolean}                                                                       Czy katalog istnieje
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
    * TODO: Rekurencyjnie tworzy drzewo katalogów
    *
    * @param            {string}                                path_to_dir                             Ścieżka do drzewa katalogów
    * @param            {boolean}                               is_used_sd                              Czy plik ma zostać zbadany w wewnętrznym czy zewnętrznym systemie plików
    * @return           {boolean}                                                                       Czy procedura została zrealizowana poprawnie
    * @public
*/
bool class_hommie_logger::create_dir( String path_to_dir ) {
    // Logi systemowe
    #if DEBUG_INFO_FS
        Serial.println( "hommie logger -> Tworzę katalog: " + path_to_dir );
    #endif

    // Sprawdzenie czy plik istnieje w systemie plików...
    return this->fs_controller.mkdir( path_to_dir );
}

/**
    * TODO: Zmień nazwę
    *
    * @param            {string}                                path_to_file                            Nazwa aktualna
    * param             {string}                                new_path_to_file                        Nowa nazwa
    * @param            {boolean}                               is_used_sd                              Czy plik ma zostać zbadany w wewnętrznym czy zewnętrznym systemie plików
    * @return           {boolean}                                                                       Czy plik istnieje
    * @public
*/
bool class_hommie_logger::rename( String path_to_file , String new_path_to_file ) {
    // Logi systemowe
    #if DEBUG_INFO_FS
        Serial.println( "hommie logger -> Zmieniam nazwę: " + path_to_file + " na " + new_path_to_file );
    #endif

    // Sprawdzenie czy plik istnieje w systemie plików...
    return this->fs_controller.rename( path_to_file , new_path_to_file );
}

/**
    * TODO: Usuwa plik
    *
    * @param            {string}                                path_to_file                            Ścieżka do pliku
    * @param            {boolean}                               is_used_sd                              Czy plik ma zostać zbadany w wewnętrznym czy zewnętrznym systemie plików
    * @return           {boolean}                                                                       Czy procedura została zrealizowana poprawnie
    * @public
*/
bool class_hommie_logger::delete_file( String path_to_file ) {
    // Logi systemowe
    #if DEBUG_INFO_FS
        Serial.println( "hommie logger -> Usuwam plik: " + path_to_file );
    #endif

    // Sprawdzenie czy plik istnieje w systemie plików...
    return this->fs_controller.remove( path_to_file );
}