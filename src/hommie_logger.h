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
    @todo           Definition of the logger class
********************************************************************************************************************/


// UŻYJ KLASY ŚRODOWISKOWEJ - Arduino SDK
#include <Arduino.h>

// OBSŁUGA SYSTEMU PLIKÓW
#include <FS.h>

// Obsługa wewnętrznego systemu plików
#include <SPIFFS.h>


/**
    * TODO: Definicja klasy
    *
    * @public
*/
class class_hommie_logger {
    // Metody publiczne
    public:
        // TODO: File system initialization
        bool init( bool auto_start = true );

        // TODO: Set controller
        void set_controller( fs::FS &fs );

        // TODO: Set max logs
        void set_max_logs( uint16_t max_logs = 30 );

        // TODO: Set usage timestamp inline
        void set_max_logs( bool use_timestamp = true );

        // TODO: Print logs on port COM
        void print_logs( HardwareSerial &COM );

        // TODO: Creates a new record in the file using the circular buffer algorithm
        bool append( String message );

    // Metody i zmienne prywatne
    private:
        // Buffer i busy
        bool is_busy                = false;

        // Log dir
        String log_dir              = "/logs";

        // File log
        String log_file             = log_dir + "/log.log";

        // Temp file log
        String _log_file            = log_dir + "/_log.log";

        // Saved logs
        int save_logs_count         = -1;

        // Max. log
        uint16_t max_logs_line      = 30;

        // File system controller - internal by default
        fs::FS &fs_controller       = SPIFFS;

        // Usage uptime
        bool is_use_uptime          = true;

        // TODO: Download the number of text file lines
        int get_line_count( File file );

        // TODO: Pobiera uchwyt do pliku w systemie plików
        File get_handler_loc( String path_to_file , uint8_t method_access = 0 );

        // TODO: Sprawdza czy dany katalog istnieje
        bool is_dir_exists( String path_to_dir );

        // TODO: Rekurencyjnie tworzy drzewo katalogów
        bool create_dir( String path_to_dir );

        // TODO: Usuwa plik
        bool delete_file( String path_to_file );

        // TODO: Zmień nazwę
        bool rename( String path_to_file , String new_path_to_file );
};

// Przypisz klasę na zmienną dostępową
extern class_hommie_logger hommie_logger;