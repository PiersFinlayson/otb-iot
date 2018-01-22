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
  unsigned char character;
  uint8_t bytes[6];
} otb_font_6x6;

#define OTB_FONT_LEN 27
#ifndef OTB_FONT_C
otb_font_6x6 otb_font_6x6_1[OTB_FONT_LEN];
#else
otb_font_6x6 otb_font_6x6_1[OTB_FONT_LEN] = 
{
  {
    ' ',
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
    {
      0b001110,
      0b000100,
      0b000100,
      0b000100,    
      0b000100,
      0b001110
    }
  },
  {
    'J',
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
    {
      0b111110,
      0b100001,
      0b111110,
      0b100000,
      0b100000,
      0b100000
    }
  },
  {
    'Q',
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
    {
      0b111111,
      0b000010,
      0b000100,
      0b001000,
      0b010000,
      0b111111
    }
  }
};
#endif // OTB_LED_C

#endif // OTB_FONT_H_INCLUDED
