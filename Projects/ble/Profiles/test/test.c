#include "bcomdef.h"
#include "OSAL.h"
#include "linkdb.h"
#include "att.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "gattservapp.h"
#include "hal_led.h"

#include "test.h"

static char string1[] = "hello world";
static char string2[] = "fuck you";

static uint8 var1=0x55;
static uint8 var2=0xaa;
static uint8 LedValue = 0;

CONST uint8 TestServiceUUID[ATT_BT_UUID_SIZE] =
{ 
        LO_UINT16(TEST_SERVICE_UUID), HI_UINT16(TEST_SERVICE_UUID)
};

CONST uint8 Var1UUID[ATT_BT_UUID_SIZE] =
{ 
        LO_UINT16(VAR1_SERVICE_UUID), HI_UINT16(VAR1_SERVICE_UUID)
};

CONST uint8 Var2UUID[ATT_BT_UUID_SIZE] =
{ 
        LO_UINT16(VAR2_SERVICE_UUID), HI_UINT16(VAR2_SERVICE_UUID)
};

CONST uint8 EnableUUID[ATT_BT_UUID_SIZE] =
{ 
        LO_UINT16(ENABLE_SERVICE_UUID), HI_UINT16(ENABLE_SERVICE_UUID)
};

static uint8 Var1UserDesc[] = "test var1\0";
static uint8 Var2UserDesc[] = "test var2\0";
static uint8 EnableUserDesc[] = "enable\0";
static uint8 TestCharProps = GATT_PROP_READ | GATT_PERMIT_WRITE;

static CONST gattAttrType_t testService = { ATT_BT_UUID_SIZE, TestServiceUUID };

static gattAttribute_t TestAttrTbl[] = 
{
         /* test service */
        { 
                { ATT_BT_UUID_SIZE, primaryServiceUUID }, /* type */
                GATT_PERMIT_READ,                   /* permissions */
                0,                                  /* handle */
                (uint8 *)&testService                /* pValue */
        },
        
        /* prop */
        { 
                { ATT_BT_UUID_SIZE, characterUUID },
                GATT_PERMIT_READ, 
                0,
                &TestCharProps 
        },
        /* value */
        { 
                { ATT_BT_UUID_SIZE, Var1UUID },
                GATT_PERMIT_READ | GATT_PERMIT_WRITE, 
                0,
                (uint8 *)&var1 
        },
        /* desc */
        { 
                { ATT_BT_UUID_SIZE, charUserDescUUID },
                GATT_PERMIT_READ, 
                0,
                Var1UserDesc 
        },
        
        /* prop */
        { 
                { ATT_BT_UUID_SIZE, characterUUID },
                GATT_PERMIT_READ, 
                0,
                &TestCharProps 
        },
        /* value */
        { 
                { ATT_BT_UUID_SIZE, Var2UUID },
                GATT_PERMIT_READ | GATT_PERMIT_WRITE, 
                0,
                (uint8 *)&var2
        },
        /* desc */
        { 
                { ATT_BT_UUID_SIZE, charUserDescUUID },
                GATT_PERMIT_READ, 
                0,
                Var2UserDesc 
        },   

        /* prop */
        { 
                { ATT_BT_UUID_SIZE, characterUUID },
                GATT_PERMIT_READ, 
                0,
                &TestCharProps 
        },
        /* value */
        { 
                { ATT_BT_UUID_SIZE, EnableUUID },
                GATT_PERMIT_READ | GATT_PERMIT_WRITE, 
                0,
                (uint8 *)&LedValue
        },
        /* desc */
        { 
                { ATT_BT_UUID_SIZE, charUserDescUUID },
                GATT_PERMIT_READ, 
                0,
                EnableUserDesc 
        },                 
};

static uint8 test_ReadAttrCB( uint16 connHandle, gattAttribute_t *pAttr, 
                             uint8 *pValue, uint8 *pLen, uint16 offset, uint8 maxLen );
static bStatus_t test_WriteAttrCB( uint16 connHandle, gattAttribute_t *pAttr, 
                                  uint8 *pValue, uint8 len, uint16 offset );

// test Profile Service Callbacks
CONST gattServiceCBs_t testProfileCBs =
{
  test_ReadAttrCB,  // Read callback function pointer
  test_WriteAttrCB, // Write callback function pointer
  NULL                       // Authorization callback function pointer
};

bStatus_t test_InitService( uint32 services )
{
  uint8 status = SUCCESS;

    // Register GATT attribute list and CBs with GATT Server App
    status = GATTServApp_RegisterService( TestAttrTbl, GATT_NUM_ATTRS( TestAttrTbl ), &testProfileCBs);

  return ( status );
}

static uint8 test_ReadAttrCB( uint16 connHandle, gattAttribute_t *pAttr, 
                               uint8 *pValue, uint8 *pLen, uint16 offset, uint8 maxLen )
{
  uint16 uuid;
  bStatus_t status = SUCCESS;

  if ( offset > 0 )
      return ( ATT_ERR_ATTR_NOT_LONG );


  if ( pAttr->type.len == ATT_BT_UUID_SIZE )
  {    
    // 16-bit UUID
    uuid = BUILD_UINT16( pAttr->type.uuid[0], pAttr->type.uuid[1]);
    switch ( uuid )
    {
      case VAR1_SERVICE_UUID:
      case VAR2_SERVICE_UUID:
        case ENABLE_SERVICE_UUID:
                 *pLen = 1;
        pValue[0] = *pAttr->pValue;       
        break;
      
      default:
        *pLen = 0;
        status = ATT_ERR_ATTR_NOT_FOUND;
        break;
    }
  }
  else
  {
    // 128-bit UUID
    *pLen = 0;
    status = ATT_ERR_INVALID_HANDLE;
  }
  return ( status );
}

static bStatus_t test_WriteAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
                                    uint8 *pValue, uint8 len, uint16 offset )
{
  bStatus_t status = SUCCESS;
  
  if ( pAttr->type.len == ATT_BT_UUID_SIZE )
  {
    
    uint16 uuid = BUILD_UINT16( pAttr->type.uuid[0], pAttr->type.uuid[1]);
    switch ( uuid )
    {
      case VAR1_SERVICE_UUID:
      case VAR2_SERVICE_UUID:
      case ENABLE_SERVICE_UUID:
        if (len == 1)
                *pAttr->pValue = pValue[0];      
        break;      
      default:
          // Should never get here!
          status = ATT_ERR_ATTR_NOT_FOUND;
          break;
    }
  }
  else
  {
    // 128-bit UUID
    status = ATT_ERR_INVALID_HANDLE;
  }  

     HalLedSet( LedValue & 0x0f, HAL_LED_MODE_ON );
     HalLedSet( ~LedValue & 0x0f, HAL_LED_MODE_OFF );
  
  return ( status );
}

