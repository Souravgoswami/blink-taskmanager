#ifndef PARTICLES
#define PARTICLES 30
#endif

// Paint the particles
unsigned char particles[PARTICLES][4] ;

void initParticles() {
	static unsigned char *p ;

	for(unsigned char i = 0 ; i < PARTICLES ; ++i) {
		p = particles[i] ;

		p[0] = rand() & 127 ;
		p[1] = rand() & 63 ;
		p[2] = round(sin(i)) ;
		p[3] = round(cos(i)) ;
	}
}

void drawParticles() {
	static unsigned char *p ;

	for(unsigned char i = 0 ; i < PARTICLES ; ++i) {
		p = particles[i] ;

		p[0] += p[2] ;
		p[1] += p[3] ;

		if (p[1] > 64 || p[0] > 127 || p[1] < 1 || p[0] < 1) {
			p[0] = rand() & 127 ;
			p[1] = rand() & 63 ;
		}

		display.drawPixel(p[0], p[1], WHITE) ;
	}
}
