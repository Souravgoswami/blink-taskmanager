// This gets the substring from a string and copies the substring to given buf
//
// Because this is limited to ice taskmanager, this doesn't expect
// a starting number to be bigger than the stopping number.
// A check makes sense for other application, not necessary here.
void substr(char *str, char *buf, unsigned char start, unsigned char stop) {
	buf[0] = '\0' ;
	unsigned char end = stop + 1;
	unsigned char counter = 0 ;
	unsigned char j = 0 ;

	for(counter = start ; counter < end ; ++counter) {
		buf[j++] = str[counter] ;
	}

	buf[j] = '\0' ;
}
