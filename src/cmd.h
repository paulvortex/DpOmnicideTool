// cmd.h
#ifndef H_CMD_H
#define H_CMD_H

// system headers
#include <sys/types.h>
#include <sys/stat.h>
#if defined(WIN32) || defined(_WIN64)
	#include <limits.h>
	#include <direct.h>
	#include <windows.h>
#endif
#include <stdio.h>
#include <io.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <algorithm>
#include <string>
#include <vector>
#include <cmath>

// Data types
using namespace std;
typedef unsigned char  byte;
typedef unsigned short ushort;
typedef unsigned long  ulong;
typedef unsigned int   uint;

// Strings
size_t strlcat(char *dst, const char *src, size_t siz);
size_t strlcpy(char *dst, const char *src, size_t siz);
int strncasecmp (char *s1, char *s2, int n);
int strcasecmp (char *s1, char *s2);
char *strunescape(const char *src);
char *strstrc(char *s1, char c);
size_t strcount(char *s, char c);
size_t strncount(char *s, char c, size_t len);

// Console stuff
extern bool verbose;
extern bool noprint;
extern bool solidpacifier;
void Print(char *str, ...);
void Verbose(char *str, ...);
void PercentPacifier(char *str, ...);
void Pacifier(char *format, ...);
void SimplePacifier();
void PacifierEnd();
void Warning(char *str, ...);
void WaitForKey();
void Error(char *error, ...);

// Time
double TimeDouble(void);

// CreatePath
void CreatePath(char *createpath);

// CheckParm
extern int	myargc;
extern char **myargv;
bool CheckParm(char *check);

// Files
unsigned int filelength(FILE *f);
size_t FileSize(const char *filename);
FILE *SafeOpen (char *filename, char mode[]);
void SafeRead (FILE *f, void *buffer, int count);
void SafeWrite(FILE *f, void *buffer, int count);
int LoadFile(char *filename, byte **bufferptr);
size_t LoadFileUnsafe(char *filename, byte **bufferptr);
void SaveFile(char *filename, byte *buffer, size_t size);

// File path
void DefaultPath (char *path, char *basepath);
void ReplaceExtension (char *path, char *oldextension, char *replacementextension, char *missingextension);
void DefaultExtension (char *path, const char *extension, size_t size_path);
void ExtractFilePath (char *path, char *dest);
void ExtractFileBase (char *path, char *dest);
void ExtractFileName(char *path, char *dest);
void ExtractFileSuffix(char *path, char *dest, char suffix_sym);
void StripFileExtension(char *path, char *dest);
void ExtractFileExtension (char *path, char *dest);
void AddSlash(char *path);
void AddSlash(string &path);

// Parse
int ParseHex(char *hex);
int ParseNum(char *str);
void ParseTokens(const char *tokenize_string, vector<string> &separators, vector<string> &tokens, size_t max_tokens, bool clear_tokens_array, bool trim_tokens);
void ParseTokens(const char *tokenize_string, const char *separator, vector<string> &tokens, size_t max_tokens, bool clear_tokens_array, bool trim_tokens);

#endif