#include <string.h>
#include "cmdname.h"
#include "cmdcode.h"
#include "deasm.h"


#define TwoCmd(name1, name2) cmd->name= DataSize ? name1 : name2; goto OneByteCmd;
#define OneCmd(n) cmd->name=n; goto OneByteCmd;

void Group1(uchar *code, Command *cmd);
void Group2(uchar *code, Command *cmd);
void Group3(uchar *code, Command *cmd);
void Group4(uchar *code, Command *cmd);
void Group5(uchar *code, Command *cmd);

void protectedmodecmd(uchar *code, Command *cmd);
void FPInstruction(uchar *code, Command *cmd);
uchar getmodrm(uchar *, Operand *, uchar);
uchar getreg(uchar, uchar);
void MakeDB(Command *cmd);

uchar DataSize;
uchar AddrSize;
uchar opcodesz;

uchar decode(uchar *code, Command *cmd)
{
    opcodesz=1;
    cmd->prefix=0;
    memset(&cmd->op[0], 0, sizeof(struct Operand)*3);

    switch(code[0]) {
        case ADD_Eb_Gb:
            cmd->name=CMDN_ADD;
        AriCmdEbGb:
            cmd->n=2;
            cmd->opsize=OPSZ_BYTE;
            cmd->length=1+getmodrm(code, &cmd->op[0], cmd->opsize);
            cmd->op[1].type=TYPE_REG;
            cmd->op[1].reg=getreg(code[1], cmd->opsize);
            break;
        case ADD_Ev_Gv:
            cmd->name=CMDN_ADD;
        AriCmdEvGv:
            cmd->n=2;
            cmd->opsize=OPSZ_WORD+DataSize*2;
            cmd->length=1+getmodrm(code, &cmd->op[0], cmd->opsize);
            cmd->op[1].type=TYPE_REG;
            cmd->op[1].reg=getreg(code[1], cmd->opsize);
            break;
        case ADD_Gb_Eb:
            cmd->name=CMDN_ADD;
        AriCmdGbEb:
            cmd->n=2;
            cmd->opsize=OPSZ_BYTE;
            cmd->length=1+getmodrm(code, &cmd->op[1], cmd->opsize);
            cmd->op[0].type=TYPE_REG;
            cmd->op[0].reg=getreg(code[1], cmd->opsize);
            break;
        case ADD_Gv_Ev:
            cmd->name=CMDN_ADD;
        AriCmdGvEv:
            cmd->n=2;
            cmd->opsize=OPSZ_WORD+DataSize*2;
            cmd->length=1+getmodrm(code, &cmd->op[1], cmd->opsize);
            cmd->op[0].type=TYPE_REG;
            cmd->op[0].reg=getreg(code[1], cmd->opsize);
            break;
        case ADD_AL_Ib:
            cmd->name=CMDN_ADD;
        AriCmdALIb:
            cmd->n=2;
            cmd->opsize=OPSZ_BYTE;
            cmd->length=2;
            cmd->op[0].type=TYPE_REG; cmd->op[0].reg=REG_AL;
            cmd->op[1].type=TYPE_IMM; cmd->op[1].imm=IMM8;
            break;
        case ADD_eAX_Iv:
            cmd->name=CMDN_ADD;
        AriCmdeAXIv:
            cmd->n=2;
            cmd->opsize=OPSZ_WORD + DataSize*OPSZ_WORD;
            cmd->length=3 + DataSize*2;
            cmd->op[0].type=TYPE_REG; cmd->op[0].reg=REG_AX|DataSize;
            cmd->op[1].type=TYPE_IMM; cmd->op[1].imm=IMM16|DataSize;
            break;
        case PUSH_ES:
            cmd->name=CMDN_PUSH;
        PushES:
            cmd->n=1;
            cmd->opsize=OPSZ_WORD;
            cmd->length=1;
            cmd->op[0].type=TYPE_REG; cmd->op[0].reg=REG_ES;
            break;
        case POP_ES:
            cmd->name=CMDN_POP;
            goto PushES;
        case  OR_Eb_Gb:
            cmd->name=CMDN_OR;
            goto AriCmdEbGb;
        case  OR_Ev_Gv:
            cmd->name=CMDN_OR;
            goto AriCmdEvGv;
        case  OR_Gb_Eb:
            cmd->name=CMDN_OR;
            goto AriCmdGbEb;
        case  OR_Gv_Ev:
            cmd->name=CMDN_OR;
            goto AriCmdGvEv;
        case  OR_AL_Ib:
            cmd->name=CMDN_OR;
            goto AriCmdALIb;
        case  OR_eAX_Iv:
            cmd->name=CMDN_OR;
            goto AriCmdeAXIv;
        case PUSH_CS:
            cmd->name=CMDN_PUSH;
            cmd->n=1;
            cmd->length=1;
            cmd->op[0].type=TYPE_REG; cmd->op[0].reg=REG_CS;
            break;
        case CODE0F:
            protectedmodecmd(code, cmd);
            break;
        case ADC_Eb_Gb:
            cmd->name=CMDN_ADC;
            goto AriCmdEbGb;
        case ADC_Ev_Gv:
            cmd->name=CMDN_ADC;
            goto AriCmdEvGv;
        case ADC_Gb_Eb:
            cmd->name=CMDN_ADC;
            goto AriCmdGbEb;
        case ADC_Gv_Ev:
            cmd->name=CMDN_ADC;
            goto AriCmdGvEv;
        case ADC_AL_Ib:
            cmd->name=CMDN_ADC;
            goto AriCmdALIb;
        case ADC_eAX_Iv:
            cmd->name=CMDN_ADC;
            goto AriCmdeAXIv;
        case PUSH_SS:
            cmd->name=CMDN_PUSH;
        PushSS:
            cmd->n=1;
            cmd->length=1;
            cmd->op[0].type=TYPE_REG; cmd->op[0].reg=REG_SS;
            break;
        case POP_SS:
            cmd->name=CMDN_POP;
            goto PushSS;
        case SBB_Eb_Gb:
            cmd->name=CMDN_SBB;
            goto AriCmdEbGb;
        case SBB_Ev_Gv:
            cmd->name=CMDN_SBB;
            goto AriCmdEvGv;
        case SBB_Gb_Eb:
            cmd->name=CMDN_SBB;
            goto AriCmdGbEb;
        case SBB_Gv_Ev:
            cmd->name=CMDN_SBB;
            goto AriCmdGvEv;
        case SBB_AL_Ib:
            cmd->name=CMDN_SBB;
            goto AriCmdALIb;
        case SBB_eAX_Iv:
            cmd->name=CMDN_SBB;
            goto AriCmdeAXIv;
        case PUSH_DS:
            cmd->name=CMDN_PUSH;
        PushDS:
            cmd->n=1;
            cmd->length=1;
            cmd->op[0].type=TYPE_REG; cmd->op[0].reg=REG_DS;
            break;
        case POP_DS:
            cmd->name=CMDN_POP;
            goto PushDS;
        case AND_Eb_Gb:
            cmd->name=CMDN_AND;
            goto AriCmdEbGb;
        case AND_Ev_Gv:
            cmd->name=CMDN_AND;
            goto AriCmdEvGv;
        case AND_Gb_Eb:
            cmd->name=CMDN_AND;
            goto AriCmdGbEb;
        case AND_Gv_Ev:
            cmd->name=CMDN_AND;
            goto AriCmdGvEv;
        case AND_AL_Ib:
            cmd->name=CMDN_AND;
            goto AriCmdALIb;
        case AND_eAX_Iv:
            cmd->name=CMDN_AND;
            goto AriCmdeAXIv;
        case SEG_ES:
            cmd->name=CMDN_SEGES;
            cmd->prefix=CMD_SEGES;
        Prefix:
            cmd->n=0;
            cmd->length=1;
            break;
        case DAA:
            cmd->name=CMDN_DAA;
        OneByteCmd:
            cmd->n=0;
            cmd->length=1;
            break;
        case SUB_Eb_Gb:
            cmd->name=CMDN_SUB;
            goto AriCmdEbGb;
        case SUB_Ev_Gv:
            cmd->name=CMDN_SUB;
            goto AriCmdEvGv;
        case SUB_Gb_Eb:
            cmd->name=CMDN_SUB;
            goto AriCmdGbEb;
        case SUB_Gv_Ev:
            cmd->name=CMDN_SUB;
            goto AriCmdGvEv;
        case SUB_AL_Ib:
            cmd->name=CMDN_SUB;
            goto AriCmdALIb;
        case SUB_eAX_Iv:
            cmd->name=CMDN_SUB;
            goto AriCmdeAXIv;
        case SEG_CS:
             cmd->name=CMDN_SEGCS;
             cmd->prefix=CMD_SEGCS;
             goto Prefix;
        case DAS:
             cmd->name=CMDN_DAS;
             goto OneByteCmd;
        case XOR_Eb_Gb:
            cmd->name=CMDN_XOR;
            goto AriCmdEbGb;
        case XOR_Ev_Gv:
            cmd->name=CMDN_XOR;
            goto AriCmdEvGv;
        case XOR_Gb_Eb:
            cmd->name=CMDN_XOR;
            goto AriCmdGbEb;
        case XOR_Gv_Ev:
            cmd->name=CMDN_XOR;
            goto AriCmdGvEv;
        case XOR_AL_Ib:
            cmd->name=CMDN_XOR;
            goto AriCmdALIb;
        case XOR_eAX_Iv:
            cmd->name=CMDN_XOR;
            goto AriCmdeAXIv;
        case SEG_SS:
            cmd->name=CMDN_SEGSS;
            cmd->prefix=CMD_SEGSS;
            goto Prefix;
        case AAA:
            cmd->name=CMDN_AAA;
            goto OneByteCmd;
        case CMP_Eb_Gb:
            cmd->name=CMDN_CMP;
            goto AriCmdEbGb;
        case CMP_Ev_Gv:
            cmd->name=CMDN_CMP;
            goto AriCmdEvGv;
        case CMP_Gb_Eb:
            cmd->name=CMDN_CMP;
            goto AriCmdGbEb;
        case CMP_Gv_Ev:
            cmd->name=CMDN_CMP;
            goto AriCmdGvEv;
        case CMP_AL_Ib:
            cmd->name=CMDN_CMP;
            goto AriCmdALIb;
        case CMP_eAX_Iv:
            cmd->name=CMDN_CMP;
            goto AriCmdeAXIv;
        case SEG_DS:
            cmd->name=CMDN_SEGDS;
            cmd->prefix=CMD_SEGDS;
            goto Prefix;
        case AAS:
            cmd->name=CMDN_AAS;
            goto OneByteCmd;
        case INC_eAX: cmd->op[0].reg=REG_AX|DataSize; goto CmdIncReg;
        case INC_eCX: cmd->op[0].reg=REG_CX|DataSize; goto CmdIncReg;
        case INC_eDX: cmd->op[0].reg=REG_DX|DataSize; goto CmdIncReg;
        case INC_eBX: cmd->op[0].reg=REG_BX|DataSize; goto CmdIncReg;
        case INC_eSP: cmd->op[0].reg=REG_SP|DataSize; goto CmdIncReg;
        case INC_eBP: cmd->op[0].reg=REG_BP|DataSize; goto CmdIncReg;
        case INC_eSI: cmd->op[0].reg=REG_SI|DataSize; goto CmdIncReg;
        case INC_eDI: cmd->op[0].reg=REG_DI|DataSize;
        CmdIncReg:
            cmd->name=CMDN_INC;
        CmdIncReg1:
            cmd->n=1;
            cmd->length=1;
            cmd->op[0].type=TYPE_REG;
            break;
        case DEC_eAX: cmd->op[0].reg=REG_AX|DataSize; goto CmdDecReg;
        case DEC_eCX: cmd->op[0].reg=REG_CX|DataSize; goto CmdDecReg;
        case DEC_eDX: cmd->op[0].reg=REG_DX|DataSize; goto CmdDecReg;
        case DEC_eBX: cmd->op[0].reg=REG_BX|DataSize; goto CmdDecReg;
        case DEC_eSP: cmd->op[0].reg=REG_SP|DataSize; goto CmdDecReg;
        case DEC_eBP: cmd->op[0].reg=REG_BP|DataSize; goto CmdDecReg;
        case DEC_eSI: cmd->op[0].reg=REG_SI|DataSize; goto CmdDecReg;
        case DEC_eDI: cmd->op[0].reg=REG_DI|DataSize;
        CmdDecReg:
            cmd->name=CMDN_DEC;
            goto CmdIncReg1;
        case PUSH_eAX: cmd->op[0].reg=REG_AX|DataSize; goto CmdPushReg;
        case PUSH_eCX: cmd->op[0].reg=REG_CX|DataSize; goto CmdPushReg;
        case PUSH_eDX: cmd->op[0].reg=REG_DX|DataSize; goto CmdPushReg;
        case PUSH_eBX: cmd->op[0].reg=REG_BX|DataSize; goto CmdPushReg;
        case PUSH_eSP: cmd->op[0].reg=REG_SP|DataSize; goto CmdPushReg;
        case PUSH_eBP: cmd->op[0].reg=REG_BP|DataSize; goto CmdPushReg;
        case PUSH_eSI: cmd->op[0].reg=REG_SI|DataSize; goto CmdPushReg;
        case PUSH_eDI: cmd->op[0].reg=REG_DI|DataSize;
        CmdPushReg:
            cmd->name=CMDN_PUSH;
            goto CmdIncReg1;
        case POP_eAX: cmd->op[0].reg=REG_AX|DataSize; goto CmdPopReg;
        case POP_eCX: cmd->op[0].reg=REG_CX|DataSize; goto CmdPopReg;
        case POP_eDX: cmd->op[0].reg=REG_DX|DataSize; goto CmdPopReg;
        case POP_eBX: cmd->op[0].reg=REG_BX|DataSize; goto CmdPopReg;
        case POP_eSP: cmd->op[0].reg=REG_SP|DataSize; goto CmdPopReg;
        case POP_eBP: cmd->op[0].reg=REG_BP|DataSize; goto CmdPopReg;
        case POP_eSI: cmd->op[0].reg=REG_SI|DataSize; goto CmdPopReg;
        case POP_eDI: cmd->op[0].reg=REG_DI|DataSize;
        CmdPopReg:
            cmd->name=CMDN_POP;
            goto CmdIncReg1;
        case PUSHA:
            cmd->name= DataSize ? CMDN_PUSHAD : CMDN_PUSHA;
            goto OneByteCmd;
        case POPA:
            cmd->name= DataSize ? CMDN_POPAD : CMDN_POPA;
            goto OneByteCmd;
        case BOUND_Gv_Ma:
            cmd->name=CMDN_BOUND;
            cmd->n=2;
            cmd->opsize=OPSZ_DWORD+DataSize*OPSZ_DWORD;
            cmd->op[0].type=TYPE_REG;
            cmd->op[0].reg=getreg(code[1], OPSZ_WORD+DataSize*OPSZ_WORD);
            cmd->length=1+getmodrm(code, &cmd->op[1], cmd->opsize);
            break;
        case ARPL_Ew_Rw:
            cmd->name=CMDN_ARPL;
            cmd->n=2;
            cmd->opsize=OPSZ_WORD;
            cmd->length=1+getmodrm(code, &cmd->op[0], cmd->opsize);
            cmd->op[1].type=TYPE_REG;
            cmd->op[1].reg=getreg(code[1], cmd->opsize);
            break;
        case SEG_FS:
            cmd->name=CMDN_SEGFS;
            cmd->prefix=CMD_SEGFS;
            goto Prefix;
        case SEG_GS:
            cmd->name=CMDN_SEGGS;
            cmd->prefix=CMD_SEGGS;
            goto Prefix;
        case OPSIZE:
            cmd->name=CMDN_OPSIZE;
            cmd->prefix=CMD_OPSIZE;
            goto Prefix;
        case ADDRSIZE:
            cmd->name=CMDN_ADDRSIZE;
            cmd->prefix=CMD_ADDRSIZE;
            goto Prefix;
        case PUSH_Iv:
            cmd->length=3+DataSize*2;
            cmd->op[0].imm=IMM16|DataSize;
        PushIv:
            cmd->name=CMDN_PUSH;
            cmd->n=1;
            cmd->op[0].type=TYPE_IMM;
            break;
        case IMUL_Gv_Ev_Iv:
            cmd->length=3+DataSize*2;
            cmd->op[2].imm=IMM16|DataSize;
        ImulGvEvIv:
            cmd->name=CMDN_IMUL;
            cmd->n=3;
            cmd->opsize=OPSZ_WORD*(1+DataSize);
            cmd->op[0].type=TYPE_REG;
            cmd->op[0].reg=getreg(code[1], cmd->opsize);
            cmd->length+=getmodrm(code, &cmd->op[1], cmd->opsize);
            cmd->op[2].type=TYPE_IMM;
            break;
        case PUSH_Isb:
            cmd->length=2;
            cmd->op[0].imm=IMMS8;
            goto PushIv;
        case IMUL_Gv_Ev_Isb:
            cmd->length=2;
            cmd->op[2].imm=IMMS8;
            goto ImulGvEvIv;
        case INSB:
            OneCmd(CMDN_INSB);
        case INSW_D:
            TwoCmd(CMDN_INSD, CMDN_INSW);
        case OUTSB:
            OneCmd(CMDN_OUTSB);
        case OUTSW_D:
            TwoCmd(CMDN_OUTSD, CMDN_OUTSW);
        case JO_Jb:  cmd->name=CMDN_JO; goto JumpCC;
        case JNO_Jb: cmd->name=CMDN_JNO; goto JumpCC;
        case JB_Jb:  cmd->name=CMDN_JB; goto JumpCC;
        case JAE_Jb: cmd->name=CMDN_JAE; goto JumpCC;
        case JZ_Jb:  cmd->name=CMDN_JZ; goto JumpCC;
        case JNZ_Jb: cmd->name=CMDN_JNZ; goto JumpCC;
        case JBE_Jb: cmd->name=CMDN_JBE; goto JumpCC;
        case JA_Jb:  cmd->name=CMDN_JA; goto JumpCC;
        case JS_Jb:  cmd->name=CMDN_JS; goto JumpCC;
        case JNS_Jb: cmd->name=CMDN_JNS; goto JumpCC;
        case JPE_Jb: cmd->name=CMDN_JPE; goto JumpCC;
        case JPO_Jb: cmd->name=CMDN_JPO; goto JumpCC;
        case JL_Jb:  cmd->name=CMDN_JL; goto JumpCC;
        case JGE_Jb: cmd->name=CMDN_JGE; goto JumpCC;
        case JLE_Jb: cmd->name=CMDN_JLE; goto JumpCC;
        case JG_Jb:  cmd->name=CMDN_JG;
        JumpCC:
            cmd->n=1;
            cmd->length=2;
            cmd->op[0].type=TYPE_REL; cmd->op[0].rel=REL8;
            break;
        case TEST_Eb_Gb:
            cmd->name=CMDN_TEST;
            goto AriCmdEbGb;
        case TEST_Ev_Gv:
            cmd->name=CMDN_TEST;
            goto AriCmdEvGv;
        case XCHG_Eb_Gb:
            cmd->name=CMDN_XCHG;
            goto AriCmdEbGb;
        case XCHG_Ev_Gv:
            cmd->name=CMDN_XCHG;
            goto AriCmdEvGv;
        case MOV_Eb_Gb:
            cmd->name=CMDN_MOV;
            goto AriCmdEbGb;
        case MOV_Ev_Gv:
            cmd->name=CMDN_MOV;
            goto AriCmdEvGv;
        case MOV_Gb_Eb:
            cmd->name=CMDN_MOV;
            goto AriCmdGbEb;
        case MOV_Gv_Ev:
            cmd->name=CMDN_MOV;
            goto AriCmdGvEv;
        case MOV_Ew_Sw:
            cmd->name=CMDN_MOV;
            cmd->n=2;
            cmd->opsize=OPSZ_WORD;
            cmd->length=1+getmodrm(code, &cmd->op[0], OPSZ_WORD);
            cmd->op[1].type=TYPE_REG;
            cmd->op[1].reg=(getreg(code[1], OPSZ_WORD)>>1)+0x18;
            break;
        case LEA_Gv_M:
            cmd->name=CMDN_LEA;
            cmd->n=2;
            cmd->opsize=OPSZ_WORD*(1+DataSize);
            cmd->op[0].type=TYPE_REG;
            cmd->op[0].reg=getreg(code[1], cmd->opsize);
            cmd->length=1+getmodrm(code, &cmd->op[1], cmd->opsize);
            break;
        case MOV_Sw_Ew:
            cmd->name=CMDN_MOV;
            cmd->n=2;
            cmd->opsize=OPSZ_WORD;
            cmd->op[0].type=TYPE_REG;
            cmd->op[0].reg=(getreg(code[1], OPSZ_WORD)>>1)+0x18;
            cmd->length=1+getmodrm(code, &cmd->op[1], OPSZ_WORD);
            break;
        case POP_Ev:
            cmd->name=CMDN_POP;
            cmd->n=1;
            cmd->opsize=OPSZ_WORD*(1+DataSize);
            cmd->length=1+getmodrm(code, &cmd->op[0], cmd->opsize);
            break;
        case NOP:
            cmd->name=CMDN_NOP;
            goto OneByteCmd;
        case XCHG_eCX_eAX: cmd->op[0].reg=REG_CX|DataSize; goto XchgRegReg;
        case XCHG_eDX_eAX: cmd->op[0].reg=REG_DX|DataSize; goto XchgRegReg;
        case XCHG_eBX_eAX: cmd->op[0].reg=REG_BX|DataSize; goto XchgRegReg;
        case XCHG_eSP_eAX: cmd->op[0].reg=REG_SP|DataSize; goto XchgRegReg;
        case XCHG_eBP_eAX: cmd->op[0].reg=REG_BP|DataSize; goto XchgRegReg;
        case XCHG_eSI_eAX: cmd->op[0].reg=REG_SI|DataSize; goto XchgRegReg;
        case XCHG_eDI_eAX: cmd->op[0].reg=REG_DI|DataSize;
        XchgRegReg:
            cmd->n=2;
            cmd->name=CMDN_XCHG;
            cmd->op[1].type=TYPE_REG;
            cmd->op[1].reg=REG_AX|DataSize;
            cmd->length=1;
            cmd->op[0].type=TYPE_REG;
            break;
        case CBW_CWDE:
            cmd->name= DataSize ? CMDN_CWDE : CMDN_CBW;
            goto OneByteCmd;
        case CWD_CDQ:
            cmd->name= DataSize ? CMDN_CDQ : CMDN_CWD;
            goto OneByteCmd;
        case CALL_Ap:
            cmd->name=CMDN_CALL;
            goto JumpAp;
        case WAIT:
            cmd->name=CMDN_WAIT;
            goto OneByteCmd;
        case PUSF_Fv:
            cmd->name= DataSize ? CMDN_PUSHFD : CMDN_PUSHF;
            goto OneByteCmd;
        case POPF_Fv:
            cmd->name = DataSize ? CMDN_POPFD : CMDN_POPF;
            goto OneByteCmd;
        case SAHF:
            cmd->name = CMDN_SAHF;
            goto OneByteCmd;
        case LAHF:
            cmd->name = CMDN_LAHF;
            goto OneByteCmd;
        case MOV_AL_Ob: cmd->op[0].reg=REG_AL; goto MovAXMem;
        case MOV_eAX_Ov: cmd->op[0].reg=REG_AX|DataSize;
        MovAXMem:
            cmd->name=CMDN_MOV;
            cmd->n=2;
            cmd->op[0].type=TYPE_REG;
            cmd->op[1].type=TYPE_MEM; cmd->op[1].mem.disp=DISP16|DataSize;
            break;
        case MOV_Ob_AL: cmd->op[1].reg=REG_AL; goto MovMemAX;
        case MOV_Ov_eAX: cmd->op[1].reg=REG_AX|DataSize;
        MovMemAX:
            cmd->name=CMDN_MOV;
            cmd->n=2;
            cmd->op[0].type=TYPE_MEM;
            cmd->op[0].mem.disp=DISP16|DataSize;
            cmd->op[1].type=TYPE_REG;
            break;
        case MOVSB:
            OneCmd(CMDN_MOVSB);
        case MOVSW_D:
            TwoCmd(CMDN_MOVSD, CMDN_MOVSW);
        case CMPSB:
            OneCmd(CMDN_CMPSB);
        case CMPSW_D:
            TwoCmd(CMDN_CMPSD, CMDN_CMPSW);
        case TEST_AL_Ib:
            cmd->name=CMDN_TEST;
            goto AriCmdALIb;
        case TEST_eAX_Iv:
            cmd->name=CMDN_TEST;
            goto AriCmdeAXIv;
        case STOSB:
            OneCmd(CMDN_STOSB);
        case STOSW_D:
            TwoCmd(CMDN_STOSD, CMDN_STOSW);
        case LODSB:
            OneCmd(CMDN_LODSB);
        case LODSW_D:
            TwoCmd(CMDN_LODSD, CMDN_LODSW);
        case SCASB:
            OneCmd(CMDN_SCASB);
        case SCASW_D:
            TwoCmd(CMDN_SCASD, CMDN_SCASW);
        case MOV_AL_Ib: cmd->op[0].reg=REG_AL; goto MovRbImm;
        case MOV_CL_Ib: cmd->op[0].reg=REG_CL; goto MovRbImm;
        case MOV_DL_Ib: cmd->op[0].reg=REG_DL; goto MovRbImm;
        case MOV_BL_Ib: cmd->op[0].reg=REG_BL; goto MovRbImm;
        case MOV_AH_Ib: cmd->op[0].reg=REG_AH; goto MovRbImm;
        case MOV_CH_Ib: cmd->op[0].reg=REG_CH; goto MovRbImm;
        case MOV_DH_Ib: cmd->op[0].reg=REG_DH; goto MovRbImm;
        case MOV_BH_Ib: cmd->op[0].reg=REG_BH;
        MovRbImm:
            cmd->name=CMDN_MOV;
            cmd->n=2;
            cmd->length=2;
            cmd->op[0].type=TYPE_REG;
            cmd->op[1].type=TYPE_IMM; cmd->op[1].imm=IMM8;
            break;
        case MOV_eAX_Iv: cmd->op[0].reg=REG_AX|DataSize; goto MovRvImm;
        case MOV_eCX_Iv: cmd->op[0].reg=REG_CX|DataSize; goto MovRvImm;
        case MOV_eDX_Iv: cmd->op[0].reg=REG_DX|DataSize; goto MovRvImm;
        case MOV_eBX_Iv: cmd->op[0].reg=REG_BX|DataSize; goto MovRvImm;
        case MOV_eSP_Iv: cmd->op[0].reg=REG_SP|DataSize; goto MovRvImm;
        case MOV_eBP_Iv: cmd->op[0].reg=REG_BP|DataSize; goto MovRvImm;
        case MOV_eSI_Iv: cmd->op[0].reg=REG_SI|DataSize; goto MovRvImm;
        case MOV_eDI_Iv: cmd->op[0].reg=REG_DI|DataSize;
        MovRvImm:
            cmd->name=CMDN_MOV;
            cmd->n=2;
            cmd->length=3+DataSize*2;
            cmd->op[0].type=TYPE_REG;
            cmd->op[1].type=TYPE_IMM; cmd->op[1].imm=IMM16|DataSize;
            break;
        case RETN_Iv:
            cmd->name=CMDN_RETN;
            cmd->n=1;
            cmd->op[0].type=TYPE_IMM; cmd->op[0].imm=IMM16|DataSize;
            break;
        case RETN:
            OneCmd(CMDN_RETN);
        case LES_Gv_Mp:
            cmd->name=CMDN_LES;
            goto CmdLds;
        case LDS_Gv_Mp:
            cmd->name=CMDN_LDS;
        CmdLds:
            cmd->n=2;
            cmd->opsize=OPSZ_DWORD+2*DataSize;
            cmd->op[0].type=TYPE_REG;
            cmd->op[0].reg=getreg(code[1], OPSZ_WORD*(1+DataSize));
            cmd->length=1+getmodrm(code, &cmd->op[1], cmd->opsize);
            break;
        case MOV_Eb_Ib:
            cmd->opsize=OPSZ_BYTE;
            cmd->op[1].imm=IMM8;
            cmd->length=2;
            goto MovRMImm;
        case MOV_Ev_Iv:
            cmd->opsize=OPSZ_WORD*(1+DataSize);
            cmd->op[1].imm=IMM16|DataSize;
            cmd->length=3+DataSize*2;
        MovRMImm:
            cmd->name=CMDN_MOV;
            cmd->n=2;
            cmd->length+=getmodrm(code, &cmd->op[0], cmd->opsize);
            cmd->op[1].type=TYPE_IMM;
            break;
        case ENTER_Iv_Ib:
            cmd->name=CMDN_ENTER;
            cmd->n=2;
            cmd->length=4+DataSize*2;
            cmd->op[0].type=TYPE_IMM; cmd->op[0].imm=IMM16|DataSize;
            cmd->op[1].type=TYPE_IMM; cmd->op[1].imm=IMM8;
            break;
        case LEAVE:
            OneCmd(CMDN_LEAVE);
        case RETF_Iv:
            cmd->name=CMDN_RETF;
            cmd->n=1;
            cmd->length=3+DataSize*2;
            cmd->op[0].type=TYPE_IMM; cmd->op[0].imm=IMM16|DataSize;
            break;
        case RETF:
            OneCmd(CMDN_RETF);
        case INT3:
            cmd->name=CMDN_INT;
            cmd->n=1;
            cmd->length=1;
            cmd->op[0].type=TYPE_IMM3;
            break;
        case INT_Ib:
            cmd->name=CMDN_INT;
            cmd->n=1;
            cmd->length=2;
            cmd->op[0].type=TYPE_IMM; cmd->op[0].imm=IMM8;
            break;
        case INTO:
            OneCmd(CMDN_INTO);
        case IRET:
            TwoCmd(CMDN_IRETD, CMDN_IRET);
        case AAM: cmd->name=CMDN_AAM; goto CmdAAD;
        case AAD: cmd->name=CMDN_AAD;
        CmdAAD:
            cmd->n=1;
            cmd->length=2;
            cmd->op[0].type=TYPE_IMM; cmd->op[0].imm=IMM8;
            break;
        case XLAT:
            OneCmd(CMDN_XLAT);
        case ESC0:
        case ESC1:
        case ESC2:
        case ESC3:
        case ESC4:
        case ESC5:
        case ESC6:
        case ESC7:
            FPInstruction(code, cmd);
            break;
        case LOOPNE_Jb:
            cmd->name=CMDN_LOOPNZ;
            goto JumpCC;
        case LOOPE_Jb:
            cmd->name=CMDN_LOOPZ;
            goto JumpCC;
        case LOOP_Jb:
            cmd->name=CMDN_LOOP;
            goto JumpCC;
        case JCXZ_JECXZ_Jb:
            cmd->name= AddrSize ? CMDN_JECXZ : CMDN_JCXZ;
            goto JumpCC;
        case IN_AL_Ib: cmd->op[0].reg=REG_AL; goto InAxPort;
        case IN_eAX_Ib: cmd->op[0].reg=REG_AX|DataSize;
        InAxPort:
            cmd->name=CMDN_IN;
            cmd->n=2;
            cmd->length=2;
            cmd->op[0].type=TYPE_REG;
            cmd->op[1].type=TYPE_IMM; cmd->op[1].imm=IMM8;
            break;
        case OUT_Ib_AL: cmd->op[1].reg=REG_AL; goto OutAxPort;
        case OUT_Ib_eAX: cmd->op[1].reg=REG_AX|DataSize;
        OutAxPort:
            cmd->name=CMDN_OUT;
            cmd->n=2;
            cmd->length=2;
            cmd->op[0].type=TYPE_IMM; cmd->op[0].imm=IMM8;
            cmd->op[1].type=TYPE_REG;
            break;
        case CALL_Jv:
            cmd->name=CMDN_CALL;
            goto JumpJv;
        case JMP_Jv:
            cmd->name=CMDN_JMP;
        JumpJv:
            cmd->n=1;
            cmd->length=3+DataSize*2;
            cmd->op[0].type=TYPE_REL; cmd->op[0].rel=REL16|DataSize;
            break;
        case JMP_Ap:
            cmd->name=CMDN_JMP;
        JumpAp:
            cmd->n=1;
            cmd->length=5+DataSize*2;
            cmd->op[0].type=TYPE_PTR; cmd->op[0].ptr=PTR32|DataSize;
            break;
        case JMP_Jb:
            cmd->name=CMDN_JMP;
            goto JumpCC;
        case IN_AL_DX: cmd->op[0].reg=REG_AL; goto InAxDX;
        case IN_eAX_DX: cmd->op[0].reg=REG_AX|DataSize;
        InAxDX:
            cmd->name=CMDN_IN;
            cmd->n=2;
            cmd->length=1;
            cmd->op[0].type=TYPE_REG;
            cmd->op[1].type=TYPE_REG; cmd->op[1].reg=REG_DX;
            break;
        case OUT_DX_AL: cmd->op[1].reg=REG_AL; goto OutAxDX;
        case OUT_DX_eAX: cmd->op[1].reg=REG_AX|DataSize;
        OutAxDX:
            cmd->name=CMDN_OUT;
            cmd->n=2;
            cmd->length=1;
            cmd->op[0].type=TYPE_REG; cmd->op[0].reg=REG_DX;
            cmd->op[1].type=TYPE_REG;
            break;
        case LOCK:
            cmd->name=CMDN_LOCK;
            cmd->prefix=CMD_LOCK;
            goto Prefix;
        case REPNE:
            cmd->name=CMDN_REPZ;
            cmd->prefix=CMD_REPZ;
            goto Prefix;
        case REP:
            cmd->name=CMDN_REP;
            cmd->prefix=CMD_REP;
            goto Prefix;
        case HLT:
            OneCmd(CMDN_HLT);
        case CMC:
            OneCmd(CMDN_CMC);
        case CLC:
            OneCmd(CMDN_CLC);
        case STC:
            OneCmd(CMDN_STC);
        case CLI:
            OneCmd(CMDN_CLI);
        case STI:
            OneCmd(CMDN_STI);
        case CLD:
            OneCmd(CMDN_CLD);
        case STD:
            OneCmd(CMDN_STD);
        case IMMEDIATEGRP1_Eb_Ib:
        case IMMEDIATEGRP1_Ev_Iv:
        case UNDOCUMENTEDGRP1_Eb_Isb:
        case IMMEDIATEGRP1_Ev_Isb:
            Group1(code, cmd);
            break;
        case SHIFTGRP2_Eb_Ib:
        case SHIFTGRP2_Ev_Ib:
        case SHIFTGRP2_Eb_1:
        case SHIFTGRP2_Ev_1:
        case SHIFTGRP2_Eb_CL:
        case SHIFTGRP2_Ev_CL:
            Group2(code, cmd);
            break;
        case UNARYGRP3_Eb:
        case UNARYGRP3_Ev:
            Group3(code, cmd);
            break;
        case INCDECGRP4:
            Group4(code, cmd);
            break;
        case INDIRCTGRP5:
            Group5(code, cmd);
            break;
        default:
            MakeDB(cmd);
            break;
    }
    return opcodesz;
}

void MakeDB(Command *cmd)
{
    cmd->name=CMDN_DB;
    cmd->length=1;
    cmd->n=1;
    cmd->op[0].type=TYPE_IMM;
    cmd->op[0].imm=IMM8;
    opcodesz=0;
}

void Group1(uchar *code, Command *cmd)
{
    uchar c=(code[1]&0x38)>>3;
    cmd->n=2;
    switch(c) {
        case GR1_ADD: cmd->name=CMDN_ADD; break;
        case GR1_OR:  cmd->name=CMDN_OR; break;
        case GR1_ADC: cmd->name=CMDN_ADC; break;
        case GR1_SBB: cmd->name=CMDN_SBB; break;
        case GR1_AND: cmd->name=CMDN_AND; break;
        case GR1_SUB: cmd->name=CMDN_SUB; break;
        case GR1_XOR: cmd->name=CMDN_XOR; break;
        case GR1_CMP: cmd->name=CMDN_CMP; break;
    }
    switch(code[0]) {
        case IMMEDIATEGRP1_Eb_Ib:
            cmd->opsize=OPSZ_BYTE;
            cmd->length=2;
            cmd->op[1].imm=IMM8;
            goto __1;
        case IMMEDIATEGRP1_Ev_Iv:
            cmd->opsize=OPSZ_WORD*(1+DataSize);
            cmd->length=3+DataSize*2;
            cmd->op[1].imm=IMM16|DataSize;
        __1:
            cmd->length+=getmodrm(code, &cmd->op[0], cmd->opsize);
            cmd->op[1].type=TYPE_IMM;
            break;
        case UNDOCUMENTEDGRP1_Eb_Isb:
            cmd->opsize=OPSZ_BYTE;
            goto __2;
        case IMMEDIATEGRP1_Ev_Isb:
            cmd->opsize=OPSZ_WORD*(1+DataSize);
        __2:
            cmd->length=2+getmodrm(code, &cmd->op[0], cmd->opsize);
            cmd->op[1].type=TYPE_IMM;
            cmd->op[1].imm=IMMS8;
            break;
    }
}

void Group2(uchar *code, Command *cmd)
{
    uchar c=(code[1]&0x38)>>3;
    cmd->n=2;
    switch(c) {
        case GR2_ROL:    cmd->name=CMDN_ROL; break;
        case GR2_ROR:    cmd->name=CMDN_ROR; break;
        case GR2_RCL:    cmd->name=CMDN_RCL; break;
        case GR2_RCR:    cmd->name=CMDN_RCR; break;
        case GR2_SHR:    cmd->name=CMDN_SHR; break;
        case GR2_SHL:    cmd->name=CMDN_SHL; break;
        case GR2UND_SAL: cmd->name=CMDN_SAL; break;
        case GR2_SAR:    cmd->name=CMDN_SAR; break;
    }
    switch(code[0]) {
        case SHIFTGRP2_Eb_Ib:
            cmd->op[1].type=TYPE_IMM; cmd->op[1].imm=IMM8;
            cmd->length=2;
            goto __1;
        case SHIFTGRP2_Eb_1:
            cmd->op[1].type=TYPE_IMM1;
            cmd->length=1;
            goto __1;
        case SHIFTGRP2_Eb_CL:
            cmd->op[1].type=TYPE_REG; cmd->op[1].reg=REG_CL;
            cmd->length=1;
        __1:
            cmd->opsize=OPSZ_BYTE;
            cmd->length+=getmodrm(code, &cmd->op[0], cmd->opsize);
            break;
        case SHIFTGRP2_Ev_Ib:
            cmd->op[1].type=TYPE_IMM; cmd->op[1].imm=IMM8;
            cmd->length=2;
            goto __2;
        case SHIFTGRP2_Ev_1:
            cmd->op[1].type=TYPE_IMM1;
            cmd->length=1;
            goto __2;
        case SHIFTGRP2_Ev_CL:
            cmd->op[1].type=TYPE_REG; cmd->op[1].reg=REG_CL;
            cmd->length=1;
        __2:
            cmd->opsize=OPSZ_WORD*(1+DataSize);
            cmd->length+=getmodrm(code, &cmd->op[0], cmd->opsize);
            break;
    }
}

void Group3(uchar *code, Command *cmd)
{
    uchar c=(code[1]&0x38)>>3;
    cmd->n=1;
    cmd->length=1;
    switch(c) {
        case GR3_TEST:
            cmd->n=2;
            cmd->name=CMDN_TEST;
            cmd->op[1].type=TYPE_IMM;
            if(code[0]==UNARYGRP3_Eb) {
               cmd->op[1].imm=IMM8;
               cmd->length=2;
            }
            else {
               cmd->op[1].imm=IMM16|DataSize;
               cmd->length=3+DataSize*2;
            }
            break;
        case GR3_INV1:
            MakeDB(cmd);
            return;
        case GR3_NOT: cmd->name=CMDN_NOT; break;
        case GR3_NEG: cmd->name=CMDN_NEG; break;
        case GR3_MUL: cmd->name=CMDN_MUL; break;
        case GR3_IMUL:cmd->name=CMDN_IMUL; break;
        case GR3_DIV: cmd->name=CMDN_DIV; break;
        case GR3_IDIV:cmd->name=CMDN_IDIV; break;
    }
    switch(code[0]) {
        case UNARYGRP3_Eb:
            cmd->opsize=OPSZ_BYTE;
            goto __1;
        case UNARYGRP3_Ev:
            cmd->opsize=OPSZ_WORD*(1+DataSize);
        __1:
            cmd->length+=getmodrm(code, &cmd->op[0], cmd->opsize);
            break;
    }
}

void Group4(uchar *code, Command *cmd)
{
    uchar c=(code[1]&0x38)>>3;
    cmd->n=1;
    switch(c) {
        case GR4_INC_Eb: cmd->name=CMDN_INC; goto __1;
        case GR4_DEC_Eb: cmd->name=CMDN_DEC;
        __1:
            cmd->opsize=OPSZ_BYTE;
            cmd->length=1+getmodrm(code, &cmd->op[0], OPSZ_BYTE);
            break;
        default:
            MakeDB(cmd);
            break;
    }
}

void Group5(uchar *code, Command *cmd)
{
    uchar c=(code[1]&0x38)>>3;
    cmd->n=1;
    switch(c) {
        case GR5_INC_Ev: cmd->name=CMDN_INC; goto __1;
        case GR5_DEC_Ev: cmd->name=CMDN_DEC; goto __1;
        case GR5_CALL_Ev: cmd->name=CMDN_CALL; goto __1;
        case GR5_JMP_Ev:  cmd->name=CMDN_JMP; goto __1;
        case GR5_PUSH_Ev: cmd->name=CMDN_PUSH;
        __1:
            cmd->opsize=OPSZ_WORD*(1+DataSize);
            cmd->length=1+getmodrm(code, &cmd->op[0], cmd->opsize);
            break;
        case GR5_CALL_Ep: cmd->name=CMDN_CALL; goto __2;
        case GR5_JMP_Ep:  cmd->name=CMDN_JMP; goto __2;
        __2:
            cmd->opsize=OPSZ_WORD+OPSZ_WORD*(1+DataSize);
            cmd->length=1+getmodrm(code, &cmd->op[0], cmd->opsize);
            break;
        default:
            MakeDB(cmd);
            break;
    }
}

uchar getreg(uchar code, uchar sz)
{
    code&=0x38;
    code>>=3;
    if(sz==OPSZ_BYTE) {
        switch(code) {
            case 0: return REG_AL;
            case 1: return REG_CL;
            case 2: return REG_DL;
            case 3: return REG_BL;
            case 4: return REG_AH;
            case 5: return REG_CH;
            case 6: return REG_DH;
            case 7: return REG_BH;
        }
    }
    else if(sz==OPSZ_WORD) {
        switch(code) {
            case 0: return REG_AX;
            case 1: return REG_CX;
            case 2: return REG_DX;
            case 3: return REG_BX;
            case 4: return REG_SP;
            case 5: return REG_BP;
            case 6: return REG_SI;
            case 7: return REG_DI;
        }
    }
    else {
        switch(code) {
            case 0: return REG_EAX;
            case 1: return REG_ECX;
            case 2: return REG_EDX;
            case 3: return REG_EBX;
            case 4: return REG_ESP;
            case 5: return REG_EBP;
            case 6: return REG_ESI;
            case 7: return REG_EDI;
        }
    }
    return 0;
}

#define base(r) o->mem.base=r
#define index(r) o->mem.index=r
#define disp(d) o->mem.disp=d

uchar rmsiblength;
void sibpresent(uchar c, uchar mod, Operand *o);

uchar getmodrm(uchar *code, Operand *o, uchar sz)
{
    uchar rm, mod;

    rmsiblength=1;

    opcodesz++;
    rm=code[1]&0x07;
    mod=code[1]>>6;

    o->type=TYPE_MEM;
    if(AddrSize==0) {
        switch(mod) {
            case 0:
                switch(rm) {
                    case 0: base(REG_BX); index(REG_SI); break;
                    case 1: base(REG_BX); index(REG_DI); break;
                    case 2: base(REG_BP); index(REG_SI); break;
                    case 3: base(REG_BP); index(REG_DI); break;
                    case 4: index(REG_SI); break;
                    case 5: index(REG_DI); break;
                    case 6: rmsiblength+=2;
                            disp(DISP16);
                            break;
                    case 7: base(REG_BX); break;
                }
                break;
            case 1:
                disp(DISP8);
                rmsiblength++;
            setbaseandindex:
                switch(rm) {
                    case 0: base(REG_BX); index(REG_SI); break;
                    case 1: base(REG_BX); index(REG_DI); break;
                    case 2: base(REG_BP); index(REG_SI); break;
                    case 3: base(REG_BP); index(REG_DI); break;
                    case 4: index(REG_SI); break;
                    case 5: index(REG_DI); break;
                    case 6: base(REG_BP); break;
                    case 7: base(REG_BX); break;
                }
                break;
            case 2:
                disp(DISP16);
                rmsiblength+=2;
                goto setbaseandindex;
            case 3:
                o->type=TYPE_REG;
                o->reg=getreg(rm << 3, sz);
                break;
        }
    }
    else {
        switch(mod) {
            case 0:
                switch(rm) {
                    case 0: base(REG_EAX); break;
                    case 1: base(REG_ECX); break;
                    case 2: base(REG_EDX); break;
                    case 3: base(REG_EBX); break;
                    case 4: sibpresent(code[2], mod, o); break;
                    case 5: disp(DISP32);
                            rmsiblength+=4;
                            break;
                    case 6: base(REG_ESI); break;
                    case 7: base(REG_EDI); break;
                }
                break;
            case 1:
                disp(DISP8);
                rmsiblength++;
                switch(rm) {
                    case 0: base(REG_EAX); break;
                    case 1: base(REG_ECX); break;
                    case 2: base(REG_EDX); break;
                    case 3: base(REG_EBX); break;
                    case 4: rmsiblength--;
                            disp(0);
                            sibpresent(code[2], mod, o);
                            break;
                    case 5: base(REG_EBP); break;
                    case 6: base(REG_ESI); break;
                    case 7: base(REG_EDI); break;
                }
                break;
            case 2:
                disp(DISP32);
                rmsiblength+=4;
                switch(rm) {
                    case 0: base(REG_EAX); break;
                    case 1: base(REG_ECX); break;
                    case 2: base(REG_EDX); break;
                    case 3: base(REG_EBX); break;
                    case 4: rmsiblength-=4;
                            disp(0);
                            sibpresent(code[2], mod, o);
                            break;
                    case 5: base(REG_EBP); break;
                    case 6: base(REG_ESI); break;
                    case 7: base(REG_EDI); break;
                }
                break;
            case 3:
                o->type=TYPE_REG;
                o->reg=getreg(rm << 3, sz);
                break;
        }
    }
    return rmsiblength;
}

void sibpresent(uchar c, uchar mod, Operand *o)
{
    uchar _scale;
    uchar _index;
    uchar _base;

    rmsiblength++;
    opcodesz++;
    _scale=c>>6;
    _index=(c&0x38)>>3;
    _base=c&0x07;
    switch(_base) {
        case 0: base(REG_EAX); break;
        case 1: base(REG_ECX); break;
        case 2: base(REG_EDX); break;
        case 3: base(REG_EBX); break;
        case 4: base(REG_ESP); break;
        case 5: if(mod==0) {
                    disp(DISP32);
                    rmsiblength+=4;
                }
                else
                    base(REG_EBP);
                break;
        case 6: base(REG_ESI); break;
        case 7: base(REG_EDI); break;
    }
    if(mod==1) {
        disp(DISP8);
        rmsiblength++;
    }
    else if(mod==2) {
        disp(DISP32);
        rmsiblength+=4;
    }
    switch(_index) {
        case 0: index(REG_EAX); break;
        case 1: index(REG_ECX); break;
        case 2: index(REG_EDX); break;
        case 3: index(REG_EBX); break;
        case 4: index(REG_ESP); break;
        case 5: index(REG_EBP); break;
        case 6: index(REG_ESI); break;
        case 7: index(REG_EDI); break;
    }
    o->mem.scale=1<<_scale;
}