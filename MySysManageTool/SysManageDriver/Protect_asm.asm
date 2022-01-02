;

;.586
;.model flat,C

;DisableWriteProtect_ PROTO,oldAttr:unsigned long

code ;segment
	DisableWriteProtect_:
		mov eax, oldAttr
		mov cr0, eax
		sti;
		push        ebp   　　　　;ebp=0030FB10 esp=0030FA00
		mov         ebp,esp      ;ebp=0030FA00
		sub         esp,0C0h     ;esp=0030F940
		push        ebx  　　　　 ;保护现场，esp=0030F93C
		push        esi  　　　　 ;esp=0030F938
		push        edi　　　　　　;edi=0030FB10, esp=0030F934
		lea         edi,[ebp-0C0h]  ;edi=0030F940
		mov         ecx,30h   　　　 ;初始化
		mov         eax,0CCCCCCCCh
		rep stos    dword ptr es:[edi]
		mov         eax,dword ptr [a]   ;取参数值，并计算
		add         eax,dword ptr [b]
		add         eax,dword ptr [c]
		ret

	EnableWriteProtect_:
		cli;
		mov  eax, cr0;
		mov  uAttr, eax;
		and  eax, 0FFFEFFFFh; // CR0 16 BIT = 0 
		mov  cr0, eax;
		ret
