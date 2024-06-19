// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.4
// LVGL version: 8.3.6
// Project name: SquareLine_Project

#ifndef _UI_EVENTS_H
#define _UI_EVENTS_H

#ifdef __cplusplus
extern "C" {
#endif

void ScrPlayInit(lv_event_t * e);
void EvPulseOn(lv_event_t * e);
void EvPulseOff(lv_event_t * e);
void EvFlowOn(lv_event_t * e);
void EvFlowOff(lv_event_t * e);
void EvWaveOn(lv_event_t * e);
void EvWaveOff(lv_event_t * e);
void EvRightChanged(lv_event_t * e);
void EvLeftChanged(lv_event_t * e);
void EvCenterChanged(lv_event_t * e);
void EvPlayOn(lv_event_t * e);
void EvPlayOff(lv_event_t * e);
void EvVolumeChanged(lv_event_t * e);
void EvSpeedChanged(lv_event_t * e);
void EventRebootClicked(lv_event_t * e);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif