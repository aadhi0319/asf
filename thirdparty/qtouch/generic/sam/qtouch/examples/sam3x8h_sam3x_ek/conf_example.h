/**
 * \file
 *
 * \brief Example configuration.
 *
 * Copyright (c) 2011-2018 Microchip Technology Inc. and its subsidiaries.
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

#ifndef CONF_EXAMPLE_H_INCLUDED
#define CONF_EXAMPLE_H_INCLUDED

/** Qtouch key number */
#define BOARD_KEY_NUM_5

/** The PIO numbers for silder is before key */
#define BOARD_SILDER_BEFOR_KEY

/** Qtouch valid key ID */
#define BOARD_VALID_KEY_ID    1

/** Qtouch up key ID */
#define BOARD_UP_KEY_ID    2

/** Qtouch down key ID */
#define BOARD_DOWN_KEY_ID    3

/** Qtouch left key ID */
#define BOARD_LEFT_KEY_ID    4

/** Qtouch right key ID */
#define BOARD_RIGHT_KEY_ID    5

/** Qtouch valid key channel */
#define BOARD_VALID_KEY_CHANNEL    CHANNEL_25

/** Qtouch up key channel */
#define BOARD_UP_KEY_CHANNEL    CHANNEL_26

/** Qtouch down key channel */
#define BOARD_DOWN_KEY_CHANNEL    CHANNEL_27

/** Qtouch left key channel */
#define BOARD_LEFT_KEY_CHANNEL    CHANNEL_28

/** Qtouch right key channel */
#define BOARD_RIGHT_KEY_CHANNEL    CHANNEL_29

/** Qtouch slider start channel */
#define BOARD_SLIDER_START_CHANNEL    CHANNEL_0

/** Qtouch slider end channel */
#define BOARD_SLIDER_END_CHANNEL    CHANNEL_2

#endif /* CONF_EXAMPLE_H_INCLUDED */
