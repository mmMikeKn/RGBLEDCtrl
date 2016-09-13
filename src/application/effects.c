#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "global.h"

static int start_pos = 15, cur_pos = 0, cur_cnt = 0;
static int cur_r, cur_g, cur_b;
static bool clockwise = TRUE;
static uint32_t lastTicks = 0;

static int getCirclePosition(int p) {
	if(p >= NUM_LEDS) return p-NUM_LEDS;
	if(p < 0) return p+NUM_LEDS;
	return p;
}

inline static uint8_t trimVal(int val) {
	if(val < 0) return val;
	if(val > 255) return 255;
	return val;
}

static void setColor() {
	if(cur_pos > NUM_LEDS) cur_pos = 0;
	if(cur_pos < 0) cur_pos = NUM_LEDS;

	leds[cur_pos].r = trimVal(cur_r);
	leds[cur_pos].g = trimVal(cur_g);
	leds[cur_pos].b = trimVal(cur_b);
	cur_cnt++;
	if(clockwise) cur_pos++;
	else cur_pos--;
}

static void moveStartPos() {
	if(clockwise) {
		start_pos++;
		if(start_pos > NUM_LEDS) start_pos = 0;
	} else {
		start_pos--;
		if(start_pos < 0) start_pos = NUM_LEDS;
	}

	static uint32_t t = 0;
	if((_sysTicks - t) > 5000) {
		t = _sysTicks;
#ifdef HAS_USOUND_SRC
		clockwise = (usoundGetRange() % 50) < 25;
#else
		clockwise = (rand() % 50) < 25;
#endif
	}
}

static const uint8_t fireColors[256][3]={{0,0,0}, {1,0,0}, {3,0,0}, {4,0,0}, {6,0,0}, {8,0,0}, {9,0,0}, {11,0,0}, {12,0,0}, {14,0,0}, {16,0,0}, {17,0,0}, {19,0,0}, {20,1,0}, {22,1,0}, {24,1,0}, {25,1,0}, {27,1,0}, {28,2,0}, {30,2,0}, {32,2,0}, {33,2,0}, {35,3,0}, {36,3,0}, {38,3,0}, {40,3,0}, {41,4,0}, {43,4,0}, {44,4,0}, {46,5,0}, {48,5,0}, {49,6,0}, {51,6,0}, {52,6,0}, {54,7,0}, {56,7,0}, {57,8,0}, {59,8,0}, {60,9,0}, {62,9,0}, {64,10,0}, {65,10,0}, {67,11,0}, {68,11,0}, {70,12,0}, {72,12,0}, {73,13,0}, {75,13,0}, {76,14,0}, {78,15,0}, {80,15,0}, {81,16,0}, {83,16,0}, {84,17,0}, {86,18,0}, {88,18,0}, {89,19,0}, {91,20,0}, {92,21,0}, {94,21,0}, {96,22,0}, {97,23,0}, {99,24,0}, {100,24,0}, {102,25,0}, {104,26,0}, {105,27,0}, {107,28,0}, {108,29,0}, {110,29,0}, {112,30,0}, {113,31,0}, {115,32,0}, {116,33,0}, {118,34,0}, {120,35,0}, {121,36,0}, {123,37,0}, {124,38,0}, {126,39,0}, {128,40,0}, {129,41,0}, {131,42,0}, {132,43,0}, {134,44,0}, {136,45,0}, {137,46,0}, {139,47,0}, {140,48,0}, {142,49,0}, {144,50,0}, {145,51,0}, {147,53,0}, {148,54,0}, {150,55,0}, {152,56,0}, {153,57,0}, {155,59,0}, {156,60,0}, {158,61,0}, {160,62,0}, {161,64,0}, {163,65,0}, {164,66,0}, {166,67,0}, {168,69,0}, {169,70,0}, {171,71,0}, {172,73,0}, {174,74,0}, {176,75,0}, {177,77,0}, {179,78,0}, {180,80,0}, {182,81,0}, {184,82,0}, {185,84,0}, {187,85,0}, {188,87,0}, {190,88,0}, {192,90,0}, {193,91,0}, {195,93,0}, {196,94,0}, {198,96,0}, {200,98,0}, {201,99,0}, {203,101,0}, {204,102,0}, {206,104,0}, {208,106,0}, {209,107,0}, {211,109,0}, {212,110,0}, {214,112,0}, {216,114,0}, {217,116,0}, {219,117,0}, {220,119,0}, {222,121,0}, {224,122,0}, {225,124,0}, {227,126,0}, {228,128,0}, {230,130,0}, {232,131,0}, {233,133,0}, {235,135,0}, {236,137,0}, {238,139,0}, {240,141,0}, {241,143,0}, {243,144,0}, {244,146,0}, {246,148,0}, {248,150,0}, {249,152,0}, {251,154,0}, {252,156,0}, {254,158,0}, {255,160,0}, {255,161,2}, {255,163,4}, {254,165,5}, {255,166,7}, {254,168,9}, {255,169,10}, {255,171,12}, {255,172,13}, {255,174,15}, {254,175,17}, {255,177,18}, {255,178,20}, {255,180,21}, {255,181,23}, {255,182,25}, {255,184,26}, {254,185,28}, {255,186,29}, {254,188,31}, {255,189,32}, {255,191,34}, {254,192,36}, {255,193,37}, {254,194,39}, {255,196,41}, {254,197,42}, {255,198,44}, {255,200,45}, {255,201,47}, {255,202,48}, {254,203,50}, {255,204,52}, {255,206,53}, {255,207,55}, {255,208,57}, {255,209,58}, {255,210,60}, {254,211,61}, {255,212,63}, {255,214,65}, {255,215,66}, {255,216,68}, {254,217,69}, {255,218,71}, {254,219,73}, {255,220,74}, {254,221,76}, {255,222,77}, {255,223,79}, {254,224,81}, {255,225,82}, {254,226,84}, {255,227,85}, {255,228,87}, {255,228,89}, {255,229,90}, {255,230,92}, {255,231,93}, {254,232,95}, {255,233,97}, {255,234,98}, {255,234,100}, {255,235,101}, {254,236,103}, {255,237,105}, {254,238,106}, {255,238,108}, {255,239,109}, {255,240,111}, {255,241,113}, {254,241,114}, {255,242,116}, {255,243,117}, {255,243,119}, {255,244,121}, {255,245,122}, {255,245,124}, {254,246,125}, {255,246,127}, {255,247,129}, {255,248,130}, {255,248,132}, {254,249,133}, {255,249,135}, {254,250,137}, {255,250,138}, {254,251,140}, {255,251,141}, {255,252,143}, {254,252,145}, {255,253,146}, {254,253,148}, {255,254,149}, {254,254,151}, {254,255,153}, };

static void makeFireSpot(int p0, int sz) {
	for(int i = 0; i < sz; i++) {
		int16_t p = getCirclePosition(p0+i);
		int fireLevel = round(255.0-i*255.0/sz);
		if(i > 4) fireLevel += 5-(rand()%10);
		if(fireLevel < 0) fireLevel = 0;
		if(fireLevel > 255) fireLevel = 255;
		cur_r = fireColors[fireLevel][0];
		cur_g = fireColors[fireLevel][1];
		cur_b = fireColors[fireLevel][2];
		if(leds[p].r < cur_r) {
			leds[p].r = cur_r; leds[p].g = cur_g; leds[p].b = cur_b;
		}
		p = getCirclePosition(p0-i);
		if(leds[p].r < cur_r) {
			leds[p].r = cur_r; leds[p].g = cur_g; leds[p].b = cur_b;
		}
	}
}
//================================================================================
void effectRainbow() {
	int16_t step = (256*5+NUM_LEDS)/NUM_LEDS;
	cur_r = 0; cur_g = 0; cur_b = 0;
	cur_cnt = 0;
	cur_pos = start_pos;

	memset((void*)leds, 0, sizeof(leds));
	while(cur_r < 255) { setColor(); cur_r += step;	} 	//r:0..255	g:0			b:0
	while(cur_g < 255) { setColor(); cur_g += step;	} 	//r:255		g:0..255	b:0
	while(cur_r > 0) { setColor(); cur_r -= step; }		//r:255..0	g:255		b:0
	while(cur_b < 255) {								//r:0		g:255..0	b:0..255
		setColor(); cur_g -= step;	cur_b += step;
	}
	while(cur_g > 0 ) { setColor(); cur_g += step; }	//r:0		g:0..255	b:255
	cur_r = 0; cur_g = 0; cur_b = 0;
	while(cur_cnt < NUM_LEDS) setColor();
	moveStartPos();
}

void effectBlastPoints() {
	if((_sysTicks - lastTicks) < (1000/24)) return;
	memset((void*)leds, 0, sizeof(leds));
	lastTicks = _sysTicks;
	static RGB_t point = {255,255,255};
	static uint8_t blast_max_size = 20;
	static bool blast_fade;

	if(cur_pos < 0) {
		makeFireSpot(getCirclePosition(start_pos+(NUM_LEDS/2)), -cur_pos);
		if(cur_pos < -blast_max_size) {
			blast_fade = TRUE;
		}
		if(blast_fade)	cur_pos++;
		else cur_pos--;
		if(cur_pos == 0) {
			start_pos = getCirclePosition(start_pos+(10 - rand()%20));
			point.r = 64+rand()%191;
			point.b = 64+rand()%191;
			point.g = 64+rand()%191;
			blast_max_size = 10+rand()%50;
		}
	} else {
		int16_t p = getCirclePosition(start_pos+cur_pos);
		leds[p].r = point.r;
		leds[p].b = point.b;
		leds[p].g = point.g;
		p = getCirclePosition(start_pos-cur_pos);
		leds[p].r = point.r;
		leds[p].b = point.b;
		leds[p].g = point.g;
		cur_pos++;
		if(cur_pos > (NUM_LEDS/2)) {
			cur_pos = -1; blast_fade = FALSE;
		}
	}
}

void effectWave() {
	if((_sysTicks - lastTicks) < (1000/24)) return;
	lastTicks = _sysTicks;

	memset((void*)leds, 0, sizeof(leds));
	cur_pos = start_pos;
	for(int i = 0; i < NUM_LEDS; i++) {
		float angle = M_PI * 2.0 * i /NUM_LEDS;
		cur_r = sin(angle) * 127 + 128;
		cur_g = sin(angle * 2/3 + M_PI/3) * 127 + 128;
		cur_b = sin(angle * 4/5 + M_PI*2/3) * 127 + 128;
		setColor();
	}
	moveStartPos();
}

void effectMoveSpot() {
	const RGB_t l[15] = {
			{0, 25, 50},{0, 50, 100},{0, 75, 150},{25, 75, 100},{50, 100, 125},
			{75, 125, 100},{100, 125, 100},{125, 125, 100},{150, 125, 100},
			{175, 125, 100},{250, 125, 100},{250, 100, 75},{250, 75, 50},
			{250, 50, 25}, {250, 0, 0}
	};
	memset((void*)leds, 0, sizeof(leds));
	cur_pos = start_pos;
	for(int i = 0; i < 15; i++) {
		cur_r = l[i].r; cur_g = l[i].g;	cur_b = l[i].b; setColor();
	}
	moveStartPos();
}

void effectRunningLights() {
	if((_sysTicks - lastTicks) < (1000/24)) return;
	cur_pos = start_pos;
	memset((void*)leds, 0, sizeof(leds));
	for(int i = 0; i < NUM_LEDS; i++) {
		float v = sin((float)i/3.0)*255;
		static int8_t colorVal = 0;
		if((_sysTicks%1000) < 200) {
#ifdef HAS_USOUND_SRC
			colorVal = usoundGetRange() % 20;
#else
			colorVal = rand() % 20;
#endif
		}
		cur_r = colorVal < 4 ? 0:v;
		cur_g = colorVal > 16 ? 0:v;
		cur_b = colorVal > 8 && colorVal < 13 ? 0:v;
		setColor();
	}
	moveStartPos();
}


#ifdef HAS_SOUND_FFT_SRC
void effectColourMusic() {
	uint16_t *fft = soundFFT_result();
	cur_pos = start_pos;
	static const float kMap = (float)(FFT_RESULT_SIZE-2)/(float)NUM_LEDS;
	static const int FFT_MAX = 380;
	static const int delta_FFT_MAX = FFT_MAX/5;

	memset((void*)leds, 0, sizeof(leds));
	for(int i = 0; i < NUM_LEDS; i++) {
		int n = round(i*kMap);
		if(n >= FFT_RESULT_SIZE || n < 3) continue;
		uint16_t level = fft[n]; // 0..FFT_MAX
		int dc = (level%delta_FFT_MAX)*256/delta_FFT_MAX;

		switch(level/delta_FFT_MAX) {
		case 0:	cur_r = level < 8? 0: dc;		cur_g = 0;		cur_b = 0; break;
		case 1:	cur_r = 255;	cur_g = dc; 	cur_b = 0; break;
		case 2: cur_r = 255-dc; cur_g = 255; 	cur_b = 0; break;
		case 3: cur_r = 0; 		cur_g = 255; 	cur_b = dc; break;
		case 4: cur_r = 0; 		cur_g = 255-dc;	cur_b = 255; break;
		default: cur_r = 128+dc; cur_g = 128+dc;	cur_b = 128+dc; break;
		}
		setColor();
	}
}
#endif // HAS_SOUND_FFT_SRC


static const float ecgTable[72] = { // step 0.01s 0.72s period -> 83b/min
// 0.12, 0.15, 0.14, 0.12, 0.08, 0.08, 0.08, 0.08, 0.08, 0.08, 0.08, 0.08, 0.08, 0.08, -0.02, 0.25, 1.15, 0.25, -0.15, -0.05, 0.08, 0.08, 0.08, 0.08, 0.08, 0.08, 0.08, 0.08, 0.08, 0.08, 0.08, 0.08, 0.08, 0.1, 0.15, 0.2, 0.22, 0.23, 0.24, 0.21, 0.18, 0.16, 0.1, 0.06, 0.06, 0.09, 0.1, 0.09, 0.08, 0.08, 0.08, 0.08, 0.08, 0.08, 0.08, 0.08, 0.08, 0.08, 0.08, 0.08, 0.08, 0.08, 0.08, 0.08, 0.08, 0.08, 0.08, 0.08, 0.08, 0.08, 0.08, 0.1
 0.104347826, 0.130434783, 0.12173913, 0.104347826, 0.069565217, 0.069565217, 0.069565217, 0.069565217, 0.069565217, 0.069565217, 0.069565217, 0.069565217, 0.069565217, 0.069565217, -0.017391304, 0.217391304, 1, 0.217391304, -0.130434783, -0.043478261, 0.069565217, 0.069565217, 0.069565217, 0.069565217, 0.069565217, 0.069565217, 0.069565217, 0.069565217, 0.069565217, 0.069565217, 0.069565217, 0.069565217, 0.069565217, 0.086956522, 0.130434783, 0.173913043, 0.191304348, 0.2, 0.208695652, 0.182608696, 0.156521739, 0.139130435, 0.086956522, 0.052173913, 0.052173913, 0.07826087, 0.086956522, 0.07826087, 0.069565217, 0.069565217, 0.069565217, 0.069565217, 0.069565217, 0.069565217, 0.069565217, 0.069565217, 0.069565217, 0.069565217, 0.069565217, 0.069565217, 0.069565217, 0.069565217, 0.069565217, 0.069565217, 0.069565217, 0.069565217, 0.069565217, 0.069565217, 0.069565217, 0.069565217, 0.069565217, 0.086956522
};

// 93 - (40cm+50cm/2)/100cm * 144LCD/m
#define MIDLE_LEFT_POS 30
#define MIDLE_RIGHT_POS 98
void effectHeartRhythm() {
	if(_sysTicks > 0xFFFFFFF) _sysTicks = 0;
	uint32_t t = _sysTicks;
	uint32_t k = 500;
//	if(usoundGetRange() > 20) {
//		k = 500;
//		if(usoundGetRange() < 80) k += (usoundGetRange()-20)*10; //0..60 -> 500+0..600
//	}
	t = t * k/1000;
	int val = ecgTable[(t % 720)/10] * (NUM_LEDS/3) ;
	int delta = val < 0? -1:1;
	memset((void*)leds, 0, sizeof(leds));
	for(int i = 0; i != val; i+=delta) {
		int left = abs(val-i);
		cur_r = left < 5 ? 255/left:255;
		cur_g = cur_b = 0;

		cur_pos = MIDLE_LEFT_POS+i;
		setColor();
		cur_pos = MIDLE_RIGHT_POS-i;
		setColor();
	}
}

void effect2Wave() {
	if((_sysTicks - lastTicks) < (1000/24)) return;
	lastTicks = _sysTicks;
	int step = (_sysTicks % 15000) / 5000;

	cur_pos = start_pos;
	memset((void*)leds, 0, sizeof(leds));
	cur_r = cur_g = cur_b = 0;
	for(int i = 0; i < NUM_LEDS; i++) {
		float angle = M_PI * 10 * i / NUM_LEDS;
		int v = sin(angle) * 127;
		if(step == 0) cur_r = v;
		else if(step == 1) cur_g = v;
		else cur_b = v;
		setColor();
	}
	cur_pos = start_pos;
	bool old_clockwise = clockwise;
	clockwise = !clockwise;
	for(int i = 0; i < NUM_LEDS; i++) {
		float angle = M_PI * 8 * i /NUM_LEDS;
		cur_r = leds[cur_pos].r;
		cur_g = leds[cur_pos].g;
		cur_b = leds[cur_pos].b;
		int v = sin(angle) * 127;
		if(step == 0) cur_b = v;
		else if(step == 1) cur_r = v;
		else cur_g = v;
		setColor();
	}
	clockwise = old_clockwise;
	moveStartPos();
}


typedef struct {
	int pos;
	int dd_pos;
	int burn_sz;
	int burn_speed;
	uint8_t acive_state;
} SPARK ;

#define SPARK_ACTIVE 0x34

#define MAX_SPARKS 4
static SPARK list[MAX_SPARKS];

#define SPARK_BOTTOM_POS_MIN 5
#define SPARK_BOTTOM_POS_MAX 20
#define SPARK_START 65

#define K_MULT 1000

void effectFlame() {
	memset((void*)leds, 0, sizeof(leds));

	int cnt_sparks = 0;
	for(int i = 0; i < MAX_SPARKS; i++) {
		if(list[i].acive_state == SPARK_ACTIVE) {
			cnt_sparks++;
			SPARK *sp = &list[i];
			int p0 = getCirclePosition(sp->pos /K_MULT);
			if(p0 < SPARK_BOTTOM_POS_MAX && p0 > SPARK_BOTTOM_POS_MIN) {
				int new_p = sp->pos+(K_MULT/2) - (rand() % K_MULT);
				p0 = getCirclePosition(new_p /K_MULT);
				if(p0 < SPARK_BOTTOM_POS_MAX && p0 > SPARK_BOTTOM_POS_MIN) {
					sp->pos = new_p;
				}
				sp->burn_sz -= sp->burn_speed;
				if(sp->burn_sz < 0) sp->acive_state = 0;
			} else {
				sp->pos += sp->dd_pos;
				if(sp->burn_sz < (30*K_MULT)) {
					sp->burn_sz += sp->burn_speed;
				}
			}
		}
	}

	if(cnt_sparks < MAX_SPARKS && (_sysTicks - lastTicks) > (uint32_t)(cnt_sparks*300 + rand()%800)) {
		lastTicks = _sysTicks;
		for(int i = 0; i < MAX_SPARKS; i++) {
			SPARK *sp = &list[i];
			if(sp->acive_state != SPARK_ACTIVE) {
				sp->pos = SPARK_START * K_MULT;
				sp->burn_sz = K_MULT;
				sp->burn_speed = (K_MULT/50) + rand()%(K_MULT/15);
				sp->dd_pos = (K_MULT/100) + rand()%(K_MULT/20);
				if((rand()%100) > 50) sp->dd_pos = -sp->dd_pos;
				sp->acive_state = SPARK_ACTIVE;
				break;
			}
		}
	}
	for(int i = 0; i < MAX_SPARKS; i++) {
		if(list[i].acive_state == SPARK_ACTIVE) {
			makeFireSpot(getCirclePosition(list[i].pos/K_MULT),	list[i].burn_sz/K_MULT);
		}
	}
}
