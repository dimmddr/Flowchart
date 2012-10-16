#pragma once

#define PREFIX_LOCK 0xf0
#define PREFIX_ADDRESS_SIZE 0x67

typedef struct {
	UINT8 opcode;
	
	BOOL prefixLock;
	UINT8 prefixLockCount;
	BOOL prefixAddress;
	UINT8 prefixAddressCount;
	
	//для того чтобы можно было отличить случаи, когда значение этих байт = 0x0
	//от случая, когда этих байт нет в инструкции, вводим bool переменные
	BOOL modRM;
	BOOL SIB;
	UINT8 modRMValue;
	UINT8 SIBValue;
	
	UINT displacement;
	UINT immediate;
	//флаг корректности инструкции. Надо еще обдумать его наличие, но пока пусть будет
	BOOL valid;
} INSTRUCTION, *PINSTRUCTION;