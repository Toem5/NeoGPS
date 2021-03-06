#include <Arduino.h>
#include "ubxNMEA.h"

//======================================================================
//  Program: PUBX.ino
//
//  Description:  This program parses NMEA proprietary messages from
//     ublox devices.  It is an extension of NMEAfused.ino.
//
//  Prerequisites:
//     1) You have a ublox GPS device
//     2) NMEAfused.ino works with your device
//     3) You have installed ubxNMEA.H and ubxNMEA.CPP
//     4) At least one NMEA standard or proprietary sentence has been enabled.
//     5) Implicit Merging is disabled.
//
//  Serial is for debug output to the Serial Monitor window.
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

#include "Streamers.h"
#ifdef NeoHWSerial_h
  #define DEBUG_PORT NeoSerial
#else
  #define DEBUG_PORT Serial
#endif

//------------------------------------------------------------
// Check that the config files are set up properly

#if !defined( NMEAGPS_PARSE_GGA ) & !defined( NMEAGPS_PARSE_GLL ) & \
    !defined( NMEAGPS_PARSE_GSA ) & !defined( NMEAGPS_PARSE_GSV ) & \
    !defined( NMEAGPS_PARSE_RMC ) & !defined( NMEAGPS_PARSE_VTG ) & \
    !defined( NMEAGPS_PARSE_ZDA ) & !defined( NMEAGPS_PARSE_GST ) & \
    !defined( NMEAGPS_PARSE_PUBX_00 ) & !defined( NMEAGPS_PARSE_PUBX_04 )

  #error No NMEA sentences enabled: no fix data available for fusing.

#endif

#ifndef NMEAGPS_EXPLICIT_MERGING
  #error You must define NMEAGPS_EXPLICIT_MERGING in NMEAGPS_cfg.h
#endif

//------------------------------------------------------------

static ubloxNMEA gps         ; // This parses received characters
static gps_fix   fused;

//----------------------------------------------------------------

static void poll()
{
  gps.send_P( &gps_port, F("PUBX,00") );
  gps.send_P( &gps_port, F("PUBX,04") );
}

//----------------------------------------------------------------

static void doSomeWork()
{
  // Print all the things!
  trace_all( DEBUG_PORT, gps, fused );

  //  Ask for the proprietary messages again
  poll();
  
} // doSomeWork

//------------------------------------

static void GPSloop()
{  
  while (gps.available( gps_port )) {
    fused = gps.read();

    doSomeWork();
  }
} // GPSloop
  
//--------------------------

void setup()
{
  // Start the normal trace output
  DEBUG_PORT.begin(9600);
  while (!DEBUG_PORT)
    ;

  DEBUG_PORT.print( F("PUBX: started\n") );
  DEBUG_PORT.print( F("fix object size = ") );
  DEBUG_PORT.println( sizeof(gps.fix()) );
  DEBUG_PORT.print( F("ubloxNMEA object size = ") );
  DEBUG_PORT.println( sizeof(gps) );
  DEBUG_PORT.println( F("Looking for GPS device on " USING_GPS_PORT) );

  trace_header( DEBUG_PORT );

  DEBUG_PORT.flush();
  
  // Start the UART for the GPS device
  gps_port.begin(9600);

  // Ask for the special PUBX sentences
  poll();
}

//--------------------------

void loop()
{
  GPSloop();
}
