/*
 *
 * OTB-IOT - Out of The Box Internet Of Things
 *
 * Copyright (C) 2016 Piers Finlayson
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version. 
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for 
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

// #define OTB_DEBUG 1
#define OTB_ARDUINO 1

#include <Arduino.h>
//#include "user_interface.h"
#include "osapi.h"
#include "c_types.h"
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"
#include "smartconfig.h"
#include "espconn.h"
#include "mqtt_msg.h"
#include "mqtt_user_config.h"
#include "mqtt.h"
#include "queue.h"
#include "otb_def.h"
#include "otb_globals.h"
#include "otb_macros.h"
#include "otb_main.h"
#include "otb_wifi.h"
#include "otb_util.h"
#include "otb_temp.h"
#include "otb_ow.h"
#include "otb_mqtt.h"
#include "otb_sched.h"

// Arduino C++ functions exposed as C functions
extern void reset(void);
