//Databus 8 bit PB 0-7

#define frame PB10
#define latch  PB8
#define clock  PB9

#define Yplus PB12
#define Xneg  PB13
#define Yneg  PB14
#define Xplus PB15

#define clocklow  GPIOB->regs->BRR  = 0x0200
#define clockhigh GPIOB->regs->BSRR = 0x0200
#define linelow  GPIOB->regs->BRR  = 0x0100
#define linehigh GPIOB->regs->BSRR = 0x0100

unsigned int pixelX,pixelY,touchX,touchY;
byte bufr[19200], fCol, intrOn ; 

//==================== Clock only =================

float sx = 0, sy = 1, mx = 1, my = 0, hx = -1, hy = 0;    // Saved H, M, S x & y multipliers
float sdeg=0, mdeg=0, hdeg=0;
uint16_t osx=62, osy=62, omx=62, omy=62, ohx=62, ohy=62;  // Saved H, M, S x & y coords
int xx0=0, xx1=0, yy0=0, yy1=0;
unsigned long targetTime = 0,sumX,sumY;                    // for next 1 second timeout

boolean initial = 1;

uint8_t conv2d(const char* p) {
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9')
    v = *p - '0';
  return 10 * v + *++p - '0';
}
uint8_t hh=conv2d(__TIME__), mm=conv2d(__TIME__+3), ss=conv2d(__TIME__+6);  // Get H, M, S from compile time

//===================== Clock end ==================

void setup() {
 disableDebugPorts();
for(byte i=16; i<32; i++)pinMode(i, OUTPUT);
 /*
 pinMode(PB0, OUTPUT); pinMode(PB1, OUTPUT); 
 pinMode(PB2, OUTPUT); pinMode(PB3, OUTPUT);
 pinMode(PB4, OUTPUT); pinMode(PB5, OUTPUT);
 pinMode(PB6, OUTPUT); pinMode(PB7, OUTPUT);//D0-D7 LCD 
 pinMode(PB12, OUTPUT); pinMode(PB13, OUTPUT);
 pinMode(PB14, OUTPUT); pinMode(PB15, OUTPUT);//Touch 
 pinMode(frame, OUTPUT); //Frame 56.25Hz
 pinMode(latch,  OUTPUT); //latch  18kHz
 pinMode(clock,    OUTPUT); //clock 1.44 Mhz
*/
 wipe();
 for (unsigned int j=9600; j< 19120; j++) {
  //for (int i=0; i< 80; i++) {lcd[j][i]= j;}
 //bufr[j]= random(256); //j%256;
 bufr[j]= j/256;
 }
 //delay(1000); 
 
wipe();

Move(0,200);
Bcdout(2022); Cout(32);
Print("Boda Research");
drawLine(200,200,639,239,1);
hLine (220,20,400,1);

vLine (320,20,200,1);
Move(320,140);

Print("<This point 320,140");//Bcdout(PB12);Bcdout(PB13);
Rect(250,250,100,40,1);
textColor(1);
Move(253,253);Print("Reverse Color");
drawTriangle(170,170,184,132,216,196,1);
drawCircle(300,300,20,1); 

//================= Draw clock 
   drawCircle(62, 62, 59,  1);
   drawCircle(62, 62, 55,  1);

  // Draw 12 lines
  for(int i = 0; i<360; i+= 30) {
    sx = cos((i-90)*0.0174532925);
    sy = sin((i-90)*0.0174532925);
    xx0 = sx*57+62;
    yy0 = sy*57+62;
    xx1 = sx*50+62;
    yy1 = sy*50+62;

     drawLine(xx0, yy0, xx1, yy1, 1);
  }

  // Draw 60 dots
  for(int i = 0; i<360; i+= 6) {
    sx = cos((i-90)*0.0174532925);
    sy = sin((i-90)*0.0174532925);
    xx0 = sx*51+62;
    yy0 = sy*51+62;
    // Draw minute markers
     drawPixel(xx0, yy0, 1);
    // Draw main quadrant dots
    if(i==0 || i==180)  fillCircle(xx0, yy0, 2, 1);
    if(i==90 || i==270)  fillCircle(xx0, yy0, 2, 1);
  }
  //============== clock end ================ 
  textColor(0);
  bufr[19120]= 0xaa;
 stdby(); intrOn=0; //touch
}

void handleTouch(void){
  detachInterrupt(7); intrOn=0;
  //read x
 touchX=coordX(); 
 //read y
 touchY=coordY();
  stdby();
 }

void loop() 
//========================== Main ============================== 
{ static int klok;
  static long timer;
  
 if (digitalRead(PA7) ==1 && !intrOn ) {
  Move(10,160); Print("PA6=");  Bcdout(touchX);
  Move(100,160);Print("PA7="); Bcdout(touchY);
  Move(10,180);Print("Sec ");  Bcdout(ss);
  attachInterrupt(7, handleTouch, FALLING); intrOn=1;
  }
 
 if (millis() > timer) {
    long t=millis(); 
    klok++; Move(253,273); textColor(1); Bcdout(klok); textColor(0); timer=millis()+1000; Pointer();
   }
  
  else {picture();}
  }
  
  void  stdby(void){ 
    //============= Touch stdby =============== 
    byte d[]={0,1,1,1};
    for (byte i=0; i<4; i++) digitalWrite(28+i, d[i]);
    pinMode(PA7,INPUT_PULLUP); //  pullup
  } 
  
  int coordX(void){ 
    //============= Touch read X =============== 
    byte d[]={0,0,1,1}; pinMode(PA7,INPUT_ANALOG);
    for (byte i=0; i<4; i++) digitalWrite(28+i, d[i]);
    unsigned long sum=0;
    for (byte j=0; j<50; j++) {sum+= analogRead(PA7); //delay(1);
    }
    return(sum/50);
  }   
  
  int coordY(void){ 
    //============= Touch read Y =============== 
    byte d[]={1,1,0,0}; pinMode(PA6,INPUT_ANALOG);
    for (byte i=0; i<4; i++) digitalWrite(28+i, d[i]);
    unsigned long sum=0;
    for (byte j=0; j<50; j++) {sum+= analogRead(PA6); //delay(1);
    }
    return(sum/50);
  } 
//========================== Main end ============================== 
  void picture(void) {  
    
   digitalWrite(frame, HIGH);//    
   clock1(); 
   digitalWrite(frame, LOW); //
    
    for (int j=0; j< 240; j++) {    
    for (int i=0; i< 80; i++) {
        
      GPIOB->regs->ODR &= ~(0x00FF);  //clear bits PB0 - PB7
      GPIOB->regs->ODR |= (bufr[j*80+i])  ; //set output bits PB0 - PB7
      clockhigh;clocklow;
    } 
      clock1(); 
    } //240 lines done
    }

    
  void clock1(void) {
    linehigh; linelow;
    }
  
  
#define swap(a, b) { int16_t t = a; a = b; b = t; }

void drawPixel(int x, int y, int color) {
  x = x%640; y=y%240;
if (color) bufr[x/8+y*80] |= (0x80>>(x&7));  
else bufr[x/8+y*80] &= ~(0x80>>(x&7)); 
}

void hLine(int x, int y, int w, byte color) {
 while (w--)  drawPixel(x+w,y,1);
    }

void vLine(int x, int y, int h, byte color) {
 while (h--)  drawPixel(x,y+h,1);
    }

void Rect(int x, int y, int w, int h, byte color)
  {
   while (h--)  hLine(x,y+h,w,1);
  }

void wipe(void) {
  //for (int h=0; h<=255; h++ ) bufr[h]=0;
  memset(bufr, 0, 19200);
  }
  
  
void Move(int x, int y){
   x = x%640; y=y%240;
  pixelX=x; pixelY=y; //0x80>>(x&7)
  }

void textColor (byte col) 
{ fCol=col; 
  }  

// Draw a circle outline
void drawCircle(int x0, int y0, int r, unsigned int color) {

  int f = 1 - r, ddF_x = 1, ddF_y = -2 * r, x = 0, y = r;
  drawPixel(x0  , y0+r, color);
  drawPixel(x0  , y0-r, color);
  drawPixel(x0+r, y0  , color);
  drawPixel(x0-r, y0  , color);

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
  
    drawPixel(x0 + x, y0 + y, color);
    drawPixel(x0 - x, y0 + y, color);
    drawPixel(x0 + x, y0 - y, color);
    drawPixel(x0 - x, y0 - y, color);
    drawPixel(x0 + y, y0 + x, color);
    drawPixel(x0 - y, y0 + x, color);
    drawPixel(x0 + y, y0 - x, color);
    drawPixel(x0 - y, y0 - x, color);
  }   
}

void fillCircle(int x0, int y0, int r, byte color) {
  while (r) {drawCircle(x0,y0,r--,1); }
}

void drawLine(int x0, int y0,
          int x1, int y1,
          unsigned int color) {
        int dx, dy , err,  ystep;
  int steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {swap(x0, y0);swap(x1, y1);}
 if (x0 > x1) {swap(x0, x1);swap(y0, y1);}
  dx = x1 - x0;dy = abs(y1 - y0); err = dx / 2;
  if (y0 < y1) {ystep = 1;} 
  else {ystep = -1;}

  for (; x0<=x1; x0++) {
    if (steep) {
      drawPixel(y0, x0, color);
    } else {
      drawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {y0 += ystep;err += dx;}
  }
}

// Draw a rounded rectangle
void drawRoundRect(int x, int y, int w,
  int h, int r, unsigned int color) {
  // smarter version
  hLine(x+r  , y    , w-2*r, color); // Top
  hLine(x+r  , y+h-1, w-2*r, color); // Bottom
  vLine(x    , y+r  , h-2*r, color); // Left
  vLine(x+w-1, y+r  , h-2*r, color); // Right
  // draw four corners
  drawCircleHelper(x+r    , y+r    , r, 1, color);
  drawCircleHelper(x+w-r-1, y+r    , r, 2, color);
  drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
  drawCircleHelper(x+r    , y+h-r-1, r, 8, color);
}

void fillRect(int x, int y, int w, int h,
          unsigned int color) {
 int i;
  for ( i=x; i<x+w; i++) {
    vLine(i, y, h, color);
  }
}
// Fill a rounded rectangle
void fillRoundRect(int x, int y, int w,
         int h, int r, unsigned int color) {
  // smarter version
  fillRect(x+r, y, w-2*r, h, color);
// draw four corners
  fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
  fillCircleHelper(x+r    , y+r, r, 2, h-2*r-1, color);
}

void drawCircleHelper( int x0, int y0, int r, char cornername, unsigned int color) {
  int f     = 1 - r;
  int ddF_x = 1;
  int ddF_y = -2 * r;
  int x     = 0;
  int y     = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;
    if (cornername & 0x4) {
      drawPixel(x0 + x, y0 + y, color);
      drawPixel(x0 + y, y0 + x, color);
    } 
    if (cornername & 0x2) {
      drawPixel(x0 + x, y0 - y, color);
      drawPixel(x0 + y, y0 - x, color);
    }
    if (cornername & 0x8) {
      drawPixel(x0 - y, y0 + x, color);
      drawPixel(x0 - x, y0 + y, color);
    }
    if (cornername & 0x1) {
      drawPixel(x0 - y, y0 - x, color);
      drawPixel(x0 - x, y0 - y, color);
    }
  }
}

// Used to do circles and roundrects
void fillCircleHelper(int x0, int y0, int r,
    char cornername, int delta, unsigned int color) {

  int f     = 1 - r;
  int ddF_x = 1;
  int ddF_y = -2 * r;
  int x     = 0;
  int y     = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;

    if (cornername & 0x1) {
      vLine(x0+x, y0-y, 2*y+1+delta, color);
      vLine(x0+y, y0-x, 2*x+1+delta, color);
    }
    if (cornername & 0x2) {
      vLine(x0-x, y0-y, 2*y+1+delta, color);
      vLine(x0-y, y0-x, 2*x+1+delta, color);
    }
  }
}
// Draw a rectangle
void drawRect(int x, int y,
          int w, int h,
          unsigned int color) {
  hLine(x, y, w, color);
  hLine(x, y+h-1, w, color);
  vLine(x, y, h, color);
  vLine(x+w-1, y, h, color);
}

// Draw a triangle
void drawTriangle(int x0, int y0,
        int x1, int y1,
        int x2, int y2, unsigned int color) {
  drawLine(x0, y0, x1, y1, color);
  drawLine(x1, y1, x2, y2, color);
  drawLine(x2, y2, x0, y0, color);
}

// Fill a triangle
void fillTriangle ( int x0, int y0,
                    int x1, int y1,
                    int x2, int y2, unsigned int color) {

  int a, b, y, last;
  int dx01,dy01,dx02,dy02,dx12,dy12; 
  long sa,sb;
  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (y0 > y1) {swap(y0, y1); swap(x0, x1);}
  if (y1 > y2) {swap(y2, y1); swap(x2, x1);}
  if (y0 > y1) {swap(y0, y1); swap(x0, x1);}

  if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
    a = b = x0;
    if(x1 < a)      a = x1;
    else if(x1 > b) b = x1;
    if(x2 < a)      a = x2;
    else if(x2 > b) b = x2;
    hLine(a, y0, b-a+1, color);
    return;
  }
 dx01 = x1 - x0,dy01 = y1 - y0,dx02 = x2 - x0,dy02 = y2 - y0,
 dx12 = x2 - x1,dy12 = y2 - y1;sa   = 0, sb   = 0;
 // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
  if(y1 == y2) last = y1;   // Include y1 scanline
  else         last = y1-1; // Skip it

  for(y=y0; y<=last; y++) {
    a   = x0 + sa / dy01;
    b   = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;
    // longhand: a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
    //b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
  if(a > b) swap(a,b);
    hLine(a, y, b-a+1, color);
  }

  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2.  This loop is skipped if y1=y2.
  sa = dx12 * (y - y1);
  sb = dx02 * (y - y0);
  for(; y<=y2; y++) {
    a   = x1 + sa / dy12;
    b   = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;
    // longhand: a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
    //  b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    if(a > b) swap(a,b);
    hLine(a, y, b-a+1, color);
  }
}

 const byte Chartable[] = //5 pixels horizontal,byte vertical
{ 0x00 , 0x00 , 0x00 , 0x00 , 0x00, // Sp
0x00 , 0x00 , 0x2F , 0x00 , 0x00, // !
0x00 , 0x07 , 0x00 , 0x07 , 0x00, // ""
0x14 , 0x7F , 0x14 , 0x7F , 0x14, // #
//0x24 , 0x2A , 0x7F , 0x2A , 0x12, // $
0x7f , 0x7f , 0x7F , 0x7f , 0x7f, // $:full square
0x23 , 0x13 , 0x08 , 0x64 , 0x62, // % 
0x36 , 0x49 , 0x55 , 0x22 , 0x50, // &
0x00 , 0x05 , 0x03 , 0x00 , 0x00, // '
0x00 , 0x1C , 0x22 , 0x41 , 0x00, // (
0x00 , 0x41 , 0x22 , 0x1C , 0x00, // )
0x14 , 0x08 , 0x3E , 0x08 , 0x14, // *
0x08 , 0x08 , 0x3E , 0x08 , 0x08, // +
0x00 , 0x00 , 0x50 , 0x30 , 0x00, // ,
0x10 , 0x10 , 0x10 , 0x10 , 0x10, // -
0x00 , 0x60 , 0x60 , 0x00 , 0x00, // .
0x20 , 0x10 , 0x08 , 0x04 , 0x02, // /
0x3E , 0x51 , 0x49 , 0x45 , 0x3E, // 0
0x00 , 0x42 , 0x7F , 0x40 , 0x00, // 1
0x42 , 0x61 , 0x51 , 0x49 , 0x46, // 2
0x21 , 0x41 , 0x45 , 0x4B , 0x31, // 3
0x18 , 0x14 , 0x12 , 0x7F , 0x10, // 4
0x27 , 0x45 , 0x45 , 0x45 , 0x39, // 5
0x3C , 0x4A , 0x49 , 0x49 , 0x30, // 6
0x01 , 0x71 , 0x09 , 0x05 , 0x03, // 7
0x36 , 0x49 , 0x49 , 0x49 , 0x36, // 8
0x06 , 0x49 , 0x49 , 0x29 , 0x1E, // 9
0x00 , 0x36 , 0x36 , 0x00 , 0x00, // :
0x00 , 0x56 , 0x36 , 0x00 , 0x00, // ;
0x08 , 0x14 , 0x22 , 0x41 , 0x00, // <
0x14 , 0x14 , 0x14 , 0x14 , 0x14, // =
0x00 , 0x41 , 0x22 , 0x14 , 0x08, // >
0x02 , 0x01 , 0x51 , 0x09 , 0x06, // ???
0x32 , 0x49 , 0x59 , 0x51 , 0x3E, // @
0x7E , 0x11 , 0x11 , 0x11 , 0x7E, // A UpperCase
0x7F , 0x49 , 0x49 , 0x49 , 0x36, // B
0x3E , 0x41 , 0x41 , 0x41 , 0x22, // C
0x7F , 0x41 , 0x41 , 0x22 , 0x1C, // D
0x7F , 0x49 , 0x49 , 0x49 , 0x41, // E
0x7F , 0x09 , 0x09 , 0x09 , 0x01, // F
0x3E , 0x41 , 0x49 , 0x49 , 0x7A, // G
0x7F , 0x08 , 0x08 , 0x08 , 0x7F, // H
0x00 , 0x41 , 0x7F , 0x41 , 0x00, // I
0x20 , 0x40 , 0x41 , 0x3F , 0x01, // J
0x7F , 0x08 , 0x14 , 0x22 , 0x41, // K
0x7F , 0x40 , 0x40 , 0x40 , 0x40, // L
0x7F , 0x02 , 0x0C , 0x02 , 0x7F, // M
0x7F , 0x04 , 0x08 , 0x10 , 0x7F, // N
0x3E , 0x41 , 0x41 , 0x41 , 0x3E, // O
0x7F , 0x09 , 0x09 , 0x09 , 0x06, // P
0x3E , 0x41 , 0x51 , 0x21 , 0x5E, // Q
0x7F , 0x09 , 0x19 , 0x29 , 0x46, // R
0x46 , 0x49 , 0x49 , 0x49 , 0x31, // S
0x01 , 0x01 , 0x7F , 0x01 , 0x01, // T
0x3F , 0x40 , 0x40 , 0x40 , 0x3F, // U
0x1F , 0x20 , 0x40 , 0x20 , 0x1F, // V
0x3F , 0x40 , 0x38 , 0x40 , 0x3F, // W
0x63 , 0x14 , 0x08 , 0x14 , 0x63, // X
0x07 , 0x08 , 0x70 , 0x08 , 0x07, // Y
0x61 , 0x51 , 0x49 , 0x45 , 0x43, // Z
0x00 , 0x7F , 0x41 , 0x41 , 0x00, // [
0x02 , 0x04 , 0x08 , 0x10 , 0x20, // Bksp
0x00 , 0x41 , 0x41 , 0x7F , 0x00, // ]
0x11 , 0x39 , 0x55 , 0x11 , 0x1F, // Lf
0x40 , 0x40 , 0x40 , 0x40 , 0x40, // _
0x10 , 0x38 , 0x54 , 0x10 , 0x1F, // Cr
0x20 , 0x54 , 0x54 , 0x54 , 0x78, // A LowerCase
0x7F , 0x48 , 0x44 , 0x44 , 0x38, // B
0x38 , 0x44 , 0x44 , 0x44 , 0x20, // C
0x38 , 0x44 , 0x44 , 0x48 , 0x7F, // D
0x38 , 0x54 , 0x54 , 0x54 , 0x18, // E
0x08 , 0x7E , 0x09 , 0x01 , 0x02, // F
0x0C , 0x52 , 0x52 , 0x52 , 0x3E, // G
0x7F , 0x08 , 0x04 , 0x04 , 0x78, // H
0x00 , 0x44 , 0x7D , 0x40 , 0x00, // I
0x20 , 0x40 , 0x44 , 0x3D , 0x00, // J
0x7F , 0x10 , 0x28 , 0x44 , 0x00, // K
0x00 , 0x41 , 0x7F , 0x40 , 0x00, // L
0x7C , 0x04 , 0x18 , 0x04 , 0x78, // M
0x7C , 0x08 , 0x04 , 0x04 , 0x78, // N
0x38 , 0x44 , 0x44 , 0x44 , 0x38, // O
0x7C , 0x14 , 0x14 , 0x14 , 0x08, // P
0x08 , 0x14 , 0x14 , 0x18 , 0x7C, // Q
0x7C , 0x08 , 0x04 , 0x04 , 0x08, // R
0x48 , 0x54 , 0x54 , 0x54 , 0x20, // S
0x04 , 0x3F , 0x44 , 0x40 , 0x20, // T
0x3C , 0x40 , 0x40 , 0x20 , 0x7C, // U
0x1C , 0x20 , 0x40 , 0x20 , 0x1C, // V
0x3C , 0x40 , 0x30 , 0x40 , 0x3C, // W
0x44 , 0x28 , 0x10 , 0x28 , 0x44, // X
0x0C , 0x50 , 0x50 , 0x50 , 0x3C, // Y
0x44 , 0x64 , 0x54 , 0x4C , 0x44, // Z
0x11 , 0x39 , 0x55 , 0x11 , 0x1F, // Lf
0x10 , 0x38 , 0x54 , 0x10 , 0x1F, // Cr
0x20 , 0x54 , 0x57 , 0x54 , 0x78, // A'
0x38 , 0x54 , 0x57 , 0x54 , 0x18, // E'
0x00 , 0x48 , 0x7B , 0x40 , 0x00, // I'
0x38 , 0x44 , 0x47 , 0x44 , 0x38, // O'
0x38 , 0x47 , 0x44 , 0x47 , 0x38, // O''
0x38 , 0x45 , 0x44 , 0x45 , 0x38, // O:
0x3C , 0x40 , 0x47 , 0x20 , 0x7C, // U'
0x3C , 0x47 , 0x40 , 0x27 , 0x7C, // U''
0x3C , 0x41 , 0x40 , 0x21 , 0x7C }; // U:

void Cout(int Byte)
{ unsigned int t5;
Byte -= 32 ; Byte *= 5 ;  //character 5 bytes, first byte
for (byte t4 = 0; t4 < 5; t4++) { //5 pixels horizontal, table byte vertical
  t5= Chartable[Byte+t4];
  byte mask=0x01; 
for (byte j=0; j<8; j++) {
  if (!fCol) drawPixel(pixelX+t4,pixelY+j,t5&mask); else drawPixel(pixelX+t4,pixelY+j,!(t5&mask));  
  mask=mask*2;
  }
}
 pixelX+=6;
}
  
void Print(const char *txt){
while (*txt) Cout(*txt++);
} 

void Bcdout(unsigned int bcd)
{ unsigned int t5;
if (bcd>9999) bcd=9999;
for (t5=1000;t5>0;t5/=10) {Cout(0x30+bcd/t5); bcd%=t5;}
} 

void Sgnout( int bcd)
{ unsigned t5;
if (bcd<0) {Cout('-'); bcd=abs(bcd);} else Cout(32);
for (t5=1000;t5>0;t5/=10) {Cout(0x30+bcd/t5); bcd%=t5;}
} 

void Bcd2( char bcd) {
if (bcd>9) Cout ((bcd/10) + 0x30); else Cout(32);
Cout ((bcd%10)+0x30);
}

void Hexout (unsigned char key){
unsigned char n;
n=(key/0x10)+0x30; if (n>0x39) n+=7; Cout(n); //convert to hex char
n=(key%0x10)+0x30; if (n>0x39) n+=7; Cout(n);
} 

void dly(void) {
  __asm__("nop\n\t"); 
  }
  
/* 
float sx = 0, sy = 1, mx = 1, my = 0, hx = -1, hy = 0;    // Saved H, M, S x & y multipliers
float sdeg=0, mdeg=0, hdeg=0;
uint16_t osx=62, osy=62, omx=62, omy=62, ohx=62, ohy=62;  // Saved H, M, S x & y coords
int xx0=0, xx1=0, yy0=0, yy1=0;
uint32_t targetTime = 0;                    // for next 1 second timeout

boolean initial = 1;

uint8_t conv2d(const char* p) {
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9')
    v = *p - '0';
  return 10 * v + *++p - '0';
}
uint8_t hh=conv2d(__TIME__), mm=conv2d(__TIME__+3), ss=conv2d(__TIME__+6);  // Get H, M, S from compile time

void setup() { 
 begin(); 
delay(100);
//Serial.begin(9600); int i=0; while (i<99) Serial.Println(i++);   
 setRotation(1);
 fillScreen( 1);
  
   setTextColor(WHITE,  1);  // Adding a background colour erases previous text automatically
  
  // Draw clock face
   fillCircle(62, 62, 59,  1);
   fillCircle(62, 62, 55,  1);

  // Draw 12 lines
  for(int i = 0; i<360; i+= 30) {
    sx = cos((i-90)*0.0174532925);
    sy = sin((i-90)*0.0174532925);
    xx0 = sx*57+62;
    yy0 = sy*57+62;
    xx1 = sx*50+62;
    yy1 = sy*50+62;

     drawLine(xx0, yy0, xx1, yy1, YELLOW);
  }

  // Draw 60 dots
  for(int i = 0; i<360; i+= 6) {
    sx = cos((i-90)*0.0174532925);
    sy = sin((i-90)*0.0174532925);
    xx0 = sx*51+62;
    yy0 = sy*51+62;
    // Draw minute markers
     drawPixel(xx0, yy0, WHITE);
    
    // Draw main quadrant dots
    if(i==0 || i==180)  fillCircle(xx0, yy0, 2, WHITE);
    if(i==90 || i==270)  fillCircle(xx0, yy0, 2, WHITE);
  }

  // fillCircle(60, 62, 3, WHITE);

  targetTime = millis() + 1000; 
}


void loop(void){

*/
void Pointer(void) {
static int i; 
static int targetTime;

  //if (targetTime < millis()) {
    //targetTime = millis()+1000;
    
    ss++;              // Advance second
    if (ss==60) {
      ss=0;
      mm++;            // Advance minute
      if(mm>59) {
        mm=0;
        hh++;          // Advance hour
        if (hh>23) {
          hh=0;
        }
      }
    }

    // Pre-compute hand degrees, x & y coords for a fast screen update
    sdeg = ss*6;                  // 0-59 -> 0-354
    mdeg = mm*6+sdeg*0.01666667;  // 0-59 -> 0-360 - includes seconds
    hdeg = hh*30+mdeg*0.0833333;  // 0-11 -> 0-360 - includes minutes and seconds
    hx = cos((hdeg-90)*0.0174532925);    
    hy = sin((hdeg-90)*0.0174532925);
    mx = cos((mdeg-90)*0.0174532925);    
    my = sin((mdeg-90)*0.0174532925);
    sx = cos((sdeg-90)*0.0174532925);    
    sy = sin((sdeg-90)*0.0174532925);

    if (ss==0 || initial) {
      initial = 0;
      // Erase hour and minute hand positions every minute
       drawLine(ohx, ohy, 62, 62,  0);
      ohx = hx*31+62;    
      ohy = hy*31+62;
       drawLine(omx, omy, 62, 62,  0);
      omx = mx*42+62;    
      omy = my*42+62;
    }

      // Redraw new hand positions, hour and minute hands not erased here to avoid flicker
       drawLine(osx, osy, 62, 62,  0);
      osx = sx*45+62;    
      osy = sy*45+62;
       drawLine(osx, osy, 62, 62, 1);
       drawLine(ohx, ohy, 62, 62, 1);
       drawLine(omx, omy, 62, 62, 1);
       drawLine(osx, osy, 62, 62, 1);
       drawCircle(62, 62, 3, 1);
     
       Move(129,20);  Print("TIME");
       Move(126,30);  Print("FLIES");
       Move(132,50);  Bcdout(hh); Print(":h");
       Move(132,60);  Bcdout(mm); Print(":m");
       Move(132,70);  Bcdout(ss); Print(":s");
  
}
