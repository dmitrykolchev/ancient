#include <string.h>
#include "cmdname.h"
#include "cmdcode.h"
#include "deasm.h"

void Group6(uchar *code, Command *cmd);
void Group7(uchar *code, Command *cmd);
void Group8(uchar *code, Command *cmd);
void FPInstruction(uchar *code, Command *cmd);

uchar getmodrm(uchar *, Operand *, uchar);
uchar getreg(uchar, uchar);
void MakeDB(Command *cmd);

extern uchar DataSize;
extern uchar AddrSize;
extern uchar opcodesz;

void protectedmodecmd(uchar *code, Command *cmd)
{
    opcodesz++;
    switch(code[1]) {
        case GRP6:
            Group6(code, cmd);
            break;
        case GRP7:
            Group7(code, cmd);
            break;
        case LAR_Gw_Ew:
            cmd->name=CMDN_LAR;
            cmd->n=2;
            cmd->opsize=OPSZ_WORD;
            cmd->length=2+getmodrm(&code[1], &cmd->op[1], cmd->opsize);
            cmd->op[0].type=TYPE_REG;
            cmd->op[0].reg=getreg(code[2], cmd->opsize);
            break;
        case LSL_Gv_Ew:
            cmd->n=2;
            cmd->opsize=OPSZ_WORD;
            cmd->length=2+getmodrm(&code[1], &cmd->op[1], cmd->opsize);
            cmd->op[0].type=TYPE_REG;
            cmd->op[0].reg=getreg(code[2], OPSZ_WORD*(1+DataSize));
            break;
        case LOADALL286:
        case LOADALL386:
            cmd->name=CMDN_LOADALL;
        LoadAll:
            cmd->n=0;
            cmd->length=2;
            break;
        case CLTS:
            cmd->name=CMDN_CLTS;
            goto LoadAll;
        case INVD:
            cmd->name=CMDN_INVD;
            goto LoadAll;
        case WBINVD:
            cmd->name=CMDN_WBINVD;
            goto LoadAll;
        case MOV_CRd_Rd: cmd->op[0].type=TYPE_CRx; goto MovSpecReg;
        case MOV_DRd_Rd: cmd->op[0].type=TYPE_DRx; goto MovSpecReg;
        case MOV_TRd_Rd: cmd->op[0].type=TYPE_TRx;
        MovSpecReg:
            cmd->name=CMDN_MOV;
            cmd->n=2;
            cmd->length=3;
            getmodrm(&code[1], &cmd->op[1], OPSZ_DWORD);
            cmd->op[0].reg=getreg(code[2], OPSZ_BYTE)>>1;
            break;
        case MOV_Rd_CRd: cmd->op[1].type=TYPE_CRx; goto MovRegSpec;
        case MOV_Rd_DRd: cmd->op[1].type=TYPE_DRx; goto MovRegSpec;
        case MOV_Rd_TRd: cmd->op[1].type=TYPE_TRx;
        MovRegSpec:
            cmd->name=CMDN_MOV;
            cmd->n=2;
            cmd->length=3;
            getmodrm(&code[1], &cmd->op[0], OPSZ_DWORD);
            cmd->op[1].reg=getreg(code[2], OPSZ_BYTE)>>1;
            break;
        case JO_Jv: cmd->name=CMDN_JO; goto JumpCC;
        case JNO_Jv:cmd->name=CMDN_JNO; goto JumpCC;
        case JB_Jv: cmd->name=CMDN_JB ; goto JumpCC;
        case JAE_Jv:cmd->name=CMDN_JAE; goto JumpCC;
        case JZ_Jv: cmd->name=CMDN_JZ ; goto JumpCC;
        case JNZ_Jv:cmd->name=CMDN_JNZ; goto JumpCC;
        case JBE_Jv:cmd->name=CMDN_JBE; goto JumpCC;
        case JA_Jv: cmd->name=CMDN_JA ; goto JumpCC;
        case JS_Jv: cmd->name=CMDN_JS ; goto JumpCC;
        case JNS_Jv:cmd->name=CMDN_JNS; goto JumpCC;
        case JPE_Jv:cmd->name=CMDN_JPE; goto JumpCC;
        case JPO_Jv:cmd->name=CMDN_JPO; goto JumpCC;
        case JL_Jv: cmd->name=CMDN_JL ; goto JumpCC;
        case JGE_Jv:cmd->name=CMDN_JGE; goto JumpCC;
        case JLE_Jv:cmd->name=CMDN_JLE; goto JumpCC;
        case JG_Jv: cmd->name=CMDN_JG ;
        JumpCC:
            cmd->n=1;
            cmd->length=4+DataSize*2;
            cmd->op[0].type=TYPE_REL; cmd->op[0].rel=REL16|DataSize;
            break;
        case SETO:  cmd->name=CMDN_SETO; goto SetCC;
        case SETNO: cmd->name=CMDN_SETNO; goto SetCC;
        case SETB:  cmd->name=CMDN_SETB ; goto SetCC;
        case SETAE: cmd->name=CMDN_SETAE; goto SetCC;
        case SETZ:  cmd->name=CMDN_SETZ ; goto SetCC;
        case SETNZ: cmd->name=CMDN_SETNZ; goto SetCC;
        case SETBE: cmd->name=CMDN_SETBE; goto SetCC;
        case SETA:  cmd->name=CMDN_SETA ; goto SetCC;
        case SETS:  cmd->name=CMDN_SETS ; goto SetCC;
        case SETNS: cmd->name=CMDN_SETNS; goto SetCC;
        case SETPE: cmd->name=CMDN_SETPE; goto SetCC;
        case SETPO: cmd->name=CMDN_SETPO; goto SetCC;
        case SETL:  cmd->name=CMDN_SETL ; goto SetCC;
        case SETGE: cmd->name=CMDN_SETGE; goto SetCC;
        case SETLE: cmd->name=CMDN_SETLE; goto SetCC;
        case SETG:  cmd->name=CMDN_SETG ;
        SetCC:
             cmd->n=1;
             cmd->opsize=OPSZ_BYTE;
             cmd->length=2+getmodrm(&code[1], &cmd->op[0], OPSZ_BYTE);
             break;
        case PUSH_GS: cmd->op[0].reg=REG_GS; goto PushFS;
        case PUSH_FS: cmd->op[0].reg=REG_FS;
        PushFS:
             cmd->name=CMDN_PUSH;
             cmd->length=2;
             cmd->n=1;
             cmd->op[0].type=TYPE_REG;
             break;
        case POP_GS: cmd->op[0].reg=REG_GS; goto PopFS;
        case POP_FS: cmd->op[0].reg=REG_FS;
        PopFS:
             cmd->name=CMDN_POP;
             cmd->length=2;
             cmd->n=1;
             cmd->op[0].type=TYPE_REG;
             break;
        case CMPXCHG_Eb_Gb: cmd->opsize=OPSZ_BYTE; goto CmdCMPXCHG;
        case CMPXCHG_Ev_Gv: cmd->opsize=OPSZ_WORD*(1+DataSize);
        CmdCMPXCHG:
            cmd->name=CMDN_CMPXCHG;
            cmd->n=2;
            cmd->length=2+getmodrm(&code[1], &cmd->op[0], cmd->opsize);
            cmd->op[1].type=TYPE_REG;
            cmd->op[1].reg=getreg(code[2], cmd->opsize);
            break;
        case SHLD_Ev_Gv_Ib: cmd->name=CMDN_SHLD; goto CmdSHLDIb;
        case SHRD_Ev_Gv_Ib: cmd->name=CMDN_SHRD;
        CmdSHLDIb:
            cmd->n=3;
            cmd->opsize=OPSZ_WORD*(1+DataSize);
            cmd->length=3+getmodrm(&code[1], &cmd->op[0], cmd->opsize);
            cmd->op[1].type=TYPE_REG;
            cmd->op[1].reg=getreg(code[2], cmd->opsize);
            cmd->op[2].type=TYPE_IMM; cmd->op[2].imm=IMM8;
            break;
        case SHLD_Ev_Gv_CL: cmd->name=CMDN_SHLD; goto CmdSHLDCL;
        case SHRD_Ev_Gv_CL: cmd->name=CMDN_SHRD;
        CmdSHLDCL:
            cmd->n=3;
            cmd->opsize=OPSZ_WORD*(1+DataSize);
            cmd->length=2+getmodrm(&code[1], &cmd->op[0], cmd->opsize);
            cmd->op[1].type=TYPE_REG;
            cmd->op[1].reg=getreg(code[2], cmd->opsize);
            cmd->op[2].type=TYPE_REG; cmd->op[2].imm=REG_CL;
            break;
        case LSS_Gv_Mp: cmd->name=CMDN_LSS; goto CmdLGS;
        case LFS_Gv_Mp: cmd->name=CMDN_LFS; goto CmdLGS;
        case LGS_Gv_Mp: cmd->name=CMDN_LGS;
        CmdLGS:
            cmd->n=2;
            cmd->opsize=OPSZ_DWORD*(1+DataSize);
            cmd->op[0].type=TYPE_REG;
            cmd->op[0].reg=getreg(code[2], OPSZ_WORD*(1+DataSize));
            cmd->length=2+getmodrm(&code[1], &cmd->op[1], cmd->opsize);
            break;
        case GRP8_Ev_Ib:
            Group8(code, cmd);
            break;
        case IMUL_Gv_Ev: cmd->name=CMDN_IMUL; goto CmdBSR;
        case BSF_Gv_Ev:  cmd->name=CMDN_BSF;  goto CmdBSR;
        case BSR_Gv_Ev:  cmd->name=CMDN_BSR;
        CmdBSR:
            cmd->n=2;
            cmd->opsize=OPSZ_WORD+DataSize*2;
            cmd->length=2+getmodrm(&code[1], &cmd->op[1], cmd->opsize);
            cmd->op[0].type=TYPE_REG;
            cmd->op[0].reg=getreg(code[2], cmd->opsize);
            break;
        case MOVZX_Gv_Eb: cmd->opsize=OPSZ_BYTE; goto CmdMOVZX;
        case MOVZX_Gv_Ew: cmd->opsize=OPSZ_WORD;
        CmdMOVZX:
            cmd->name=CMDN_MOVZX;
        CmdMOVZX1:
            cmd->n=2;
            cmd->length=2+getmodrm(&code[1], &cmd->op[1], cmd->opsize);
            cmd->op[0].type=TYPE_REG;
            cmd->op[0].reg=getreg(code[2], OPSZ_WORD*(1+DataSize));
            break;
        case MOVSX_Gv_Eb: cmd->opsize=OPSZ_BYTE; goto CmdMOVSX;
        case MOVSX_Gv_Ew: cmd->opsize=OPSZ_WORD;
        CmdMOVSX:
            cmd->name=CMDN_MOVSX;
            goto CmdMOVZX1;
        case BTC_Ev_Gv:  cmd->name=CMDN_BTC; goto CmdBTR;
        case BT_Ev_Gv:   cmd->name=CMDN_BT; goto CmdBTR;
        case BTS_Ev_Gv:  cmd->name=CMDN_BTS; goto CmdBTR;
        case BTR_Ev_Gv:  cmd->name=CMDN_BTR;
        CmdBTR:
            cmd->n=2;
            cmd->opsize=OPSZ_WORD+DataSize*2;
            cmd->length=2+getmodrm(&code[1], &cmd->op[0], cmd->opsize);
            cmd->op[1].type=TYPE_REG;
            cmd->op[1].reg=getreg(code[2], cmd->opsize);
            break;
        case XADD_Eb_Gb:
            cmd->opsize=OPSZ_BYTE;
        Xadd:
            cmd->name=CMDN_XADD;
            cmd->n=2;
            cmd->length=2+getmodrm(&code[1], &cmd->op[0], cmd->opsize);
            cmd->op[1].type=TYPE_REG;
            cmd->op[1].reg=getreg(code[2], cmd->opsize);
            break;
        case XADD_Ev_Gv:
            cmd->opsize=OPSZ_WORD*(1+DataSize);
            goto Xadd;
        case BSWAP_eAX: cmd->op[0].reg=REG_AX|DataSize; goto Bswap;
        case BSWAP_eCX: cmd->op[0].reg=REG_CX|DataSize; goto Bswap;
        case BSWAP_eDX: cmd->op[0].reg=REG_DX|DataSize; goto Bswap;
        case BSWAP_eBX: cmd->op[0].reg=REG_BX|DataSize; goto Bswap;
        case BSWAP_eSP: cmd->op[0].reg=REG_SP|DataSize; goto Bswap;
        case BSWAP_eBP: cmd->op[0].reg=REG_BP|DataSize; goto Bswap;
        case BSWAP_eSI: cmd->op[0].reg=REG_SI|DataSize; goto Bswap;
        case BSWAP_eDI: cmd->op[0].reg=REG_DI|DataSize;
        Bswap:
            cmd->name=CMDN_BSWAP;
            cmd->n=1;
            cmd->length=2;
            cmd->op[0].type=TYPE_REG;
            break;
        default:
            MakeDB(cmd);
            break;
    }
}

#pragma argsused

void Group6(uchar *code, Command *cmd)
{
    switch((code[2]&0x38)>>3) {
        case GR6_SLDT_Ew: cmd->name=CMDN_SLDT; goto __1;
        case GR6_STR_Ew:  cmd->name=CMDN_STR;  goto __1;
        case GR6_LLDT_Ew: cmd->name=CMDN_LLDT; goto __1;
        case GR6_LTR_Ew:  cmd->name=CMDN_LTR;  goto __1;
        case GR6_VERR_Ew: cmd->name=CMDN_VERR; goto __1;
        case GR6_VERW_Ew: cmd->name=CMDN_VERW;
        __1:
            cmd->n=1;
            cmd->opsize=OPSZ_WORD;
            cmd->length=2+getmodrm(&code[1], &cmd->op[0], OPSZ_WORD);
            break;
        default:
            MakeDB(cmd);
    }
}

#pragma argsused

void Group7(uchar *code, Command *cmd)
{
    switch((code[2]&0x38)>>3) {
        case GR7_SGDT_Ms: cmd->name=CMDN_SGDT; goto __1;
        case GR7_SIDT_Ms: cmd->name=CMDN_SIDT;  goto __1;
        case GR7_LGDT_Ms: cmd->name=CMDN_LGDT; goto __1;
        case GR7_LIDT_Ms: cmd->name=CMDN_LIDT;  goto __1;
        case GR7_SMSW_Ew: cmd->name=CMDN_SMSW; goto __1;
        case GR7_LMSW_Ew: cmd->name=CMDN_LMSW; goto __1;
        case GR7_INVLPG:  cmd->name=CMDN_INVLPG;
        __1:
            cmd->n=1;
            cmd->opsize=0;
            cmd->length=2+getmodrm(&code[1], &cmd->op[0], OPSZ_WORD);
            break;
        default:
            MakeDB(cmd);
    }
}

#pragma argsused

void Group8(uchar *code, Command *cmd)
{
    uchar c=(code[2]&0x38)>>3;
    cmd->n=2;
    cmd->opsize=OPSZ_WORD*(1+DataSize);
    cmd->length=3+getmodrm(&code[1], &cmd->op[0], cmd->opsize);
    cmd->op[1].type=TYPE_IMM; cmd->op[1].imm=IMM8;
    switch(c) {
        case GR8_BT:  cmd->name=CMDN_BT;  break;
        case GR8_BTS: cmd->name=CMDN_BTS; break;
        case GR8_BTR: cmd->name=CMDN_BTR; break;
        case GR8_BTC: cmd->name=CMDN_BTC; break;
        default:
            MakeDB(cmd);
            break;
    }
}

void FPInstruction(uchar *code, Command *cmd)
{
    uchar c=(code[0]&0x07)<<3;
    c|=(code[1]&0x38)>>3;
    cmd->name=CMDN_ESC;
    cmd->n=2;
    cmd->op[0].type=TYPE_IMMVAL;
    cmd->op[0].imm=c;
    cmd->opsize=0;
    cmd->length=1+getmodrm(code, &cmd->op[1], OPSZ_BYTE);
    if(cmd->op[1].type==TYPE_REG) {
        cmd->op[1].type=TYPE_STREG;
        cmd->op[1].reg>>=1;
    }
}
