#include "bcomdef.h"
#include "OSAL.h"
#include "OSAL_PwrMgr.h"
#include "OnBoard.h"
#include "hal_adc.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_lcd.h"
#include "gatt.h"
#include "hci.h"

#include "app.h"

static uint8 led_task_id;   
static uint8 key_task_id; 
static uint8 d2_state;
static uint8 d3_state;
static uint8 d4_state;
static uint8 d6_state;

#define     KEY_DETECT_PERIOD        10     /* key detect period 100ms */

/* event */
#define     KEY_DETECT_EVENT         0x0001

#define   BIT(x)    (1 << x)

uint16 led_ProcessEvent(uint8 task_id, uint16 events)
{
  return 0;
}

uint16 key_ProcessEvent(uint8 task_id, uint16 events)
{
  static uint8 value = 0x0f;
  uint8 key_mask = 0x0f;
  int key_status;
 
  if ( events & KEY_DETECT_EVENT )
  {
#if 1    
    key_status = read_key();
    set_led(key_mask, key_status);    
#else
    value = ~value;    
    set_led(0x0f, value); 
#endif    
    
    osal_start_timerEx(key_task_id, KEY_DETECT_EVENT, KEY_DETECT_PERIOD);    
  }
  
  return 0;
}

void set_led(uint8 mask, uint8 value)
{
  if ((mask & BIT(0)) && (value & BIT(0)))    /* D2 on */
    P1 &= ~BIT(0);
  else if ((mask & BIT(0)) && !(value & BIT(0)))
    P1 |= BIT(0);
 
  if ((mask & BIT(1)) && (value & BIT(1)))    /* D3 on */
    P1 &= ~BIT(1);
  else if ((mask & BIT(1)) && !(value & BIT(1)))
    P1 |= BIT(1);  

  if ((mask & BIT(2)) && (value & BIT(2)))    /* D4 on */
    P2 &= ~BIT(0);
  else if ((mask & BIT(2)) && !(value & BIT(2)))
    P2 |= BIT(0);  

  if ((mask & BIT(3)) && (value & BIT(3)))    /* D6 on */
    P1 &= ~BIT(3);
  else if ((mask & BIT(3)) && !(value & BIT(3)))
    P1 |= BIT(3);
}

uint8 read_key(void)
{
  uint8 mask = 0;

  if (P0 & BIT(0))      /* read K1 */
    mask |= BIT(0); 
  if (P0 & BIT(1))      /* read K2 */
    mask |= BIT(1);    
  if (P0 & BIT(4))      /* read K3 */
    mask |= BIT(2);
  if (P1 & BIT(2))      /* read K6 */
    mask |= BIT(3);
  
  return mask;
}

void led_init(uint8 task_id)
{
  led_task_id = task_id;
  
  P0SEL = 0;      // Configure Port 0 as GPIO
  P1SEL = 0;      // Configure Port 1 as GPIO
  P2SEL = 0;      // Configure Port 2 as GPIO
  
  /* set gpio direction for led */
  P1DIR |= (BIT(0) | BIT(1) | BIT(3));     
  P2DIR |= (BIT(0)); 
  
  /* set gpio direction for key */
  P0DIR &= ~(BIT(0) | BIT(1) | BIT(4));     
  P1DIR &= ~(BIT(2)); 

 /* init led status */  
  set_led(0x0f, 0x05);     /* D2, D4 on */
  
  d2_state = 1;
  d3_state = 0;
  d4_state = 1;
  d6_state = 0;
}

void key_init(uint8 task_id)
{
  key_task_id = task_id;
  
  osal_start_timerEx(key_task_id, KEY_DETECT_EVENT, KEY_DETECT_PERIOD);
}

/*********************  for app test  ***********************/
static uint8 app_task_id;
void app_init(uint8 task_id)
{
  app_task_id = task_id;

  P0SEL = 0; // Configure Port 0 as GPIO
  P1SEL = 0; // Configure Port 1 as GPIO
  P2SEL = 0; // Configure Port 2 as GPIO                                             
  
  /* set gpio direction for led */  
  P1DIR |= (BIT(0) | BIT(1) | BIT(3));     
  P2DIR |= (BIT(0)); 
  
  /* set gpio direction for key */
  P0DIR &= ~(BIT(0) | BIT(1) | BIT(4));     
  P1DIR &= ~(BIT(2));    
  
//  RegisterForKeys( task_id );
  
  HalLedSet( (HAL_LED_1 | HAL_LED_2), HAL_LED_MODE_ON );

//    HalLedSet( (HAL_LED_1 | HAL_LED_3), HAL_LED_MODE_OFF );
//    HalLedSet( (HAL_LED_2 | HAL_LED_4), HAL_LED_MODE_OFF );
//            HalLedSet( (HAL_LED_1 | HAL_LED_3), HAL_LED_MODE_ON );     
}

uint16 app_ProcessEvent(uint8 task_id, uint16 events)
{
#if 0
  if ( events & SYS_EVENT_MSG )
  {   
    uint8 *pMsg;

//        HalLedSet( (HAL_LED_1 | HAL_LED_3), HAL_LED_MODE_ON );     

    if ( (pMsg = osal_msg_receive( app_task_id )) != NULL )
    {
      app_ProcessOSALMsg( (osal_event_hdr_t *)pMsg );

      VOID osal_msg_deallocate( pMsg );
    }

    return (events ^ SYS_EVENT_MSG);
  }
#endif
  return 0;
}

static void app_ProcessOSALMsg( osal_event_hdr_t *pMsg )
{
  switch ( pMsg->event )
  {
    case KEY_CHANGE:   
      app_HandleKeys( ((keyChange_t *)pMsg)->state, ((keyChange_t *)pMsg)->keys );
      break;
  }
}

static void app_HandleKeys( uint8 shift, uint8 keys )
{
#if 0
  if (keys & HAL_KEY_SW_1)
  {
    HalLedSet( HAL_LED_1, HAL_LED_MODE_ON );
  }
  
  if (keys & HAL_KEY_SW_2)
  {
    HalLedSet( HAL_LED_2, HAL_LED_MODE_ON );
  }
  
  if (keys & HAL_KEY_SW_3)
  {
    HalLedSet( HAL_LED_3, HAL_LED_MODE_ON );
  }
  
  if (keys & HAL_KEY_SW_4)
  {
    HalLedSet( HAL_LED_4, HAL_LED_MODE_ON );
  }  
#endif  
}