#pragma inline
#include <dos.h>
#include "deasm.h"
#include "cmdname.h"
#include "pm.h"
#include "vkey.h"

typedef struct tagTableReg {
    ushort limit;
    ulong  base;
} TableReg;

typedef struct tagRegisters386 {
    ushort regGS;
    ushort _res1;
    ushort regFS;
    ushort _res2;
    ushort regES;
    ushort _res3;
    ushort regDS;
    ushort _res4;
    union {
        ulong regEDI;
        ushort regDI;
    };
    union {
        ulong regESI;
        ushort regSI;
    };
    union {
        ulong regEBP;
        ushort regBP;
    };
    ulong regUnd;
    union {
        ulong regEBX;
        ushort regBX;
        struct {
            uchar regBL;
            uchar regBH;
        };
    };
    union {
        ulong regEDX;
        ushort regDX;
        struct {
            uchar regDL;
            uchar regDH;
        };
    };
    union {
        ulong regECX;
        ushort regCX;
        struct {
            uchar regCL;
            uchar regCH;
        };
    };
    union {
        ulong regEAX;
        ushort regAX;
        struct {
            uchar regAL;
            uchar regAH;
        };
    };
    union {
        ulong regEIP;
        ushort regIP;
    };
    ushort regCS;
    ushort _res5;
    union {
        ulong regEFLAGS;
        ushort regFLAGS;
    };
    union {
        ulong regESP;
        ushort regSP;
    };
    ushort regSS;
    ushort _res6;

    TableReg regGDTR;
    TableReg regIDTR;
    ushort regLDTR;
    ushort regTR;
    union {
        ulong regCR0;
        ushort regMSW;
    };
    ulong regCR2;
    ulong regCR3;
    ulong regDR0;
    ulong regDR1;
    ulong regDR2;
    ulong regDR3;
    ulong regDR6;
    ulong regDR7;
    ulong regTR6;
    ulong regTR7;

} Registers386;

extern "C" {
    extern Registers386 R;
};

#define mdPrintAll      0x001F
#define mdPrintGen      0x0001
#define mdPrintTbl      0x0002
#define mdPrintCRx      0x0004
#define mdPrintDRx      0x0008
#define mdPrintTRx      0x0010
#define mdPrintWrd      0x0020

void PrintRegisters(int mode);
void FillCmdBuff(ushort Sel, ulong Off);
int SetPrefixes(ushort prefix);
void PrintCommand(Command& cmd, uchar clen);

uchar cmdbuf[12];

uchar savebuf[30];

ushort prnum;
ushort segovr;
ushort lock;
ushort opsize;
ushort adsize;
ushort repeat;

extern "C" {
   void cdecl far interface(void);
}

void cdecl far interface(void)
{
    Command cmd;
    uchar clen;
    Message msg;
    uchar ch;

    enable();
    prnum=segovr=lock=opsize=adsize=0;

    PrintRegisters(mdPrintAll);

    do {
        FillCmdBuff(R.regCS, R.regEIP+prnum);
        clen=decode(cmdbuf, &cmd);
        if(SetPrefixes(cmd.prefix)) {
            if(prnum<30) savebuf[prnum-1]=cmdbuf[0];
        }
        else {
            break;
        }
    } while(1);
    PrintCommand(cmd, clen);

    outstr("\n\r-");

    while(1) {
        GetMessage(&msg);
        TranslateMessage(&msg);
        if(msg.type==MSG_CHAR) {
            ch=LOBYTE(msg.wParam);
            outchr(ch);
            outstr("\n\r");
            switch(ch) {
                case 'T':
                case 't':
                   return;
                case 'Q':
                case 'q':
                   setrealmode();
                   return;
                case 'R':
                case 'r':
                   PrintRegisters(mdPrintAll);
                   PrintCommand(cmd, clen);
                   break;
                default:
                   outstr("Invalid command\n\r-");
                   break;
            }
        }
    }
}

void PrintCommand(Command& cmd, uchar clen)
{
    int i;
    int count=16;

    pmprintf("%X:%lX ", R.regCS, R.regEIP);
    for(i=0; i<prnum && count>0; i++, count--) {
        pmprintf("%cX",savebuf[i]);
    }
    for(i=0; i<cmd.length && count>0; i++, count--) {
        pmprintf("%cX", cmdbuf[i]);
    }
    if(count==0 && i!=cmd.length)
        pmprintf("+ ");
    else {
        while(count--) pmprintf("  ");
        pmprintf("  ");
    }
    if(lock) pmprintf("LOCK ");
    if(repeat==CMD_REP) pmprintf("REP ");
    else if(repeat==CMD_REPZ) pmprintf("REPZ ");
    pmprintf("%s     \n\r", cmd.name);
    for(i=0; i<cmd.n; i++) {
        switch(cmd.op[i].type) {
            case TYPE_REG:
            case TYPE_IMM:
            case TYPE_REL:
            case TYPE_PTR:
            case TYPE_MEM:
            case TYPE_IMM1:
            case TYPE_IMM3:
            case TYPE_IMMVAL:
            case TYPE_TRx:
            case TYPE_DRx:
            case TYPE_CRx:
            case TYPE_STREG:
                break;
        }
    }
}

int SetPrefixes(ushort prefix)
{
    if(prefix) {
         prnum++;
         switch(prefix) {
             case CMD_SEGES:
             case CMD_SEGDS:
             case CMD_SEGFS:
             case CMD_SEGGS:
             case CMD_SEGSS:
             case CMD_SEGCS:
                 segovr=prefix;
                 break;
             case CMD_LOCK:
                 lock=1;
                 break;
             case CMD_OPSIZE:
                 opsize=1;
                 break;
             case CMD_ADDRSIZE:
                 adsize=1;
                 break;
             case CMD_REP:
             case CMD_REPZ:
                 repeat=prefix;
                 break;
         }
         return 1;
    }
    return 0;
}

void FillCmdBuff(ushort Sel, ulong Off)
{
    asm lsl     eax,Sel
    asm sub     eax,Off
    asm cmp     eax,12
    asm jae     LimitOk
    asm mov     ecx,eax
    asm jmp     LimitLow
LimitOk:
    asm mov     ecx,12
LimitLow:
    asm mov     fs,Sel
    asm mov     esi,Off
    asm mov     ax,ds
    asm mov     es,ax
    asm mov     edi,offset cmdbuf
    asm cld
    asm rep movs byte ptr es:[edi],byte ptr fs:[esi]
}

void PrintGeneralWord(void)
{
    pmprintf("AX=%X BX=%X CX=%X DX=%X BP=%X SI=%X DI=%X\n\r"
           "DS=%X ES=%X FS=%X GS=%X FLAGS=%X\n\r"
           "SS:SP=%X:%X CS:IP=%X:%X\n\n\r",
           R.regAX,R.regBX,R.regCX,R.regDX,R.regBP,R.regSI,R.regDI,
           R.regDS,R.regES,R.regFS,R.regGS,R.regFLAGS,
           R.regSS,R.regSP,R.regCS,R.regIP);
}

void PrintGeneralDword(void)
{
    pmprintf("EAX=%lX EBX=%lX ECX=%lX EDX=%lX EBP=%lX\n\r"
           "ESI=%lX EDI=%lX\n\r"
           "DS=%X ES=%X FS=%X GS=%X EFLAGS=%lX\n\r"
           "SS:ESP=%X:%lX CS:EIP=%X:%lX\n\n\r",
           R.regEAX,R.regEBX,R.regECX,R.regEDX,R.regEBP,R.regESI,R.regEDI,
           R.regDS,R.regES,R.regFS,R.regGS,R.regEFLAGS,
           R.regSS,R.regESP,R.regCS,R.regEIP);
}

void PrintTableRegs(void)
{
    pmprintf("GDTR: base=%lX limit=%X  IDTR: base=%lX limit=%X\n\r"
           "LDTR=%X  Task Register=%X  MSW=%X\n\n\r",
           R.regGDTR.base, R.regGDTR.limit, R.regIDTR.base, R.regIDTR.limit,
           R.regLDTR, R.regTR, R.regMSW);
}

void PrintRegisters(int mode)
{
    if(mode&mdPrintGen) {
        if(mode&mdPrintWrd) {
             PrintGeneralWord();
        }
        else {
             PrintGeneralDword();
        }
    }
    if(mode&mdPrintTbl) {
        PrintTableRegs();
    }
}