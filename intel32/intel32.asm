;Ϊ vt_driver �ṩ32λ�ĺ������ÿ�
.686
.model flat,c
.code

asm_GetCpuVendor PROC 
	mov eax,0
	cpuid
	mov eax,dword ptr [esp+4]
	mov dword ptr [eax],ebx
	mov eax,dword ptr [esp+8]
	mov dword ptr [eax],ecx
	mov eax,dword ptr [esp+12]
	mov dword ptr [eax],edx
	ret
asm_GetCpuVendor ENDP
;������Դ��amd�ֲ� 24594.pdf 195ҳ

asm_IsCpuSupportedCPUID PROC
	pushfd
	pop eax
	mov ebx,eax
	xor eax,00200000h
	push eax
	popfd
	pushfd
	pop eax
	cmp eax,ebx
	jz NO_CPUID
	mov eax,1
	ret
NO_CPUID:
	mov eax,0
	ret
asm_IsCpuSupportedCPUID ENDP

;intel�ֲ� 781ҳ
asm_IsCpuSupportedVMX PROC
	mov eax,1
	cpuid
	xor ecx,10000b	
	jz NO_VMX
	mov eax,1
	ret
NO_VMX:
	mov eax,0
	ret
asm_IsCpuSupportedVMX ENDP
;intel�ֲ� 764ҳ
asm_GetLogicProcessNum PROC
	mov eax,1
	cpuid
	shr ebx,8h
	and ebx,00FFh
	mov eax,ebx
	ret
asm_GetLogicProcessNum ENDP
end
