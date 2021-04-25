#ifndef PARTICLES
#define PARTICLES 15
#endif

// Paint the particles
unsigned char particles[PARTICLES][3] ;

void initParticles() {
	unsigned char *p ;

	for(unsigned char i = 0 ; i < PARTICLES ; ++i) {
		p = particles[i] ;

		p[0] = rand() % 128 ;
		p[1] = rand() % 64 * -1 ;
		p[2] = rand() % 3 + 1 ;
	}
}

void drawParticles() {
	unsigned char *p ;

	for(unsigned char i = 0 ; i < PARTICLES ; ++i) {
		p = particles[i] ;
		p[1] += p[2] ;

		if (p[1] > 64) {
			p[0] = rand() % 128 ;
			p[1] = rand() % 64 * -1 ;
			particles[i][2] = rand() % 3 + 1 ;
		} else if (rand() % 10 < 8) {
			display.drawPixel(p[0], p[1], WHITE) ;
		}
	}
}
