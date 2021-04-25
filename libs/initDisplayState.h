unsigned long blink_time = millis() + 100 ;

void initDisplayState() {
	display.clearDisplay() ;
	display.setCursor(0, 0) ;

	drawBitmap((char *)blinkTMLogoArt, 30, 8) ;

	if(blink_time > millis()) {
		drawBitmap((char *)fireArt, 30, 18) ;
	} else if (millis() > blink_time + 100) {
		blink_time = millis() + 100 ;
	}

	drawBitmap((char *)blinkTMArt, 52, 14) ;

	display.setCursor(20, 34) ;
	display.println(F("Device Initialized")) ;

	display.setCursor(16, 46) ;
	display.println(F("Please run blink-tm")) ;
	display.setCursor(38, 56) ;
	display.println(F("command")) ;
}
