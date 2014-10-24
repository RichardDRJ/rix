/*  Drivers */
#include "keyboarddriver.h"
#include "harddiskdriver.h"

/*  I/O */
#include "readfat.h"
#include "kprint.h"
#include "stdin.h"

/*  Memory management */
#include "walloc.h"
#include "palloc.h"

/*  Utilities */
#include "string.h"


#include "interrupts.h"
#include "system.h"
#include "systemcall.h"

extern unsigned char *ptrmemtablestart;
extern unsigned char *ptrmemtableend;

extern unsigned executable_start;
extern unsigned etext;

void setIRQHandlers(void)
{
	irqInstallHandler(1, &handleKeyPress);
	irqInstallHandler(0x60, &systemCall);
	initSystemCalls();
}

void main(void)
{
	setuppalloc((unsigned)&executable_start, (unsigned)&etext, ptrmemtablestart, ptrmemtableend);

	installIDT();
	isrsinstall();
	irqsinstall();
	setIRQHandlers();

	__asm ("sti");

	cls();

	setupstdin();

/*	unsigned char *command;
	command = palloc(200 * sizeof(char));

	void *testpalloc;

	while(stdingets(command, 200))
	{
		char *commandhead = strtok((char*)command, " \n\t\r");

		if(!strcmp(commandhead, "enigma"))
		{
			void (*program)(int, char**) = (void (*)(int, char**))readfile("ENIGMA  BIN");

			int eargv = 0;
			char *argsbuff[40];
			argsbuff[0] = "enigma";

			char nexttokpipe = 0;

			while( (argsbuff[++eargv] = strtok(0, " \n\t\r")) )
			{
				if(nexttokpipe)
				{
					redirect(readfiletoFILE(filenametoshort(argsbuff[eargv])));
					nexttokpipe = 0;
					--eargv;
				}
				if(!strcmp(argsbuff[eargv], "<"))
				{
					--eargv;
					nexttokpipe = 1;
				}
			}

			char **eargc;
			eargc = argsbuff;

			cls();
			(*program)(eargv, eargc);
		}
		else if(!strcmp(commandhead, "palloc"))
		{
			testpalloc = palloc(512);
		}
		else if(!strcmp(commandhead, "free"))
		{
			pfree(testpalloc);
		}
	}

	pfree(command);*/
}
