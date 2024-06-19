// for sched.h
#define _GNU_SOURCE

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <sched.h>

#include <pthread.h>

#include "led.h"

#define SPI_DEV "/dev/spidev1.1"

//#define MAX_LED 33
#define MAX_LED 48
#define MAX_SEGMENT 3
#define RESET_LEN 120

#define STATE_STOPPED 0
#define STATE_PLAY 1
#define STATE_STOPPING 2

#define WS2812B 0

#define PULSE_00 0x88
#define PULSE_01 0x8E
#define PULSE_10 0xE8
#define PULSE_11 0xEE

static FILE* spi = NULL;
static uint8_t leds[MAX_LED * 3]; //3 bytes per LED (R-G-B)
#if WS2812B
static uint32_t ledbuf[(RESET_LEN / 3) + (MAX_LED * 4)]; //RESET_LEN bytes reset, 12 bytes per LED (R-G-B)
#else
static uint8_t ledbuf[(RESET_LEN / 3) + (MAX_LED * 12)]; //RESET_LEN bytes reset, 12 bytes per LED (R-G-B)
#endif
static pthread_t thread1;
static uint8_t fxPulse = 0;
static uint8_t fxWave = 0;
static uint8_t fxFlow = 0;

static int32_t brightness = 0;
static uint8_t state = STATE_STOPPED;
static volatile uint8_t reboot = 0;

//color bar - RGB values of colour slider
static uint8_t cbar[300 * 3 + 1] = 
{
  "\377\000\024\377\000\027\377\000\033\377\000!\377\000&\377\000+\377\000,\377\000\060\377\000:\377"
  "\000@\377\000E\377\000J\377\000O\377\000T\377\000Y\377\000^\377\000c\377\000h\377\000m\377\000"
  "r\377\000w\377\000}\377\000\202\377\000\207\377\000\214\377\000\221\377\000\226\377\000\233"
  "\377\000\240\377\000\246\377\000\253\377\000\260\377\000\265\377\000\272\377\000\277\377"
  "\000\304\377\000\311\377\000\316\377\000\323\377\000\330\377\000\335\377\000\343\377\000"
  "\350\377\000\355\377\000\362\376\000\366\373\000\371\371\000\373\366\000\376\362\000\377"
  "\355\000\377\350\000\377\343\000\377\335\000\377\330\000\377\323\000\377\316\000\377\311"
  "\000\377\304\000\377\277\000\377\272\000\377\265\000\377\260\000\377\253\000\377\246\000"
  "\377\240\000\377\233\000\377\226\000\377\221\000\377\214\000\377\207\000\377\202\000\377"
  "}\000\377w\000\377r\000\377m\000\377h\000\377c\000\377^\000\377Y\000\377T\000\377O\000\377J\000"
  "\377E\000\377@\000\377:\000\377\065\000\377\060\000\377+\000\377&\000\377!\000\377\034\000\377"
  "\027\000\377\021\000\377\014\000\377\007\000\377\002\000\377\000\002\377\000\007\377\000\014\377\000\021"
  "\377\000\027\377\000\034\377\000!\377\000&\377\000+\377\000\060\377\000\065\377\000:\377\000@\377"
  "\000E\377\000J\377\000O\377\000T\377\000Y\377\000^\377\000c\377\000h\377\000m\377\000r\377\000"
  "w\377\000}\377\000\202\377\000\207\377\000\214\377\000\221\377\000\226\377\000\233\377"
  "\000\240\377\000\246\377\000\253\377\000\260\377\000\265\377\000\272\377\000\277\377\000"
  "\304\377\000\311\377\000\316\377\000\323\377\000\330\377\000\335\377\000\343\377\000\350"
  "\377\000\355\377\000\362\377\000\366\376\000\371\373\000\373\371\000\376\366\000\377\362"
  "\000\377\355\000\377\350\000\377\343\000\377\335\000\377\330\000\377\323\000\377\316\000"
  "\377\311\000\377\304\000\377\277\000\377\272\000\377\265\000\377\260\000\377\253\000\377"
  "\246\000\377\240\000\377\233\000\377\226\000\377\221\000\377\214\000\377\207\000\377\202"
  "\000\377}\000\377w\000\377r\000\377m\000\377h\000\377c\000\377^\000\377Y\000\377T\000\377O\000"
  "\377J\000\377E\000\377@\000\377:\000\377\065\000\377\060\000\377+\000\377&\000\377!\000\377\034"
  "\000\377\027\000\377\021\000\377\014\000\377\007\000\377\002\002\377\000\007\377\000\014\377\000\021\377"
  "\000\027\377\000\034\377\000!\377\000&\377\000+\377\000\060\377\000\065\377\000:\377\000@\377\000"
  "E\377\000J\377\000O\377\000T\377\000Y\377\000^\377\000c\377\000h\377\000m\377\000r\377\000w\377"
  "\000}\377\000\202\377\000\207\377\000\214\377\000\221\377\000\226\377\000\233\377\000\240"
  "\377\000\246\377\000\253\377\000\260\377\000\265\377\000\272\377\000\277\377\000\304\377"
  "\000\311\377\000\316\377\000\323\377\000\330\377\000\335\377\000\343\377\000\350\377\000"
  "\355\377\000\362\377\000\366\376\000\371\373\000\373\371\000\376\366\000\377\362\000\377"
  "\355\000\377\350\000\377\343\000\377\335\000\377\330\000\377\323\000\377\316\000\377\311"
  "\000\377\304\000\377\277\000\377\272\000\377\265\000\377\260\000\377\253\000\377\246\000"
  "\377\240\000\377\233\000\377\226\000\377\221\000\377\214\000\377\207\000\377\202\000\377"
  "}\000\377w\000\377r\000\377m\000\377h\000\377c\000\377^\000\377Y\000\377T\000\377O\000\377J\000"
  "\377E\000\377@\000\377:\000\377\065\000\377\060\000\377+\000\377&\000\377!\000\377\034\000\377"
  "\027\000\377\021\000\377\014\000\377\007\000\377\002\000\377\000\000\377\000\000\377\000\000",
};


typedef struct led_segment_t {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint32_t a;   // brightness ((in 1/256-th  of a pixel) WAVE
    int32_t aAnim;
    int32_t aMove; //brightenss movement increment (in 1/256-th  of a pixel) WAVE
    
    int32_t x; // center of the segment within the LED strip (in 1/256-th  of a pixel)   
    int32_t xMove; //x movement increment (in 1/256-th  of a pixel) FLOW

    int32_t w; //width
    int32_t wAnim;
    int32_t wMove; //width movement increment ((in 1/256-th  of a pixel) PULSE
    
    uint8_t pixels[MAX_LED]; // alpha of the segment
} led_segment;

// an array of segments
static led_segment segments[MAX_SEGMENT];


static void schedule_cpu(int cpuIndex) {
    cpu_set_t mask;
    
    CPU_ZERO(&mask);
    CPU_SET(cpuIndex, &mask);
	if (sched_setaffinity(0, sizeof(mask), &mask) < 0) {
		printf("set_cpu: failed to set thread affinity\n");
	}
}

// convert colour component in byte format to SPI format (4 bytes)
static inline uint32_t encodeComponent32(uint8_t val) {
    int i = 0;
    uint32_t res;
       
    i = val & 0b11;
    res = (i == 0) ? PULSE_00 : (i==1) ? PULSE_01 : (i==2) ? PULSE_10 : PULSE_11;
    res <<= 8;

    i = (val >> 2) & 0b11;
    res = (i == 0) ? PULSE_00 : (i==1) ? PULSE_01 : (i==2) ? PULSE_10 : PULSE_11;
    res <<= 8;
    
    i = (val >> 4) & 0b11;
    res = (i == 0) ? PULSE_00 : (i==1) ? PULSE_01 : (i==2) ? PULSE_10 : PULSE_11;
    res <<= 8;
    
    i = (val >> 6) & 0b11;
    res = (i == 0) ? PULSE_00 : (i==1) ? PULSE_01 : (i==2) ? PULSE_10 : PULSE_11;
    
    //printf("encoding: %02x to %08x\n", val, res);

    return res;
}


// convert colour component in byte format to SPI format (4 bytes)
static inline void encodeComponent8p(uint8_t val, uint8_t* p) {
    int i = 0;
       
    i = (val >> 6) & 0b11;
    *p = (i == 0) ? PULSE_00 : (i==1) ? PULSE_01 : (i==2) ? PULSE_10 : PULSE_11;
    p++;

    i = (val >> 4) & 0b11;
    *p = (i == 0) ? PULSE_00 : (i==1) ? PULSE_01 : (i==2) ? PULSE_10 : PULSE_11;
    p++;

    i = (val >> 2) & 0b11;
    *p = (i == 0) ? PULSE_00 : (i==1) ? PULSE_01 : (i==2) ? PULSE_10 : PULSE_11;
    p++;

    i = val & 0b11;
    *p = (i == 0) ? PULSE_00 : (i==1) ? PULSE_01 : (i==2) ? PULSE_10 : PULSE_11;
}

// convert the LED colours into the SPI buffer format and write it to SPI device
void led_write(void) {
    int i;
    int wrOffset = (RESET_LEN / 3); //skip the reset sequence encoded as 0 (120bytes)
    uint32_t val;
    
    if (spi == NULL) {
        return;
    }
    
#if WS2812B
    for (i = 0; i < MAX_LED * 3; ) {
        uint8_t r = leds[i++];
        uint8_t g = leds[i++];
        uint8_t b = leds[i++];
        
        ledbuf[wrOffset++] = encodeComponent32(g);
        ledbuf[wrOffset++] = encodeComponent32(r);
        ledbuf[wrOffset++] = encodeComponent32(b);
    }
#else
    for (i = 0; i < MAX_LED * 3; ) {
        uint8_t r = leds[i++];
        uint8_t g = leds[i++];
        uint8_t b = leds[i++];
        
        encodeComponent8p(r, ledbuf + wrOffset);
        encodeComponent8p(g, ledbuf + wrOffset + 4);
        encodeComponent8p(b, ledbuf + wrOffset + 8);
        wrOffset += 12;
    }
#endif
    i  = fwrite(ledbuf, sizeof(ledbuf),1, spi);
    fflush(spi);
}

// Sets the segment's colour.
void led_segment_set_colour(int index, uint32_t argb) {
    if (index < 0 || index >= MAX_SEGMENT) {
        return;
    }
    segments[index].r = (argb >> 16) & 0xFF;
    segments[index].g = (argb >> 8) & 0xFF;
    segments[index].b = (argb) & 0xFF;
    segments[index].a = ((argb >> 24) & 0xFF) << 8;
    
    // debug - less bright
    //segments[index].a = 100 << 8;
}

// Sets the segment's position within the LED string.
void led_segment_set_position(int index, uint32_t position) {
    if (index < 0 || index >= MAX_SEGMENT) {
        return;
    }
    segments[index].x = position << 8;
}

// Sets the segment width (volume)
void led_segment_set_width(int index, uint32_t width) {
    if (index < 0 || index >= MAX_SEGMENT) {
        return;
    }
    segments[index].w = width << 8;
}

// Called from within the UI to enable and disable Pulse effect.
void led_set_fx_pulse(uint8_t on) {
    fxPulse = on ? 1 : 0;
}

// Called from within the UI to enable and disable Wave effect.
void led_set_fx_wave(uint8_t on) {
    fxWave = on ? 1 : 0;
}

// Called from within the UI to enable and disable Flow effect.
void led_set_fx_flow(uint8_t on) {
    fxFlow = on ? 1 : 0;
}



// calculate alpha of each pixel within the segment
void led_segment_update(int index) {
    int32_t i, step;
    int32_t a, a2;
    int32_t w;
    uint32_t xPos;
    int32_t wStep;
    led_segment* s;
    
    
    if (index < 0 || index >= MAX_SEGMENT) {
        return;
    }
    s = &segments[index];
    memset(s->pixels, 0, MAX_LED);
    if (brightness == 0) {
        return;
    }
    
    w = s->w;
    
    // adjust width of the segment based on the 'w' animation
    if (fxPulse) {
        //if (index == 0)
        //printf("seg: %i update w=%i sW=%i wStep=%i wMove=%i wAnim=%d\n", index, w, s->w, wStep, s->wMove, s->wAnim);
        s->wAnim += s->wMove;
        if (s->wAnim > s->w) {
            s->wAnim = s->w;
            s->wMove = -s->wMove;
        } else
        if (s->wAnim < -(s->wMove>> 2)) {
            s->wAnim = -(s->wMove>> 2);
            s->wMove = -s->wMove;
        }
        w += s->wAnim;
        //if (index == 0)
        //printf("seg: %i update w=%i sW=%i wStep=%i wMove=%i wAnim=%d\n", index, w, s->w, wStep, s->wMove, s->wAnim);
    }
    
    // move the position of the segment
    if (fxFlow) {
        // central segment flows in opposite direction
        if (index ==1) {
            s->x -= (s->xMove / 2);
        } else
        if (index == 0) {
            s->x += (s->xMove*4) / 3;
        } else {
            s->x += s->xMove;
        }
        if (s->x > (MAX_LED << 8)) {
            s->x -= (MAX_LED << 8);
        }
        if (s->x < 0) {
            s->x += (MAX_LED << 8);
        }
    }
    
    wStep = s->a;
    wStep <<= 8;
    wStep /= (w + 1);

    xPos = (MAX_LED << 8) + s->x;
    
    w >>=  8;
    if (w > (MAX_LED >> 1)) {
        w = MAX_LED >> 1;
    }
    a = s->a;
    

    s->pixels[((xPos >> 8) + i) % MAX_LED] = a >> 8;
    
    //calculate alpha for each of the pixel
    for (i = 1; i <= w; i++) {
        uint32_t aa;
        a -= wStep;
        aa = a >> 8;
        s->pixels[((xPos >> 8) + i) % MAX_LED] = aa;
        s->pixels[((xPos >> 8) - i) % MAX_LED] = aa;
        //printf("  alpha: %i = %i\n", ((xPos >> 8) + i) % MAX_LED, aa);
    }
    
    // apply Wave effect
    if (fxWave) {
        uint32_t x;
        s->aAnim += s->aMove;
        if (s->aAnim > 0xFFFF) {
            s->aAnim = 0xFFFF;
            s->aMove = -s->aMove;
        } else
        if (s->aAnim < 0x1000) {
            s->aAnim = 0x1000;
            s->aMove = -s->aMove;
        }
        a = s->aAnim ;
        // The central segment waves in oposite phase
        if (index == 1) {
            a = ((0xFFFF + 0x1000) - s->aAnim);
        }
        a >>= 8;
        for (i = 0; i < MAX_LED; i++) {
            s->pixels[i] = (s->pixels[i] * (a)) >> 8;
        }
    }
}

// a colour blending function : blends 2 colours together based on the Alpha channels
static inline void blend_pixel(uint8_t* dst, uint32_t a1, uint32_t r1, uint32_t g1, uint32_t b1, uint32_t a2, uint32_t r2, uint32_t g2, uint32_t b2) {
    register uint32_t a2r;

    // source is fully transparent
    if (a2 == 0) {
        //dst[0] = r1;
        //dst[1] = g1;
        //dst[2] = b1;
        return;
    }

    // destination pixel is completely transparent or source is fully opaque
    if (a1 == 0 || a2 == 0xFF) {
        dst[0] = r2;
        dst[1] = g2;
        dst[2] = b2;
        return;
    }

    //assume destination alpha is 0xFF (fully opaque)
    a1 <<= 8;
    a2 <<= 8;
    a2r = 0xFF00 - a2;
    dst[0] = ((r2 * a2) + (r1 * a2r)) >> 16;
    dst[1] = ((g2 * a2) + (g1 * a2r)) >> 16;
    dst[2] = ((b2 * a2) + (b1 * a2r)) >> 16;
}

// blend the segment (source) into the LED pixel buffer (destination)
void led_segment_blend(int index) {
    int32_t i;
    uint32_t srcA, srcR, srcG, srcB;
    uint32_t pos = 0;
    led_segment* s;

    if (index < 0 || index >= MAX_SEGMENT) {
        return;
    }
    s = &segments[index];

    srcR = s->r;
    srcG = s->g;
    srcB = s->b;
    
    for (i = 0; i < MAX_LED; i++) {
        //printf("blend: seg=%i pos=%i dstA=%i\n", index,  i, s->pixels[i]);
        blend_pixel(leds + pos, 
            0xFF, leds[pos], leds[pos+1], leds[pos+2],
            s->pixels[i], srcR, srcG, srcB);
        pos += 3;
    }
}

static void apply_brightness() {
    int i, pos = 0;
    if (brightness == 0xFF || brightness == 0) {
        return;
    }
    for (i = 0; i < MAX_LED; i++) {
        leds[pos] = (leds[pos] * brightness) >> 8;
        leds[pos+1] = (leds[pos + 1] * brightness) >> 8;
        leds[pos+2] = (leds[pos + 2] * brightness) >> 8;
        pos += 3;
    }
}

// Update all segments and blend them together, then write the colour buffer
// to the SPI device.
void led_update(void) {
    uint32_t i;
    
    memset(leds, 0, sizeof(leds));
    if (state != STATE_STOPPED) {
        for (i = 0; i < MAX_SEGMENT; i++) {
            led_segment_update(i);
            led_segment_blend(i);
        }
        apply_brightness();
        led_write();
    }
}

// Called from within the UI when the Speed slider is used.
void led_set_speed(uint32_t speed) {
    uint32_t i;
    int32_t s;
    int32_t a;
    int32_t x;

    s = (MAX_LED * speed) / 20;
    a = speed * 50;
    x = speed * 2;
    
    //printf("pulse speed=%i\n",s);
    for (i = 0; i < MAX_SEGMENT; i++) {
        segments[i].wMove = segments[i].wMove >= 0 ? s : -s;
        segments[i].aMove = segments[i].aMove >= 0 ? a : -a;
        segments[i].xMove = segments[i].xMove >= 0 ? x : -x;
    }
}

// Called from within the UI when the Volume slider is used.
void led_set_volume(uint32_t volume) {
    uint32_t i;
    uint32_t w;

    w = (MAX_LED * volume) / 200;

    for (i = 0; i < MAX_SEGMENT; i++) {
        led_segment_set_width(i, w);
    }
}

// Sets the colour of a segment based on the index into the colour bar.
// This function is called from the UI when segment colour slider is used.
void led_set_colour(int segment, uint32_t colourIndex) {
    uint32_t pos = (100 - colourIndex) * 3;
    uint32_t col = 0xFF000000;

    if (segment < 0 || segment >= MAX_SEGMENT) {
        return;
    }
    if (pos >= 100 * 3) {
        pos = 99 * 3;
    }
    pos *= 3;
    
    //printf("col= %i %i %i\n", cbar[pos + 0],cbar[pos + 1],cbar[pos + 2]);

    col |= ((uint32_t) cbar[pos + 0]) << 16;
    col |= ((uint32_t) cbar[pos + 1]) << 8;
    col |= ((uint32_t) cbar[pos + 2]);

    led_segment_set_colour(segment, col);
}


void led_set_play(uint8_t on) {
    if (on) {
        state = STATE_PLAY;
    } else {
        if (state != STATE_STOPPED) {
            state = STATE_STOPPING;
        }
    }
}

// this function runs forever in its own thread and updates the LED strip
void* led_update_thread(void* data) {
    // schedule on CPU core 2
    schedule_cpu(2);


    while(!reboot) {
        // handle states & brightness
        switch (state) {
            case STATE_PLAY:
                if (brightness != 0xFF) {
                    brightness += 8;
                    if (brightness > 0xFF) {
                        brightness = 0xFF;
                    }
                }
            break;
            case STATE_STOPPING:
                if (brightness > 0) {
                    brightness -= 8;
                    if (brightness <  0) {
                        brightness = 0;
                    }
                }
            break;
        }
        if (!reboot) {
            usleep(20 * 1000); // 20msec = ~40..50 FPS, depending on the number of colours
        }
        if (!reboot) {
            led_update();
        }
        
        //printf("brightness=%i\n", brightness);
        // switch the state after the LED strip was completely cleared
        if (state == STATE_STOPPING && brightness == 0) {
            state = STATE_STOPPED;
        }
    }
    reboot = 2;
}

// test the RGB functionality by using colour fading effect
static void led_test(void){
    int i;
    int x = 0 * 3;

    for (i = 0; i < 10000; i++) {
        uint8_t val = (i & 0x1F) << 3;
        //uint8_t val = i & 0xFF;
        leds[x] = val;
        leds[x+1] = 0x00;
        leds[x+2] = 0x00;

        leds[x+3] = 0x00;
        leds[x+4] = val;
        leds[x+5] = 0x00;

        leds[x+6] = 0x00;
        leds[x+7] = 0x00;
        leds[x+8] = val;
        led_write();
        usleep(30 * 1000);
    }
}

void led_init(void) {
    int i;
    //already initialised?
    if (spi != NULL) {
        return;
    }
    
    // schedule on CPU core 1
    schedule_cpu(1);

    //open SPI device
    spi = fopen(SPI_DEV, "w");
    if (spi == NULL) {
        printf("SPI open failed\n");
    } else {
        printf("SPI open OK\n");
    }

    //clear the output buffers
    memset(leds, 0, sizeof(leds));
    memset(ledbuf, 0, sizeof(ledbuf));
    memset(segments, 0, sizeof(segments));

    // reset the LED strip to dark
    led_write();

    // set the initial position and colours of the segments 
    i = (MAX_LED << 8) / 4;
    led_segment_set_colour(SEGMENT_LEFT, 0xFFFF0000);
    led_segment_set_position(SEGMENT_LEFT, i >> 8);
    led_segment_set_width(SEGMENT_LEFT, MAX_LED / 4);

    led_segment_set_colour(SEGMENT_CENTER, 0xFF00FF00);
    led_segment_set_position(SEGMENT_CENTER, (i*2) >> 8);
    led_segment_set_width(SEGMENT_CENTER, MAX_LED / 4);

    led_segment_set_colour(SEGMENT_RIGHT, 0xFF0000FF);
    led_segment_set_position(SEGMENT_RIGHT, (i*3) >> 8);
    led_segment_set_width(SEGMENT_RIGHT, MAX_LED / 4);

#if LED_TEST
    // only for debugging
    led_test();
#else
    brightness = 0;
    reboot = 0;
    //start the LED update thread
    pthread_create(&thread1, NULL, led_update_thread, NULL);
   
#endif /*LED_TEST */
}

void led_close(void) {
    if (spi == NULL) {
        return;
    }
    fclose(spi);
    spi = NULL;
}

static void finish() {
    reboot = 1;
    while(reboot != 2) {
        usleep(2 * 1000);
    }
    memset(leds, 0, sizeof(leds));
    led_write();
    led_close();
}

void led_reboot(void) {
    finish();
    system("reboot");
}
