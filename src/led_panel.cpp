#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
#include <gamma.h>
#include <RGBmatrixPanel.h>
#include <TinyGPS++.h>
#include <TimeLib.h>
#include "RTClib.h"
#include <Adafruit_GPS.h>
#include <Wire.h>

#if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)
// Required for Serial on Zero based boards
#define Serial SERIAL_PORT_USBVIRTUAL
#endif

// BME680 libraries
// Adafruit Unified Sensor Library
// Adafruit BME680 Library
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME680 bme; // I2C

// https://learn.adafruit.com/using-atsamd21-sercom-to-add-more-spi-i2c-serial-ports/overview
// https://learn.adafruit.com/adafruit-bme680-humidity-temperature-barometic-pressure-voc-gas/overview
// https://learn.adafruit.com/32x16-32x32-rgb-led-matrix/overview
// https://learn.adafruit.com/adafruit-metro-m0-express-designed-for-circuitpython
// https://github.com/2dom/PxMatrix

#define CLK 8 // USE THIS ON ARDUINO UNO, ADAFRUIT METRO M0, etc.
//#define CLK A4 // USE THIS ON METRO M4 (not M0)
//#define CLK 11 // USE THIS ON ARDUINO MEGA
#define OE 9
#define LAT 10
#define A A0
#define B A1
#define C A2

int last_update_time = 0;
int current_time = 1571022337;
// Last parameter = 'true' enables double-buffering, for flicker-free,
// buttery smooth animation.  Note that NOTHING WILL SHOW ON THE DISPLAY
// until the first call to swapBuffers().  This is normal.
RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, true);
#define SerialGPS Serial1
Adafruit_GPS GPS(&SerialGPS);
TinyGPSPlus gps;
unsigned long age;
int Year, Month, Day, Hour, Minute, Second;
// GPS/UTC time offset
const int offset = -6;

#define PRINT_SECONDS false
bool printSerialData = true;
bool PRINT_SENSOR_DATA = false;
const byte interruptPin = 13;
// Initialize RTC module
RTC_DS3231 rtc;

// 0 to 254 (<70 turns LED's off)
uint8_t BRIGHTNESS = 70;
// 0 to 254
uint8_t SATURATION = 254;
//0 to 1535 (ROYGBIV)
long COLOR = int(297.0 / 360.0 * 1535);

bool GAMMA = true;
bool TWELVE_HOUR = true;

bool VALID_GPS_FIX = false;

String top_current_mode = "time";
String bottom_current_mode = "date";

String possible_modes[] = {
	"date",
	"latitude",
	"longitude",
	"altitude",
	"humidity",
	"temperature"
};

// Optional mode that rotates between display settings
String bottom_alternating_mode = "fixed";
int bottom_rotate_time = 10;
int time_since_last_rotation = 0;

DateTime local_time, utc_time;

// Fill background with red/green with alternate color text
String color_mode = "fuck_colorblind_people";

void syncGPS()
{
	/* 	Sets the RTC clock to the GPS clock, if there is a valid time received
		from the GPS receiver
	*/
	while (SerialGPS.available())
	{
		if (gps.encode(SerialGPS.read()))
		{ // process gps messages
			if (gps.time.isValid())
			{
				Hour = gps.time.hour();
				Minute = gps.time.minute();
				Second = gps.time.second();
				Year = gps.date.year();
				Month = gps.date.month();
				Day = gps.date.day();

				last_update_time = current_time;
				// Set RTC to UTC time
				utc_time = DateTime(Year, Month, Day, Hour, Minute, Second);
				local_time = utc_time.operator+(TimeSpan(3600 * offset));
				rtc.adjust(local_time);
			}
		}
	}
}

void setPanel(int hour, int minute, int second)
{
	// Set the time on the RGB panel
	matrix.fillScreen(0);

	//matrix.setTextColor(matrix.Color333(7, 7, 7));
	uint16_t color = matrix.ColorHSV(COLOR, SATURATION, BRIGHTNESS, GAMMA);
	matrix.setTextColor(color);
	if (top_current_mode == "time")
	{
		String hour_string = String(hour);
		String minute_string = String(minute);
		if (hour < 10)
		{
			hour_string = String("0" + hour_string);
		}

		if (minute < 10)
		{
			minute_string = String("0" + minute_string);
		}
		if (PRINT_SECONDS)
		{
			matrix.setCursor(0, 0); // start at top left, with one pixel of spacing

			matrix.print(hour_string);

			matrix.setCursor(11, 0);
			matrix.print(minute_string);

			matrix.setCursor(22, 0);
			matrix.print(second);
		}
		else
		{
			matrix.setCursor(1, 0); // start at top left, with one pixel of spacing

			matrix.print(hour_string);

			matrix.print(":");

			// matrix.setCursor(21, 0);
			matrix.print(minute_string);
		}
	}
	// Set to bottom row
	matrix.setCursor(1, 8);
	if (bottom_current_mode == "temperature")
	{
		bme.readTemperature();
		matrix.print(bme.temperature);
	}
	else if (bottom_current_mode == "humidity")
	{
		bme.readHumidity();
		matrix.print(bme.humidity);
	} 
	else if (bottom_current_mode == "latitude")
	{
		matrix.print(gps.location.lat());
	}
	else if (bottom_current_mode == "longitude")
	{
		matrix.print(gps.location.lng());
	}
	else if (bottom_current_mode == "altitude"){
		matrix.print(gps.altitude.feet());
	}
	else if (bottom_current_mode == "date"){
		matrix.print(local_time.month());
		matrix.print("-");
		matrix.print(local_time.day());
	}

	// For smooth animation -- drawing always takes place in the "back" buffer;
	// this method pushes it to the "front" for display.  Passing "true", the
	// updated display contents are then copied to the new back buffer and can
	// be incrementally modified.  If "false", the back buffer then contains
	// the old front buffer contents -- your code can either clear this or
	// draw over every pixel.  (No effect if double-buffering is not enabled.)
	matrix.swapBuffers(false);
}

void buttonFunction()
{
	Serial.println("The button has been pressed!");
	syncGPS();
}

void setup()
{
	// put your setup code here, to run once:
	Serial.begin(9600);
	Serial.println("Starting up clock");
	matrix.begin();
	SerialGPS.begin(9600);

	rtc.begin();

	// fill the screen with black
	matrix.fillScreen(0);
	matrix.setTextWrap(false); // Allow text to run off right edge
	matrix.setTextSize(0);	 // size 1 == 8 pixels high

	if (!bme.begin())
	{
		Serial.println("Could not find a valid BME680 sensor, check wiring!");
		while (1)
			;
	}

	// Set up oversampling and filter initialization
	bme.setTemperatureOversampling(BME680_OS_8X);
	bme.setHumidityOversampling(BME680_OS_2X);
	bme.setPressureOversampling(BME680_OS_4X);
	bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
	bme.setGasHeater(320, 150); // 320*C for 150 ms

	attachInterrupt(digitalPinToInterrupt(interruptPin), buttonFunction, RISING);
	matrix.fillScreen(0);
	while (year() == 2000)
	{
		syncGPS();
	}
	setPanel(hour(), minute(), second());
}

void loop()
{
	// put your main code here, to run repeatedly:

	syncGPS();

	DateTime now = rtc.now();

		if (TWELVE_HOUR && now.hour() > 12){
			setPanel(now.hour() - 12, now.minute(), now.second());
		} else {
			setPanel(now.hour(), now.minute(), now.second());
		}

	if (PRINT_SENSOR_DATA)
	{
		//*
		//Serial.print(now.year());
		//Serial.print(now.month());
		//Serial.print(now.day());
		//Serial.print(now.hour());
		//Serial.print(now.minute());
		//Serial.print(now.second());
		//Serial.println();
		//
		Serial.print("Temperature = ");
		Serial.print(bme.temperature);
		Serial.println(" *C");

		Serial.print("Pressure = ");
		Serial.print(bme.pressure / 100.0);
		Serial.println(" hPa");

		Serial.print("Humidity = ");
		Serial.print(bme.humidity);
		Serial.println(" %");

		Serial.print("Gas = ");
		Serial.print(bme.gas_resistance / 1000.0);
		Serial.println(" KOhms");

		Serial.print("Approx. Altitude = ");
		Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
		Serial.println(" m");
	}
}
