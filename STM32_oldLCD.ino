
#define frame PB7  //40Hz
#define Msig  PA12  //20Hz
#define load  PB8  //10kHz
#define cp    PB9  //980kHz
#define ena   PA11


#define cplow  GPIOB->regs->BRR  = 0x0200
#define cphigh GPIOB->regs->BSRR = 0x0200

byte lcd[9600]={0}; int mptr;

void setup() {
 disableDebugPorts();
 pinMode(PB3,   OUTPUT); pinMode(PB4, OUTPUT);
 pinMode(PB5,   OUTPUT); pinMode(PB6, OUTPUT); //D0-D3 LCD
 pinMode(frame, OUTPUT); //Frame 56.25Hz
 pinMode(load,  OUTPUT); //Load  18kHz
 pinMode(cp,    OUTPUT); //CP 1.44 Mhz
 pinMode(Msig,  OUTPUT); //Frame per 2
 pinMode(ena,   OUTPUT); //Display on / EL backlight on
 //for (int j=0; j< 120; j++) {for (int i=0; i< 40; i++) {lcd[i+j*40]= j;}}
for (int i=0; i<40; i++)
{Move (78-i*2,39-i); Hexout(i); Cout(32); Print("Boda Research Tolna 2017");
}
digitalWrite(ena, HIGH);
}

void loop() {
    static byte x;
    digitalWrite(frame, HIGH);//
    for (int j=0; j< 240; j++) {
       //digitalWrite(load, LOW);
    for (int i=0; i< 40; i++) {
      byte x=lcd[i+40*j];

      GPIOB->regs->ODR &= ~(0x0078);  //clear bits of PB3-6
      GPIOB->regs->ODR |= (x & 0xf0)>>1  ; //set output bits D0 - D3
        dly(); cplow; dly(); cphigh;
      if (i==39) digitalWrite(load, HIGH);
      GPIOB->regs->ODR &= ~(0x0078);  //clear bits
      GPIOB->regs->ODR |= (x & 0x0f)<<3  ; //set output bits D0 - D3
        dly(); cplow; dly(); cphigh;
    }  digitalWrite(load, LOW);// end of line, 40 bytes shifted out
       digitalWrite(frame, LOW); //only at first byte
    } //240 lines done
    digitalWrite(Msig, x); x= 1-x;
}

void I2cdata (byte dat){
   mptr = mptr%9600;;
  lcd[mptr]=dat;
  }

void Move(char Ypos,char Xpos)
{
mptr = Ypos*40 + Xpos;
mptr = mptr%9600;
}

void Clearscreen(void) {
for (int x = 0; x< 9600 ; x++) lcd[x]=0;
Move (0 , 0);
}

void Print(char *txt){
while (*txt) Cout(*txt++);
}

void Bcdout(unsigned int bcd)
{ unsigned t5;
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
const char Chartable[] =
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
0x3C , 0x41 , 0x40 , 0x21 , 0x7C}; // U:

void Cout(char Byte)
{ char T4 , T6; int T5;
for (T4 = 0; T4<= 4; T4++) {
T5 = Byte-32 ; T5 *= 5 ; T5 += T4;
T6 = Chartable [T5];
I2cdata (T6); mptr+=40; mptr = mptr%9600;
}
I2cdata (0); mptr+=40; mptr = mptr%9600;
}

void Coutinv(char Byte)
{ char T4 , T6; int T5;
for (T4 = 0; T4<= 4; T4++) {
T5 = Byte-32 ; T5 *= 5 ; T5 += T4;
T6 = ~Chartable [T5];
I2cdata (T6);
}
I2cdata (255);
}