#if !defined(__VKEY_H__)

#define __VKEY_H__

#include "pm.h"

//
//  Message codes
//
#define MSG_NOTHING     0
#define MSG_KEYDOWN     1
#define MSG_KEYUP       2
#define MSG_CBREAK      3
#define MSG_SYSRQST     4
#define MSG_PAUSE       5
#define MSG_CHAR        6
#define MSG_CHARDEAD    7

//
//  Parameter for interrupt handler
//
#ifdef __cplusplus
#   define INTPARM ...
#else
#   define INTPARM void
#endif
//
//   internal keyboard handler state codes
//
#define FLAG_E0               0x0001
#define FLAG_E1               0x0002
//
//      Defines for keyboard key codes.
//
#define VK_0            1
#define VK_1            2
#define VK_2            3
#define VK_3            4
#define VK_4            5
#define VK_5            6
#define VK_6            7
#define VK_7            8
#define VK_8            9
#define VK_9            10

#define VK_TILDE        11
#define VK_LBRACKET     12
#define VK_RBRACKET     13
#define VK_BACKSLASH    14
#define VK_COLON        15
#define VK_QUOTE        16
#define VK_COMMA        17
#define VK_PERIOD       18
#define VK_SLASH        19
#define VK_MINUS        20
#define VK_PLUS         21

#define VK_TAB          22

#define VK_NUMPAD0      23
#define VK_NUMPAD1      24
#define VK_NUMPAD2      25
#define VK_NUMPAD3      26
#define VK_NUMPAD4      27
#define VK_NUMPAD5      28
#define VK_NUMPAD6      29
#define VK_NUMPAD7      30
#define VK_NUMPAD8      31
#define VK_NUMPAD9      32
#define VK_NUMPADPERIOD 33

#define VK_ESC          34
#define VK_BACKSPACE    35
#define VK_ENTER        36
#define VK_NUMPADENTER  37
#define VK_SPACE        38
#define VK_PRINTSCREEN  39
#define VK_PAUSE        40
#define VK_NUMPADSLASH  41
#define VK_MULTIPLY     42
#define VK_NUMPADMINUS  43
#define VK_NUMPADPLUS   44

#define VK_INSERT       45
#define VK_END          46
#define VK_DOWN         47
#define VK_PGDN         48
#define VK_LEFT         49
#define VK_CENTER       50
#define VK_RIGHT        51
#define VK_HOME         52
#define VK_UP           53
#define VK_PGUP         54
#define VK_DELETE       55

#define VK_LSHIFT       56
#define VK_RSHIFT       57
#define VK_LCTRL        58
#define VK_RCTRL        59
#define VK_LALT         60
#define VK_RALT         61
#define VK_CAPSLOCK     62
#define VK_NUMLOCK      63
#define VK_SCROLLLOCK   64

#define VK_A            65
#define VK_B            66
#define VK_C            67
#define VK_D            68
#define VK_E            69
#define VK_F            70
#define VK_G            71
#define VK_H            72
#define VK_I            73
#define VK_J            74
#define VK_K            75
#define VK_L            76
#define VK_M            77
#define VK_N            78
#define VK_O            79
#define VK_P            80
#define VK_Q            81
#define VK_R            82
#define VK_S            83
#define VK_T            84
#define VK_U            85
#define VK_V            86
#define VK_W            87
#define VK_X            88
#define VK_Y            89
#define VK_Z            90

#define VK_F1           91
#define VK_F2           92
#define VK_F3           93
#define VK_F4           94
#define VK_F5           95
#define VK_F6           96
#define VK_F7           97
#define VK_F8           98
#define VK_F9           99
#define VK_F10          100
#define VK_F11          101
#define VK_F12          102

#define VK_102          103

#define KF_LSHIFT       0x0008
#define KF_RSHIFT       0x0010
#define KF_SHIFT        (KF_LSHIFT|KF_RSHIFT)
#define KF_LCTRL        0x0020
#define KF_RCTRL        0x0040
#define KF_CTRL         (KF_LCTRL|KF_RCTRL)
#define KF_LALT         0x0080
#define KF_RALT         0x0100
#define KF_ALT          (KF_LALT|KF_RALT)
#define KF_INSERT       0x0200
#define KF_CAPSLOCK     0x0004
#define KF_NUMLOCK      0x0002
#define KF_SCROLLLOCK   0x0001

#ifdef __cplusplus 
extern "C" {
#endif

typedef struct Message {
    ushort  type;
    ushort  wParam;
    ulong   lParam;
} Message;

typedef enum { false=0, true=1 } boolean;

void PutMessage(ushort type, ushort wParam, ulong lParam);
void GetMessage(Message* msg);
boolean PeekMessage(Message* msg, boolean remove);
boolean TranslateMessage(Message* msg);
void KeyboardHandler(void);

#ifdef __cplusplus
}
#endif

#endif