/**************************************************************************
 *
 * \file
 *
 * \brief Processing of USB device specific enumeration requests.
 *
 * This file contains the specific request decoding for enumeration process.
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 ***************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */


//_____ I N C L U D E S ____________________________________________________

#include "conf_usb.h"


#if USB_DEVICE_FEATURE == true

#include "usb_drv.h"
#include "usb_descriptors.h"
#include "usb_standard_request.h"
#include "usb_specific_request.h"


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________


//_____ P R I V A T E   D E C L A R A T I O N S ____________________________

extern volatile uint8_t    usb_hid_report_mouse[];
uint8_t g_u8_report_rate=0;

extern const    void *pbuffer;
extern          uint16_t   data_to_transfer;


//_____ D E C L A R A T I O N S ____________________________________________



//! @brief This function manages hid set idle request.
//!
//! @param u8_report_id    0 the idle rate applies to all input reports, else only applies to the Report ID
//! @param u8_duration     When the upper byte of wValue is 0 (zero), the duration is indefinite else from 0.004 to 1.020 seconds

//!
void usb_hid_set_idle (uint8_t u8_report_id, uint8_t u8_duration )
{
   uint16_t wInterface;

   // Get interface number to put in idle mode
   wInterface=Usb_read_endpoint_data(EP_CONTROL, 16);
   Usb_ack_setup_received_free();

   if( wInterface == INTERFACE_NB_MOUSE )
     g_u8_report_rate = u8_duration;

   Usb_ack_control_in_ready_send();
   while (!Is_usb_control_in_ready());
}


//! @brief This function manages hid get idle request.
//!
//! @param u8_report_id    0 the idle rate applies to all input reports, else only applies to the Report ID
//!
void usb_hid_get_idle (uint8_t u8_report_id)
{
   uint16_t wLength;
   uint16_t wInterface;

   // Get interface number to put in idle mode
   wInterface=Usb_read_endpoint_data(EP_CONTROL, 16);
   wLength   =Usb_read_endpoint_data(EP_CONTROL, 16);
   Usb_ack_setup_received_free();

   if( (wLength != 0) && (wInterface == INTERFACE_NB_MOUSE) )
   {
      Usb_write_endpoint_data(EP_CONTROL, 8, g_u8_report_rate);
      Usb_ack_control_in_ready_send();
   }

   while (!Is_usb_control_out_received());
   Usb_ack_control_out_received_free();
}


//! This function manages the HID Get_Descriptor request.
//!
static void hid_get_descriptor(uint8_t size_of_report, const uint8_t* p_usb_hid_report)
{
  bool  zlp;
  uint16_t   wIndex;
  uint16_t   wLength;

  zlp = false;                                              /* no zero length packet */

  data_to_transfer = size_of_report;
  pbuffer          = p_usb_hid_report;

  wIndex = Usb_read_endpoint_data(EP_CONTROL, 16);
  wIndex = usb_format_usb_to_mcu_data(16, wIndex);
  wLength = Usb_read_endpoint_data(EP_CONTROL, 16);
  wLength = usb_format_usb_to_mcu_data(16, wLength);
  Usb_ack_setup_received_free();                          //!< clear the setup received flag

  if (wLength > data_to_transfer)
  {
    zlp = !(data_to_transfer % EP_CONTROL_LENGTH);  //!< zero length packet condition
  }
  else
  {
    data_to_transfer = wLength; //!< send only requested number of data bytes
  }

  Usb_ack_nak_out(EP_CONTROL);

  while (data_to_transfer && (!Is_usb_nak_out(EP_CONTROL)))
  {
    while( !Is_usb_control_in_ready() && !Is_usb_nak_out(EP_CONTROL) );

    if( Is_usb_nak_out(EP_CONTROL) )
       break;    // don't clear the flag now, it will be cleared after

    Usb_reset_endpoint_fifo_access(EP_CONTROL);
    data_to_transfer = usb_write_ep_txpacket(EP_CONTROL, pbuffer,
                                             data_to_transfer, &pbuffer);
    if( Is_usb_nak_out(EP_CONTROL) )
       break;
    else
       Usb_ack_control_in_ready_send();  //!< Send data until necessary
  }

  if ( zlp && (!Is_usb_nak_out(EP_CONTROL)) )
  {
    while (!Is_usb_control_in_ready());
    Usb_ack_control_in_ready_send();
  }

  while (!(Is_usb_nak_out(EP_CONTROL)));
  Usb_ack_nak_out(EP_CONTROL);
  while (!Is_usb_control_out_received());
  Usb_ack_control_out_received_free();
}


//! @brief This function configures the endpoints of the device application.
//! This function is called when the set configuration request has been received.
//!
void usb_user_endpoint_init(uint8_t conf_nb)
{
#if (USB_HIGH_SPEED_SUPPORT==false)
  (void)Usb_configure_endpoint(EP_HID_MOUSE_IN,
                               EP_ATTRIBUTES_1,
                               DIRECTION_IN,
                               EP_SIZE_1_FS,
                               SINGLE_BANK);

#else
   if( Is_usb_full_speed_mode() )
   {
     (void)Usb_configure_endpoint(EP_HID_MOUSE_IN,
                                  EP_ATTRIBUTES_1,
                                  DIRECTION_IN,
                                  EP_SIZE_1_FS,
                                  SINGLE_BANK);
   }else{
     (void)Usb_configure_endpoint(EP_HID_MOUSE_IN,
                                  EP_ATTRIBUTES_1,
                                  DIRECTION_IN,
                                  EP_SIZE_1_HS,
                                  SINGLE_BANK);
   }
#endif
}


//! This function is called by the standard USB read request function when
//! the USB request is not supported. This function returns true when the
//! request is processed. This function returns false if the request is not
//! supported. In this case, a STALL handshake will be automatically
//! sent by the standard USB read request function.
//!
bool usb_user_read_request(uint8_t type, uint8_t request)
{
   uint8_t    wValue_msb;
   uint8_t    wValue_lsb;

   // Read wValue
   wValue_lsb = Usb_read_endpoint_data(EP_CONTROL, 8);
   wValue_msb = Usb_read_endpoint_data(EP_CONTROL, 8);
/*
   uint8_t descriptor_type;

  Usb_read_endpoint_data(EP_CONTROL, 8); // string_type
  descriptor_type = Usb_read_endpoint_data(EP_CONTROL, 8);
*/

   //** Specific request from Class HID
   if( 0x81 == type )   // USB_SETUP_GET_STAND_INTERFACE
   {
      switch( request )
      {
         case GET_DESCRIPTOR:
         switch( wValue_msb ) // Descriptor ID
         {
#if (USB_HIGH_SPEED_SUPPORT==false)
            case HID_DESCRIPTOR:
            hid_get_descriptor(
               sizeof(usb_conf_desc_fs.hid_mouse)
            ,  (const uint8_t*)&usb_conf_desc_fs.hid_mouse);
            return true;
#else
            case HID_DESCRIPTOR:
            if( Is_usb_full_speed_mode() )
            {
               hid_get_descriptor(
                  sizeof(usb_conf_desc_fs.hid_mouse)
               ,  (const uint8_t*)&usb_conf_desc_fs.hid_mouse);
            }else{
               hid_get_descriptor(
                  sizeof(usb_conf_desc_hs.hid_mouse)
               ,  (const uint8_t*)&usb_conf_desc_hs.hid_mouse);
            }
            return true;
#endif

            case HID_REPORT_DESCRIPTOR:
            hid_get_descriptor(
               sizeof(usb_hid_report_descriptor_mouse)
            ,  usb_hid_report_descriptor_mouse);
            return true;

            case HID_PHYSICAL_DESCRIPTOR:
            // TODO
            break;
         }
         break;
      }
   }
   if( 0x21 == type ) // USB_SETUP_SET_CLASS_INTER
   {
      switch( request )
      {
         case HID_SET_REPORT:
         // The MSB wValue field specifies the Report Type
         // The LSB wValue field specifies the Report ID
         switch (wValue_msb)
         {
            case HID_REPORT_INPUT:
            // TODO
            break;

            case HID_REPORT_OUTPUT:
            Usb_ack_setup_received_free();
            while (!Is_usb_control_out_received());
            Usb_ack_control_out_received_free();
            Usb_ack_control_in_ready_send();
            while (!Is_usb_control_in_ready());
            return true;

            case HID_REPORT_FEATURE:
            break;
         }
         break;

         case HID_SET_IDLE:
         usb_hid_set_idle(wValue_lsb, wValue_msb);
         return true;

         case HID_SET_PROTOCOL:
         // TODO
         break;
      }
   }
   if( 0xA1 == type ) // USB_SETUP_GET_CLASS_INTER
   {
      switch( request )
      {
         case HID_GET_REPORT:
         // TODO
         break;
         case HID_GET_IDLE:
         usb_hid_get_idle(wValue_lsb);
         return true;
         case HID_GET_PROTOCOL:
         // TODO
         break;
      }
   }
   return false;  // No supported request
}


//! This function returns the size and the pointer on a user information
//! structure
//!
bool usb_user_get_descriptor(uint8_t type, uint8_t string)
{
  pbuffer = NULL;

  switch (type)
  {
  case STRING_DESCRIPTOR:
    switch (string)
    {
    case LANG_ID:
      data_to_transfer = sizeof(usb_user_language_id);
      pbuffer = &usb_user_language_id;
      break;

    case MAN_INDEX:
      data_to_transfer = sizeof(usb_user_manufacturer_string_descriptor);
      pbuffer = &usb_user_manufacturer_string_descriptor;
      break;

    case PROD_INDEX:
      data_to_transfer = sizeof(usb_user_product_string_descriptor);
      pbuffer = &usb_user_product_string_descriptor;
      break;

    case SN_INDEX:
      data_to_transfer = sizeof(usb_user_serial_number);
      pbuffer = &usb_user_serial_number;
      break;

    default:
      break;
    }
    break;

  default:
    break;
  }

  return pbuffer != NULL;
}


#endif  // USB_DEVICE_FEATURE == true
