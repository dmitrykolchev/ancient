#pragma inline

#include <dos.h>
#include <stdio.h>
#include "pm.h"
#include "vkey.h"

Message MsgQueue[256];
uchar   MsgHead=0;
uchar   MsgTail=0;

void PutMessage(ushort type, ushort wParam, ulong lParam)
{
    Message *msg;
    if( MsgTail+1 != MsgHead ) {
        msg=&MsgQueue[MsgTail++];
        msg->type=type;
        msg->wParam=wParam;
        msg->lParam=lParam;
    }
}

void GetMessage(Message* msg)
{
    while(MsgTail==MsgHead);
    *msg=MsgQueue[MsgHead++];
}

boolean PeekMessage(Message* msg, boolean remove)
{
    if(MsgTail==MsgHead)
        return false;
    *msg=MsgQueue[MsgHead];
    if(remove) MsgHead++;
    return true;
}

static uchar caps=0;

boolean TranslateMessage(Message* msg)
{
    static ushort kbdstate=0;
    static short altcode=-1;
    register ushort code, ascii=0;
    static uchar t1shift[]=")!@#$%^&*(~{}|:\"<>?_+\x68";
    static uchar t1free[]="0123456789`[]\\;\',./-=\t";
    static uchar t2gray[]="\x1B\x8\xD\xD\x20\0\0/*-+";
    static uchar t3numpad[]="0123456789.";

    code=msg->wParam;
    kbdstate&=0xFFF8;
    kbdstate|=caps;
    if(msg->type==MSG_KEYDOWN) {
        switch(code) {
            case VK_LSHIFT: kbdstate|=KF_LSHIFT; return false;
            case VK_RSHIFT: kbdstate|=KF_RSHIFT; return false;
            case VK_LCTRL:  kbdstate|=KF_LCTRL; return false;
            case VK_RCTRL:  kbdstate|=KF_RCTRL; return false;
            case VK_LALT:   kbdstate|=KF_LALT; return false;
            case VK_RALT:   kbdstate|=KF_RALT; return false;
            case VK_INSERT: kbdstate^=KF_INSERT; break;
            case VK_CAPSLOCK:
            case VK_NUMLOCK: 
            case VK_SCROLLLOCK: return false;
        }
    }
    else if(msg->type==MSG_KEYUP) {
        switch(code) {
            case VK_LSHIFT: kbdstate&=~KF_LSHIFT; return false;
            case VK_RSHIFT: kbdstate&=~KF_RSHIFT; return false;
            case VK_LCTRL:  kbdstate&=~KF_LCTRL; return false;
            case VK_RCTRL:  kbdstate&=~KF_RCTRL; return false;
            case VK_LALT:   kbdstate&=~KF_LALT; goto testAltCode;
            case VK_RALT:   kbdstate&=~KF_RALT;
            testAltCode:
                if(altcode!=-1) {
                    msg->type=MSG_CHAR;
                    msg->wParam=altcode;
                    msg->lParam=kbdstate;
                    altcode=-1;
                    return true;
                }
                return false;
            case VK_CAPSLOCK:
            case VK_NUMLOCK:
            case VK_SCROLLLOCK: return false;
            case VK_INSERT: break;
        }
    }
    else
        return false;

    if(code>=VK_A && code<=VK_Z) {
        if( kbdstate&KF_ALT ) {
            if(altcode!=-1 && msg->type==MSG_KEYDOWN) altcode=0;
        }
        else {
            if( kbdstate&KF_CTRL ) {
                ascii=code-64;
            }
            else {
                if( ((kbdstate&(KF_SHIFT|KF_CAPSLOCK)) == 0) ||
                    ( (kbdstate&KF_SHIFT) && (kbdstate&KF_CAPSLOCK)) ) {
                    ascii=code+32;
                }
                else {
                    ascii=code;
                }
            }
        }
    }
    else if(code>=VK_0 && code<=VK_TAB) {
        if((kbdstate&(KF_ALT|KF_CTRL))==0) {
            if(kbdstate&KF_SHIFT) {
                ascii=t1shift[code-VK_0];
            }
            else {
                ascii=t1free[code-VK_0];
            }
        }
        else if(kbdstate&KF_ALT) {
            if(altcode!=-1 && msg->type==MSG_KEYDOWN) altcode=0;
        }
    }
    else if(code>=VK_ESC && code<=VK_NUMPADPLUS) {
        ascii=t2gray[code-VK_ESC];
        if(altcode!=-1 && msg->type==MSG_KEYDOWN) altcode=0;
    }
    else if(code>=VK_NUMPAD0 && code<=VK_NUMPADPERIOD) {
        if((kbdstate&KF_ALT)!=0) {
            if(msg->type==MSG_KEYDOWN) {
                if(altcode==-1) altcode=0;
                altcode*=10;
                altcode+=code-VK_NUMPAD0;
                altcode&=0x00FF;
            }
        }
        else if((kbdstate&KF_CTRL)==0) {
            ascii=t3numpad[code-VK_NUMPAD0];
        }
    }
    msg->wParam<<=8;
    msg->wParam|=ascii;
    msg->lParam=kbdstate;
    if(ascii!=0) {
        if(msg->type==MSG_KEYUP) {
            msg->type=MSG_CHARDEAD;
        }
        else if(msg->type==MSG_KEYDOWN) {
            msg->type=MSG_CHAR;
        }
    }
    return true;
}

static uchar table[0x80]=
{
     0, VK_ESC, VK_1, VK_2, VK_3, VK_4, VK_5, VK_6, VK_7, VK_8, VK_9, VK_0, VK_MINUS, VK_PLUS, VK_BACKSPACE, VK_TAB,
    VK_Q, VK_W, VK_E, VK_R, VK_T, VK_Y, VK_U, VK_I, VK_O, VK_P, VK_LBRACKET, VK_RBRACKET, VK_ENTER, VK_LCTRL, VK_A, VK_S,
    VK_D, VK_F, VK_G, VK_H, VK_J, VK_K, VK_L, VK_COLON, VK_QUOTE, VK_TILDE, VK_LSHIFT, VK_BACKSLASH, VK_Z, VK_X, VK_C, VK_V,
    VK_B, VK_N, VK_M, VK_COMMA, VK_PERIOD, VK_SLASH, VK_RSHIFT, VK_MULTIPLY, VK_LALT, VK_SPACE, VK_CAPSLOCK, VK_F1,VK_F2,VK_F3,VK_F4,VK_F5,
    VK_F6,VK_F7,VK_F8,VK_F9,VK_F10, VK_NUMLOCK, VK_SCROLLLOCK, VK_NUMPAD7, VK_NUMPAD8, VK_NUMPAD9, VK_NUMPADMINUS, VK_NUMPAD4, VK_NUMPAD5, VK_NUMPAD6, VK_NUMPADPLUS, VK_NUMPAD1,
    VK_NUMPAD2,VK_NUMPAD3,VK_NUMPAD0,VK_NUMPADPERIOD, VK_PRINTSCREEN,  0, VK_102, VK_F11, VK_F12,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

boolean senddata(uchar databyte)
{
    asm xor cx,cx
__1:
    asm in  al,0x64
    asm test al,0x02
    asm loopnz __1
    asm jnz    senddataerror
    asm mov  al,databyte
    asm out  0x60,al
    return true;
senddataerror:
    return false;
}

void KeyboardHandler(void)
{
    register uchar code;
    static ushort flags=0;
    static uchar lockflags=0;
    register ushort msg;
    register ushort wParam=0;

    code=inportb(0x60);

    if(code==0xE0) {
        flags=FLAG_E0;
        goto Return;
    }
    else if(code==0xE1) {
        flags=FLAG_E1;
        goto Return;
    }
    else if(code==0xFA) {
        if(lockflags!=caps) {
            if(!senddata(lockflags)) lockflags=caps;
            else caps=lockflags;
        }
        goto Return;
    }
    if(flags==0) {
        wParam=table[code&0x7F];
        if(wParam==VK_PRINTSCREEN) {
            msg=MSG_SYSRQST;
            if(code&0x80) wParam=1;
            else wParam=0;
        }
        else {
            if(code&0x80) msg=MSG_KEYUP;
            else msg=MSG_KEYDOWN;
        }
    }
    else if(flags==FLAG_E0) {
        flags=0;
        if(code&0x80) msg=MSG_KEYUP;
        else msg=MSG_KEYDOWN;
        switch(code&0x7F) {
            case 0x38: wParam=VK_RALT; break;
            case 0x1D: wParam=VK_RCTRL; break;
            case 0x1C: wParam=VK_NUMPADENTER; break;
            case 0x52: wParam=VK_INSERT; break;
            case 0x53: wParam=VK_DELETE; break;
            case 0x4B: wParam=VK_LEFT; break;
            case 0x47: wParam=VK_HOME; break;
            case 0x4F: wParam=VK_END; break;
            case 0x48: wParam=VK_UP; break;
            case 0x50: wParam=VK_DOWN; break;
            case 0x49: wParam=VK_PGUP; break;
            case 0x51: wParam=VK_PGDN; break;
            case 0x4D: wParam=VK_RIGHT; break;
            case 0x35: wParam=VK_NUMPADSLASH; break;
            case 0x37: wParam=VK_PRINTSCREEN; break;
            case 0x46: if(code&0x80) {
                           msg=MSG_CBREAK;
                       }
                       else {
                           goto Return;
                       }
                       break;
            case 0x2A: goto Return;
        }
    }
    else if(flags==FLAG_E1) {
        if(code==0xC5) {
            msg=MSG_PAUSE;
            flags=0;
        }
        else {
            goto Return;
        }
    }
    if(msg==MSG_KEYDOWN) {
        switch(wParam) {
            case VK_CAPSLOCK: lockflags^=0x04; goto SetResetLED;
            case VK_NUMLOCK:  lockflags^=0x02; goto SetResetLED;
            case VK_SCROLLLOCK: lockflags^=0x01;
            SetResetLED:
               if(!senddata(0xED)) lockflags=caps;
               break;
        };
    }
    PutMessage(msg, wParam, 0);
Return:
    outportb(0x20,0x20);
}

