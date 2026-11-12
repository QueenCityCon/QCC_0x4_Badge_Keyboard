#define SHIFT_LED      13
#define SYM_LED        23
#define FN_LED         24

#include <Wire.h>

#define Set_Bit(val, bitn)    (val |=(1<<(bitn)))
#define Clr_Bit(val, bitn)     (val&=~(1<<(bitn)))
#define Get_Bit(val, bitn)    (val &(1<<(bitn)) )

#define COL_0   254
#define COL_1   253
#define COL_2   251
#define COL_3   247
#define COL_4   239
#define COL_5   223
#define COL_6   191
#define COL_7   127
#define COL_8   14
#define COL_9   13
#define COL_10  11
#define COL_11  7

//       d0   d1     d2  d3 d4 d5 d6 d7 d8 d9 d10 d11
//A3：   esc   1      2  3  4  5  6  7  8  9  0   del
//A2:    tab   q      w  e  r  t  y  u  i  o  p
//A1:   left   up     a  s  d  f  g  h  j  k  l   enter
//A0:   down   right  z  x  c  v  b  n  m  ,  .   space
//sym: d15
//shift: d12
//fn: d14

unsigned char KeyMap[48][7] =
{ //nor, shift,long_shift, sym,long_sym,fn,long_fn,
  {  27,  27,  27,  27,  27, 128, 128},//esc
  { '1', '1', '1', '!', '!', 129, 129},//1
  { '2', '2', '2', '@', '@', 130, 130},//2
  { '3', '3', '3', '#', '#', 131, 131},//3
  { '4', '4', '4', '$', '$', 132, 132},//4
  { '5', '5', '5', '%', '%', 133, 133},//5
  { '6', '6', '6', '^', '^', 134, 134},//6
  { '7', '7', '7', '&', '&', 135, 135},//7
  { '8', '8', '8', '*', '*', 136, 136},//8
  { '9', '9', '9', '(', '(', 137, 137},//9
  { '0', '0', '0', ')', ')', 138, 138},//0
  {  8 , 127 , 127,  8 ,  8 , 139, 139}, //del
  {  9 ,  9 ,  9 ,  9 ,  9 , 140, 140},//tab
  { 'q', 'Q', 'Q', '{', '{', 141, 141},//q
  { 'w', 'W', 'W', '}', '}', 142, 142},//w
  { 'e', 'E', 'E', '[', '[', 143, 143},//e
  { 'r', 'R', 'R', ']', ']', 144, 144},//r
  { 't', 'T', 'T', '/', '/', 145, 145},//t
  { 'y', 'Y', 'Y', '\\', '\\', 146, 146}, //y
  { 'u', 'U', 'U', '|', '|', 147, 147},//u
  { 'i', 'I', 'I', '~', '~', 148, 148},//i
  { 'o', 'O', 'O', '\'', '\'', 149, 149}, //o
  { 'p', 'P', 'P', '"', '"', 150, 150},//p
  {  0 ,  0 ,  0 ,  0 ,  0 , 0, 0},    //  no key
  {  180,  180,  180,  180,  180, 152, 152},//LEFT
  {  181,  181,  181,  181,  181, 153, 153},//UP
  { 'a', 'A', 'A', ';', ';', 154, 154},//a
  { 's', 'S', 'S', ':', ':', 155, 155},//s
  { 'd', 'D', 'D', '`', '`', 156, 156},//d
  { 'f', 'F', 'F', '+', '+', 157, 157},//f
  { 'g', 'G', 'G', '-', '-', 158, 158},//g
  { 'h', 'H', 'H', '_', '_', 159, 159},//h
  { 'j', 'J', 'J', '=', '=', 160, 160},//j
  { 'k', 'K', 'K', '?', '?', 161, 161},//k
  { 'l', 'L', 'L',   0,   0, 162, 162},//l
  { 13 , 13 ,  13,  13,  13, 163, 163},//enter
  {  182, 182 ,  182,  182,  182, 164, 164},//DOWN
  {  183, 183 ,  183,  183,  183, 165, 165},//RIGHT
  { 'z', 'Z', 'Z',  0 ,  0 , 166, 166},//z
  { 'x', 'X', 'X',  0 ,  0 , 167, 167},//x
  { 'c', 'C', 'C',  0 ,  0 , 168, 168},//c
  { 'v', 'V', 'V',  0 ,  0 , 169, 169},//v
  { 'b', 'B', 'B',  0 ,  0 , 170, 170},//b
  { 'n', 'N', 'N',  0 ,  0 , 171, 171},//n
  { 'm', 'M', 'M',  0 ,  0 , 172, 172},//m
  { ',', ',', ',', '<', '<', 173, 173},//,
  { '.', '.', '.', '>', '>', 174, 174},//.
  { ' ' , ' ', ' ', ' ', ' ', 175, 175}//space
};
#define shiftPressed (PINB & 0x10 ) != 0x10
#define symPressed (PINB & 0x80 ) != 0x80
#define fnPressed (PINB & 0x40 ) != 0x40
int _shift = 0, _fn = 0, _sym = 0, idle = 0;
unsigned char KEY = 0, hadPressed = 0;
int Mode = 0; //0->normal.1->shift 2->long_shift, 3->sym, 4->long_shift 5->fn,6->long_fn
void flashOn()
{
  digitalWrite(SHIFT_LED,LOW);
  digitalWrite(SYM_LED,LOW);
  digitalWrite(FN_LED,LOW);
  delay(5);
}
void flashOff()
{
  digitalWrite(SHIFT_LED,HIGH);
  digitalWrite(SYM_LED,HIGH);
  digitalWrite(FN_LED,HIGH);
}
void requestEvent()
{
  if (hadPressed == 1)
  {
    Wire.write(KeyMap[KEY - 1][Mode]);
    //KEY=0;
    if ((Mode == 1) || (Mode == 3) || (Mode == 5)) {
      Mode = 0;
      _shift = 0;
      _sym = 0;
      _fn = 0;
    }

    hadPressed = 0;
    return;
  }
}


void setup()
{
  pinMode(SHIFT_LED, OUTPUT);
  pinMode(FN_LED, OUTPUT);
  pinMode(SYM_LED, OUTPUT);
  flashOn();
  delay(500);
  flashOff();
  pinMode(A3, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A0, OUTPUT);
  digitalWrite(A0, HIGH);
  digitalWrite(A1, LOW);
  digitalWrite(A2, LOW);
  digitalWrite(A3, LOW);
  DDRB = 0x20;
  PORTB = 0xff;
  DDRD = 0x00;
  PORTD = 0xff;

  Wire.begin(0x5f);
  Wire.onRequest(requestEvent);
}

unsigned char GetInput()
{
  digitalWrite(A3, LOW);
  digitalWrite(A2, HIGH);
  digitalWrite(A1, HIGH);
  digitalWrite(A0, HIGH);
  delay(2);
  switch (PIND)
  {
    case COL_0: while (PIND != 0xff) {
        flashOn();
        //delay(1);
      } flashOff();   hadPressed = 1; return  1; break;
    case COL_1: while (PIND != 0xff) {
        flashOn();
        // delay(1);
      } flashOff();  hadPressed = 1; return 2; break;
    case COL_2: while (PIND != 0xff) {
        flashOn();
        // delay(1);
      } flashOff();  hadPressed = 1;  return 3; break;
    case COL_3: while (PIND != 0xff) {
        flashOn();
        // delay(1);
      } flashOff();  hadPressed = 1;  return 4; break;
    case COL_4: while (PIND != 0xff) {
        flashOn();
        //delay(1);
      } flashOff();  hadPressed = 1; return 5; break;
    case COL_5: while (PIND != 0xff) {
        flashOn();
        //   delay(1);
      } flashOff();  hadPressed = 1; return 6; break;
    case COL_6: while (PIND != 0xff) {
        flashOn();
        // delay(1);
      } flashOff();  hadPressed = 1; return 7; break;
    case COL_7: while (PIND != 0xff) {
        flashOn();
        // delay(1);
      } flashOff();  hadPressed = 1; return  8; break;
  }
  switch ((PINB&0x0f))
  {
    case COL_8: while ((PINB&0x0f)!=0x0f) {
        flashOn();
        //delay(1);
      } flashOff();  hadPressed = 1; return 9; break;
    case COL_9: while ((PINB&0x0f)!=0x0f) {
        flashOn();
        //delay(1);
      } flashOff();  hadPressed = 1; return 10; break;
    case COL_10: while ((PINB&0x0f)!=0x0f) {
        flashOn();
        // delay(1);
      } flashOff();  hadPressed = 1; return 11; break;
    case COL_11: while ((PINB&0x0f)!=0x0f) {
        flashOn();
        // delay(1);
      } flashOff();  hadPressed = 1; return 12; break;
  }

  digitalWrite(A3, HIGH);
  digitalWrite(A2,  LOW);
  digitalWrite(A1, HIGH);
  digitalWrite(A0, HIGH);
  delay(2);
  switch (PIND)
  {
    case COL_0: while (PIND != 0xff) {
        flashOn();
        //  delay(1);
      } flashOff();  hadPressed = 1; return  13; break;
    case COL_1: while (PIND != 0xff) {
        flashOn();
        //delay(1);
      } flashOff();  hadPressed = 1; return 14; break;
    case COL_2: while (PIND != 0xff) {
        flashOn();
        // delay(1);
      } flashOff();  hadPressed = 1;  return 15; break;
    case COL_3: while (PIND != 0xff) {
        flashOn();
        delay(1);
      } flashOff();  hadPressed = 1;  return 16; break;
    case COL_4: while (PIND != 0xff) {
        flashOn();
        // delay(1);
      } flashOff();  hadPressed = 1; return 17; break;
    case COL_5: while (PIND != 0xff) {
        flashOn();
        // delay(1);
      } flashOff();  hadPressed = 1; return 18; break;
    case COL_6: while (PIND != 0xff) {
        flashOn();
        //delay(1);
      } flashOff();  hadPressed = 1; return 19; break;
    case COL_7: while (PIND != 0xff) {
        flashOn();
        //  delay(1);
      } flashOff();  hadPressed = 1; return  20; break;
  }
  switch ((PINB&0x0f))
  {
    case COL_8: while ((PINB&0x0f)!=0x0f) {
        flashOn();
        //delay(1);
      } flashOff();  hadPressed = 1; return 21; break;
    case COL_9: while ((PINB&0x0f)!=0x0f) {
        flashOn();
        // delay(1);
      } flashOff();  hadPressed = 1; return 22; break;
    case COL_10: while ((PINB&0x0f)!=0x0f) {
        flashOn();
        //delay(1);
      } flashOff();  hadPressed = 1; return 23; break;
    case COL_11: while ((PINB&0x0f)!=0x0f) {
        flashOn();
        // delay(1);
      } flashOff();  hadPressed = 1; return 24; break;
  }

  digitalWrite(A3, HIGH);
  digitalWrite(A2, HIGH);
  digitalWrite(A1, LOW);
  digitalWrite(A0, HIGH);
  delay(2);
  switch (PIND)
  {
    case COL_0: while (PIND != 0xff) {
        flashOn();
        //delay(1);
      } flashOff();  hadPressed = 1; return  25; break;
    case COL_1: while (PIND != 0xff) {
        flashOn();
        // delay(1);
      } flashOff();  hadPressed = 1; return 26; break;
    case COL_2: while (PIND != 0xff) {
        flashOn();
        //delay(1);
      } flashOff();  hadPressed = 1;  return 27; break;
    case COL_3: while (PIND != 0xff) {
        flashOn();
        // delay(1);
      } flashOff();  hadPressed = 1;  return 28; break;
    case COL_4: while (PIND != 0xff) {
        flashOn();
        //  delay(1);
      } flashOff();  hadPressed = 1; return 29; break;
    case COL_5: while (PIND != 0xff) {
        flashOn();
        // delay(1);
      } flashOff();  hadPressed = 1; return 30; break;
    case COL_6: while (PIND != 0xff) {
        flashOn();
        //  delay(1);
      } flashOff();  hadPressed = 1; return 31; break;
    case COL_7: while (PIND != 0xff) {
        flashOn();
        //  delay(1);
      } flashOff();  hadPressed = 1; return  32; break;
  }
  switch ((PINB&0x0f))
  {
    case COL_8: while ((PINB&0x0f)!=0x0f) {
        flashOn();
        //  delay(1);
      } flashOff();  hadPressed = 1; return 33; break;
    case COL_9: while ((PINB&0x0f)!=0x0f) {
        flashOn();
        //delay(1);
      } flashOff();  hadPressed = 1; return 34; break;
    case COL_10: while ((PINB&0x0f)!=0x0f) {
        flashOn();
        //delay(1);
      } flashOff();  hadPressed = 1; return 35; break;
    case COL_11: while ((PINB&0x0f)!=0x0f) {
        flashOn();
        // delay(1);
      } flashOff();  hadPressed = 1; return 36; break;
  }

  digitalWrite(A3, HIGH);
  digitalWrite(A2, HIGH);
  digitalWrite(A1, HIGH);
  digitalWrite(A0, LOW);
  delay(2);
  switch (PIND)
  {
    case COL_0: while (PIND != 0xff) {
        flashOn();
        //delay(1);
      } flashOff();  hadPressed = 1; return  37; break;
    case COL_1: while (PIND != 0xff) {
        flashOn();
        //delay(1);
      } flashOff();  hadPressed = 1; return 38; break;
    case COL_2: while (PIND != 0xff) {
        flashOn();
        //delay(1);
      } flashOff();  hadPressed = 1;  return 39; break;
    case COL_3: while (PIND != 0xff) {
        flashOn();
        // delay(1);
      } flashOff();  hadPressed = 1;  return 40; break;
    case COL_4: while (PIND != 0xff) {
        flashOn();
        // delay(1);
      } flashOff();  hadPressed = 1; return 41; break;
    case COL_5: while (PIND != 0xff) {
        flashOn();
        // delay(1);
      } flashOff();  hadPressed = 1; return 42; break;
    case COL_6: while (PIND != 0xff) {
        flashOn();
        //delay(1);
      } flashOff();  hadPressed = 1; return 43; break;
    case COL_7: while (PIND != 0xff) {
        flashOn();
        //delay(1);
      } flashOff();  hadPressed = 1; return  44; break;
  }
  switch ((PINB&0x0f))
  {
    case COL_8: while ((PINB&0x0f)!=0x0f) {
        flashOn();
        //delay(1);
      } flashOff();  hadPressed = 1; return 45; break;
    case COL_9: while ((PINB&0x0f)!=0x0f) {
        flashOn();
        // delay(1);
      } flashOff();  hadPressed = 1; return 46; break;
    case COL_10: while ((PINB&0x0f)!=0x0f) {
        flashOn();
        //delay(1);
      } flashOff();  hadPressed = 1; return 47; break;
    case COL_11: while ((PINB&0x0f)!=0x0f) {
        flashOn();
        //delay(1);
      } flashOff();  hadPressed = 1; return 48; break;
  }
  hadPressed = 0;
  return 255;
}



void loop()
{
  if (shiftPressed)
  {
    _sym = 0; _fn = 0; idle = 0;
    while (shiftPressed)delay(1);
    if (_shift == 0)
    {
      delay(200);
      if (shiftPressed)
      {
        while (shiftPressed)delay(1);
        _shift = 2;
        Mode = 2;
      }
      else
      {
        _shift = 1;
        Mode = 1;
      }
    }
    else
    {
      delay(200);
      if (shiftPressed)
      {
        while (shiftPressed)delay(1);
        if (_shift == 2)
        {
          Mode = 0;
          _shift = 0;
        }
        else
        {
          Mode = 2;
          _shift = 2;
        }
      }
      else
      {
        Mode = 0;
        _shift = 0;
      }
    }
  }


  if (symPressed)
  {
    _shift = 0; _fn = 0; idle = 0;
    while (symPressed)delay(1);
    if (_sym == 0)
    {
      delay(200);
      if (symPressed)
      {
        while (symPressed)delay(1);
        _sym = 2;
        Mode = 4;
      }
      else
      {
        _sym = 1;
        Mode = 3;
      }
    }
    else
    {
      delay(200);
      if (symPressed)
      {
        while (symPressed)delay(1);
        if (_sym == 2)
        {
          Mode = 0;
          _sym = 0;
        }
        else
        {
          Mode = 4;
          _sym = 2;
        }
      }
      else
      {
        Mode = 0;
        _sym = 0;
      }
    }
  }

  if (fnPressed)
  {
    _sym = 0; _shift = 0; idle = 0;
    while (fnPressed)delay(1);
    if (_fn == 0)
    {
      delay(200);
      if (fnPressed)
      {
        while (fnPressed)delay(1);
        _fn = 2;
        Mode = 6;
      }
      else
      {
        _fn = 1;
        Mode = 5;
      }
    }
    else
    {
      delay(200);
      if (fnPressed)
      {
        while (fnPressed)delay(1);
        if (_fn == 2)
        {
          Mode = 0;
          _fn = 0;
        }
        else
        {
          Mode = 6;
          _fn = 2;
        }
      }
      else
      {
        Mode = 0;
        _fn = 0;
      }
    }
  }




  switch (Mode)
  {
    case 0://normal
      digitalWrite(SHIFT_LED,HIGH);
      digitalWrite(FN_LED,HIGH);
      digitalWrite(SYM_LED,HIGH);
      break;
    case 1://shift
      if ((idle / 6) % 2 == 1)
        digitalWrite(SHIFT_LED,HIGH);
      else
        digitalWrite(SHIFT_LED,LOW); break;
    case 2://long_shift
      digitalWrite(SHIFT_LED,LOW); break;
    case 3://sym
      if ((idle / 6) % 2 == 1)
        digitalWrite(SYM_LED,HIGH);
      else
        digitalWrite(SYM_LED,LOW); break;
    case 4://long_sym
      digitalWrite(SYM_LED,LOW); break;
    case 5://fn
      if ((idle / 6) % 2 == 1)
        digitalWrite(FN_LED,HIGH);
      else
        digitalWrite(FN_LED,LOW); break;
    case 6://long_fn
      digitalWrite(FN_LED,LOW); break;
  }
  
  if (hadPressed == 0)
  {
    KEY = GetInput();
  }
  idle++;
  delay(10);
}










