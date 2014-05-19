#ifndef KPRINT_H
#define KPRINT_H

#define CHARSPERLINE 80
#define LINESPERSCREEN 25
#define VIDMEMSTART 0xB8000

void writes(char*);
void writec(char);
void writei(unsigned int i);

void incChar(unsigned short);
void newLine(void);
void cls(void);

void backspace(void);

#endif
