#ifdef __cplusplus

#define SCALE1  0x01
#define SCALE2  0x02
#define SCALE4  0x03
#define SCALE8  0x04

#define IMMS8   0x01
#define IMM16   0x02
#define IMM32   0x03
#define IMM8    0x04

#define REL8    0x01
#define REL16   0x02
#define REL32   0x03

#define PTR32   0x02
#define PTR48   0x03

#define DISP8   0x01
#define DISP16  0x02
#define DISP32  0x03

#define REG_AL          0x0000
#define REG_CL          0x0002
#define REG_DL          0x0004
#define REG_BL          0x0006
#define REG_AH          0x0008
#define REG_CH          0x000A
#define REG_DH          0x000C
#define REG_BH          0x000E

#define REG_AX          0x0010
#define REG_EAX         0x0011
#define REG_CX          0x0012
#define REG_ECX         0x0013
#define REG_DX          0x0014
#define REG_EDX         0x0015
#define REG_BX          0x0016
#define REG_EBX         0x0017
#define REG_SP          0x0018
#define REG_ESP         0x0019
#define REG_BP          0x001A
#define REG_EBP         0x001B
#define REG_SI          0x001C
#define REG_ESI         0x001D
#define REG_DI          0x001E
#define REG_EDI         0x001F

#define REG_ES          0x0020
#define REG_CS          0x0021
#define REG_SS          0x0022
#define REG_DS          0x0023
#define REG_FS          0x0024
#define REG_GS          0x0025

#define REG_CR0         0x0000
#define REG_CR1         0x0001
#define REG_CR2         0x0002
#define REG_CR3         0x0003
#define REG_CR4         0x0004
#define REG_CR5         0x0005
#define REG_CR6         0x0006
#define REG_CR7         0x0007

#define REG_DR0         0x0000
#define REG_DR1         0x0001
#define REG_DR2         0x0002
#define REG_DR3         0x0003
#define REG_DR4         0x0004
#define REG_DR5         0x0005
#define REG_DR6         0x0006
#define REG_DR7         0x0007

#define REG_TR0         0x0000
#define REG_TR1         0x0001
#define REG_TR2         0x0002
#define REG_TR3         0x0003
#define REG_TR4         0x0004
#define REG_TR5         0x0005
#define REG_TR6         0x0006
#define REG_TR7         0x0007

#define TYPE_MEM        0x0001
#define TYPE_REG        0x0002
#define TYPE_IMM        0x0003
#define TYPE_REL        0x0004
#define TYPE_PTR        0x0005
#define TYPE_IMM3       0x0006
#define TYPE_IMM1       0x0007
#define TYPE_TRx        0x0008
#define TYPE_DRx        0x0009
#define TYPE_CRx        0x000A
#define TYPE_IMMVAL     0x000B
#define TYPE_STREG      0x000C


typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;

struct Operand {
    uchar type;
    union {
        struct {
            uchar base;
            uchar index;
            uchar scale;
            uchar disp;
        } mem;
        uchar reg;
        uchar imm;
        uchar rel;
        uchar ptr;
    };
};

#define OPSZ_BYTE      0x0001
#define OPSZ_WORD      0x0002
#define OPSZ_DWORD     0x0004
#define OPSZ_FWORD     0x0006
#define OPSZ_QWORD     0x0008
#define OPSZ_TBYTEe    0x000A

struct Command {
    uchar *name;
    ushort length;
    ushort prefix;
    uchar n;
    uchar opsize;
    Operand op[3];
};

uchar decode(uchar *code, Command *cmd);

#endif

#ifdef __cplusplus
extern "C" {
#endif

    void far trace(void);
    void cdecl far interface(void);

#ifdef __cplusplus
}
#endif
