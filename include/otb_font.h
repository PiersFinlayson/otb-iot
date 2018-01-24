/*
 * OTB-IOT - Out of The Box Internet Of Things
 *
 * Copyright (C) 2016-8 Piers Finlayson
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version. 
 *
 * This program is distributed in the hope that it will be useful, but WITfHOUT
 * ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for 
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#ifndef OTB_FONT_H_INCLUDED
#define OTB_FONT_H_INCLUDED

typedef struct otb_font_6x6
{
  // Which character via MQTT displays this character
  unsigned char character;

  // Length of character in bits wide (measuring from MSB, i.e bit 5 of 0-5)
  uint8_t len;

  // Bytes making up this character, only 6 LSB used
  uint8_t bytes[6];
} otb_font_6x6;

#define OTB_FONT_LEN 1 + 26 + 26 + 10 + 5 + 2 // space, upper case alphabet, lower case, digits, @, !, ?, ", #, heart(~), degrees(^)
#ifndef OTB_FONT_C
otb_font_6x6 otb_font_6x6_1[OTB_FONT_LEN];
#else
otb_font_6x6 otb_font_6x6_1[OTB_FONT_LEN] = 
{
  {
    ' ',
    2,
    {
      0b000000,
      0b000000,
      0b000000,
      0b000000,
      0b000000,
      0b000000,
    }
  },
  {
    'A',
    6,
    {
      0b011110,
      0b100001,
      0b100001,
      0b111111,
      0b100001,
      0b100001
    }
  },
  {
    'B',
    6,
    {
      0b111110,
      0b100001,
      0b111110,
      0b100001,
      0b100001,
      0b111110
    }
  },
  {
    'C',
    6,
    {
      0b011110,
      0b100001,
      0b100000,
      0b100000,
      0b100001,
      0b011110
    }
  },
  {
    'D',
    6,
    {
      0b111100,
      0b100010,
      0b100001,
      0b100001,
      0b100010,
      0b111100
    }
  },
  {
    'E',
    6,
    {
      0b111111,
      0b100000,
      0b111110,
      0b100000,
      0b100000,
      0b111111
    }
  },
  {
    'F',
    6,
    {
      0b111111,
      0b100000,
      0b111110,
      0b100000,
      0b100000,
      0b100000
    }
  },
  {
    'G',
    6,
    {
      0b011110,
      0b100001,
      0b100000,
      0b100111,
      0b100001,
      0b011110
    }
  },
  {
    'H',
    6,
    {
      0b100001,
      0b100001,
      0b111111,
      0b100001,
      0b100001,
      0b100001
    }
  },
  {
    'I',
    3,
    {
      0b111000,
      0b010000,
      0b010000,
      0b010000,    
      0b010000,
      0b111000
    }
  },
  {
    'J',
    6,
    {
      0b000111,
      0b000010,
      0b000010,
      0b100010,
      0b100010,
      0b011100
    }
  },
  {
    'K',
    6,
    {
      0b100010,
      0b100100,
      0b111000,
      0b100100,
      0b100010,
      0b100001
    }
  },
  {
    'L',
    6,
    {
      0b100000,
      0b100000,
      0b100000,
      0b100000,
      0b100000,
      0b111111
    }
  },
  {
    'M',
    6,
    {
      0b100001,
      0b110011,
      0b101101,
      0b100001,
      0b100001,
      0b100001
    }
  },
  {
    'N',
    6,
    {
      0b100001,
      0b110001,
      0b101001,
      0b100101,
      0b100011,
      0b100001
    }
  },
  {
    'O',
    6,
    {
      0b011110,
      0b100001,
      0b100001,
      0b100001,
      0b100001,
      0b011110
    }
  },
  {
    'P',
    6,
    {
      0b111110,
      0b100001,
      0b100001,
      0b111110,
      0b100000,
      0b100000
    }
  },
  {
    'Q',
    6,
    {
      0b011110,
      0b100001,
      0b100001,
      0b100101,
      0b100011,
      0b011110
    }
  },
  {
    'R',
    6,
    {
      0b111110,
      0b100001,
      0b100001,
      0b111110,
      0b100010,
      0b100001
    }
  },
  {
    'S',
    6,
    {
      0b011111,
      0b100000,
      0b011110,
      0b000001,
      0b000001,
      0b111110
    }
  },
  {
    'T',
    5,
    {
      0b111110,
      0b001000,
      0b001000,
      0b001000,
      0b001000,
      0b001000
    }
  },
  {
    'U',
    6,
    {
      0b100001,
      0b100001,
      0b100001,
      0b100001,
      0b100001,
      0b011110
    }
  },
  {
    'V',
    6,
    {
      0b100001,
      0b100001,
      0b100001,
      0b100001,
      0b010010,
      0b001100
    }
  },
  {
    'W',
    6,
    {
      0b100001,
      0b100001,
      0b100001,
      0b100001,
      0b101101,
      0b010010
    }
  },
  {
    'X',
    6,
    {
      0b100001,
      0b010010,
      0b001100,
      0b001100,
      0b010010,
      0b100001
    }
  },
  {
    'Y',
    5,
    {
      0b100010,
      0b010100,
      0b001000,
      0b001000,
      0b001000,
      0b001000
    }
  },
  {
    'Z',
    6,
    {
      0b111111,
      0b000010,
      0b000100,
      0b001000,
      0b010000,
      0b111111
    }
  },
  {
    'a',
    6,
    {
      0b000000,
      0b011110,
      0b000001,
      0b011111,
      0b100001,
      0b011111,
    }
  },
  {
    'b',
    6,
    {
      0b100000,
      0b100000,
      0b111110,
      0b100001,
      0b100001,
      0b111110
    }
  },
  {
    'c',
    4,
    {
      0b000000,
      0b011100,
      0b100000,
      0b100000,
      0b100000,
      0b011100
    }
  },
  {
    'd',
    6,
    {
      0b000001,
      0b000001,
      0b011111,
      0b100001,
      0b100001,
      0b011111
    }
  },
  {
    'e',
    6,
    {
      0b000000,
      0b011110,
      0b100001,
      0b111111,
      0b100000,
      0b011111
    }
  },
  {
    'f',
    3,
    {
      0b011000,
      0b100000,
      0b110000,
      0b100000,
      0b100000,
      0b100000
    }
  },
  {
    'g',
    6,
    {
      0b000000,
      0b011111,
      0b100001,
      0b011111,
      0b000001,
      0b011110
    }
  },
  {
    'h',
    6,
    {
      0b100000,
      0b100000,
      0b111110,
      0b100001,
      0b100001,
      0b100001
    }
  },
  {
    'i',
    3,
    {
      0b010000,
      0b000000,
      0b110000,
      0b010000,
      0b010000,
      0b111000
    }
  },
  {
    'j',
    6,
    {
      0b001000,
      0b000000,
      0b001000,
      0b001000,
      0b101000,
      0b010000
    }
  },
  {
    'k',
    4,
    {
      0b100000,
      0b100100,
      0b101000,
      0b110000,
      0b101000,
      0b100100

    }
  },
  {
    'l',
    3,
    {
      0b100000,
      0b100000,
      0b100000,
      0b100000,
      0b100000,
      0b011000
    }
  },
  {
    'm',
    5,
    {
      0b000000,
      0b111100,
      0b101010,
      0b101010,
      0b101010,
      0b101010
    }
  },
  {
    'n',
    4,
    {
      0b000000,
      0b111000,
      0b100100,
      0b100100,
      0b100100,
      0b100100
    }
  },
  {
    'o',
    6,
    {
      0b000000,
      0b011110,
      0b100001,
      0b100001,
      0b100001,
      0b011110
    }
  },
  {
    'p',
    6,
    {
      0b000000,
      0b111110,
      0b100001,
      0b100001,
      0b111110,
      0b100000
    }
  },
  {
    'q',
    6,
    {
      0b000000,
      0b011111,
      0b100001,
      0b100001,
      0b011111,
      0b000001
    }
  },
  {
    'r',
    4,
    {
      0b000000,
      0b101110,
      0b110000,
      0b100000,
      0b100000,
      0b100000
    }
  },
  {
    's',
    6,
    {
      0b000000,
      0b011110,
      0b100000,
      0b011110,
      0b000001,
      0b111110
    }
  },
  {
    't',
    4,
    {
      0b010000,
      0b111000,
      0b010000,
      0b010000,
      0b010000,
      0b001100
    }
  },
  {
    'u',
    6,
    {
      0b000000,
      0b100001,
      0b100001,
      0b100001,
      0b100001,
      0b011110
    }
  },
  {
    'v',
    6,
    {
      0b000000,
      0b100001,
      0b100001,
      0b100001,
      0b010010,
      0b001100
    }
  },
  {
    'w',
    6,
    {
      0b000000,
      0b100001,
      0b100001,
      0b100001,
      0b101101,
      0b010010
    }
  },
  {
    'x',
    5,
    {
      0b000000,
      0b100010,
      0b010100,
      0b001000,
      0b010100,
      0b100010
    }
  },
  {
    'y',
    6,
    {
      0b000000,
      0b100001,
      0b100001,
      0b011111,
      0b000001,
      0b011110
    }
  },
  {
    'z',
    5,
    {
      0b000000,
      0b111110,
      0b000100,
      0b001000,
      0b010000,
      0b111110
    }
  },
  {
    '0',
    6,
    {
      0b011110,
      0b100011,
      0b100101,
      0b101001,
      0b110001,
      0b011110
    }
  },
  {
    '1',
    5,
    {
      0b001000,
      0b011000,
      0b101000,
      0b001000,
      0b001000,
      0b111110
    }
  },
  {
    '2',
    6,
    {
      0b011110,
      0b100001,
      0b000001,
      0b011110,
      0b100000,
      0b111111
    }
  },
  {
    '3',
    6,
    {
      0b011110,
      0b100001,
      0b000110,
      0b000001,
      0b100001,
      0b011110
    }
  },
  {
    '4',
    6,
    {
      0b000010,
      0b000110,
      0b001010,
      0b010010,
      0b111111,
      0b000010
    }
  },
  {
    '5',
    6,
    {
      0b111111,
      0b100000,
      0b111110,
      0b000001,
      0b100001,
      0b011110
    }
  },
  {
    '6',
    6,
    {
      0b011110,
      0b100000,
      0b111110,
      0b100001,
      0b100001,
      0b011110
    }
  },
  {
    '7',
    6,
    {
      0b111111,
      0b000001,
      0b000010,
      0b000100,
      0b001000,
      0b001000
    }
  },
  {
    '8',
    6,
    {
      0b011110,
      0b100001,
      0b011110,
      0b100001,
      0b100001,
      0b011110
    }
  },
  {
    '9',
    6,
    {
      0b011110,
      0b100001,
      0b100001,
      0b011111,
      0b000001,
      0b011110
    }
  },
  {
    '@',
    6,
    {
      0b011110,
      0b100001,
      0b101101,
      0b101111,
      0b100000,
      0b011110
    }
  },
  {
    '!',
    1,
    {
      0b100000,
      0b100000,
      0b100000,
      0b100000,
      0b000000,
      0b100000
    }
  },
  {
    '?',
    5,
    {
      0b011100,
      0b100010,
      0b000100,
      0b001000,
      0b000000,
      0b001000
    }
  },
  {
    '"',
    3,
    {
      0b101000,
      0b101000,
      0b000000,
      0b000000,
      0b000000,
      0b000000
    }
  },
  {
    '#',
    6,
    {
      0b010010,
      0b111111,
      0b010010,
      0b010010,
      0b111111,
      0b010010,
    }
  },
  {
    '~',
    5,
    {
      // heart
      0b000000,
      0b010100,
      0b111110,
      0b111110,
      0b011100,
      0b001000
    }
  },
  {
    // degree symbol
    '^',
    3,
    {
      0b111000,
      0b101000,
      0b111000,
      0b000000,
      0b000000,
      0b000000
    }
  },
#if 0
  // Template to add more chars
  {
    '',
    {
    }
  },
#endif
};
#endif // OTB_LED_C

#endif // OTB_FONT_H_INCLUDED
