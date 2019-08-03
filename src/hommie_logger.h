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


// Use enviromental class - Arduino SDK
#include <Arduino.h>

// Filesystem library
#include <FS.h>

// Support for the internal file system
#include <SPIFFS.h>


/**
    * TODO: Class defined
    *
    * @public
*/
class class_hommie_logger {
    // Public method
    public:
        // TODO: File system initialization
        bool init( bool auto_start = true );

        // TODO: Set controller
        void set_controller( fs::FS &fs );

        // TODO: Set max logs
        void set_max_logs( uint16_t max_logs = 30 );

        // TODO: Set usage timestamp inline
        void set_usage_timestamp( bool use_timestamp = true );

        // TODO: Print logs on port COM
        void print_logs( HardwareSerial &COM );

        // TODO: Creates a new record in the file using the circular buffer algorithm
        bool append( String message );

        // TODO: Gets the file handle to read
        File get_read_handler();

    // Private methods and variables
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

        // TODO: Gets the file handle in the file system
        File get_handler_loc( String path_to_file , uint8_t method_access = 0 );

        // TODO: Checks whether a given directory exists
        bool is_dir_exists( String path_to_dir );

        // TODO: Recursively creates a directory tree
        bool create_dir( String path_to_dir );

        // TODO: Delete file
        bool delete_file( String path_to_file );

        // TODO: Renemae file
        bool rename( String path_to_file , String new_path_to_file );
};

// Assign a class to an access variable
extern class_hommie_logger hommie_logger;