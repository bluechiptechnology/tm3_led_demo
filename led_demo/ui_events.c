// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.4
// LVGL version: 8.3.6
// Project name: SquareLine_Project

#include <stdio.h>
#include "ui.h"
#include "led.h"

void EvPulseOn(lv_event_t * e)
{
	led_set_fx_pulse(1);
}

void EvPulseOff(lv_event_t * e)
{
	led_set_fx_pulse(0);
}

void EvPlayOn(lv_event_t * e)
{
	led_set_play(1);
}

void EvPlayOff(lv_event_t * e)
{
	led_set_play(0);
}

void EvFlowOn(lv_event_t * e)
{
	led_set_fx_flow(1);
}

void EvFlowOff(lv_event_t * e)
{
	led_set_fx_flow(0);
}

void EvWaveOn(lv_event_t * e)
{
	led_set_fx_wave(1);
}

void EvWaveOff(lv_event_t * e)
{
	led_set_fx_wave(0);
}

void ScrPlayInit(lv_event_t * e)
{
	led_init();
}

void EvLeftChanged(lv_event_t * e)
{
	if (e->target) {
		int32_t val = lv_slider_get_value(e->target);
		//printf("left: val=%d\n", val); fflush(stdout);
		led_set_colour(SEGMENT_LEFT, val);
	}
}

void EvRightChanged(lv_event_t * e)
{
	if (e->target) {
		int32_t val = lv_slider_get_value(e->target);
		//printf("right: val=%d\n", val); fflush(stdout);
		led_set_colour(SEGMENT_RIGHT, val);
	}
}

void EvCenterChanged(lv_event_t * e)
{
	if (e->target) {
		int32_t val = lv_slider_get_value(e->target);
		//printf("center: val=%d\n", val); fflush(stdout);
		led_set_colour(SEGMENT_CENTER, val);
	}
}

void EvVolumeChanged(lv_event_t * e)
{
	if (e->target) {
		int32_t val = lv_slider_get_value(e->target);
		//printf("volume: val=%d\n", val); fflush(stdout);
		led_set_volume((uint32_t)val);
	}
}

void EvSpeedChanged(lv_event_t * e)
{
	if (e->target) {
		int32_t val = lv_slider_get_value(e->target);
		//printf("speed: val=%d\n", val); fflush(stdout);
		led_set_speed((uint32_t)val);
	}
}

void EventRebootClicked(lv_event_t * e)
{
	led_reboot();
}