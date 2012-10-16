#include <windows.h>
#include <intrin.h>
#pragma intrinsic(__rdtsc)
typedef unsigned __int64 ticks;
#define getticks __rdtsc

#include <imagehlp.h>

#pragma comment(lib, "imagehlp")

#include "flowchart.h"

#define PRINT_ERROR(f, file, line) \
  printf("%s: error %u at %s:%d\n", f, GetLastError(), file, line);
  
#define CURRENT_BYTE (*((PUINT8) g_va))  

#define COUNT 10000 //10 000
#define BYTE 256
#define STARTLINE 16

//global variables
PVOID g_va;
UINT8 b;

void getByte() {
	b = CURRENT_BYTE;
	++((PUINT8) g_va);
}

void initialize(PVOID va) {
	g_va = va;
}

void getAllPrefix(INSTRUCTION *instr) {
	switch(b) {
		case 0xf0: 
			instr->prefixLock = TRUE;
			instr->prefixLockCount++;
			getByte();
			getAllPrefix(instr);
			break;
		case 0x67:
			instr->prefixAddress = TRUE;
			instr->prefixAddressCount++;
			getByte();
			getAllPrefix(instr);
			break;
	}
}

void getOpcode(INSTRUCTION *instr) {
	switch(b) {
		case 0x00:
		case 0x20:
		case 0x08:
		case 0xF6:
			instr->opcode = b;
			instr->valid = TRUE;
			break;
		default:
			instr->valid = FALSE;
	}
}

void getModAndSib(INSTRUCTION *instr) {
	getByte();
	instr->modRM = TRUE;
	instr->modRMValue = b;
	if(instr->prefixAddress) {
		instr->SIB = FALSE;
	} else {
		switch(b) {
			case 0x04:
			case 0x14:
			case 0x24:
			case 0x34:
			case 0x44:
			case 0x54:
			case 0x64:
			case 0x74:
			case 0x84:
			case 0xA4:
			case 0xB4:
			case 0xC4:
			case 0xD4:
			case 0xE4:
			case 0xF4:
			case 0x0C:
			case 0x1C:
			case 0x2C:
			case 0x3C:
			case 0x4C:
			case 0x5C:
			case 0x6C:
			case 0x7C:
			case 0x8C:
			case 0xAC:
			case 0xBC:
			case 0xCC:
			case 0xDC:
			case 0xEC:
			case 0xFC:
				getByte();
				instr->SIB = TRUE;
				instr->SIBValue = b;
				break;
			default:
				instr->SIB = FALSE;
		}
	}
}

void getInstruction(INSTRUCTION *instr) {
	getByte();
	getAllPrefix(instr);
	getOpcode(instr);
	getModAndSib(instr);
}


void print(INSTRUCTION *instr, BOOL qwer) {
	if(instr->valid) {
		if(instr->prefixLock)
			printf("lock ");
		if(instr->prefixAddress)
			printf("adress size ");
		if(qwer) {
			switch(instr->opcode) {
				case 0x00: printf("add ");
					break;
				case 0x20: printf("and ");
					break;
				case 0x08: printf("or ");
					break;
				case 0xf6: printf("not ");
					break;
			}		
		}
		else {
			printf("%0x %0x", instr->opcode, instr->modRMValue);
		}
			printf("\n");
	}
	else
		printf("Unknown instruction. \n");
}


VOID main(INT argc, PSTR argv[])
{
	INT resInstr[COUNT];
	INSTRUCTION instr[COUNT];
	
	LOADED_IMAGE image;
	PSTR imageFilename;
	INT i;
	INT ii;
	PVOID va;
	unsigned __int64 tickCount;
	
	imageFilename = argv[1];
	
	if (!MapAndLoad(imageFilename, NULL, &image, FALSE, TRUE)) {
		PRINT_ERROR("MapAndLoad", __FILE__, __LINE__);
		return;
	}
	va = ImageRvaToVa(image.FileHeader, image.MappedAddress,
						  image.FileHeader->OptionalHeader.BaseOfCode, NULL);
	
	for(ii = 0; ii < COUNT; ++ii) {
		initialize(va);
		_asm{
			lfence
		}
		tickCount = getticks();
		for(i = 0; i < COUNT; ++i) {
			getInstruction(&instr[i]);
			//print(&instr[i], FALSE);
		}
		
		tickCount = (getticks() - tickCount);
		resInstr[ii] = tickCount;
	}
	
	for(ii = 0; ii < COUNT; ++ii) {
		printf("%d \n\r", resInstr[ii]/10000);
	}
	
	/*
	for(i = 0; i < COUNT; ++i) {
		if(0 < resInstr[i]) {
			printf("%d : %d \n", i, resInstr[i]);
			break;
			}
	}
	*/
}