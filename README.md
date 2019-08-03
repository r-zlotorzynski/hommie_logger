# hommie LOGGER (for Arduino)





## What is this?

*Creates logs in file using the circular buffer algorithm*

By default, internal dataflash (SPIFFS) is used to store logs, but it is also possible to use an SD card or RAM (soon).





## Board

Library tested on the ESP32 platform.




## Using the library

Examples of use can be found in the catalog: [Examples](https://github.com/r-zlotorzynski/hommie_logger/tree/master/examples/)

Initialization:

```
// Define library
#include "hommie_logger.h"


// Optional - copy class on variable
class_hommie_logger hommie_logger;
```

Using a library with SPIFFS:

```
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
    hommie_logger.append( "Hello hommie!" );

    // Delay for tests
    delay( 5000 );

    // Show file logs
    Serial.println( "------------ List save logs ------------" );
        hommie_logger.print_logs( Serial );
    Serial.println( "----------------------------------------" );
}
```

Reading the file:

```
// Buffer
String buffer;

// Download the handle to the file
File file = hommie_logger.get_read_handler();

// Read file (example). You can use any method of the file system!
buffer = file.readString();

// Print - Serial COM
Serial.println( buffer );
```