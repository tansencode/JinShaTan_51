       ORG 0000H   ;����ʼ��ַ
	   MOV P0,#7FH		;��P0�ڸ�ֵΪ0000 0001
START: MOV A,#7FH		;����ʼֵ��AΪ0000 00001
       CLR C			  ;����־λ��0
	   MOV R2,#7
LOOP:  RLC A		;����λ����
       MOV P0,A
	   CALL DELAY
	   DJNZ R2,LOOP
	   MOV R2,#7

LOOP1: RRC A    	  ;����λ����
       MOV P0,A
	   CALL DELAY
	   DJNZ R2,LOOP1
	   JMP START
       
DELAY: MOV R3,#250
D1:    MOV R4,#100
D2:    MOV R5,#10
       DJNZ R5,$
	   DJNZ R4,D2
	   DJNZ R3,D1
	   RET
	   END