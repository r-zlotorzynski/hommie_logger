![https://hommie.pl](https://repository-images.githubusercontent.com/200381668/44b8ac80-b616-11e9-84df-30c062688a73)

# [hommie](https://hommie.pl) LOGGER (for Arduino)





## What is this?

*Creates logs in file using the circular buffer algorithm*

By default, internal dataflash (SPIFFS) is used to store logs, but it is also possible to use an SD card or RAM (soon).





## Board

Library tested on the ESP32 platform.




## Using the library

Examples of use can be found in the catalog: [Examples](https://github.com/r-zlotorzynski/hommie_logger/tree/master/examples/)

## Initialization:

```
// Define library
#include "hommie_logger.h"


// Optional - copy class on variable
class_hommie_logger hommie_logger;
```

## Using a library with SPIFFS example:

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

## Using a library with SD controller example:

```
// Support for the external file system
#include <SD.h>


// Setup
void setup ( void ) {
    // Start COM for debug
    Serial.begin( 115200 );

    // Delay...
    delay( 500 );

    // Initialize lib
    hommie_logger.init( false );

    // Add new log
    hommie_logger.set_controller( SD );
}
```

## Reading the file example:

```
// Filesystem library
#include <FS.h>


// Setup
void setup ( void ) {}

// Loop
void loop ( void ) {
    // Buffer
    String buffer;

    // Download the handle to the file
    File file = hommie_logger.get_read_handler();

    // Read file (example). You can use any method of the file system!
    buffer = file.readString();

    // Print - Serial COM
    Serial.println( buffer );

    // Delay...
    delay( 5000 );
}
```

## Print file example:

```
// Start COM for debug
Serial.begin( 115200 );

// Delay...
delay( 500 );

// Print complete file on serial COM
hommie_logger.print_logs( Serial );
```

## Creates a new record in the file using the circular buffer algorithm:

```
// Add new log
hommie_logger.append( "Hello hommie!" );
```

## Set max logs:

```
// Set max logs. Default: 30. Max: 65535
hommie_logger.set_max_logs( 30 );
```

## Set usage timestamp inline:

```
// Set usage timestamp inline. Deafult: true
hommie_logger.set_usage_timestamp( false );

/* OUTPUT */
    Hello hommie!


// Set usage timestamp inline. Deafult: true
hommie_logger.set_usage_timestamp( true );

/* OUTPUT - Where 1234 is the time stamp (millisecond of the device's working time) */
    1234 - Hello hommie!
```



# Made by [hommie](https://hommie.pl)

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
