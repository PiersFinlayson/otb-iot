/*
 * OTB-IOT - Out of The Box Internet Of Things
 *
 * Copyright (C) 2016-2020 Piers Finlayson
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
 */

#define OTB_UTIL_C
#include "otb.h"
#include <stdarg.h>
#include <limits.h>
#include <errno.h>
#include <uart_register.h>

void ICACHE_FLASH_ATTR otb_util_booted(void)
{
  DEBUG("UTIL: otb_util_booted entry");

  if (!otb_util_booted_g)
  {
    INFO("OTB: Boot sequence completed");
    otb_util_booted_g = TRUE;
  }

  DEBUG("UTIL: otb_util_booted exit");

  return;
}

void ICACHE_FLASH_ATTR otb_util_flash_init(void)
{

  DEBUG("UTIL: otb_util_flash_init entry");
  
#ifdef OTB_SUPER_BIG_FLASH_8266  
  uint32_t id_size;
  uint32_t actual_size;
  int ii;

  id_size = (flashchip->deviceId >> 16) & 0xff;
  otb_flash_size_actual = 1;
  for (ii = 0; ii < id_size; ii++)
  {
    otb_flash_size_actual *= 2;
  }
  otb_flash_size_sdk = flashchip->chip_size;
  otb_flash_size_actual = actual_size;
  DETAIL("UTIL: Flash size from sdk: %d bytes", otb_flash_size_sdk);
  DETAIL("UTIL: Flash size actual:   %d bytes", otb_flash_size_actual);
  OTB_ASSERT(otb_flash_size_actual >= otb_flash_size_sdk); 
  if (otb_flash_size_actual == otb_flash_size_sdk)
  {
    // Optimisation so we can avoid a bit of processing when calling spi_flash_ functions
    otb_flash_size_actual = 0;
  }
#endif // OTB_SUPER_BIG_FLASH_8266  

  DEBUG("UTIL: otb_util_flash_init exit");
  
  return;
}

#ifdef OTB_SUPER_BIG_FLASH_8266

SpiFlashOpResult ICACHE_FLASH_ATTR otb_util_spi_flash_erase_sector_big(uint16 sector)
{
  int8 status;

  flashchip->chip_size = otb_flash_size_actual;

  status = spi_flash_erase_sector(sector);

  flashchip->chip_size = otb_flash_size_sdk;

  return status;
}

SpiFlashOpResult ICACHE_FLASH_ATTR otb_util_spi_flash_write_big(uint32 des_addr, uint32 *src_addr, uint32 size)
{
  int8 status;

  flashchip->chip_size = otb_flash_size_actual;

  status = spi_flash_write(des_addr, src_addr, size);

  flashchip->chip_size = otb_flash_size_sdk;

  return status;
}

SpiFlashOpResult ICACHE_FLASH_ATTR otb_util_spi_flash_read_big(uint32 src_addr, uint32 *des_addr, uint32 size)
{
  int8 status;
  
  flashchip->chip_size = otb_flash_size_actual;

  status = spi_flash_read(src_addr, des_addr, size);

  flashchip->chip_size = otb_flash_size_sdk;

  return status;
}

#endif // OTB_SUPER_BIG_FLASH_8266

#if 0

void ICACHE_FLASH_ATTR otb_util_factory_reset(void)
{
  rc = FALSE;

  DEBUG("UTIL: otb_util_factory_reset entry");
  
  // First of all reset config
  otb_led_wifi_blink();  // Will turn off
  otb_conf_init_config(otb_conf); 
  rc = otb_conf_save(otb_conf);
  if (!rc)
  {
    // But plow on regardless
    WARN("UTIL: Failed to reset config to factory");
  }
  otb_led_wifi_blink();  // Will turn back on
  
  // Now update boot slot
  rc = otb_rboot_update_slot(0);  // Updates slot to 0
  if (!rc)
  {
    // But plow on regardless
    WARN("UTIL: Failed to reset boot slot to 0");
  }

  // Now actually do the firmware update
  
  // check factory partition is valid
  if (!ota_rboot_check_factory_image())
  {
    // Can't recover from this
    ERROR("UTIL: No good factory image");
    goto EXIT_LABEL;
  }
  
  if (ota_rboot_use_factory_image())
  {
    rc = TRUE;
  }
  else
  {
    // Can't recover from this
    ERROR("UTIL: Failed to update boot image with factory");
    goto EXIT_LABEL;
  }
  
  // Get length of recovery partition
  // Copy a sector at a time from recovery parition to partition 0
  // Blinking between each sector

  // Cyan for good, blue for bad
  if (rc)
  {
    otb_led_wifi_update(OTB_LED_NEO_COLOUR_CYAN, TRUE);
  }
  else
  {
    otb_led_wifi_update(OTB_LED_NEO_COLOUR_BLUE, TRUE);
  }
  
  // Blink for 5s before reset to indicate completion
  otb_led_wifi_init_blink_timer();
  otb_reset_schedule(otb_util_factory_reset_reason, 5000, FALSE);
  
  DEBUG("UTIL: otb_util_factory_reset exit");
  
  return;
}
#endif

size_t ICACHE_FLASH_ATTR otb_util_copy_flash_to_ram(char *dst, const char *from_ptr_byte, int size)
{
        int from, to;
        uint32_t current32, byte;
        uint8_t current8;
        const uint32_t *from_ptr;

        from_ptr = (const uint32_t *)(const void *)from_ptr_byte;

        for(from = 0, to = 0; (int)(from * sizeof(*from_ptr)) < (size - 1); from++)
        {
                current32 = from_ptr[from];

                for(byte = 4; byte > 0; byte--)
                {
                        if((current8 = (current32 & 0x000000ff)) == '\0')
                                goto done;

                        if((to + 1) >= size)
                                goto done;

                        dst[to++] = (char)current8;
                        current32 = (current32 >> 8) & 0x00ffffff;
                }
        }

done:
        dst[to] = '\0';

        return(to);
}

void ICACHE_FLASH_ATTR otb_util_convert_char_to_char(char *text, int from, int to)
{
  char froms[2];
  char *match;
  
  DEBUG("UTIL: otb_util_convert_char_to_char entry");
  
  OTB_ASSERT(from != to);
  
  froms[0] = (char)from;
  froms[1] = 0;
  match = os_strstr(text, froms);
  while (match)
  {
    *match = to;
    match = os_strstr(match, froms);
  }

  DEBUG("UTIL: otb_util_convert_char_to_char exit");

  return;  
}

// 3 options:
// - if have serial number (from eeprom) use it
// - otherwise if can get station mac use it
// - otherwise use device chip id
// Note ap MAC uses invalid vendor ID, which is why we use the station mac
void ICACHE_FLASH_ATTR otb_util_get_chip_id(void)
{
  bool mac_rc;
  char mac[OTB_WIFI_MAC_ADDRESS_STRING_LENGTH];
  char mac_bit[3];
  int ii, jj, kk, offset;
  bool trailing;
  bool rc = TRUE;

  DEBUG("UTIL: otb_util_get_chip_id entry");

  OTB_ASSERT(OTB_MAIN_CHIPID_STR_LENGTH >= (OTB_EEPROM_HW_SERIAL_LEN+1));
  os_memset(OTB_MAIN_CHIPID, 0, OTB_MAIN_CHIPID_STR_LENGTH);
  
  if ((otb_eeprom_main_board_g != NULL) &&
      (os_strnlen(otb_eeprom_main_board_g->common.serial,
                  OTB_EEPROM_HW_SERIAL_LEN+1) <= OTB_EEPROM_HW_SERIAL_LEN) &&
      (os_strnlen(otb_eeprom_main_board_g->common.serial,
                  OTB_EEPROM_HW_SERIAL_LEN+1) > 0))
  {
    DEBUG("UTIL: Using serial number as chipid");
    // Use serial number
    
    // Get rid of any spaces
    trailing = TRUE;
    for (ii = 0, jj = 0; ii < OTB_EEPROM_HW_SERIAL_LEN; ii++)
    {
      if (otb_eeprom_main_board_g->common.serial[ii] == 0)
      {
        break;
      }
      else if (isalnum(otb_eeprom_main_board_g->common.serial[ii]))
      {
        trailing = FALSE;
        OTB_MAIN_CHIPID[jj] = otb_eeprom_main_board_g->common.serial[ii];
        jj++;
      }
      else if (!trailing)
      {
        OTB_MAIN_CHIPID[jj] = '_';
        jj++;
      }
    }
  }
  else
  {
#ifdef OTB_DEBUG
    mac[0] = 0;
#endif
    mac_rc = otb_wifi_get_mac_addr(STATION_IF, mac);
    DEBUG("UTIL: Result of MAC addr query: %d", mac_rc);
    DEBUG("UTIL: MAC address string: %s", mac);
    // Check MAC address is exactly format (length) we expect which is:
    // aa:bb:cc:dd:ee:ff (17 bytes)
    if (mac_rc &&
        (os_strnlen(mac, OTB_WIFI_MAC_ADDRESS_STRING_LENGTH) <=
                                      (OTB_WIFI_MAC_ADDRESS_STRING_LENGTH-1)))
    {
      // Use STA MAC address
      DETAIL("UTIL: Using station MAC address as chipid");
      for (ii = 0, jj = 0, kk = 0;
           ii < os_strnlen(mac, OTB_WIFI_MAC_ADDRESS_STRING_LENGTH);
           ii++)
      {
        if (isalnum(mac[ii]))
        {
          DEBUG("UTIL: Is alnum: %x", mac[ii]);
          if (kk < 2)
          {
            DEBUG("UTIL: kk < 2: %d", kk);
            mac_bit[kk] = mac[ii];
            kk++;
          }
          else
          {
            WARN("UTIL: Unexpected MAC address format: %c %s", mac[ii], mac);
            rc = FALSE;
            mac[OTB_WIFI_MAC_ADDRESS_STRING_LENGTH-1] = 0;
            goto EXIT_LABEL;
          }
        }
        else if (mac[ii] == 0)
        {
          DEBUG("UTIL: 0");
          if (kk == 1)
          {
            DEBUG("UTIL: kk = 1");
            OTB_MAIN_CHIPID[jj] = '0';
            OTB_MAIN_CHIPID[jj+1] = mac_bit[0];
            jj += 2;
            kk = 0;
          }
          if (os_strnlen(OTB_MAIN_CHIPID, OTB_EEPROM_HW_SERIAL_LEN+1) < 6)
          {
            WARN("UTIL: Unexpected MAC address format: %s", mac);
            rc = 0;
          }
          break;
        }
        else if (mac[ii] != ':')
        {
          WARN("UTIL: Unexpected MAC address format: %c %s", mac[ii], mac);
          rc = FALSE;
          mac[OTB_WIFI_MAC_ADDRESS_STRING_LENGTH-1] = 0;
          goto EXIT_LABEL;
        }
        
        if (kk == 2)
        {
          DEBUG("UTIL: kk=2");
          OTB_MAIN_CHIPID[jj] = mac_bit[0];
          OTB_MAIN_CHIPID[jj+1] = mac_bit[1];
          jj += 2;
          kk = 0;
        }
        else if ((kk == 1) && (mac[ii] == ':'))
        {
          DEBUG("UTIL: kk=1 && mac=:");
          OTB_MAIN_CHIPID[jj] = '0';
          OTB_MAIN_CHIPID[jj+1] = mac_bit[0];
          jj += 2;
          kk = 0;
        }
      }
      // If we get here and kk is non zero we still have some bits to write.
      // This happens if the last byte begins 0.  E.g 0a - this is read as
      // xx:xx:xx:xx:xx:a - so we don't realise we've finished until the for
      // loop ends
      if (kk > 0)
      {
        if (kk == 1)
        {
          OTB_MAIN_CHIPID[jj] = '0';
          OTB_MAIN_CHIPID[jj+1] = mac_bit[0];
        }
        else
        {
          OTB_MAIN_CHIPID[jj] = mac_bit[0];
          OTB_MAIN_CHIPID[jj+1] = mac_bit[1];
        }
        jj += 2;
        kk = 0;
      }
    }
    else
    {
      rc = FALSE;
    }
  }
  
EXIT_LABEL:

  if (!rc)
  {  
    // Use ESP chip id
    DETAIL("UTIL: Using ESP chipid as chipid");
    os_memset(OTB_MAIN_CHIPID, 0, OTB_MAIN_CHIPID_STR_LENGTH);
    os_sprintf(OTB_MAIN_CHIPID, "%06x", system_get_chip_id());
  }
  
  INFO("OTB: Chip ID: %s", OTB_MAIN_CHIPID);  
  
  DEBUG("UTIL: otb_util_get_chip_id exit");
  
  return;
}

void ICACHE_FLASH_ATTR otb_util_log_useful_info(bool recovery)
{
  char recovery_str[16];

  if (recovery)
  {
    os_strncpy(recovery_str, "/!!!RECOVERY!!!", 16);
  }
  else
  {
    recovery_str[0] = 0;
  }

  // Set up and log some useful info
  os_sprintf(otb_compile_date, "%s", __DATE__);
  otb_util_convert_char_to_char(otb_compile_date, ' ', '_');
  otb_util_convert_char_to_char(otb_compile_date, '.', '_');
  otb_util_convert_char_to_char(otb_compile_date, ':', '.');
  os_sprintf(otb_compile_time, "%s", __TIME__);
  otb_util_convert_char_to_char(otb_compile_time, ' ', '_');
  otb_util_convert_char_to_char(otb_compile_time, ':', '_');
  os_snprintf(otb_version_id,
              OTB_MAIN_MAX_VERSION_LENGTH,
              "%s:%s:Build_%u:%s:%s",
              OTB_MAIN_OTB_IOT,
              OTB_MAIN_FW_VERSION,
              OTB_BUILD_NUM,
              otb_compile_date, 
              otb_compile_time);
  otb_version_id[OTB_MAIN_MAX_VERSION_LENGTH-1] = 0;
  os_snprintf(otb_sdk_version_id,
              OTB_MAIN_SDK_MAX_VERSION_LENGTH,
              "%s", 
              STRINGIFY(ESP_SDK_VERSION));
  otb_sdk_version_id[OTB_MAIN_SDK_MAX_VERSION_LENGTH-1] = 0;
  // First log needs a line break!
  INFO("OTB: %s%s", otb_version_id, recovery_str);
  INFO("OTB: SDK Version: %s", otb_sdk_version_id);
  INFO("OTB: Boot slot: %d", otb_rboot_get_slot(FALSE));
  DETAIL("OTB: Free heap size: %d bytes", system_get_free_heap_size());
  
  // This is updated later when the EEPROM is read
  os_sprintf(otb_hw_info, "%04x:%04x", 0xffff, 0xffff);
  
  if (recovery)
  {
    INFO("");
    INFO("OTB: ---------------------");
    INFO("OTB: !!! Recovery Mode !!!");
    INFO("OTB: ---------------------");
    INFO("");
  }
  return;
}

void ICACHE_FLASH_ATTR otb_util_init_logging(void)
{
  otb_util_log_level = OTB_LOG_LEVEL_DEFAULT;

  // Set up serial logging
  uart_div_modify(0, UART_CLK_FREQ / OTB_MAIN_BAUD_RATE);
  uart_div_modify(1, UART_CLK_FREQ / OTB_MAIN_BAUD_RATE);
  
  otb_util_asserting = FALSE;
  
  // Configure ESP SDK logging (only)
  system_set_os_print(OTB_MAIN_SDK_LOGGING);

  // Initialize our log buffer
  otb_util_log_buf = (char *)otb_util_log_buf_int32;
  os_memset(otb_util_log_buf, 0, OTB_UTIL_LOG_BUFFER_LEN);
  otb_util_log_buffer_struct.buffer = otb_util_log_buf;
  otb_util_log_buffer_struct.current = otb_util_log_buf;
  otb_util_log_buffer_struct.len = OTB_UTIL_LOG_BUFFER_LEN;

  // Sanity check uint16 in otb_util_log_buffer is enough
  OTB_ASSERT(OTB_UTIL_LOG_BUFFER_LEN <= 2^16);
  
  // Check log location is on a sector boundary
  OTB_ASSERT(OTB_BOOT_LOG_LOCATION % 0x1000 == 0);
  
  // Check log buffer will fit in a sector
  OTB_ASSERT(OTB_UTIL_LOG_BUFFER_LEN <= 0x1000);
  
  OTB_ASSERT((uint32)otb_util_log_buf % 4 == 0);
  
  return;
}

void ICACHE_FLASH_ATTR otb_util_resume_init(void)
{
  // Log some useful info
  otb_util_log_useful_info(FALSE);

  // Do some sanity checking
  otb_util_check_defs();

  // Initial internal I2C bus (must be done before try and read eeprom)
  otb_i2c_initialize_bus_internal();
  
  // Read the eeprom (if present) - this initializes the chip ID
  otb_eeprom_read();

  // Relog heap size (now have read into eeprom)
  DETAIL("OTB: Free heap size: %d bytes", system_get_free_heap_size());

  // Initialise flash access (this makes it work if OTB_SUPER_BIG_FLASH_8266 if defined).
  otb_flash_init();
  
  // Initialize GPIO.  Must happen before we clear reset (as this uses GPIO), but
  // after have populated them 
  otb_gpio_init();
  
  // Reset GPIO - pull pin 16 high
  otb_util_clear_reset();
  
  // Initialize serial
  otb_serial_init();

  // Initialize wifi - mostly this just disables wifi until we're ready to turn it on!
  otb_wifi_init();

  // Initialize nixie module
  otb_nixie_module_init();

  // Initialize and load config
  otb_conf_init();
  otb_conf_load();
  
  otb_led_wifi_update(OTB_LED_NEO_COLOUR_BLUE, TRUE);

#ifdef OTB_DEBUG
  otb_util_log_heap_size_start_timer();
#endif // OTB_DEBUG

  otb_util_check_for_break();
}

void ICACHE_FLASH_ATTR otb_util_log_store(void)
{
  uint8 spi_rc = SPI_FLASH_RESULT_ERR;
  char scratch[4];
  uint8_t mod4;
  int16_t write_len;
  uint16_t rem_len;
  
  // This is fiddly cos:
  // - Need to write multiple of 4 bytes
  // - We are writing a circular buffer
  // Fixed the first of these by making sure each log is 4 byte aligned, so don't need
  // to worry here (still does, but buffer will always 4 byte align).
  
  // write_len must be signed as may go negative
  // (as may need to write up 3 bytes too many)
  write_len = OTB_UTIL_LOG_BUFFER_LEN;
  
  // Going to ignore spi_rc - not much we can do if these commands fail

  // Erase the sector
  spi_rc = spi_flash_erase_sector(OTB_BOOT_LOG_LOCATION / 0x1000);
  //ets_printf("log flash erased\r\n");

  //ets_printf("buffer 0x%08x\r\n", otb_util_log_buffer_struct.buffer);
  //ets_printf("current 0x%08x\r\n", otb_util_log_buffer_struct.current);

  // Write from current position to whatever is closest to the end but a total length of
  // a multiple of 4 bytes
  rem_len = otb_util_log_buffer_struct.len -
            (int16_t)(otb_util_log_buffer_struct.current -
                      otb_util_log_buffer_struct.buffer);
  mod4 = rem_len % 4;
  rem_len -= mod4;
  //ets_printf("mod4 %d\r\n", mod4);
  //ets_printf("rem_len %d\r\n", rem_len);
    
  spi_rc = spi_flash_write(OTB_BOOT_LOG_LOCATION,
                           (uint32*)otb_util_log_buffer_struct.current,
                           (uint32)rem_len);
  write_len -= rem_len;

  // Now create 4 bytes from what was left over, and the first few bytes of the buffer
  if (write_len > 0)
  {
    os_memcpy(scratch, otb_util_log_buffer_struct.current + rem_len, mod4);
    os_memcpy(scratch+rem_len, otb_util_log_buffer_struct.buffer, (4-mod4));
    spi_rc = spi_flash_write(OTB_BOOT_LOG_LOCATION + rem_len,
                             (uint32*)scratch,
                             4);
    write_len -= 4;
  }
  
  if (write_len > 0)
  {
    // Ensure multiple of 4 bytes
    write_len = ((write_len/4) + 1) * 4;
    
    // XXX NULL out the extra bytes
    spi_rc = spi_flash_write(OTB_BOOT_LOG_LOCATION + rem_len + 4,
                             (uint32*)(otb_util_log_buffer_struct.buffer + (4-mod4)),
                             write_len);
  }

  return;
}

// XXX Should really break this out§
char ICACHE_FLASH_ATTR *otb_util_get_log_ram(uint8 index)
{
  uint16_t len_b4;
  int ii;
  sint16 times;
  bool non_null;
  bool first_loop;
  char *log_start;
  sint16 len;
  sint16 new_len;
  sint16 buffer_left;
  sint16 scratch_left;

  // Write from current position to whatever is closest to the end but a total length of
  // a multiple of 4 bytes
  len_b4 = (int16_t)(otb_util_log_buffer_struct.current -
                      otb_util_log_buffer_struct.buffer);

  // go backwards looking for null string terminator
  ii = len_b4;
  ii--;
  if (ii < 0)
  {
    ii = otb_util_log_buffer_struct.len-1;
    first_loop = FALSE;
  }
  else
  {
    first_loop = TRUE;
  }
  log_start = NULL;

  for (times = index; times >= 0; times--)
  {
    non_null = FALSE;
    while (TRUE)
    {
      // Need to skip over any trailing 0s (may be up to 4).
      if (otb_util_log_buffer_struct.buffer[ii] != 0)
      {
        non_null = TRUE;
      }
    
      if (first_loop)
      {
        if (ii < 0)
        {
          // Start from end
          ii = otb_util_log_buffer_struct.len - 1;
          first_loop = FALSE;
        }
      }
      else
      {
        if (ii <= len_b4)
        {
          // Gone all the way around with no match :-(
          break;
        }
      }

      if (non_null && otb_util_log_buffer_struct.buffer[ii] == 0)
      {
        if (times == 0)
        {
          // Have found first string.  Hurrah!
          ii++;
          if (ii > (otb_util_log_buffer_struct.len - 1))
          {
            ii = 0;
          }
          log_start = otb_util_log_buffer_struct.buffer + ii;
        }
        break;
      }
      ii--;
    }
  }
  
  // This is a bit of a hack - we'll basically copy the whole buffer into the log string
  // - well as much as there's space for.  However, "our" string will be NULL terminated
  // so it works
  if (log_start != NULL)
  {
    // Have a match, copy it into otb_log_s
    scratch_left = OTB_MAIN_MAX_LOG_LENGTH;
    buffer_left = otb_util_log_buffer_struct.len - ii;
    len = OTB_MIN(buffer_left, scratch_left);
    os_memcpy(otb_log_s, log_start, len);
    scratch_left -= len;
    buffer_left -= len;
    if (buffer_left > 0)
    {
      // done
      otb_log_s[len] = 0;
    }
    else
    {
      buffer_left = otb_util_log_buffer_struct.len - len;
      log_start = otb_util_log_buffer_struct.buffer;
      new_len = OTB_MIN(buffer_left, scratch_left);
      os_memcpy(otb_log_s+len, log_start, new_len);
      scratch_left -= new_len;
      buffer_left -= new_len;
      // done
    }
    otb_log_s[OTB_MAIN_MAX_LOG_LENGTH-1] = 0;
    log_start = otb_log_s;
  }

  return(log_start);
}

void ICACHE_FLASH_ATTR otb_util_log_save(char *text)
{
  uint16_t text_len;
  uint16_t overflow_len;
  int16_t rem_len;
  uint8_t mod4;
  
  // Get string length plus the NULL terminating bytes, as we'll log this too.
  text_len = os_strlen(text) + 1;

  rem_len = otb_util_log_buffer_struct.len -
            (uint16_t)(otb_util_log_buffer_struct.current -
                       otb_util_log_buffer_struct.buffer);
           
  if (rem_len < text_len)
  {
    overflow_len = text_len - rem_len;
    os_memcpy(otb_util_log_buffer_struct.current, text, rem_len);
    os_memcpy(otb_util_log_buffer_struct.buffer, text+rem_len, overflow_len);
    otb_util_log_buffer_struct.current = otb_util_log_buffer_struct.buffer + overflow_len;
    rem_len = otb_util_log_buffer_struct.len - overflow_len;
  }
  else if (rem_len >= text_len)
  {
    os_memcpy(otb_util_log_buffer_struct.current, text, text_len);
    otb_util_log_buffer_struct.current += text_len;
    rem_len = rem_len - text_len;
  }

  // 4 byte align each log  
  mod4 = ((int32)otb_util_log_buffer_struct.current) % 4;
  if (mod4 != 0)
  { 
    mod4 = 4 - mod4;
  }
  while (mod4 > 0)
  {
    if (rem_len > 0)
    {
      *otb_util_log_buffer_struct.current = 0;
      otb_util_log_buffer_struct.current++;
    }
    else
    {
      otb_util_log_buffer_struct.current = otb_util_log_buffer_struct.buffer;
      *otb_util_log_buffer_struct.current = 0;
      rem_len = otb_util_log_buffer_struct.len;
    }
    rem_len--;
    mod4--;
  }
  
  return;
}

void ICACHE_FLASH_ATTR otb_util_log_fn(char *text)
{
  if (!OTB_MAIN_DISABLE_OTB_LOGGING)
  {
    ets_printf(text);
    ets_printf("\n");
  }
  
  otb_util_log_save(text);
  
  return;
}

char ALIGN4 otb_util_assert_error_string[] = "UTIL: Assertion Failed";
void ICACHE_FLASH_ATTR otb_util_assert(bool value, char *value_s)
{
  DEBUG("UTIL: otb_util_assert entry");

  if(!value && !otb_util_asserting)
  {
    otb_util_asserting = TRUE;
    ERROR("------------- ASSERTION FAILED -------------");
    ERROR_VAR(value_s);
    ERROR("Rebooting");
    ERROR("--------------------------------------------");
    otb_reset_error(otb_util_assert_error_string);
    otb_util_delay_ms(1000);
  }

  DEBUG("UTIL: otb_util_assert exit");
  
  return;
}


void ICACHE_FLASH_ATTR otb_reset_schedule(uint32_t timeout,
                                          const char *reason,
                                          bool error)
{

  DEBUG("UTIL: otb_reset_schedule entry");

  os_memset(&otb_reset_reason_struct, 0, sizeof(otb_reset_reason_struct));
  otb_reset_reason_struct.reason = reason;  
  otb_reset_reason_struct.error = error;  

  otb_util_timer_set((os_timer_t *)&otb_util_reset_timer,
                     (os_timer_func_t *)otb_reset_timer,
                     (void *)&otb_reset_reason_struct,
                     timeout,
                     0);

  DEBUG("UTIL: otb_reset_schedule exit");
  
  return;
}

void ICACHE_FLASH_ATTR otb_reset_timer(void *arg)
{
  struct otb_reset_reason *reason_struct = (struct otb_reset_reason *)arg;

  DEBUG("UTIL: otb_reset_timer entry");
  
  otb_reset_internal((char *)reason_struct->reason, reason_struct->error);
  
  DEBUG("UTIL: otb_reset_timer exit");
  
  return;
}

void ICACHE_FLASH_ATTR otb_reset_error(char *text)
{
  DEBUG("OTB: otb_reset_error entry");
  
  otb_reset_schedule(1000, (const char *)text, TRUE);
  
  DEBUG("OTB: otb_reset_error exit");
  
  return;
}

void ICACHE_FLASH_ATTR otb_reset(char *text)
{
  DEBUG("OTB: otb_reset entry");
  
  otb_reset_internal(text, FALSE);
  
  DEBUG("OTB: otb_reset exit");
  
  return;
}

void ICACHE_FLASH_ATTR otb_reset_internal(char *text, bool error)
{
  bool rc;
  bool same;
  uint8_t log_level;

  DEBUG("OTB: otb_reset_internal entry");
  
  otb_util_reset_store_reason(text, &same);
  
  if (error)
  {
    log_level = OTB_LOG_LEVEL_ERROR;
  }
  else
  {
    log_level = OTB_LOG_LEVEL_WARN;
  }

  if (text != NULL)
  {
    LOG(log_level, "OTB: Resetting: %s", text);
  }
  else
  {
    LOG(log_level, "OTB: Resetting");
  }

  if (error && !same)
  {
    // Only store logs if not the same reset reason as last time (to avoid destroying
    // the flash)
    otb_util_log_store();
  }
  
  // Delay to give any serial logs time to output.
  otb_util_delay_ms(1000);

  #if 0
  // Reset by pulling reset GPIO (connected to reset) low
  // XXX Only works for GPIO 16
  WRITE_PERI_REG(RTC_GPIO_OUT,
                   (READ_PERI_REG(RTC_GPIO_OUT) & (uint32)0xfffffffe) | (uint32)(0));
  #else
  system_restart();
  #endif

  DEBUG("OTB: otb_reset_internal exit");

  return;
}

bool ICACHE_FLASH_ATTR otb_util_reset_store_reason(char *text, bool *same)
{
  bool rc = TRUE;
  uint8 tries = 0;
  uint16 len;
  uint8 mod4;
  uint8 spi_rc;
  char ALIGN4 stored[OTB_BOOT_LAST_REBOOT_LEN];

  DEBUG("UTIL: otb_util_reset_store_reason entry");
  
  *same = FALSE;
  
  // First read the current stored string - as only want to write if it differs (so we 
  // don't write the flash too many times
  
  rc = otb_util_flash_read((uint32)OTB_BOOT_LAST_REBOOT_REASON,
                           (uint32 *)stored,
                           OTB_BOOT_LAST_REBOOT_LEN);
  if (rc)
  {
    if (os_strncmp(text, stored, OTB_BOOT_LAST_REBOOT_LEN))
    {
      len = os_strlen(text);
      spi_rc = spi_flash_erase_sector(OTB_BOOT_LAST_REBOOT_REASON / 0x1000);
      rc = otb_util_flash_write_string((uint32)OTB_BOOT_LAST_REBOOT_REASON,
                                       (uint32 *)text,
                                       (uint32)len);
      if (!rc)
      {
        DEBUG("UTIL: Failed to actually store reset reason in flash");
      }
    }
    else
    {
      DEBUG("UTIL: New reset reason same as old one");
      *same = TRUE;
    }
  }
  
  if (!rc)
  {
    ERROR("UTIL: Failed to store reset reason");
  }
 
EXIT_LABEL:

  DEBUG("UTIL: otb_util_reset_store_reason exit");

  return rc;
}

void ICACHE_FLASH_ATTR otb_util_clear_reset(void)
{
  DEBUG("UTIL: otb_util_clear_reset entry");

  // Set GPIO 16 (reset GPIO) to high 
  // XXX Only works for GPIO16
  WRITE_PERI_REG(PAD_XPD_DCDC_CONF,
                 (READ_PERI_REG(PAD_XPD_DCDC_CONF) & 0xffffffbc) | (uint32)0x1);      // mux configuration for XPD_DCDC to output rtc_gpio0

  WRITE_PERI_REG(RTC_GPIO_CONF,
                 (READ_PERI_REG(RTC_GPIO_CONF) & (uint32)0xfffffffe) | (uint32)0x0);  //mux configuration for out enable

  WRITE_PERI_REG(RTC_GPIO_ENABLE,
                 (READ_PERI_REG(RTC_GPIO_ENABLE) & (uint32)0xfffffffe) | (uint32)0x1);        //out enable
  WRITE_PERI_REG(RTC_GPIO_OUT,
                 (READ_PERI_REG(RTC_GPIO_OUT) & (uint32)0xfffffffe) | (uint32)(1));

  DEBUG("UTIL: otb_util_clear_reset exit");

  return;
}

bool ICACHE_FLASH_ATTR otb_util_flash_read(uint32 location,
                                           uint32 *dest,
                                           uint32 len)
{
  bool rc = TRUE;
  uint8 tries;
  uint8 spi_rc = SPI_FLASH_RESULT_ERR;

  DEBUG("UTIL: otb_util_flash_read_string entry");
  
  // Check everything is 4 byte aligned
  OTB_ASSERT((uint32)location % 4 == 0);
  OTB_ASSERT((uint32)dest % 4 == 0);
  OTB_ASSERT((uint32)len % 4 == 0);
  
  tries = 0;
  while ((tries < 2) && (spi_rc != SPI_FLASH_RESULT_OK))
  {
    spi_rc = spi_flash_read(location, dest, len);
  }

  if (tries == 2)
  {
    ERROR("UTIL: Failed to read at 0x%08x, error %d", location, spi_rc);
    rc = FALSE;
    goto EXIT_LABEL;
  }
  
EXIT_LABEL:  
  
  DEBUG("UTIL: otb_util_flash_read_string exit");
  
  return rc;
}                                                  

bool ICACHE_FLASH_ATTR otb_util_flash_write_string(uint32 location,
                                                   uint32 *source,
                                                   uint32 len)
{
  bool rc;
  uint32 newlen;
  uint8 mod4;
  
  DEBUG("UTIL: otb_util_flash_write_string entry");
  
  // 4 byte align the string - this should be safeish - we're not writing past the end
  // of the passed in string, but we are ready past it so may log some rubbish
  mod4 = len % 4;
  newlen = len + (4-mod4);

  rc = otb_util_flash_write(location, source, newlen);
  
  DEBUG("UTIL: otb_util_flash_write_string exit");
  
  return(rc);
}

bool ICACHE_FLASH_ATTR otb_util_flash_write(uint32 location, uint32 *source, uint32 len)
{
  bool rc = TRUE;
  uint8 tries;
  uint8 spi_rc = SPI_FLASH_RESULT_ERR;
  
  DEBUG("UTIL: otb_util_flash_write entry");
  
  // Check everything is 4 byte aligned
  OTB_ASSERT((uint32)location % 4 == 0);
  OTB_ASSERT((uint32)source % 4 == 0);
  OTB_ASSERT((uint32)len % 4 == 0);

  tries = 0;
  while ((tries < 2) && (spi_rc != SPI_FLASH_RESULT_OK))
  {
    spi_rc = spi_flash_write(location, source, len);
    tries++;
  }

  if (tries == 2)
  {
    ERROR("UTIL: Failed to write at 0x%08x, error %d", location, spi_rc);
    rc = FALSE;
    goto EXIT_LABEL;
  }
  
EXIT_LABEL:  

  DEBUG("UTIL: otb_util_flash_write exit");
  
  return rc;
}
 
void ICACHE_FLASH_ATTR otb_util_log_heap_size_start_timer(void)
{
  DEBUG("UTIL: otb_util_log_heap_size_start_timer entry");

  otb_util_timer_set(&otb_util_heap_timer,
                     otb_util_log_heap_size_timer,
                     NULL,
                     1000,
                     1);

  DEBUG("UTIL: otb_util_log_heap_size_start_timer exit");

  return;
}

void ICACHE_FLASH_ATTR otb_util_log_heap_size_timer(void *arg)
{
  uint32 size;

  DEBUG("UTIL: otb_util_log_heap_size_timer entry");
  
  size = system_get_free_heap_size();  
  DETAIL("UTIL: Free heap size: %d", size);
  
  DEBUG("UTIL: otb_util_log_heap_size_timer exit");

  return;
}

void ICACHE_FLASH_ATTR otb_util_get_heap_size(void)
{
  uint32 size;
  char size_s[8];

  DEBUG("UTIL: otb_util_get_heap_size entry");
  
  size = system_get_free_heap_size();  
  os_snprintf(size_s, 8, "%d", size);
  otb_mqtt_send_status(OTB_MQTT_STATUS_HEAP_SIZE, size_s, "", "");
  
  DEBUG("UTIL: otb_util_get_heap_size exit");

  return;
}  

bool otb_util_vdd33_flash_ok = FALSE;

bool ICACHE_FLASH_ATTR otb_util_get_vdd33(uint16 *vdd33)
{
  bool rc = FALSE;
  char output[16];
  unsigned char ALIGN4 flash[0x1000];
  uint8 spi_rc = SPI_FLASH_RESULT_ERR;

  DEBUG("UTIL: otb_util_get_vdd33 entry");

  // To work byte 107 of ESP user.bin (0x3fc000 in 4MB flash) must be 0xff.  Really.
  // And you need to reboot afterwards.
  if (!otb_util_vdd33_flash_ok)
  {
    // Sigh.  Make it OK.
    DEBUG("UTIL: read flash 0x%p", flash);
    rc = otb_util_flash_read(OTB_BOOT_ESP_USER_BIN,
                             (uint32 *)flash,
                             0x1000);
    if (rc)
    {
      if (flash[107] != 0xff)
      {
        DEBUG("UTIL: write flash");
        flash[107] = 0xff;
        spi_rc = spi_flash_erase_sector(OTB_BOOT_ESP_USER_BIN / 0x1000);
        if (spi_rc != SPI_FLASH_RESULT_ERR)
        {
          rc = otb_util_flash_write(OTB_BOOT_ESP_USER_BIN,
                                    (uint32 *)flash,
                                    0x1000);
        }
        else
        {
          WARN("UTIL: Failed to erase flash");
          rc = FALSE;
        }
        if (rc)
        {
          otb_util_vdd33_flash_ok = TRUE;
          otb_mqtt_send_status(OTB_MQTT_STATUS_VDD33, OTB_MQTT_STATUS_ERROR, "reset first", "");
          goto EXIT_LABEL;
        }
        else
        {
          WARN("UTIL: Failed to update ESP user bin to flash 0x%x", OTB_BOOT_ESP_USER_BIN);
        }
      }
      else
      {
        DEBUG("UTIL: Byte 107 of ESP user bin 0x%x already 0xff", OTB_BOOT_ESP_USER_BIN);
        otb_util_vdd33_flash_ok = TRUE;
      }
    }
    else
    {
      WARN("UTIL: Failed to read ESP user bin from flash 0x%x", OTB_BOOT_ESP_USER_BIN);
    }
  }  

  rc = FALSE;
  if (otb_util_vdd33_flash_ok)  
  {
    *vdd33 = system_get_vdd33();  
    rc = TRUE;
  }

EXIT_LABEL:
  
  DEBUG("UTIL: otb_util_get_vdd33 exit");

  return rc;
}  

void ICACHE_FLASH_ATTR otb_util_timer_cancel(os_timer_t *timer)
{

  DEBUG("UTIL: otb_util_timer_cancel entry");
  
  // Set the timer function to NULL so we can test whether this is set
  os_timer_disarm(timer);
  os_timer_setfn(timer, (os_timer_func_t *)NULL, NULL);
  
  DEBUG("UTIL: otb_util_timer_cancel exit");
  
  return;
}

bool ICACHE_FLASH_ATTR otb_util_timer_is_set(os_timer_t *timer)
{
  bool rc = TRUE;
  
  DEBUG("UTIL: otb_util_timer_is_set entry");
  
  if (timer->timer_func == NULL)
  {
    DEBUG("UTIL: timer isn't set");
    rc = FALSE;
  }
  
  DEBUG("UTIL: otb_util_timer_is_set exit");

  return(rc);
}

void ICACHE_FLASH_ATTR otb_util_timer_set(os_timer_t *timer,
                                          os_timer_func_t *timerfunc,
                                          void *arg,
                                          uint32_t timeout,
                                          bool repeat)
{

  DEBUG("UTIL: otb_util_timer_set entry");

  otb_util_timer_cancel(timer);
  os_timer_setfn(timer, timerfunc, arg);
  os_timer_arm(timer, timeout, repeat);
  
  DEBUG("UTIL: otb_util_timer_set exit");
  
  return;
}

bool ICACHE_FLASH_ATTR otb_util_timer_finished(otb_util_timeout *timeout)
{
  bool rc = FALSE;
  uint32_t current_time; 
  
  DEBUG("UTIL: otb_util_timer_finished entry");

  current_time = system_get_time();
  if (timeout->start_time < timeout->end_time)
  {
    if (current_time > timeout->end_time)
    {
      DEBUG("UTIL: Timer finished", timeout->end_time);
      rc = TRUE;
    }
  }
  else
  {
    if ((current_time < timeout->start_time) && (current_time > timeout->end_time))
    {
      DEBUG("UTIL: Timer finished", timeout->end_time);
      rc = TRUE;
    }
  }

  DEBUG("UTIL: otb_util_timer_finished exit");
  
  return rc;  
}
 
void ICACHE_FLASH_ATTR otb_util_delay_ms(uint32_t value)
{
  // Can't just use Ardunio delay, cos may not be in the right context.
  // os_delay_us takes a uint16_t so can only delay up to 65535 microseconds
  // the code below needs to be checked that it will work OK across wrapping (a uint32_t
  // containing microseconds will wrap every 71 minutes, so there's a decent chance of
  // this happening
  // XXX Should test context to see whether do delay (if in sketch) or call
  // ESP function otherwise
  uint32_t wait_length = value;
  uint32_t start_time;
  uint32_t current_time;
  uint32_t end_time;
  bool going_to_wrap = FALSE;
    
  DEBUG("UTIL: otb_util_delay_ms entry");
  
  DEBUG("UTIL: wait for %d ms", value);

  OTB_ASSERT(value <= OTB_UTIL_MAX_DELAY_MS);
  
  current_time = system_get_time();
  start_time = current_time;
  end_time = current_time + (value * 1000);
  if (end_time < current_time)
  {
    going_to_wrap = TRUE;
  }
  
  while (TRUE)
  {
    if (value <= OTB_ESP_MAX_DELAY_MS)
    {
      DEBUG("UTIL: Wait for %d ms", value); 
      os_delay_us(value*1000);
    }
    else
    {
      DEBUG("UTIL: Wait for %d ms", OTB_UTIL_DELAY_WAIT_MS);
      os_delay_us(OTB_UTIL_DELAY_WAIT_MS*1000);
    }
    system_soft_wdt_feed();
    
    current_time = system_get_time();
    
    if (!going_to_wrap)
    {
      if (current_time >= end_time)
      {
        DEBUG("UTIL: Done");
        break;
      }
      else
      {
        value = (end_time - current_time) / 1000;
      }
    }
    else
    {
      if ((current_time < start_time) && (current_time >= end_time))
      {
        DEBUG("UTIL: Done");
        break;
      }
      else if (current_time < end_time)
      {
        value = (end_time - current_time) / 1000;
      }
      else
      {
        value = (end_time + (0xffffffff - current_time)) / 1000;
      }
    }
  }
  
  DEBUG("UTIL: otb_util_delay_ms exit");
  
  return;
}

void ICACHE_FLASH_ATTR otb_util_check_defs(void)
{
  DEBUG("UTIL: otb_util_check_defs entry");
  
  OTB_ASSERT(OTB_UTIL_DELAY_WAIT_MS <= OTB_ESP_MAX_DELAY_MS);
  OTB_ASSERT(strlen(OTB_MAIN_OTB_IOT) <= 8);
  OTB_ASSERT(strlen(OTB_MQTT_ROOT) <= 8);
  OTB_ASSERT(strlen(OTB_MAIN_ESPI_PREFIX) <= 8);
  OTB_ASSERT(strlen(OTB_MAIN_FW_VERSION) <= 8);
  
  DEBUG("UTIL: otb_util_check_defs exit");
  
  return;
}

void ICACHE_FLASH_ATTR otb_util_log_snprintf(char *log_string,
                                             uint16_t max_log_string_len,
                                             const char *format,
                                             va_list args)
{
  // DEBUG("UTIL: otb_util_log_snprintf entry");

  // Need to call vsnprintf not snprintf, as passing va_list
  os_vsnprintf(log_string, max_log_string_len, format, args);

  return;

  // DEBUG("UTIL: otb_util_log_snprintf exit");
}

void ICACHE_FLASH_ATTR otb_util_log(uint8_t level,
                                    char *log_string,
                                    uint16_t max_log_string_len,
                                    const char *format,
                                    ...)
{
  va_list args;

  // DEBUG("UTIL: otb_util_log entry");
  
  // Only carry on if log is at an appropriate level
  if (level < otb_util_log_level)
  {
    goto EXIT_LABEL;
  }

  // Bit of messing around to deal with var args, but basically snprintf log
  // into log buffer and then log it
  va_start(args, format);
  otb_util_log_snprintf(log_string, max_log_string_len, format, args);
  va_end(args);
  otb_util_log_fn(otb_log_s);

  // Log if MQTT if an error and we're connected
  if ((level >= OTB_LOG_LEVEL_ERROR) &&
      (otb_mqtt_client.connState == MQTT_DATA))
  {   
    otb_util_log_error_via_mqtt(otb_log_s);
  }

EXIT_LABEL:

  // DEBUG("UTIL: otb_util_log exit");
  
  return;
}

void ICACHE_FLASH_ATTR otb_util_disable_logging(void)
{
  // DEBUG("UTIL: otb_util_disable_logging entry");

  otb_util_log_level_stored = otb_util_log_level;

  // DEBUG("UTIL: otb_util_disable_logging exit");

  return;
}

void ICACHE_FLASH_ATTR otb_util_enable_logging(void)
{
  // DEBUG("UTIL: otb_util_enable_logging entry");

  otb_util_log_level = otb_util_log_level_stored;

  // DEBUG("UTIL: otb_util_enable_logging exit");

  return;
}

void ICACHE_FLASH_ATTR otb_util_log_error_via_mqtt(char *text)
{
  // DEBUG("UTIL: otb_util_log_error_via_mqtt entry");

  os_snprintf(otb_mqtt_topic_s,
              OTB_MQTT_MAX_TOPIC_LENGTH,
              "/%s/%s/%s/%s/%s/%s",
              otb_mqtt_root,
              OTB_MQTT_LOCATION_1,
              OTB_MQTT_LOCATION_2,
              OTB_MQTT_LOCATION_3,
              OTB_MAIN_CHIPID,
              OTB_MQTT_PUB_LOG);

  // QOS=1 so gets through at least once, retain=0 so last log not retained
  MQTT_Publish(&otb_mqtt_client, otb_mqtt_topic_s, text, strlen(text),  1, 0);

  // DEBUG("UTIL: otb_util_log_error_via_mqtt exit");

  return;
}

void ICACHE_FLASH_ATTR otb_init_mqtt(void *arg)
{
  DEBUG("OTB: otb_init_mqtt entry");
  
  DETAIL("OTB: Set up MQTT stack");
  otb_mqtt_initialize(otb_conf->mqtt.svr,
                      otb_conf->mqtt.port,
                      0,
                      OTB_MAIN_DEVICE_ID,
                      otb_conf->mqtt.user,
                      otb_conf->mqtt.pass,
                      OTB_MQTT_KEEPALIVE);

  // Now initialise according to the installed modules
  os_timer_disarm((os_timer_t*)&init_timer);
  otb_eeprom_init_modules();

  EXIT_LABEL:  
  
  DEBUG("OTB: otb_init_mqtt entry");

  return;
}

void ICACHE_FLASH_ATTR otb_init_ds18b20(void *arg)
{
  DEBUG("OTB: otb_init_ds18b20 entry");

  DETAIL("OTB: Set up One Wire bus");
  otb_ds18b20_initialize(OTB_DS18B20_DEFAULT_GPIO);
  otb_util_booted();

  DEBUG("OTB: otb_init_ds18b20 exit");

  return;
}

void ICACHE_FLASH_ATTR otb_init_ads(void *arg)
{
  DEBUG("OTB: otb_init_ads entry");

  DETAIL("OTB: Set up ADS (+I2C bus)");
#if 0  
  otb_ads_initialize();
#endif

  // Now set up ADS init
  os_timer_disarm((os_timer_t*)&init_timer);
  otb_util_booted();

  DEBUG("OTB: otb_init_ads exit");

  return;
}

// ip_out must be 4 byte array
bool ICACHE_FLASH_ATTR otb_util_parse_ipv4_str(char *ip_in, uint8_t *ip_out)
{
#define OTB_UTIL_MAX_IPV4_STR_LEN 15
#define OTB_UTIL_MAX_IPV4_BYTE_LEN 3
  char ip_byte_str[4][OTB_UTIL_MAX_IPV4_BYTE_LEN+1];
  bool rc = FALSE;
  size_t len;
  int ii, jj, kk;
  int ip_byte;

  DEBUG("UTIL: otb_util_parse_ipv4_str entry");

  len = os_strnlen(ip_in, OTB_UTIL_MAX_IPV4_STR_LEN+1);
  if (len > OTB_UTIL_MAX_IPV4_STR_LEN)
  {
    // Too long to be a valid IPv4 address (123.123.123.123 is 15 chars)
    DETAIL("UTIL: IPv4 address too long to be valid");
    goto EXIT_LABEL;
  }

  os_memset(ip_byte_str, 0, 4*(OTB_UTIL_MAX_IPV4_BYTE_LEN+1));
  // ii is position in up_in string, jj is digits in current "byte", kk is byte
  for (ii = 0, jj = 0, kk = 0; ii < len; ii++)
  {
    if (ip_in[ii] == '.')
    {
      jj = 0;
      kk++;
    }
    else if (jj > 2)
    {
      // byte is too many digits
      DETAIL("UTIL: IP address byte has too many digits: %d", jj);
      goto EXIT_LABEL;
    }
    else if ((ip_in[ii] >= '0') && (ip_in[ii] <= '9'))
    {
      // Digit
      ip_byte_str[kk][jj] = ip_in[ii];
      jj++;
    }
    else
    {
      // invalid character in IP address
      DETAIL("UTIL: Invalid character in IP address: %d", ip_in[ii]);
      goto EXIT_LABEL;
    }

    if (kk > 3)
    {
      // Too many "bytes"
      DETAIL("UTIL: Too many bytes in IPv4 address: %d", kk+1);
      goto EXIT_LABEL;
    }
  }
  if (kk < 3)
  {
    // Too few "bytes"
    DETAIL("UTIL: Too few bytes in IPv4 address: %d", kk+1);
    goto EXIT_LABEL;
  }

  for (ii = 0; ii < 4; ii++)
  {
    ip_byte = atoi(ip_byte_str[ii]);
    if ((ip_byte < 0) || (ip_byte > 255))
    {
      DETAIL("UTIL: IP byte less than 0 or greater than 255: %d %s", ip_byte, ip_byte_str)
      goto EXIT_LABEL;
    }
    ip_out[ii] = ip_byte;
  }

  DETAIL("UTIL: Parsed IP address: %d.%d.%d.%d",
         ip_out[0],
         ip_out[1],
         ip_out[2],
         ip_out[3]);
  rc = TRUE;

EXIT_LABEL:

  DEBUG("UTIL: otb_util_parse_ipv4_str exit");

  return rc;
}

bool ICACHE_FLASH_ATTR otb_util_ip_is_all_val(uint8_t *ip, uint8_t val)
{
  bool rc = TRUE;
  int ii;

  DEBUG("UTIL: otb_util_ip_is_all_val entry");

  for (ii = 0; ii < 4; ii++)
  {
    if (ip[ii] != val)
    {
      rc = FALSE;
      break;
    }
  }

  DEBUG("UTIL: otb_util_ip_is_all_zero exit");

  return rc;
}

bool ICACHE_FLASH_ATTR otb_util_ip_is_subnet_valid(uint8_t *subnet)
{
  bool rc = TRUE;
  uint32_t mask;

  DEBUG("UTIL: otb_util_ip_is_subnet_valid entry");

  if (otb_util_ip_is_all_val(subnet, 0))
  {
    rc = FALSE;
    goto EXIT_LABEL;
  }
  mask = (subnet[0] << 24) | (subnet[1] << 16) | (subnet[2] << 8) | subnet[3];
  if (mask & (~mask >> 1))
  {
    rc = FALSE;
    goto EXIT_LABEL;
  }

EXIT_LABEL:

  DEBUG("UTIL: otb_util_ip_is_subnet_valid exit");

  return rc;
}

void ICACHE_FLASH_ATTR otb_util_uart0_rx_en(void)
{
  DEBUG("UTIL: otb_util_uart0_rx_en entry");
  
  ETS_UART_INTR_ATTACH(otb_util_uart0_rx_intr_handler, &(UartDev.rcv_buff));
  SET_PERI_REG_MASK(UART_CONF0(UART0), UART_RXFIFO_RST);
  CLEAR_PERI_REG_MASK(UART_CONF0(UART0), UART_RXFIFO_RST);
  WRITE_PERI_REG(UART_CONF1(UART0),
  ((100 & UART_RXFIFO_FULL_THRHD) << UART_RXFIFO_FULL_THRHD_S) |
  (0x02 & UART_RX_TOUT_THRHD) << UART_RX_TOUT_THRHD_S |
  UART_RX_TOUT_EN);
  SET_PERI_REG_MASK(UART_INT_ENA(UART0), UART_RXFIFO_TOUT_INT_ENA |UART_FRM_ERR_INT_ENA);
  WRITE_PERI_REG(UART_INT_CLR(UART0), 0xffff);
  SET_PERI_REG_MASK(UART_INT_ENA(UART0), UART_RXFIFO_FULL_INT_ENA|UART_RXFIFO_OVF_INT_ENA);
  ETS_UART_INTR_ENABLE();

  DEBUG("UTIL: otb_util_uart0_rx_en exit");

  return;
}

void ICACHE_FLASH_ATTR otb_util_uart0_rx_dis(void)
{
  DEBUG("UTIL: otb_util_uart0_rx_dis entry");
  
  CLEAR_PERI_REG_MASK(UART_INT_ENA(UART0), UART_RXFIFO_TOUT_INT_ENA |UART_FRM_ERR_INT_ENA);
  WRITE_PERI_REG(UART_INT_CLR(UART0), 0xffff);
  CLEAR_PERI_REG_MASK(UART_INT_ENA(UART0), UART_RXFIFO_FULL_INT_ENA|UART_RXFIFO_OVF_INT_ENA);
  ETS_UART_INTR_DISABLE();

  DEBUG("UTIL: otb_util_uart0_rx_dis exit");

  return;
}

void ICACHE_FLASH_ATTR otb_util_check_for_log_level(void)
{

  DEBUG("UTIL: otb_util_check_for_log_level entry");

  DETAIL("UTIL: check for log level input");

  otb_util_uart0_rx_en();
  otb_util_break_enabled = FALSE;
  otb_util_log_level_checking = TRUE;
  otb_break_rx_buf_len = 0;

  // Overload the break timer as only use one at once
  otb_util_break_enable_timer(250);

  DEBUG("UTIL: otb_util_check_for_log_level exit");

  return;
}


void ICACHE_FLASH_ATTR otb_util_check_for_break(void)
{

  DEBUG("UTIL: otb_util_check_for_break entry");

  INFO("OTB: Break checking")

  otb_util_uart0_rx_en();
  otb_util_break_enabled = FALSE;
  otb_util_break_checking = TRUE;
  otb_break_rx_buf_len = 0;

  otb_util_break_enable_timer(1000);

  DEBUG("UTIL: otb_util_check_for_break exit");

  return;
}

void ICACHE_FLASH_ATTR otb_util_break_disable_timer(void)
{

  DEBUG("UTIL: otb_util_break_disable_timer entry");

  otb_util_timer_cancel((os_timer_t*)&otb_util_break_timer);

  DEBUG("UTIL: otb_util_break_disable_timer entry");

  return;
}

void ICACHE_FLASH_ATTR otb_util_break_enable_timer(uint32_t period)
{

  DEBUG("UTIL: otb_util_break_enable_timer entry");

  otb_util_timer_set((os_timer_t*)&otb_util_break_timer, 
                    (os_timer_func_t *)otb_util_break_timerfunc,
                    NULL,
                    period,
                    1);

  DEBUG("UTIL: otb_util_break_enable_timer entry");

  return;
}

char ALIGN4 otb_util_break_timerfunc_string[] = "UTIL: Break timer popped";
void ICACHE_FLASH_ATTR otb_util_break_timerfunc(void *arg)
{

  DEBUG("UTIL: otb_util_break_timerfunc entry");

  if (otb_util_log_level_checking)
  {
    // Actually checking log level not break
    otb_util_break_disable_timer();
    otb_util_uart0_rx_dis();
    otb_util_log_level_checking = FALSE;
    if (otb_break_rx_buf_len > 0)
    {
      // Collected a char - look at first char
      switch (otb_break_rx_buf[0])
      {
        case '0':
          // Note DEBUG may not be compiled in - log this later
          otb_util_log_level = OTB_LOG_LEVEL_DEBUG;
          break;

        case '1':
          otb_util_log_level = OTB_LOG_LEVEL_DETAIL;
          break;

        case '2':
          otb_util_log_level = OTB_LOG_LEVEL_INFO;
          break;
          
        case '3':
          otb_util_log_level = OTB_LOG_LEVEL_WARN;
          break;
          
        case '4':
          otb_util_log_level = OTB_LOG_LEVEL_ERROR;
          break;

        default:
          WARN("UTIL: Unexpected character received when checking log level 0x%02x", otb_break_rx_buf[0]);
      }
    }

    switch (otb_util_log_level)
    {
      case OTB_LOG_LEVEL_DEBUG:
        ERROR("\nOTB: Log level selected: DEBUG");
#ifndef OTB_DEBUG
          ERROR("UTIL: DEBUG logging selected, but not compiled into firmware")
#endif // OTB_DEBUG          
        break;

      case OTB_LOG_LEVEL_DETAIL:
        ERROR("\nOTB: Log level selected: DETAIL");
        break;

      case OTB_LOG_LEVEL_INFO:
        ERROR("\nOTB: Log level selected: INFO");
        break;

      case OTB_LOG_LEVEL_WARN:
        ERROR("\nOTB: Log level selected: WARN");
        break;

      case OTB_LOG_LEVEL_ERROR:
        ERROR("\nOTB: Log level selected: ERROR");
        break;

      default:
        OTB_ASSERT(FALSE);
    }
    otb_util_resume_init();
  }
  else if (!otb_util_break_checking)
  {
    // We are already breaked - timer has popped so restart
    otb_reset(otb_util_break_timerfunc_string);
  }
  else
  {
    // We were checking whether to break
    otb_util_uart0_rx_dis();

    // No longer checking for break
    otb_util_break_disable_timer();
    otb_util_break_checking = FALSE;

    // Actually check for a break
    
    if ((otb_break_rx_buf_len >= 5) && (!os_memcmp(otb_break_rx_buf, "break", 5)))
    {
      DETAIL("UTIL: User break received");
      otb_util_break_enabled = TRUE;
    }
    else if (otb_util_uart_rx_bytes > 0)
    {
      DETAIL("UTIL: Received %d bytes of data during break checking", otb_break_rx_buf_len);
    }

    // If the user didn't break then carry on with usual boot sequence
    if (!otb_util_break_enabled)
    {
      otb_wifi_kick_off();
    }
    else
    {
      otb_break_start();
    }
  }
  
  DEBUG("UTIL: otb_util_break_timerfunc exit");

  return;
}

void otb_util_uart0_rx_intr_handler(void *para)
{
  uint8_t rx_len;
  char rx_char;
  uint8_t ii;

  if (UART_FRM_ERR_INT_ST == (READ_PERI_REG(UART_INT_ST(UART0)) & UART_FRM_ERR_INT_ST))
  {
    // Error
    WRITE_PERI_REG(UART_INT_CLR(UART0), UART_FRM_ERR_INT_CLR);
  }
  else if (UART_RXFIFO_FULL_INT_ST == (READ_PERI_REG(UART_INT_ST(UART0)) & UART_RXFIFO_FULL_INT_ST))
  {
    // FIFO Full
    CLEAR_PERI_REG_MASK(UART_INT_ENA(UART0), UART_RXFIFO_FULL_INT_ENA|UART_RXFIFO_TOUT_INT_ENA);
    WRITE_PERI_REG(UART_INT_CLR(UART0), UART_RXFIFO_FULL_INT_CLR);
  }
  else if (UART_RXFIFO_TOUT_INT_ST == (READ_PERI_REG(UART_INT_ST(UART0)) & UART_RXFIFO_TOUT_INT_ST))
  {
    // Timeout (character received)
    CLEAR_PERI_REG_MASK(UART_INT_ENA(UART0), UART_RXFIFO_FULL_INT_ENA|UART_RXFIFO_TOUT_INT_ENA);
    WRITE_PERI_REG(UART_INT_CLR(UART0), UART_RXFIFO_TOUT_INT_CLR);
  }
  else if (UART_RXFIFO_OVF_INT_ST  == (READ_PERI_REG(UART_INT_ST(UART0)) & UART_RXFIFO_OVF_INT_ST))
  {
    WRITE_PERI_REG(UART_INT_CLR(UART0), UART_RXFIFO_OVF_INT_CLR);
  }

  rx_len = (READ_PERI_REG(UART_STATUS(UART0)) >> UART_RXFIFO_CNT_S) & UART_RXFIFO_CNT;
  for (ii = 0; ii < rx_len; ii++)
  {
    rx_char = READ_PERI_REG(UART_FIFO(UART0)) & 0xFF;
    if (otb_util_uart_rx_bytes < OTB_BREAK_RX_BUF_LEN)
    {
      otb_break_rx_buf[otb_break_rx_buf_len] = rx_char;
      otb_break_rx_buf_len++;
    }
    if ((otb_util_break_enabled) && (otb_break_rx_buf_len > 0))
    {
      otb_break_process_char();
    }
  }

  WRITE_PERI_REG(UART_INT_CLR(UART0), UART_RXFIFO_FULL_INT_CLR | UART_RXFIFO_TOUT_INT_CLR);
  SET_PERI_REG_MASK(UART_INT_ENA(UART0), UART_RXFIFO_FULL_INT_ENA|UART_RXFIFO_TOUT_INT_ENA);

}

size_t ICACHE_FLASH_ATTR otb_util_strnlen(const char *s, size_t maxlen)
{
  size_t ii;
  for (ii = 0; ii < maxlen; ii++)
  {
    if (s[ii] == 0)
    {
      break;
    }
  }
  return ii;
}

char* ICACHE_FLASH_ATTR strcat(char * dest, const char * src) {
    return strncat(dest, src, strlen(src));
}

char* ICACHE_FLASH_ATTR strncat(char * dest, const char * src, size_t n) {
    size_t i;
    size_t offset = strlen(dest);
    for(i = 0; i < n && src[i]; i++) {
        dest[i + offset] = src[i];
    }
    dest[i + offset] = 0;
    return dest;
}

int ICACHE_FLASH_ATTR islower(int c) {
    if(c >= 'a' && c <= 'z') {
        return 1;
    }
    return 0;
}

int ICACHE_FLASH_ATTR isupper(int c) {
    if(c >= 'A' && c <= 'Z') {
        return 1;
    }
    return 0;
}

int ICACHE_FLASH_ATTR isalpha(int c) {
    if(islower(c) || isupper(c)) {
        return 1;
    }
    return 0;
}

int ICACHE_FLASH_ATTR isdigit(int c) {
    if(c >= '0' && c <= '9') {
        return 1;
    }
    return 0;
}

int ICACHE_FLASH_ATTR isalnum(int c) {
    if(isalpha(c) || isdigit(c)) {
        return 1;
    }
    return 0;
}

int ICACHE_FLASH_ATTR iscntrl(int c) {
    if(c <= 0x1F || c == 0x7F) {
        return 1;
    }
    return 0;
}

int ICACHE_FLASH_ATTR isprint(int c) {
    if(!iscntrl(c)) {
        return 1;
    }
    return 0;
}

int ICACHE_FLASH_ATTR isgraph(int c) {
    if(isprint(c) && c != ' ') {
        return 1;
    }
    return 0;
}

int ICACHE_FLASH_ATTR ispunct(int c) {
    if(isgraph(c) && !isalnum(c)) {
        return 1;
    }
    return 0;
}

int ICACHE_FLASH_ATTR isxdigit(int c) {
    if(c >= 'A' && c <= 'F') {
        return 1;
    }
    if(c >= 'a' && c <= 'f') {
        return 1;
    }
    if(isdigit(c)) {
        return 1;
    }
    return 0;
}

int ICACHE_FLASH_ATTR tolower(int c) {
    if(isupper(c)) {
        c += 0x20;
    }
    return c;
}

int ICACHE_FLASH_ATTR toupper(int c) {
    if(islower(c)) {
        c -= 0x20;
    }
    return c;
}

int ICACHE_FLASH_ATTR isblank(int c) {
    switch(c) {
        case 0x20: // ' '
        case 0x09: // '\t'
            return 1;
    }
    return 0;
}


int ICACHE_FLASH_ATTR isspace(int c) {
    switch(c) {
        case 0x20: // ' '
        case 0x09: // '\t'
        case 0x0a: // '\n'
        case 0x0b: // '\v'
        case 0x0c: // '\f'
        case 0x0d: // '\r'
            return 1;
    }
    return 0;
}

/**
 * strtol() and strtoul() implementations borrowed from newlib:
 * http://www.sourceware.org/newlib/
 *      newlib/libc/stdlib/strtol.c
 *      newlib/libc/stdlib/strtoul.c
 *
 * Adapted for ESP8266 by Kiril Zyapkov <kiril.zyapkov@gmail.com>
 *
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the University of
 *    California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

long ICACHE_FLASH_ATTR strtol(const char *nptr, char **endptr, int base) {
    const unsigned char *s = (const unsigned char *)nptr;
    unsigned long acc;
    int c;
    unsigned long cutoff;
    int neg = 0, any, cutlim;

    /*
     * Skip white space and pick up leading +/- sign if any.
     * If base is 0, allow 0x for hex and 0 for octal, else
     * assume decimal; if base is already 16, allow 0x.
     */
    do {
        c = *s++;
    } while (isspace(c));
    if (c == '-') {
        neg = 1;
        c = *s++;
    } else if (c == '+')
        c = *s++;
    if ((base == 0 || base == 16) &&
        c == '0' && (*s == 'x' || *s == 'X')) {
        c = s[1];
        s += 2;
        base = 16;
    }
    if (base == 0)
        base = c == '0' ? 8 : 10;

    /*
     * Compute the cutoff value between legal numbers and illegal
     * numbers.  That is the largest legal value, divided by the
     * base.  An input number that is greater than this value, if
     * followed by a legal input character, is too big.  One that
     * is equal to this value may be valid or not; the limit
     * between valid and invalid numbers is then based on the last
     * digit.  For instance, if the range for longs is
     * [-2147483648..2147483647] and the input base is 10,
     * cutoff will be set to 214748364 and cutlim to either
     * 7 (neg==0) or 8 (neg==1), meaning that if we have accumulated
     * a value > 214748364, or equal but the next digit is > 7 (or 8),
     * the number is too big, and we will return a range error.
     *
     * Set any if any `digits' consumed; make it negative to indicate
     * overflow.
     */
    cutoff = neg ? -(unsigned long)LONG_MIN : LONG_MAX;
    cutlim = cutoff % (unsigned long)base;
    cutoff /= (unsigned long)base;
    for (acc = 0, any = 0;; c = *s++) {
        if (isdigit(c))
            c -= '0';
        else if (isalpha(c))
            c -= isupper(c) ? 'A' - 10 : 'a' - 10;
        else
            break;
        if (c >= base)
            break;
               if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
            any = -1;
        else {
            any = 1;
            acc *= base;
            acc += c;
        }
    }
    if (any < 0) {
        acc = neg ? LONG_MIN : LONG_MAX;
        errno = ERANGE;
    } else if (neg)
        acc = -acc;
    if (endptr != 0)
        *endptr = (char *) (any ? (char *)s - 1 : nptr);
    return (acc);
}

long ICACHE_FLASH_ATTR atol(const char* s) {
    char * tmp;
    return strtol(s, &tmp, 10);
}

int ICACHE_FLASH_ATTR atoi(const char* s) {
    return (int) atol(s);
}

static int errno_var = 0;

int* __errno(void) {
    // DEBUGV("__errno is called last error: %d (not current)\n", errno_var);
    return &errno_var;
}

char *os_strdup(const char *s)
{
  char *new;
  new = (char *)os_malloc(os_strlen(s)+1);
  if (new != NULL)
  {
    os_strcpy(new, s);
  }
  return new;
}
