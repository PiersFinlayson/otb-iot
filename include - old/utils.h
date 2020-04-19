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

#ifndef _UTILS_H_
#define	_UTILS_H_

#ifndef ESPUT
#include "esp_types.h"
#else
#include "esput.h"
#endif

uint32_t ICACHE_FLASH_ATTR UTILS_Atoh(const int8_t *s);
uint8_t ICACHE_FLASH_ATTR UTILS_StrToIP(const int8_t* str, void *ip);
uint8_t ICACHE_FLASH_ATTR UTILS_IsIPV4 (int8_t *str);
#endif
