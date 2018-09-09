/**
 * \file
 *
 * \brief Main functions
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
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef _MAIN_H_
#define _MAIN_H_

/*! \brief Called by CDC interface
 * Callback running when USB Host enable CDC interface
 *
 * \retval true if cdc startup is successfully done
 */
bool main_cdc_enable(void);

/*! \brief Called by CDC interface
 * Callback running when USB Host disable cdc interface
 */
void main_cdc_disable(void);

/**
 * \brief Callback that is called once a byte is received from USB
 */
void usb_rx_notify(void);

/**
 * \brief Callback that is called once a frame is received.
 *
 * \param rx_frame_array    Pointer for the received frame
 */
void at86rfx_tal_rx_frame_cb(uint8_t * rx_frame_array);

/**
 * \brief Callback that is called once tx is done.
 *
 * \param status    Status of the transmission procedure
 */
void at86rfx_tal_tx_status_cb(uint8_t status);

#endif // _MAIN_H_
