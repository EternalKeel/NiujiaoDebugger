/*
ʵ�ֻ������
*/
#include "stdafx.h"
#include "asm.h"
#include "asm_struct.h"
#include "Disasm_three_3a.h"
#include <string.h>
#include <Shlwapi.h>

CStrTrie* CAsm::m_strTrie = nullptr;
CAsm::CAsm()
{
	//��ʼ��ǰ׺�� 
	if (m_strTrie == nullptr)
	{
		m_strTrie = new CStrTrie();
		for (int i = 0; i < (sizeof(gAsmInstruct) / sizeof(SAsmInstruct)); i++)
		{
			m_strTrie->TrieAddStr(gAsmInstruct[i].m_Mnemonic, int(gAsmInstruct[i].m_Operand), 0);
		}
	}
}

int CAsm::AsmFromStr(LPCTSTR asmStr, SAsmResultSet* asmResultSet)
{
	if (asmResultSet == nullptr) return -1;
	char AsmStr[1024] = { 0 };
	//����ת�ɶ��ֽ�  ǰ׺���õ��Ƕ��ֽ��ַ�����
	WideCharToMultiByte(CP_ACP, 0, asmStr, lstrlen(asmStr), AsmStr, 1024,nullptr,nullptr);
	if (strlen(AsmStr) == 0) return false;

	//�ֽ����ַ���
	SAsmStr StructAsmStr = { 0 };
	if (SplitStr(AsmStr, &StructAsmStr) == false) return false ;

	//��ȡ�Ѷ�������Ƿ���Ϣ
	int Addr = 0;
	m_strTrie->GetDataInTrie(StructAsmStr.m_Instruct, &Addr, nullptr);

	SInstructFmt *InstructFmt = reinterpret_cast<SInstructFmt*>(Addr);
	for(int i=0;i< MAX_INSTRUCT_NUM;i++) //��������ؽṹ
	{
		bool ProcessResult = false;
		if ((InstructFmt + i)->AsmFunc !=0)
		{
			asmResultSet->m_TotalRecord += 1;
			//TODO ���ָ���Ƿ�֧�ֵ�ǰƽ̨

			SAsmResult TmpResult = { 0 };
			if ((InstructFmt + i)->m_GroupPos == -1) //�����ڷ���ָ��
			{
				//�������������Ƿ�ƥ��
				UINT64 Operand =(InstructFmt + i)->Operand;
				if (GET_OPERAND_NUM(Operand) != StructAsmStr.m_OperandNum)
				{
					(asmResultSet->m_AsmResult + i)->m_ErrorCode = ASM_ERROR_INCORRECT_OPERANDNUM;
					(asmResultSet->m_AsmResult + i)->m_TotalLength = 0;
					asmResultSet->m_FailRecord += 1;
					continue;
				}
				//�����0���������ľͲ���Ҫ����������
				if (StructAsmStr.m_OperandNum == 0)
					ProcessResult = true;
				else
					if ((InstructFmt + i)->AsmFunc == nullptr)
						ProcessResult = false;
					else
						ProcessResult = (InstructFmt + i)->AsmFunc(&StructAsmStr, &TmpResult, InstructFmt + i);
			}
			else
			{
				//����ָ��ж��ԵĴ�������
				ProcessResult = (InstructFmt + i)->AsmFunc(&StructAsmStr, &TmpResult, InstructFmt + i);
			}
			//�ϲ�����Ĳ����룬ǰ׺�ʹ������
			if (ProcessResult == false)
			{
				(asmResultSet->m_AsmResult + i)->m_TotalLength = 0;
				asmResultSet->m_FailRecord += 1;
			}
			else
			{
				//����ǰ׺
				int pos = 0;
			//	if((InstructFmt + i)->Prefix!=0)
				{
			//		(asmResultSet->m_AsmResult + i)->m_Result[pos] = (InstructFmt + i)->Prefix;
			//		pos++;
			//		(asmResultSet->m_AsmResult + i)->m_PrefixLength++;
				}
				//����������
				if((InstructFmt + i)->m_Opcode<=0xFF)
				{
					(asmResultSet->m_AsmResult + i)->m_Result[0] = (InstructFmt + i)->m_Opcode;
					(asmResultSet->m_AsmResult + i)->m_OpcodeLength++;
					pos++;
				}
				else 
				{
					char* TmpOpcode = (char*)&((InstructFmt + i)->m_Opcode);
					if ((InstructFmt + i)->m_Opcode <= 0xFFFF)
					{
						(asmResultSet->m_AsmResult + i)->m_Result[0] = TmpOpcode[1];
						(asmResultSet->m_AsmResult + i)->m_Result[1] = TmpOpcode[0];
						(asmResultSet->m_AsmResult + i)->m_OpcodeLength += 2;
						pos += 2;
					}
					else if ((InstructFmt + i)->m_Opcode <= 0xFFFFFF)
					{
						(asmResultSet->m_AsmResult + i)->m_Result[0] = TmpOpcode[2];
						(asmResultSet->m_AsmResult + i)->m_Result[1] = TmpOpcode[1];
						(asmResultSet->m_AsmResult + i)->m_Result[2] = TmpOpcode[0];
						(asmResultSet->m_AsmResult + i)->m_OpcodeLength += 3;
						pos += 3;
					}
					else if ((InstructFmt + i)->m_Opcode <= 0xFFFFFFFF)
					{
						(asmResultSet->m_AsmResult + i)->m_Result[0] = TmpOpcode[3];
						(asmResultSet->m_AsmResult + i)->m_Result[1] = TmpOpcode[2];
						(asmResultSet->m_AsmResult + i)->m_Result[2] = TmpOpcode[1];
						(asmResultSet->m_AsmResult + i)->m_Result[3] = TmpOpcode[0];
						(asmResultSet->m_AsmResult + i)->m_OpcodeLength += 4;
						pos += 4;
					}
				}
				//����������
				for(int kkk=0; kkk<TmpResult.m_TotalLength;kkk++)
				{
					(asmResultSet->m_AsmResult + i)->m_Result[pos] = TmpResult.m_Result[kkk];
					pos++;
					(asmResultSet->m_AsmResult + i)->m_OperandLength++;
				}

				(asmResultSet->m_AsmResult + i)->m_TotalLength = (asmResultSet->m_AsmResult + i)->m_OpcodeLength + (asmResultSet->m_AsmResult + i)->m_OperandLength;
				asmResultSet->m_SuccessRecord += 1;
			}
		}
		else break; //��Ŀ���ǰ����Ų��� û������ֹ����ѭ��
	}
	return 0;
}

bool CAsm::StripStr(char * str) 
{
	int len = strlen(str);
	int pos1 = 0, pos2 = len-1;
	if (len == 0) return true;
	for (int i = 0; i < len; i++,pos1++)
	{
		if (*(str+pos1) != ' ' && *(str + pos1) != '\t' && *(str + pos1) != '\n')
			break;
	}
	for (int i = len-1; i>-1; i--, pos2--)
	{
		if (*(str + pos2) != ' '&& *(str + pos2) != '\t' &&*(str + pos2) != '\n')
			break;
	}
	if (len == (pos2 - pos1)) return true;
	strncpy(str, str+pos1, pos2 - pos1 +1);
	*(str + pos2 - pos1 + 1) = 0x00;
	return true;
}

bool CAsm::RemoveSpace(char * str)
{
	int len = strlen(str);
	int NonSpace = 0;
	for(int i=0;i<len;i++)
	{
		if(*(str+i)!=' ')
		{
			*(str + NonSpace) = *(str + i);
			NonSpace++;
		}
	}
	return true;
}

bool CAsm::SplitStr(char* str, SAsmStr* StructAsmStr)
{
	char TmpStr[1024] = { 0 };
	strcpy_s(TmpStr, str);
	char * Tmp1 = nullptr;
	bool Flag = true;
	StripStr(TmpStr);
	int Len = strlen(TmpStr);
	//�и����Ƿ�
	bool HasEmptiedSpace = false;
	for(int i=0;i<Len;i++) //��һ���Ƚ�����Ҫ���ַ����
	{
		if (TmpStr[i] == '\t' || TmpStr[i] == '\n' ||  TmpStr[i] == ',')
		{
			TmpStr[i] = 0x00;
			continue;
		}
		if (TmpStr[i] == ' ' && HasEmptiedSpace == false)
		{
			TmpStr[i] = 0x00;
			HasEmptiedSpace = true;
			continue;
		}
	}

	for(int i=0;i<Len;i++) //�ڶ��鿽���ַ�
	{
		if(TmpStr[i]==0x00)
		{
			Flag = true;
			continue;
		}
		if(Flag)
		{
			StructAsmStr->m_OperandNum++;
			if (*(StructAsmStr->m_Instruct) == 0x00) strcpy(StructAsmStr->m_Instruct,TmpStr + i);
			else if (*(StructAsmStr->m_First) == 0x00) strcpy(StructAsmStr->m_First,TmpStr + i);
			else if (*(StructAsmStr->m_Second) == 0x00) strcpy(StructAsmStr->m_Second, TmpStr + i);
			else if (*(StructAsmStr->m_Third) == 0x00) strcpy(StructAsmStr->m_Third,TmpStr + i);
			else if (*(StructAsmStr->m_Forth) == 0x00) strcpy(StructAsmStr->m_Forth,TmpStr + i);
			Flag = false;
		}
	}
	StripStr(StructAsmStr->m_Instruct);
	StripStr(StructAsmStr->m_First);
	StripStr(StructAsmStr->m_Second);
	StripStr(StructAsmStr->m_Third);
	StripStr(StructAsmStr->m_Forth);
	StructAsmStr->m_OperandNum--; //��ȥָ���ж����һ��
	return true;
}

bool CAsm::IsImmValue(char * tmpStr, int * immValue)
{
	char ImmStr[16] = { 0 };
	int len = strlen(tmpStr);
	if (len > 15)
		return false;
	strncpy(ImmStr, tmpStr, len);
	RemoveSpace(ImmStr);
	int Value = 0;
	if (*ImmStr == '0' && (*(ImmStr + 1) == 'x' || *(ImmStr + 1) == 'X')) //ʮ�����Ƹ�ʽ		 
	{
		if (StrToIntExA(ImmStr, STIF_SUPPORT_HEX, &Value) == false)
			return false;
	}
	else if(*ImmStr > '0'&&*ImmStr <= '9')//ʮ���Ƹ�ʽ
	{
		if (StrToIntExA(ImmStr, STIF_DEFAULT, &Value) == false)
			return false;	
	}
	else
		return false;

	if (immValue != NULL)
		*immValue = Value;
	return true;
}

bool CAsm::IsReg(char * tmpStr, int base, int * Reg)
{
	if (tmpStr == nullptr)
		return false;
	for (int i = 16 * base; i < 16; i++)
	{
		if (strcmp(gRegister[i], tmpStr) == 0)
		{
			if (Reg) *Reg = i;
			return true;
		}
	}
	return false;
}

bool CAsm::IsMemAddressing(char * tmpStr)
{
	int pos1 = 0;
	int pos2 = 0;
	int len = strlen(tmpStr);
	for (int i = 0; i < len; i++)
	{
		if (*(tmpStr + i) == '[') pos1 = i;
		else if (*(tmpStr + i) == ']') pos2 = i;
	}
	if (pos2 > pos1) 
		return true;
	return false;
}

UINT64 CAsm::GetOpcode(UINT Opcode)
{
	if((Opcode&0xFF0000) ==0x0F0000)
	{
		if((Opcode&0xFF00)==0x3800)
			return ThreeByteCodeMap38[Opcode & 0xFF].Operand;
		else
			return ThreeByteCodeMap3A[Opcode & 0xFF].Operand;
	}
	else
	{
		if((Opcode&0xFF00)==0x0F00)
			return TwoByteCodeMap[Opcode & 0xFF].Operand;
		else
			return gOneByteCodeMap[Opcode & 0xFF].Operand;		
	}
}

bool CAsm::Asm_None(SAsmStr * asmStr, SAsmResult * asmResult, SInstructFmt* format)
{
	return false;
}

int CAsm::Asm_SIB(char * AsmStr, char ** SIBByte)
{
	//����ַ����ڵĿո�
	
	RemoveSpace(AsmStr);
	int Reg[8] = { 0 }; //����Ĵ��������� eax ecx edx ebx ebp esi edi
	int ImmValue = -1; //��������ֵ
	int SS = 0b00;
	int Index = -1;
	bool IsNonNoneReg = false; //SS�����Ƿ���ڼĴ���
	int PlusNum = 0;
	int len = strlen(AsmStr);
	for(int i=0;i<len;i++) //ͳ�� + ������ 
		if (*(AsmStr + i) == '+') PlusNum += 1;
	if (PlusNum < 0 || PlusNum>2) 
		return 0;
	PlusNum += 1; //һ���Ӻŷָ�ǰ��������  ��������
	for(int i=0;i<PlusNum;i++)  //ѭ�������� + �ŷָ� �ĸ�������
	{
		int StartPos = 0;
		int EndPos = 0;
		for(int j=EndPos;j<len;j++)
		{
			if (*(AsmStr + j) == '+'|| j == len)
			{
				EndPos = j;
				break;
			}		
		}
		//�жϵ�ǰ�����Ƿ�Ϊ������
		if(*(AsmStr+StartPos)=='0'&&(*(AsmStr + StartPos+1) == 'x'|| *(AsmStr + StartPos+1) == 'X') //ʮ�����Ƹ�ʽ
			||(*(AsmStr + StartPos)>'0'&&*(AsmStr+StartPos)<='9') ) //ʮ���Ƹ�ʽ
		{
			char tmpStr[32] = { 0 };
			strncpy(tmpStr, AsmStr + StartPos, EndPos - StartPos);
			if (StrToIntExA(tmpStr + i, STIF_SUPPORT_HEX, &ImmValue) == false)
				return 0;
			Index = 0b100;
		}
		else
		{
			bool ScaleFlag = false;
			//�жϵ�ǰ�Ĵ����Ƿ������������
			for (int j = StartPos; j < EndPos; j++)
			{
				if (*(AsmStr + j) == '*')
				{
					ScaleFlag = true;
					break;
				}
			}
			//�жϵ�ǰ�Ƿ�Ϊ�����Ĵ���
			int* SSReg = NULL;
			if (strncmp(AsmStr + StartPos, "eax", 3) == 0) SSReg = &Reg[e_Eax];
			else if (strncmp(AsmStr + StartPos, "ecx", 3) == 0) SSReg = &Reg[e_Ecx];
			else if (strncmp(AsmStr + StartPos, "edx", 3) == 0) SSReg = &Reg[e_Edx];
			else if (strncmp(AsmStr + StartPos, "ebx", 3) == 0) SSReg = &Reg[e_Ebx];
			else if (strncmp(AsmStr + StartPos, "ebp", 3) == 0) SSReg = &Reg[e_Ebp];
			else if (strncmp(AsmStr + StartPos, "esi", 3) == 0) SSReg = &Reg[e_Esi];
			else if (strncmp(AsmStr + StartPos, "edi", 3) == 0) SSReg = &Reg[e_Edi];
			else return 0;
			if (ScaleFlag)
			{
				//��������Ƿ���ȷ
				char tmpScale = *(AsmStr + StartPos + 1);
				switch(tmpScale)
				{
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '8':
				case '9':
					*SSReg += *(AsmStr + StartPos + 1) - '0';
					break;
				default:
					return 0;
				}
			}
			else
				*SSReg += 1;
			
		}
		StartPos = EndPos + 1;
	}
	//�����ϲ������ֵĽ��
	//������������ļĴ���
	int MaxReg = 4;
	int MediuReg = 4;
	int MinReg = 4; //���ֻ��ͬʱ�������ּĴ��� ʹ�������Ϊ�ж� sib �ַ����ĺϷ���
	for(int i=0;i< 8;i++)
	{
		if (Reg[i] >= Reg[MaxReg])
		{
			MinReg = MediuReg;
			MediuReg = MaxReg;
			MaxReg = i;
		}
	}
	if (MinReg > 0 || MediuReg > 2) return 0;
	//��ȡ index ���ֵ�ֵ
	Index = MaxReg;
	//��ȡ SS r32 ���ֵ�ֵ
	switch(Reg[MaxReg])
	{
	case 0:
		if (ImmValue == -1)
			return 0;
		*SIBByte[0] = 0x25;
		*SIBByte[4] = (ImmValue >> 24);
		*SIBByte[3] = (ImmValue >> 16);
		*SIBByte[2] = (ImmValue >> 8);
		*SIBByte[1] = (ImmValue & 0xFF);
		return 5;
	case 1:
		if ((MediuReg == 4 && ImmValue == -1) || (MediuReg != 4 && ImmValue != -1)) return 0;
		if (MediuReg != 4)
		{
			*SIBByte[0] = (MaxReg << 3) + MediuReg;
			return 1;
		}		
		else
		{
			*SIBByte[0] = (MaxReg << 3) + 0b101;
			*SIBByte[4] = (ImmValue >> 24);
			*SIBByte[3] = (ImmValue >> 16);
			*SIBByte[2] = (ImmValue >> 8);
			*SIBByte[1] = (ImmValue & 0xFF);
			return 5;
		}
	case 2:
		if ((MediuReg == 4 && ImmValue == -1) || (MediuReg != 4 && ImmValue != -1)) return 0;
		if (ImmValue == -1)
		{
			*SIBByte[0] =(0b01<<6)+ (MaxReg << 3) + MediuReg;
			return 1;
		}
		else
		{
			if (MediuReg != e_Ebp||ImmValue>0xFF) 
				return 0;
			*SIBByte[0] = (0b01 << 6) + (MaxReg << 3) + 0b101;
			*SIBByte[1] = (ImmValue & 0xFF);
			return 2;
		}
	case 3: 
		if ((MediuReg == 4 && ImmValue == -1) || (MediuReg != 4 && ImmValue != -1)) return 0;
		if (ImmValue == -1)
		{
			if (MediuReg == e_Ebp)
				return 0;
			*SIBByte[0] = (0b10 << 6) + (MaxReg << 3) + MediuReg;
			return 1;
		}
		else
		{
			if (MaxReg != e_Ebp || MediuReg != e_Ebp || ImmValue > 0xFFFFFFFF)
				return 0;
			*SIBByte[0] = (0b01 << 6) + (MaxReg << 3) + 0b101;
			*SIBByte[1] = (ImmValue & 0xFF);
			return 2;
		}
	case 4:
		if ((MediuReg == 4 && ImmValue == -1) || (MediuReg != 4 && ImmValue != -1)) return 0;
		if (ImmValue == -1)
		{
			*SIBByte[0] = (0b10 << 6) + (MaxReg << 3) + MediuReg;
			return 1;
		}
		else
		{
			if (MediuReg != e_Ebp || ImmValue > 0xFF)
				return 0;
			*SIBByte[0] = (0b10 << 6 )+ (MaxReg << 3) + 0b101;
			*SIBByte[4] = (ImmValue >> 24);
			*SIBByte[3] = (ImmValue >> 16);
			*SIBByte[2] = (ImmValue >> 8);
			*SIBByte[1] = (ImmValue & 0xFF);
			return 5;
		}
	case 5:
		if ((MediuReg == 4 && ImmValue == -1) || (MediuReg != 4 && ImmValue != -1)) return 0;
		if (ImmValue == -1)
		{
			if (MediuReg == e_Ebp)
				return 0;
			*SIBByte[0] = (0b10 << 6) + (MaxReg << 3) + MediuReg;
			return 1;
		}
		else
		{
			if (MaxReg != e_Ebp || MediuReg != e_Ebp || ImmValue > 0xFFFFFFFF)
				return 0;
			*SIBByte[0] = (0b10 << 6) + (MaxReg << 3) + 0b101;
			*SIBByte[4] = (ImmValue >> 24);
			*SIBByte[3] = (ImmValue >> 16);
			*SIBByte[2] = (ImmValue >> 8);
			*SIBByte[1] = (ImmValue & 0xFF);
			return 5;
		}
	case 8:
		if ((MediuReg == 4 && ImmValue == -1) || (MediuReg != 4 && ImmValue != -1)) return 0;
		if (ImmValue == -1)
		{
			*SIBByte[0] = (0b11 << 6) + (MaxReg << 3) + MediuReg;
			return 1;
		}
		else
		{
			if (MediuReg != e_Ebp || ImmValue > 0xFF)
				return 0;
			*SIBByte[0] = (0b11 << 6) + (MaxReg << 3) + 0b101;
			*SIBByte[4] = (ImmValue >> 24);
			*SIBByte[3] = (ImmValue >> 16);
			*SIBByte[2] = (ImmValue >> 8);
			*SIBByte[1] = (ImmValue & 0xFF);
			return 5;
		}
	case 9:
		if ((MediuReg == 4 && ImmValue == -1) || (MediuReg != 4 && ImmValue != -1)) return 0;
		if (ImmValue == -1)
		{
			if (MediuReg == e_Ebp)
				return 0;
			*SIBByte[0] = (0b11 << 6) + (MaxReg << 3) + MediuReg;
			return 1;
		}
		else
		{
			if (MaxReg != e_Ebp || MediuReg != e_Ebp || ImmValue > 0xFFFFFFFF)
				return 0;
			*SIBByte[0] = (0b11 << 6) + (MaxReg << 3) + 0b101;
			*SIBByte[4] = (ImmValue >> 24);
			*SIBByte[3] = (ImmValue >> 16);
			*SIBByte[2] = (ImmValue >> 8);
			*SIBByte[1] = (ImmValue & 0xFF);
			return 5;
		}
	default:
		return 0;
	}
	return 0;
}

int CAsm::Asm_ModRm(char* AsmStr, char ** ModeRmByte, int base)
{
	//dword ptr �� ds: �������ֿ�ѡ
	char tmpStr[MAX_ASM_STR_LENGTH] = { 0 };
	int len = strlen(tmpStr);
	int SegReg = RG__DS; //Ĭ��Ϊds�μĴ���
	bool IsSibByte = false;
	strncpy(tmpStr, AsmStr, len);
	int OffsetLen = -1; //0:��ƫ�� 1:8λƫ�� 2: 32λƫ��
	int ModByte = 0b00;
	int RmByte = -1;
	for(int i=0;i<len;i++)
	{
		if (*(tmpStr + i) == ' ')
		{
			*(tmpStr + i) = 0x00;
			break;
		}
	}
	int pos = 0;
	int endPos = 0;
	//��� ���˲�����λ���Ƿ����
	if (strcmp(tmpStr, "dword") == 0)
	{
		if (base != 2) return 0;
		pos += 5;
	}
	else if(strcmp(tmpStr,"word")==0)
	{
		if (base != 1)return 0;
		pos += 5;
	}
	else if(strcmp(tmpStr,"byte")==0)
	{
		if (base != 0) return 0;
		pos += 5;
	}
	for(int i=pos;*(tmpStr+i);i++)
	{
		if(*(tmpStr+i)=='p'&&*(tmpStr + i + 1) == 't'&&*(tmpStr + i + 1) == 'r')//���Һ����� ptr �ַ�  ����ַ�������û�д˱��
		{
			i += 2;
		}
		else if(*(tmpStr + i) == '[')
		{
			pos = i;
			break;
		}
		else if (*(tmpStr + i) == 'd'&&*(tmpStr + i + 1) == 's'&&*(tmpStr + i + 2) == ':') //���μĴ���
		{
			SegReg = RG__DS;
			i += 2;
		}
		else if (*(tmpStr + i) == 'e'&&*(tmpStr + i + 1) == 's'&&*(tmpStr + i + 2) == ':')
		{
			SegReg = RG__ES;
			i += 2;
		}
		else if (*(tmpStr + i) == 's'&&*(tmpStr + i + 1) == 's'&&*(tmpStr + i + 2) == ':')
		{
			SegReg = RG__SS;
			i += 2;
		}
		else if (*(tmpStr + i) == 'g'&&*(tmpStr + i + 1) == 's'&&*(tmpStr + i + 2) == ':')
		{
			SegReg = RG__GS;
			i += 2;
		}
		else if (*(tmpStr + i) == 'f'&&*(tmpStr + i + 1) == 's'&&*(tmpStr + i + 2) == ':')
		{
			SegReg = RG__FS;
			i += 2;
		}
	}
	//����β�� ] 
	for(int i=pos;i<len;i++)
	{
		if(*(tmpStr+i)==']')
		{
			endPos = i;
			break;
		}
		else if(*(tmpStr+i)=='+' || *(tmpStr + i) == '*')
		{
			IsSibByte = true; //�������ڴ��� + ���� * �� �����Ϊsib�ֽ�
		}
		else if(*(tmpStr + i) == 0x00) //û��ƥ��� ] ����
		{
			return false;
		}
	}
	//�������Ƿ����������ƫ��
	bool PlusFlag = false; //�Ӻű�־
	int ImmOperand = 0;
	for(int i=(endPos+1);i<len;i++)
	{
		if (*(tmpStr + i) == '+') PlusFlag = true;
		if(PlusFlag&&*(tmpStr+i)=='0'&&(*(tmpStr+i)=='x'|| *(tmpStr + i) == 'X'))
		{
			if (StrToIntExA(tmpStr + i, STIF_SUPPORT_HEX, &ImmOperand) == false)
				return false;			
		}
	}
	if(PlusFlag==false) //û��ƫ�Ƶ����
	{
		ModByte = 0b00;
	}
	else
	{
		if ((UINT)ImmOperand > 0xFF) //32λƫ��
			ModByte = 0b10;
		else
			ModByte = 0b01;
	}
	char tmpModRm[32] = { 0 };
	strncpy(tmpModRm, AsmStr + pos, endPos - pos);
	StripStr(tmpModRm);
	if(IsSibByte) //�����������ڵ�����
	{
		RmByte = 0b100;
		char SibByte[14] = { 0 };
		if (Asm_SIB(tmpModRm, (char**)&SibByte) == 0)
			return 0;
	}
	else
	{
		if (strcmp(tmpModRm, "eax") == 0) RmByte=0b000;
		else if (strcmp(tmpModRm, "ecx") == 0) RmByte=0b001;
		else if (strcmp(tmpModRm, "edx") == 0) RmByte=0b010;
		else if (strcmp(tmpModRm, "ebx") == 0) RmByte=0b011;
		else if (strcmp(tmpModRm, "esi") == 0) RmByte=0b110;
		else if (strcmp(tmpModRm, "edi") == 0) RmByte=0b111;
		else if ((ModByte == 0b01 || ModByte == 0b10) && (strcmp(tmpModRm, "ebp") == 0)) RmByte = 0b101;
		else if (ModByte=0b00&& StrToIntExA(tmpModRm, STIF_SUPPORT_HEX, &ImmOperand)) RmByte = 0b101;
		else //�쳣���
		{
			return false;
		}
	}
	return 0;
}

bool CAsm::Asm_Imm(SAsmStr * asmStr, SAsmResult * asmResult, SInstructFmt* format)
{
	//ֻ����ֻ��һ������������Ϊ�����������
	int ImmOperand = 0;	
	if (IsImmValue(asmStr->m_First, &ImmOperand) == false)
		return false;
	//����������С�Ƿ����
	switch(GET_OPERAND_TYPE(format->Operand,0))
	{
	case OT__b:
		if (ImmOperand > 0xFF) 
			return false;
		asmResult->m_Result[0] = ImmOperand;
		asmResult->m_TotalLength = 1;
		return true;
	case OT__w:
		if (ImmOperand > 0xFFFF) 
			return false;
		asmResult->m_Result[0] = (ImmOperand >> 8);
		asmResult->m_Result[1] = (ImmOperand &0xFF);
		asmResult->m_TotalLength = 2;
		return true;
	case OT__d:
		if (ImmOperand > 0xFFFFFFFF) 
			return false;
		asmResult->m_Result[3] = (ImmOperand >> 24);
		asmResult->m_Result[2] = (ImmOperand >> 16);
		asmResult->m_Result[1] = (ImmOperand >> 8);
		asmResult->m_Result[0] = (ImmOperand & 0xFF);
		asmResult->m_TotalLength = 4;
		return true;
	case OT__q:break;
	}
	return false;;
}

bool CAsm::Asm_Grp_80_81_82_83(SAsmStr * asmStr, SAsmResult * asmResult, SInstructFmt* format)
{
	char Mnemonic[][4] = { "add","or","adc","sbb","and","sub","xor","cmp" };
	char RegByte =  - 1;
	char ModRmByte[32] = { 0 };
	char MnemonicByte = -1;
	int base = 0;
	switch(format->m_Opcode)
	{
	case 0x80: base = 0; break;
	case 0x81: base = 2; break;
	case 0x82: base = 0; break;
	case 0x83: base = 2; break;
	}
	for (int i = 0; i < 8; i++) //����һ���������Ƿ�Ϊ�Ĵ���
	{
		if (strcmp(gRegister[i+base*16], asmStr->m_First) == 0)
		{
			RegByte = i;
			break;
		}
		if (i == 7)
		{
			return false; //����һ���������Ƿ�Ϊ�ڴ�
		}
	}
	for (int i = 0; i < 8; i++)
	{
		if (strcmp(Mnemonic[i], asmStr->m_Instruct) == 0)
		{
			MnemonicByte = i;
			break;
		}
		if (i == 7)
		{
			return false;
		}
	}
	int ImmOperand = 0;
	if (asmStr->m_Second[0] == '0' && (asmStr->m_Second[1] == 'x' || asmStr->m_Second[1] == 'X'))
	{
		if (StrToIntExA(asmStr->m_Second, STIF_SUPPORT_HEX, &ImmOperand) == false)
			return false;
	}
	else
	{
		if (StrToIntExA(asmStr->m_Second, STIF_DEFAULT, &ImmOperand) == false)
			return false;
	}
	asmResult->m_Result[0] = 0b11000000 + (MnemonicByte << 3) + RegByte;
	switch(format->m_Opcode)
	{
	case 0x80:
	case 0x82:
	case 0x83:
		if (ImmOperand > 0xFF)
			return false;
		asmResult->m_Result[1] = ImmOperand & 0xFF;
		asmResult->m_TotalLength = 2;
		return true;
	case 0x81:
		if (ImmOperand > 0xFFFFFFFF)
			return false;
		asmResult->m_Result[4] = (ImmOperand >> 24);
		asmResult->m_Result[3] = (ImmOperand >> 16);
		asmResult->m_Result[2] = (ImmOperand >> 8);
		asmResult->m_Result[1] = (ImmOperand & 0xFF);
		asmResult->m_TotalLength = 5;
		return true;
	}
	return false;
}

bool CAsm::Asm_Grp_8F(SAsmStr * asmStr, SAsmResult * asmResult, SInstructFmt* format)
{
	//�������ֻ��popָ������
	char RegByte = -1;
	for (int i = 0; i < 8; i++)
	{
		if (strcmp(gRegister[i + 2 * 16], asmStr->m_First) == 0)
		{
			RegByte = i;
			break;
		}
		if (i == 7) return false;
	}
	asmResult->m_Result[0] = 0b11000000  + RegByte;
	asmResult->m_TotalLength = 1;
	return true;
}

bool CAsm::Asm_Grp_C0_C1_D0_D1_D2_D3(SAsmStr * asmStr, SAsmResult * asmResult, SInstructFmt* format)
{
	char Mnemonic[][4] = { "rol","ror","rcl","rcr","shl","shr","","sar" };
	char RegByte = -1;
	char MnemonicByte = -1;
	int base = 0;
	switch (format->m_Opcode)
	{
	case 0xc0:
	case 0xd0:
	case 0xd2:
		base = 0; break;
	case 0xc1:
	case 0xd1:
	case 0xd3:
		base = 2; break;
	}
	for (int i = 0; i < 8; i++) //���ҵ�һ��������
	{
		if (strcmp(gRegister[i + base * 16], asmStr->m_First) == 0)
		{
			RegByte = i;
			break;
		}
		if (i == 7)
		{
			return false;
		}
	}
	for (int i = 0; i < 8; i++) //�������Ƿ�
	{
		if (strcmp(Mnemonic[i], asmStr->m_Instruct) == 0)
		{
			MnemonicByte = i;
			break;
		}
		if (i == 7)
		{
			return false;
		}
	}
	//���ڶ���������
	int ImmOperand = 0;
	if(format->m_Opcode==0xd2 || format->m_Opcode==0xd3) //�ڶ���������Ϊ�Ĵ��� cl
	{
		if (strcmp(asmStr->m_Second, "cl") != 0)
			return false;
	}
	else
	{
		if (asmStr->m_Second[0] == '0' && (asmStr->m_Second[1] == 'x' || asmStr->m_Second[1] == 'X'))
		{
			if (StrToIntExA(asmStr->m_Second, STIF_SUPPORT_HEX, &ImmOperand) == false)
				return false;
		}
		else
		{
			if (StrToIntExA(asmStr->m_Second, STIF_DEFAULT, &ImmOperand) == false)
				return false;
		}		
	}
	if(format->m_Opcode==0xd0 || format->m_Opcode==0xd1) //�ڶ���������Ϊ������ 1
	{
		if (ImmOperand != 1)
			return false;
	}
	asmResult->m_Result[0] = 0b11000000 + (MnemonicByte << 3) + RegByte;
	asmResult->m_TotalLength = 1;
	if (format->m_Opcode == 0xc0 || format->m_Opcode == 0xc1) //�ڶ���������Ϊһ���ֽڵ������� 
	{
		if (ImmOperand > 0xFF)
			return false;
		asmResult->m_Result[1] = ImmOperand & 0xFF;
		asmResult->m_TotalLength = 2;
	}
	return true;
}


bool CAsm::Asm_Grp_C6(SAsmStr * asmStr, SAsmResult * asmResult, SInstructFmt* format)
{
	return false;
}

bool CAsm::Asm_Grp_C7(SAsmStr * asmStr, SAsmResult * asmResult, SInstructFmt* format)
{
	return false;
}

bool CAsm::Asm_Grp_F6_F7(SAsmStr * asmStr, SAsmResult * asmResult, SInstructFmt* format)
{
	char Mnemonic[][8] = { "test","","not","neg","mul","imul","div","idiv" };
	char RegByte = -1;
	char MnemonicByte = -1;
	int base = 0;
	switch (format->m_Opcode)
	{
	case 0xF6: base = 0; break;
	case 0xF7: base = 2; break;
	}
	for (int i = 0; i < 8; i++) //���ҵ�һ��������
	{
		if (strcmp(gRegister[i + base * 16], asmStr->m_First) == 0)
		{
			RegByte = i;
			break;
		}
		if (i == 7)
		{
			return false;
		}
	}
	for (int i = 0; i < 8; i++) //�������Ƿ�
	{
		if (strcmp(Mnemonic[i], asmStr->m_Instruct) == 0)
		{
			MnemonicByte = i;
			break;
		}
		if (i == 7)
		{
			return false;
		}
	}
	//���ڶ���������
	asmResult->m_Result[0] = 0b11000000 + (MnemonicByte << 3) + RegByte;
	asmResult->m_TotalLength = 1;
	//���������Ƿ�û�еڶ���������
	if (MnemonicByte == 2 || MnemonicByte == 3) return true;
	//���ĸ����Ƿ��ĵڶ���������ָ��Ϊ�Ĵ��� al ���� rAX
	if (MnemonicByte == 4 || MnemonicByte == 5 || MnemonicByte == 6 || MnemonicByte == 7) 
	{
		return strcmp(asmStr->m_Second, gRegister[0 + base * 16]) == 0;
	}
	//������Ƿ��ĵڶ���������Ϊ������
	if(MnemonicByte==0)
	{
		int ImmOperand = 0;
		if (asmStr->m_Second[0] == '0' && (asmStr->m_Second[1] == 'x' || asmStr->m_Second[1] == 'X'))
		{
			if (StrToIntExA(asmStr->m_Second, STIF_SUPPORT_HEX, &ImmOperand) == false)
				return false;
		}
		else
		{
			if (StrToIntExA(asmStr->m_Second, STIF_DEFAULT, &ImmOperand) == false)
				return false;
		}
		if(base==0)
		{
			if (ImmOperand > 0xFF)
				return false;
			asmResult->m_Result[1] = ImmOperand & 0xFF;
			asmResult->m_TotalLength = 2;
			return true;
		}
		else if(base==2)
		{
			if (ImmOperand > 0xFFFFFFFF)
				return false;
			asmResult->m_Result[4] = (ImmOperand >> 24);
			asmResult->m_Result[3] = (ImmOperand >> 16);
			asmResult->m_Result[2] = (ImmOperand >> 8);
			asmResult->m_Result[1] = (ImmOperand & 0xFF);
			asmResult->m_TotalLength = 5;
			return true;		
		}
	}
	return false;
}

bool CAsm::Asm_Grp_FE(SAsmStr * asmStr, SAsmResult * asmResult, SInstructFmt* format)
{
	char Mnemonic[][8] = { "inc","dec","","","","","","" };
	char RegByte = -1;
	char MnemonicByte = -1;
	for (int i = 0; i < 8; i++) //���ҵ�һ��������
	{
		if (strcmp(gRegister[i], asmStr->m_First) == 0)
		{
			RegByte = i;
			break;
		}
		if (i == 7)
		{
			return false;
		}
	}
	for (int i = 0; i < 8; i++) //�������Ƿ�
	{
		if (strcmp(Mnemonic[i], asmStr->m_Instruct) == 0)
		{
			MnemonicByte = i;
			break;
		}
		if (i == 7)
		{
			return false;
		}
	}
	//���ڶ���������
	asmResult->m_Result[0] = 0b11000000 + (MnemonicByte << 3) + RegByte;
	asmResult->m_TotalLength = 1;
	return true;
}

bool CAsm::Asm_Grp_FF(SAsmStr * asmStr, SAsmResult * asmResult, SInstructFmt* format)
{
	return false;
}

bool CAsm::Asm_Grp_0F00(SAsmStr * asmStr, SAsmResult * asmResult, SInstructFmt* format)
{
	return false;
}

bool CAsm::Asm_Grp_0F01(SAsmStr * asmStr, SAsmResult * asmResult, SInstructFmt* format)
{
	return false;
}

bool CAsm::Asm_Grp_0F18(SAsmStr * asmStr, SAsmResult * asmResult, SInstructFmt* format)
{
	return false;
}

bool CAsm::Asm_Grp_0F71(SAsmStr * asmStr, SAsmResult * asmResult, SInstructFmt* format)
{
	return false;
}

bool CAsm::Asm_Grp_0F72(SAsmStr * asmStr, SAsmResult * asmResult, SInstructFmt* format)
{
	return false;
}

bool CAsm::Asm_Grp_0F73(SAsmStr * asmStr, SAsmResult * asmResult, SInstructFmt* format)
{
	return false;
}

bool CAsm::Asm_Grp_0FAE(SAsmStr * asmStr, SAsmResult * asmResult, SInstructFmt* format)
{
	return false;
}

bool CAsm::Asm_Grp_0FB9(SAsmStr * asmStr, SAsmResult * asmResult, SInstructFmt* format)
{
	return false;
}

bool CAsm::Asm_Grp_0FBA(SAsmStr * asmStr, SAsmResult * asmResult, SInstructFmt* format)
{
	return false;
}

bool CAsm::Asm_Grp_0FC7(SAsmStr * asmStr, SAsmResult * asmResult, SInstructFmt* format)
{
	return false;
}
