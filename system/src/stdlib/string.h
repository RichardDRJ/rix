#ifndef STRING_H
#define STRING_H

char strcmp(char *, char *);

char *strtok(char *, const char *);

char *strchr(const char *, int);

unsigned int strcspn(const char *, const char *);

unsigned int strspn(const char *, const char *);

char strcontains(char*, char);

char *filenametoshort(char*);

char iswhitespace(char);

#endif
