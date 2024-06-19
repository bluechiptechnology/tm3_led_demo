#ifndef _LED_H_
#define _LED_H_

#define SEGMENT_LEFT 0
#define SEGMENT_CENTER 1
#define SEGMENT_RIGHT 2

void led_init(void);
void led_close(void);
void led_set_volume(uint32_t volume);
void led_set_colour(int segment, uint32_t colour);
void led_set_speed(uint32_t speed);

void led_set_fx_pulse(uint8_t on);
void led_set_fx_wave(uint8_t on);
void led_set_fx_flow(uint8_t on);

void led_set_play(uint8_t on);

void led_reboot(void);

#endif
