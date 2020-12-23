

SECTION .text
global __readfsdword
__readfsdword:
	mov eax, [esp+4]
	mov eax, [fs:eax]
	ret
	