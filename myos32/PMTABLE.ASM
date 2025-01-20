TABLE_BSS       SEGMENT PARA PRIVATE 'TABLE_BSS'

                ALIGN   16

GdtMaxIndex     equ     255

_GdtTable       dq      GdtMaxIndex+1 dup(?)

_GdtMaxIndex    dw      GdtMaxIndex

                ALIGN   16

IdtmaxIndex     equ     255
_IdtTable       dq      IdtMaxIndex+1 dup(?)
_IdtMaxIndex    dw      IdtMaxIndex

                PUBLIC  _GdtMaxIndex
                PUBLIC  _GdtTable
                PUBLIC  _IdtMaxIndex
                PUBLIC  _IdtTable

TABLE_BSS       ENDS

                END