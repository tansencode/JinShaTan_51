       ORG 0000H   ;程序开始地址
	   MOV P0,#7FH		;将P0口赋值为0000 0001
START: MOV A,#7FH		;赋初始值，A为0000 00001
       CLR C			  ;将标志位清0
	   MOV R2,#7
LOOP:  RLC A		;带进位左移
       MOV P0,A
	   CALL DELAY
	   DJNZ R2,LOOP
	   MOV R2,#7

LOOP1: RRC A    	  ;带进位右移
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