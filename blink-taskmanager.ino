#pragma GCC optimize("Os")

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C
#define DISPLAYROTATION 2
#define TEXTWRAP false
#define BAUDRATE 38400
#define FONT Aldrich_Regular5pt7b
#define RECVBYTES 34
#define PARTICLES 25
#define SCANID F("BTM")
#define EXPIRY 2000
#define TIMEOUT 50

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, 4) ;

#include "libs/arts.h"
#include "libs/drawBitmap.h"
#include "libs/substr.h"
#include "fonts/aldrich-regular-5.h"
#include "libs/particles.h"
#include "libs/initDisplayState.h"

#include "libs/crc32.h"

unsigned char receiveData(char *recv, unsigned char len) {
	if(Serial.read() == 33) {
		recv[0] = '\0' ;
		unsigned char counter = 0 ;
		char str[2] ;
		char temp_ch = 0 ;
		unsigned long tm = millis() + TIMEOUT ;

		while(1) {
			if (millis() > tm) break ;

			temp_ch = Serial.read() ;
			if(temp_ch == 126) break ;

			if(temp_ch > 47 && temp_ch < 58 && counter < len) {
				sprintf(str, "%c", temp_ch) ;
				recv[counter++] = str[0] ;
			}
		}

		recv[++counter] = '\0' ;
		if (strlen(recv) == len) {
			Serial.print(getCRC(recv)) ;
			return 1 ;
		} else {
			return 0 ;
		}
	}

	return 0 ;
}

void getDataInit() {
	while(1) {
		Serial.println(SCANID) ;
		Serial.flush() ;

		initDisplayState() ;
		drawParticles() ;
		display.display() ;

		if(Serial.read() == 35) {
			unsigned long tm = millis() + TIMEOUT ;

			while(tm > millis()) {
				display.clearDisplay() ;
				display.setCursor(20, 30) ;
				display.println(F("Synchronizing...")) ;
				drawParticles() ;
				display.display() ;

				Serial.write(0x7e) ;
				Serial.flush() ;
			}

			return ;
		}
	}

	return ;
}

char tempStr[12] ;
char recv[RECVBYTES + 1] ;

unsigned long c_time ;

// Usages
unsigned char cpuU, memU,swapU ;
unsigned short netDL, netUL ;
unsigned short ioR, ioW ;

// Floating point parts
unsigned char netDLM, netULM ;
unsigned char ioRM, ioWM ;
unsigned long refresh ;

// Units
unsigned char netDL_Unit = 0 ;
unsigned char netUL_Unit = 0 ;
unsigned char ioR_Unit = 0 ;
unsigned char ioW_Unit = 0 ;

// Flashing delays
unsigned short cpu_delay = 0 ;
unsigned short mem_delay = 0 ;
unsigned short swap_delay = 0 ;
unsigned short net_delay = 0 ;
unsigned short io_delay = 0 ;

// Flashing delay = millis() + n_delay
unsigned long cpu_tm = 0 ;
unsigned long mem_tm = 0 ;
unsigned long swap_tm = 0 ;
unsigned long net_tm = 0 ;
unsigned long io_tm = 0 ;

unsigned long dataReceiveExpiry = 0 ;

void setup() {
	Serial.begin(BAUDRATE) ;

	if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
		while(1) ;
	}

	display.setRotation(DISPLAYROTATION) ;
	display.setFont(&FONT) ;
	display.setTextSize(1) ;
	display.setTextWrap(TEXTWRAP) ;
	display.setTextColor(SSD1306_WHITE) ;

	initParticles() ;
	getDataInit() ;
	dataReceiveExpiry = millis() + EXPIRY ;
}

const char *getUnit(unsigned char unit) {
	if (unit == 0) return "B" ;
	else if (unit == 1) return "KB" ;
	else if (unit == 2) return "MB" ;
	else if (unit == 3) return "GB" ;
	else if (unit == 4) return "TB" ;
	else if (unit == 5) return "PB" ;
	else if (unit == 6) return "EB" ;
	else if (unit == 7) return "ZB" ;
	else return "YB" ;
}

// Only convert bytes to Megabytes
// Needed only to blink the lights
unsigned long getVal(unsigned short val, unsigned char unit) {
	if (unit == 0) return val ;
	else if (unit == 1) return val * 1000UL ;
	else if (unit == 2) return val * 1000000UL ;
	else return 2000000000UL ;
}

unsigned long byte_value1 ;

void loop() {
	/*
	 * Get bytes the size of RECVBYTES in this format:
	 *	cpu(100) memUsed(100) swapUsed(100)
	 *	netDownload(9991) netUpload(9991)
	 *	ioWrite(9991) ioRead(9991) Refresh(1)
	 *
	 * Total bytes to receive 25
	 */

	if (receiveData(recv, RECVBYTES)) {
		substr(recv, tempStr, 0, 2) ;
		cpuU = atoi(tempStr) ;

		substr(recv, tempStr, 3, 5) ;
		memU = atoi(tempStr) ;

		substr(recv, tempStr, 6, 8) ;
		swapU = atoi(tempStr) ;

		substr(recv, tempStr, 9, 11) ;
		netDL = atoi(tempStr) ;
		substr(recv, tempStr, 12, 13) ;
		netDLM = atoi(tempStr) ;
		substr(recv, tempStr, 14, 14) ;
		netDL_Unit = atoi(tempStr) ;

		substr(recv, tempStr, 15, 17) ;
		netUL = atoi(tempStr) ;
		substr(recv, tempStr, 18, 19) ;
		netULM = atoi(tempStr) ;
		substr(recv, tempStr, 20, 20) ;
		netUL_Unit = atoi(tempStr) ;

		substr(recv, tempStr, 21, 23) ;
		ioR = atoi(tempStr) ;
		substr(recv, tempStr, 24, 25) ;
		ioRM = atoi(tempStr) ;
		substr(recv, tempStr, 26, 26) ;
		ioR_Unit = atoi(tempStr) ;

		substr(recv, tempStr, 27, 29) ;
		ioW = atoi(tempStr) ;
		substr(recv, tempStr, 30, 31) ;
		ioWM = atoi(tempStr) ;
		substr(recv, tempStr, 32, 32) ;
		ioW_Unit = atoi(tempStr) ;

		substr(recv, tempStr, 33, 33) ;
		refresh = tempStr[0] == '1' ? millis() + TIMEOUT : 0 ;

		// Activate Blinking Icons when the usages are > 75%
		cpu_delay = cpuU > 90 ? 62 : cpuU > 80 ? 125 : cpuU > 70 ? 250 : cpuU > 60 ? 500 : 0 ;
		mem_delay = memU > 90 ? 62 : memU > 80 ? 125 : memU > 70 ? 250 : memU > 60 ? 500 : 0 ;
		swap_delay = swapU > 90 ? 62 : swapU > 80 ? 125 : swapU > 70 ? 250 : swapU > 60 ? 500 : 0 ;

		byte_value1 = getVal(ioR, ioR_Unit) + getVal(ioW, ioW_Unit) ;

		// 250 MB/s
		if (byte_value1 > 250000000UL) io_delay = 62 ;
		// 150 MB/s
		else if (byte_value1 > 150000000UL) io_delay = 125 ;
		// 90 MB/s
		else if (byte_value1 > 90000000UL) io_delay = 250 ;
		// 30 MB/s
		else if (byte_value1 > 30000000UL) io_delay = 500 ;
		// 10 MB/s
		else if (byte_value1 > 10000000UL) io_delay = 1000 ;
		else io_delay = 0 ;

		byte_value1 = getVal(netDL, netDL_Unit) + getVal(netUL, netUL_Unit) ;

		// 2 MB/s
		if (byte_value1 > 2000000UL) net_delay = 62 ;
		// 1 MB/s
		else if (byte_value1 > 1000000UL) net_delay = 125 ;
		// 500 KB/s
		else if (byte_value1 > 500000UL) net_delay = 250 ;
		// 250 KB/s
		else if(byte_value1 > 250000UL) net_delay = 500 ;
		// 50 KB/s
		else if(byte_value1 > 50000UL) net_delay = 1000 ;
		else net_delay = 0 ;

		c_time = millis() ;
		dataReceiveExpiry = c_time + EXPIRY ;
	}

	c_time = millis() ;
	if (c_time > dataReceiveExpiry) {
		getDataInit() ;
	}

	display.clearDisplay() ;
	// Row 1
	// CPU
	if (cpu_delay) {
		if (c_time < cpu_tm) drawBitmap((char *)cpuArt, 26, 4) ;
		else if (c_time > cpu_tm + cpu_delay) cpu_tm = c_time + cpu_delay ;
	} else {
		drawBitmap((char *)cpuArt, 26, 4) ;
	}

	sprintf(tempStr, "%3d%%", cpuU) ;
	display.setCursor(20, 24) ;
	display.print(tempStr) ;

	// Mem
	if (mem_delay) {
		if (c_time < mem_tm) drawBitmap((char *)memArt, 58, 4) ;
		else if (c_time > mem_tm + mem_delay) mem_tm = c_time + mem_delay ;
	} else {
		drawBitmap((char *)memArt, 58, 4) ;
	}

	sprintf(tempStr, "%3d%%", memU) ;
	display.setCursor(54, 24) ;
	display.print(tempStr) ;

	// Swap
	if (swap_delay) {
		if (c_time < swap_tm) drawBitmap((char *)swapArt, 90, 4) ;
		else if (c_time > swap_tm + swap_delay) swap_tm = c_time + swap_delay ;
	} else {
		drawBitmap((char *)swapArt, 90, 4) ;
	}

	sprintf(tempStr, "%3d%%", swapU) ;
	display.setCursor(86, 24) ;
	display.print(tempStr) ;

	// Row 2
	// Net
	c_time = millis() ;
	if (net_delay) {
		if (c_time < net_tm) drawBitmap((char *)netArt, 26, 34) ;
		else if (c_time > net_tm + net_delay) net_tm = c_time + net_delay ;
	} else {
		drawBitmap((char *)netArt, 26, 34) ;
	}

	display.setCursor(8, 52) ;
	sprintf(tempStr, "%3d.%02d%2s", netDL, netDLM, getUnit(netDL_Unit)) ;
	display.print(tempStr) ;
	display.setCursor(8, 62) ;
	sprintf(tempStr, "%3d.%02d%2s", netUL, netULM, getUnit(netUL_Unit)) ;
	display.print(tempStr) ;

	c_time = millis() ;
	if (io_delay) {
		if (c_time < io_tm) drawBitmap((char *)ioArt, 90, 34) ;
		else if (c_time > io_tm + io_delay) io_tm = c_time + io_delay ;
	} else {
		drawBitmap((char *)ioArt, 90, 34) ;
	}

	display.setCursor(70, 52) ;
	sprintf(tempStr, "%3d.%02d%2s", ioR, ioRM, getUnit(ioR_Unit)) ;
	display.print(tempStr) ;
	display.setCursor(70, 62) ;
	sprintf(tempStr, "%3d.%02d%2s", ioW, ioWM, getUnit(ioW_Unit)) ;
	display.print(tempStr) ;

	c_time = millis() ;
	if (c_time < refresh) {
		drawBitmap((char *)fireArt, 116, 0) ;
	}

	drawParticles() ;
	display.display() ;
}
