#if !defined(__PM_H__)

#define __PM_H__

#ifdef __cplusplus
extern "C" {
#endif

#define DF_READABLE     0x0002
#define DF_WRITEABLE    0x0002
#define DF_CONFORMING   0x0004
#define DF_EXPANDOWN    0x0004
#define DF_STACK        (DF_EXPANDOWN|DF_WRITEABLE)
#define DF_EXECUTABLE   0x0008
#define DF_SEGMENT      0x0010
#define DF_DPL0         0x0000
#define DF_DPL1         0x0020
#define DF_DPL2         0x0040
#define DF_DPL3         0x0060
#define DF_PRESENT      0x0080
#define DF_OSAVAILABLE  0x0100
#define DF_PAGEGRAN     0x8000
#define DF_USE32        0x4000

#define DF_DATA         DF_SEGMENT
#define DF_CODE         DF_SEGMENT
#define DF_SYSTEM       0x0000
#define DF_2AVAILTSS    0x0001
#define DF_LDT          0x0002
#define DF_2BUSYTSS     0x0003
#define DF_2CALLGATE    0x0004
#define DF_TASKGATE     0x0005
#define DF_2INTGATE     0x0006
#define DF_2TRAPGATE    0x0007
#define DF_3AVAILTSS    0x0009
#define DF_3BUSYTSS     0x000B
#define DF_3CALLGATE    0x000C
#define DF_3INTGATE     0x000E
#define DF_3TRAPGATE    0x000F

typedef unsigned long ulong;
typedef unsigned short ushort;
typedef unsigned char uchar;

#define HIBYTE(x)      (*(((uchar *)&(x))+1))
#define LOBYTE(x)      ((uchar)(x))
#define LOWORD(x)      ((ushort)(x))
#define HIWORD(x)      (*(((ushort *)&(x))+1))

typedef union DiffFarPtr {
   void  _es *v;
   uchar _es *b;
   ushort _es *w;
   ulong _es *d;
} DiffFarPtr;

typedef struct GdtEntry {
   ulong u[2];
} GdtEntry;

typedef struct IdtEntry {
   ulong u[2];
} IdtEntry;

typedef struct DTR {
   ushort limit;
   ulong  base;
} DTR;

extern GdtEntry far GdtTable[];
extern ushort far GdtMaxIndex;
extern IdtEntry far IdtTable[];
extern ushort far IdtMaxIndex;

int make386dscr(ushort Sel, ushort index, ulong base, ulong limit, ushort flags);
int make386gate(ushort TSel, ushort index, ushort Sel, ulong Off, uchar Type, uchar PCnt);
void outstr(char *s);
void outchr(int ch);
int scroll(void);
void pmprintf(char *fmt, ...);
void SetInterrupts(int lev1, int lev2);
void setrealmode(void);

void PMMain(void);
void RMMain(void);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif // __PM_H__