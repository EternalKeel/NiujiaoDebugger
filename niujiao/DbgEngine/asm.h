#pragma once
/*
�������������ش�����
*/
#include <Windows.h>
#include "define.h"
#include "PubLib/StrTrie.h"

#define MAX_ASM_STR_LENGTH 16
class CAsm
{
	static CStrTrie *m_strTrie;
public:
	CAsm();
	static int AsmFromStr(LPCTSTR asmStr, SAsmResultSet* asmResultSet);
	static bool StripStr(char* str); //ȥ���ַ�����β�Ŀհ��ַ�
	static bool RemoveSpace(char* str); //ȥ���ַ�����β�Ŀհ��ַ�
	static bool SplitStr(char*, SAsmStr*); //�ָ����������Ƿ��͸���������
	static bool IsImmValue(char* tmpStr, int* immValue=NULL); //�ж��ַ����Ƿ�Ϊ������  ****
	static bool IsReg(char* tmpStr, int base,int* Reg=NULL); //�ж��ַ����Ƿ�Ϊ�Ĵ���
	static bool IsMemAddressing(char* tmpStr); //�ж��ַ����Ƿ�Ϊ�ڴ�Ѱַ
	static UINT64 GetOpcode(UINT Opcode);


	static bool Asm_None(SAsmStr* asmStr, SAsmResult* asmResult, SInstructFmt* format);
	static int Asm_SIB(char* AsmStr, char** SIBByte); //���� SIB �ֽ�
	static int Asm_ModRm(char* AsmStr,char** ModeRmByte,int base); //����ModRM�ֽ� ���ؽ�����ĳ���
	static bool Asm_Imm(SAsmStr* asmStr, SAsmResult* asmResult, SInstructFmt* format);  //��������Ϊ�������Ļ�����  ****
	static bool Asm_Grp_80_81_82_83(SAsmStr* asmStr, SAsmResult* asmResult, SInstructFmt* format); //��������Ϊ 0x80 81 82 83 �ķ���ָ��
	static bool Asm_Grp_8F(SAsmStr* asmStr, SAsmResult* asmResult, SInstructFmt* format);
	static bool Asm_Grp_C0_C1_D0_D1_D2_D3(SAsmStr* asmStr, SAsmResult* asmResult, SInstructFmt* format);
	static bool Asm_Grp_C6(SAsmStr* asmStr, SAsmResult* asmResult, SInstructFmt* format);
	static bool Asm_Grp_C7(SAsmStr* asmStr, SAsmResult* asmResult, SInstructFmt* format);
	static bool Asm_Grp_F6_F7(SAsmStr* asmStr, SAsmResult* asmResult, SInstructFmt* format);
	static bool Asm_Grp_FE(SAsmStr* asmStr, SAsmResult* asmResult, SInstructFmt* format);
	static bool Asm_Grp_FF(SAsmStr* asmStr, SAsmResult* asmResult, SInstructFmt* format);
	static bool Asm_Grp_0F00(SAsmStr* asmStr, SAsmResult* asmResult, SInstructFmt* format);
	static bool Asm_Grp_0F01(SAsmStr* asmStr, SAsmResult* asmResult, SInstructFmt* format);
	static bool Asm_Grp_0F18(SAsmStr* asmStr, SAsmResult* asmResult, SInstructFmt* format);
	static bool Asm_Grp_0F71(SAsmStr* asmStr, SAsmResult* asmResult, SInstructFmt* format);
	static bool Asm_Grp_0F72(SAsmStr* asmStr, SAsmResult* asmResult, SInstructFmt* format);
	static bool Asm_Grp_0F73(SAsmStr* asmStr, SAsmResult* asmResult, SInstructFmt* format);
	static bool Asm_Grp_0FAE(SAsmStr* asmStr, SAsmResult* asmResult, SInstructFmt* format);
	static bool Asm_Grp_0FB9(SAsmStr* asmStr, SAsmResult* asmResult, SInstructFmt* format);
	static bool Asm_Grp_0FBA(SAsmStr* asmStr, SAsmResult* asmResult, SInstructFmt* format);
	static bool Asm_Grp_0FC7(SAsmStr* asmStr, SAsmResult* asmResult, SInstructFmt* format);
};