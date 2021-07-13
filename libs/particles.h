#ifndef PARTICLES
#define PARTICLES 30
#endif

// Paint the particles
unsigned char particles[PARTICLES][2] ;

void initParticles() {
	static unsigned char *p ;

	for(unsigned char i = 0 ; i < PARTICLES ; ++i) {
		p = particles[i] ;

		p[0] = rand() % 128 ;
		p[1] = rand() % 64 ;
	}
}

void drawParticles() {
	static unsigned char *p ;

	for(unsigned char i = 0 ; i < PARTICLES ; ++i) {
		p = particles[i] ;

		p[0] += round(sin(i)) ;
		p[1] += round(cos(i)) ;

		if (p[1] > 64 || p[0] > 127 || p[1] < 1 || p[0] < 1) {
			p[0] = rand() % 128 ;
			p[1] = rand() % 64 ;
		}

		display.drawPixel(p[0], p[1], WHITE) ;
	}
}
