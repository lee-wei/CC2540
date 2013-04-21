#ifndef   __APP_H__
#define   __APP_H__

extern uint16 led_ProcessEvent(uint8 task_id, uint16 events);

extern uint16 key_ProcessEvent(uint8 task_id, uint16 events);

void set_led(uint8 mask, uint8 value);

uint8 read_key(void);

extern void led_init(uint8 task_id);

extern void key_init(uint8 task_id);

extern uint16 app_ProcessEvent(uint8 task_id, uint16 events);
extern void app_init(uint8 task_id);
void app_ProcessOSALMsg( osal_event_hdr_t *pMsg );
static void app_HandleKeys( uint8 shift, uint8 keys );

#endif