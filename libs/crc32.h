/*
	Source:
	https://commandlinefanatic.com/cgi-bin/showarticle.cgi?article=art008

	Note that this obeys the standard and matches with Ruby's Zlib.crc32(...)
*/

#define CRC32_DIVISOR 0xEDB88320

unsigned long int getCRC(char *input) {
	unsigned char len = strlen(input) ;
	unsigned long crc = 0xFFFFFFFF ;

	for (unsigned char i = 0 ; i < len ; ++i) {
		crc ^= input[i] ;

		for (unsigned char k = 8 ; k ; --k) {
			crc = crc & 1 ? (crc >> 1) ^ CRC32_DIVISOR : crc >> 1 ;
		}
	}

	return crc ^ 0xFFFFFFFF ;
}
