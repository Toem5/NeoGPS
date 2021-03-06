#include <Arduino.h>
#include "NMEAGPS.h"

//======================================================================
//  Program: NMEAtimezone.ino
//
//  Description:  This program shows how to offset the GPS dateTime member
//          into your specific timezone.  GPS devices do not know which
//          timezone they are in, so they always report a UTC time.  This
//          is the same as GMT.
//
//  Prerequisites:
//     1) NMEA.ino works with your device
//     2) GPS_FIX_TIME is enabled in GPSfix_cfg.h
//     3) NMEAGPS_PARSE_RMC is enabled in NMEAGPS_cfg.h.  You could use 
//        any sentence that contains a time field.  Be sure to change the 
//        "if" statement in GPSloop from RMC to your selected sentence.
//
//  'Serial' is for debug output to the Serial Monitor window.
//
//======================================================================

#if defined( UBRR1H )
  // Default is to use Serial1 when available.  You could also
  // use NeoHWSerial, especially if you want to handle GPS characters
  // in an Interrupt Service Routine.
  //#include <NeoHWSerial.h>
#else  
  // Only one serial port is available, uncomment one of the following:
  //#include <NeoICSerial.h>
  #include <NeoSWSerial.h>
  //#include <SoftwareSerial.h> /* NOT RECOMMENDED */
#endif
#include "GPSport.h"

#ifdef NeoHWSerial_h
  #define DEBUG_PORT NeoSerial
#else
  #define DEBUG_PORT Serial
#endif

static NMEAGPS  gps         ; // This parses received characters
static gps_fix  fix_data;

#if !defined(GPS_FIX_TIME)
  #error You must define GPS_FIX_TIME in GPSfix_cfg.h!
#endif

#if !defined(NMEAGPS_PARSE_RMC)
  #error You must define NMEAGPS_PARSE_RMC in NMEAGPS_cfg.h!
#endif

//----------------------------------------------------------------

static void doSomeWork( const gps_fix & fix )
{
  // Display the local time

  if (fix.valid.time) {
    // Set these values to the offset of your timezone from GMT
    static const int32_t         zone_hours   = -4L; // EST
    static const int32_t         zone_minutes =  0L; // usually zero
    static const NeoGPS::clock_t zone_offset  =
                      zone_hours   * NeoGPS::SECONDS_PER_HOUR +
                      zone_minutes * NeoGPS::SECONDS_PER_MINUTE;

    NeoGPS::time_t localTime( fix.dateTime + zone_offset );

    DEBUG_PORT << localTime;
  }
  DEBUG_PORT.println();

} // doSomeWork

//------------------------------------

static void GPSloop()
{  
  while (gps.available( gps_port ))
    doSomeWork( gps.read() );

} // GPSloop

//--------------------------

void setup()
{
  // Start the normal trace output
  DEBUG_PORT.begin(9600);
  while (!DEBUG_PORT)
    ;

  DEBUG_PORT.print( F("NMEAtimezone.INO: started\n") );
  DEBUG_PORT.print( F("fix object size = ") );
  DEBUG_PORT.println( sizeof(gps.fix()) );
  DEBUG_PORT.print( F("NMEAGPS object size = ") );
  DEBUG_PORT.println( sizeof(gps) );
  DEBUG_PORT.println( F("Looking for GPS device on " USING_GPS_PORT) );
  DEBUG_PORT.println( F("Local time") );
  DEBUG_PORT.flush();
  
  // Start the UART for the GPS device
  gps_port.begin( 9600 );
}

//--------------------------

void loop()
{
  GPSloop();
}
