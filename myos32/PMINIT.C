#pragma inline

#include <dos.h>
#include <process.h>
#include "deasm.h"
#include "vkey.h"
#include "pm.h"

void near protectionfault(unsigned rIP, unsigned rCS, unsigned rFLAGS);
void near invalidinstr(unsigned rCS, unsigned rFLAGS);

void TimerHandler(void);
void TimerInt(void);
void KeyboardInt(void);
 
int make386dscr(ushort Sel, ushort index, ulong base, ulong limit, ushort flags)
{
    DiffFarPtr table;
    uchar mask=0xD0;

    if(index>=8192) return 0;

    table.v=(void _es *)(index<<3);
    _ES=Sel;

    if(flags&DF_SEGMENT) {
    SetData:
        table.w[1]=LOWORD(base);
        table.b[4]=LOBYTE(HIWORD(base));
        table.b[7]=HIBYTE(HIWORD(base));
        table.w[0]=LOWORD(limit);
        table.b[6]=LOBYTE(HIWORD(limit))&0x0F;
        table.b[5]=LOBYTE(flags);
        table.b[6]|=HIBYTE(flags)&mask;
        return 1;
    }
    else {
        switch(flags&0x0F) {
            case DF_2CALLGATE:
            case DF_TASKGATE:
            case DF_2INTGATE:
            case DF_2TRAPGATE:
            case DF_3CALLGATE:
            case DF_3INTGATE:
            case DF_3TRAPGATE:
                return 0;
        }
        mask=0x80;
        goto SetData;
    }
}

int make386gate(ushort TSel, ushort index, ushort Sel, ulong Off, uchar Type, uchar PCnt)
{
    DiffFarPtr table;

    if(index>8192) return 0;
    table.v=(void _es *)(index<<3);
    _ES=TSel;
    if((Type&DF_SEGMENT)==0) {
        switch(Type&0x0F) {
            case DF_2CALLGATE:
            case DF_TASKGATE:
            case DF_2INTGATE:
            case DF_2TRAPGATE:
            case DF_3CALLGATE:
            case DF_3INTGATE:
            case DF_3TRAPGATE:
                break;
            default:
                return 0;
        }
        table.w[1]=Sel;
        table.w[0]=LOWORD(Off);
        table.w[3]=HIWORD(Off);
        table.b[5]=Type;
        table.b[4]=PCnt&0x1F;
        return 1;
    }
    return 0;
}

#define SEL_CODE   3<<3
#define SEL_DATA   4<<3
#define SEL_STACK  5<<3
#define SEL_VIDEO  6<<3

void pmproc(void);

ushort oldDS;
ushort oldCS;
ushort oldSS;
uchar  mask1;
uchar  mask2;
DTR oldIdtR;

void main()
{
    ushort GdtSel;
    ushort IdtSel;
    DTR IdtR, GdtR;
    int i;

    *(unsigned far *)MK_FP(0x40,0xA8)=_DS;
    oldSS=_SS;
    oldCS=_CS;

    GdtSel=FP_SEG(GdtTable)+(FP_OFF(GdtTable)>>4);
    IdtSel=FP_SEG(IdtTable)+(FP_OFF(IdtTable)>>4);

    GdtR.limit=((GdtMaxIndex+1)<<3)-1;
    GdtR.base=(ulong)GdtSel<<4;

    IdtR.limit=((IdtMaxIndex+1)<<3)-1;
    IdtR.base=(ulong)IdtSel<<4;

    make386dscr( GdtSel, 1, GdtR.base, GdtR.limit, DF_DATA|DF_WRITEABLE|DF_DPL0|DF_PRESENT);
    make386dscr( GdtSel, 2, IdtR.base, IdtR.limit, DF_DATA|DF_WRITEABLE|DF_DPL0|DF_PRESENT);
    make386dscr( GdtSel, 3, (long)_CS<<4, 0xFFFFu, DF_CODE|DF_EXECUTABLE|DF_READABLE|DF_DPL0|DF_PRESENT);
    make386dscr( GdtSel, 4, (long)_DS<<4, 0xFFFFu, DF_DATA|DF_WRITEABLE|DF_DPL0|DF_PRESENT);
    make386dscr( GdtSel, 5, (long)_SS<<4, 0u, DF_DATA|DF_STACK|DF_DPL0|DF_PRESENT);
    make386dscr( GdtSel, 6, 0xB800L<<4, 4095u, DF_DATA|DF_WRITEABLE|DF_PRESENT);
    make386dscr( GdtSel, 0x40>>3, 0x40L<<4, 0xFFFFu, DF_DATA|DF_WRITEABLE|DF_PRESENT);

    make386dscr( GdtSel, 7, ((long)FP_SEG(trace)<<4)+FP_OFF(trace),
                 0xFFFFu, DF_CODE|DF_READABLE|DF_EXECUTABLE|DF_DPL0|DF_PRESENT|DF_USE32);

    make386gate( IdtSel, 1, 7<<3, 0L, DF_3TRAPGATE|DF_PRESENT, 0);
    make386gate( IdtSel, 3, 7<<3, 0L, DF_3TRAPGATE|DF_PRESENT, 0);
    make386gate( IdtSel, 0x0D, 3<<3, (unsigned)protectionfault, DF_2TRAPGATE|DF_PRESENT, 0);
    make386gate( IdtSel, 0x06, 3<<3, (unsigned)invalidinstr, DF_2TRAPGATE|DF_PRESENT, 0);
    make386gate( IdtSel, 0x20, 3<<3, (unsigned)TimerInt, DF_2INTGATE|DF_PRESENT, 0);
    make386gate( IdtSel, 0x21, 3<<3, (unsigned)KeyboardInt, DF_2INTGATE|DF_PRESENT, 0);


    disable();

    mask1=inp(0x21);
    goto __1;
__1:
    mask2=inp(0xA1);

    SetInterrupts(0x20,0x28);
    
    asm mov al,0xFC
    asm out 0x21,al

    asm lgdt pword ptr GdtR
    asm sidt pword ptr oldIdtR
    asm lidt pword ptr IdtR
    asm smsw ax
    asm or   ax,1
    asm lmsw ax
    __emit__(0xEA);
    asm DW offset pmproc
    __emit__(SEL_CODE&0xFF, (SEL_CODE&0xFF00)>>8);
}

void setrealmode(void)
{
    asm mov ax,0x40
    asm mov ds,ax
    asm mov es,ax
    asm mov ss,ax
    asm mov fs,ax
    asm mov gs,ax

    asm mov eax,cr0
    asm and al,0xFE
    asm mov cr0,eax
    asm db  0xEA
    asm dw  offset realproc
    asm dw  seg realproc

    asm realproc label near
    asm mov  si,0xA8
    asm mov  ds,[si]
    asm mov  ss,oldSS
    asm lidt pword ptr oldIdtR

    SetInterrupts(0x08, 0x70);

    outp(0x21,mask1);
    outp(0xA1,mask2);

    enable();

    RMMain();

    exit(0);
}

void pmproc(void)
{
    _SS=SEL_STACK;
    _DS=SEL_DATA;
    _AX=SEL_DATA; 
    _ES=_AX;
    _FS=_AX;
    _GS=_AX;

    enable();

    PMMain();

    setrealmode();
}

static int x=0;
static int y=0;

int scroll(void)
{
    _ES=SEL_VIDEO;
    _AX=SEL_VIDEO;
    _GS=_AX;
    _SI=160;
    _DI=0;
    _CX=1920;
    asm cld
    asm seggs
    asm rep movsw
    _CX=80;
    _AX=0x0720;
    asm rep stosw
    return 1;
}

void outchr(int ch)
{
   switch(ch) {
       case '\n':
          if(y==24) scroll();
          else y++;
          break;
       case '\r':
          x=0;
          break;
       default:
          _ES=SEL_VIDEO;
          _DI=(y*80+x)*2;
          _AL=ch;
          _AH=0x07;
          asm cld
          asm stosw
          if(x==79) {
             x=0;
             (y==24) ? scroll() : y++ ;
          }
          else
             x++;
          break;
   }
}

void outstr(char *s)
{
    while(*s) outchr(*s++);
}

#define FMT_LONG    0x0002
#define FMT_SHORT   0x0004

void makehex(char *b, int f, long v)
{
    static char hextable[]="0123456789ABCDEF";
    int i;

    if(f&FMT_SHORT) {
        i=1;
    }
    else if(f&FMT_LONG) {
        i=7;
    }
    else {
        i=3;
    }
    b[i+1]='\0';
    for(; i>=0; i--) {
        b[i]=hextable[v&0x0F];
        v>>=4;
    }
}

void pmprintf(char *fmt, ...)
{
   char _ss *p;
   int flags=0;
   static char buf[9];

   asm mov  p,bp
   asm add  word ptr p, 6

   while(*fmt) {
       if(*fmt!='%') {
           if(flags&0x0001) {
               switch(*fmt) {
                   case 'l':
                       flags|=FMT_LONG;
                       fmt++;
                       break;
                   case 'c':
                       flags|=FMT_SHORT;
                       fmt++;
                       break;
                   case 'X':
                       if(flags&FMT_LONG) {
                           makehex(buf, flags, *(long _ss *)p);
                           p+=sizeof(long);
                       }
                       else {
                           makehex(buf, flags, *(int _ss *)p);
                           p+=sizeof(int);
                       }
                       outstr(buf);
                       flags=0;
                       fmt++;
                       break;
                   case 's':
                       fmt++;
                       flags=0;
                       outstr(*(char * _ss *)p);
                       p+=sizeof(char *);
                       break;
               }
           }
           else {
               outchr(*fmt++);
           }
       }
       else {
           if(flags&0x0001) {
               outchr(*fmt++);
               flags=0;
           }
           else {
               flags|=0x0001;
               fmt++;
           }
       }
   }
}

void near TimerHandler(void)
{
   asm mov al,0x20
   asm out 0x20,al
}

void near protectionfault(unsigned rIP, unsigned rCS, unsigned rFLAGS)
{
   pmprintf("\n\rGeneral protection fault GP# %X\n\r", *((&rIP)-1));
   pmprintf("CS:IP=%X:%X  FLAGS=%X\n\r", rCS, rIP, rFLAGS);
   setrealmode();
}
void near invalidinstr(unsigned rCS, unsigned rFLAGS)
{
   pmprintf("\n\rInvalid instruction encountered\n\r"
            "in address CS:IP=%X:%X  FLAGS=%X\n\r", rCS, *((&rCS)-1), rFLAGS);
   setrealmode();
}