////////////////////////////////////////////////////////////////
//
// DpOmniTool / Commandline Functions Library
// (c) Pavel [VorteX] Timofeyev
// based on functions picked from Darkplaces engine
// See LICENSE text file for a license agreement
//
////////////////////////////////

#include "cmd.h"
#include "dpomnitool.h"
#include "thread.h"
#include "mem.h"

/*
=================
 Strings
=================
*/

size_t strlcat(char *dst, const char *src, size_t siz)
{
	register char *d = dst;
	register const char *s = src;
	register size_t n = siz;
	size_t dlen;

	while(n-- != 0 && *d != '\0')
		d++;
	dlen = d - dst;
	n = siz - dlen;
	if (n == 0)
		return(dlen + strlen(s));
	while (*s != '\0')
	{
		if (n != 1)
		{
			*d++ = *s;
			n--;
		}
		s++;
	}
	*d = '\0';
	return (dlen + (s - src));
}

size_t strlcpy(char *dst, const char *src, size_t siz)
{
	register char *d = dst;
	register const char *s = src;
	register size_t n = siz;

	if (n != 0 && --n != 0)
	{
		do 
		{
			if ((*d++ = *s++) == 0)
				break;
		} while (--n != 0);
	}
	if (n == 0) 
	{
		if (siz != 0)
			*d = '\0';
		while(*s++)
			;
	}
	return (s - src - 1);
}

int strncasecmp (char *s1, char *s2, int n)
{
	int		c1, c2;
	while (1)
	{
		c1 = *s1++;
		c2 = *s2++;
		if (!n--)
			return 0;
		if (c1 != c2)
		{
			if (c1 >= 'a' && c1 <= 'z')
				c1 -= ('a' - 'A');
			if (c2 >= 'a' && c2 <= 'z')
				c2 -= ('a' - 'A');
			if (c1 != c2)
				return -1;
		}
		if (!c1)
			return 0;
	}
	return -1;
}

int strcasecmp (char *s1, char *s2)
{
	return strncasecmp (s1, s2, 99999);
}

char *strunescape(const char *src)
{
	size_t rlen = strlen(src);
	char *rstr = (char *)mem_alloc(rlen + 1);
	char *dst = rstr;
	while(*src)
	{
		if (!strncmp(src, "\\n", 2))
		{
			*dst++ = '\n';
			src += 2;
			continue;
		}
		else if (!strncmp(src, "\\r", 2))
		{
			*dst++ = '\r';
			src += 2;
			continue;
		}
		*dst++ = *src;
		src++;
	}
	*dst = '\0';
    return rstr;
}

char *strstrc(char *s1, char c)
{
	char s[2];

	if (!s1)
		return NULL;
	s[0] = c;
	s[1] = '\0';
	return strstr(s1, s);
}

size_t strcount(char *s, char c)
{
	size_t num = 0;
	for ( ; *s; s++)
		if (*s == c)
			num++;
	return num;
}

size_t strncount(char *s, char c, size_t len)
{
	size_t i, num = 0;
	for (i = 0; *s && i < len; i++, s++)
		if (*s == c)
			num++;
	return num;
}

/*
=================
 Console stuff
=================
*/

bool verbose;
bool noprint;
bool solidpacifier;

void Print(char *str, ...)
{
	va_list argptr;

	if (noprint)
		return;

	va_start(argptr, str);
	vprintf(str, argptr);
	va_end(argptr);
}

void Verbose(char *str, ...)
{
	va_list argptr;

	if (!verbose || noprint)
		return;

	va_start(argptr, str);
	vprintf(str, argptr);
	va_end(argptr);
}

void PercentPacifier(char *str, ...)
{
	va_list argptr;
	char msg[10000];

	va_start(argptr, str);
	vsprintf(msg, str, argptr);
	va_end(argptr);
	if (!solidpacifier)
		printf("\r%s%%\r", msg);
	else
		printf("\r%s%%                                      \r", msg);
	fflush(stdout);
}

void Pacifier(char *format, ...)
{
	va_list argptr;
	char msg[10000];

	va_start(argptr, format);
	vsprintf(msg, format, argptr);
	va_end(argptr);
	printf("\r%s                                        \r", msg);
	fflush(stdout);
}

char SimplePacifierChars[5] = "-\\|/";
int SimplePacifierCharNum = 0;

void SimplePacifier()
{
	if (noprint)
		return;

	SimplePacifierCharNum++;
	if (SimplePacifierCharNum >= 4)
		SimplePacifierCharNum = 0;
	printf("\r  %c                                        \r", SimplePacifierChars[SimplePacifierCharNum]);
	fflush(stdout);
}

void PacifierEnd() 
{
	if (noprint)
		return;

	printf("\n");
}

void Warning(char *str, ...)
{
	va_list argptr;
	char msg[10000];

	va_start(argptr, str);
	vsprintf(msg, str, argptr);
	va_end(argptr);
	printf("Warning: %s\n", msg);
}

void WaitForKey()
{
	#if _MSC_VER
	if (waitforkey)
	{
		printf("press any key\n");
		getchar();
	}
	#endif
}

void Error(char *error, ...)
{
	va_list argptr;
	char logfile[MAX_FPATH], msgstr[10384];
	FILE *f;

	va_start(argptr, error);
	vsprintf(msgstr, error, argptr);
	va_end(argptr);
	printf("\n*** ERROR ***\n%s\n", msgstr);
	if (errorlog)
	{
		sprintf(logfile, "%sberror.txt", progpath);
		f = fopen(logfile, "wb");
		if (f)
		{
			fwrite(msgstr, strlen(msgstr), 1, f);
			fclose(f);
		}
	}
	WaitForKey();
	exit(1);
}

/*
================
 Time
================
*/

double TimeDouble(void)
{
#if defined(WIN32) || defined(_WIN64)
	static DWORD starttime;
	static bool first = true;
	DWORD now;

	if (first)
		timeBeginPeriod (1);
	now = timeGetTime ();
	if (first)
	{
		first = false;
		starttime = now;
		return 0.0;
	}
	if (now < starttime)
		return (now / 1000.0) + (LONG_MAX - starttime / 1000.0);
	if (now - starttime == 0)
		return 0.0;
	return (now - starttime) / 1000.0;
#else
	time_t	t;
	time (&t);
	return t;
#endif
}

/*
============
 CreatePath
============
*/

void CreatePath(char *createpath)
{
	char *ofs, save, *opath;
	char path[MAX_FPATH];

	strncpy(path, createpath, sizeof(path)); 
	opath = path;
	for (ofs = path+1 ; *ofs ; ofs++)
	{
		if (*ofs == '/' || *ofs == '\\')
		{
			// create the directory
			save = *ofs;
			*ofs = 0;
			if (path[0] && path[strlen(path)-1] != ':' && strcmp(path, "..") && strcmp(path, "."))
			{
				#if defined(WIN32) || defined(_WIN64)
				  if (_mkdir (path) != -1)
				#else
				  if (mkdir (path, 0777) != -1)
				#endif
				if (errno != 0 && errno != EEXIST)
					Error ("CreatePath '%s': %s %i", opath, strerror(errno), errno);
			}
			*ofs = save;
		}
	}
}

/*
=================
 CheckParm
=================
*/

int	myargc;
char **myargv;
bool CheckParm(char *check)
{
	char *str, *next, parm[64];
	int i;

	// check exe name
	str = strstr(progname, "-");
	if (!strcmp(check, "-"))
		return (str == NULL) ? false : true;
	while(str)
	{
		next = strstr(str + 1, "-");
		if (next)
		{
			strncpy(parm, str, (int)(next - str));
			parm[(int)(next - str)] = 0;
		}
		else
		{
			strcpy(parm, str);
			parm[strlen(str)] = 0;
		}
		if (!strcasecmp(check, parm))
			return true;
		str = next;
	}
	// check commandline parms
	for (i = 1;i < myargc;i++)
		if (!strcasecmp(check, myargv[i]))
			return true;
	return 0;
}

/*
================
 Files
================
*/

unsigned int filelength (FILE *f)
{
	int pos;
	int end;

	pos = ftell (f);
	fseek (f, 0, SEEK_END);
	end = ftell (f);
	fseek (f, pos, SEEK_SET);
	return end;
}

size_t FileSize(const char *filename)
{
	FILE *f;
	size_t s;

	f = fopen(filename, "rb");
	if (!f)
		return 0;
	fseek(f, 0, SEEK_END);
	s = ftell(f);
	fclose(f);
	return s;
}

FILE *SafeOpen (char *filename, char mode[])
{
	FILE *f;
	f = fopen(filename, mode);
	if(!f)
		Error("SafeOpen %s: %s", filename, strerror(errno));
	return f;
}

void SafeRead (FILE *f, void *buffer, int count)
{
	if (fread (buffer, 1, count, f) != (size_t)count)
		Error("File read failure");
}

void SafeWrite(FILE *f, void *buffer, int count)
{
	if (fwrite(buffer, 1, count, f) != (size_t)count)
		Error("File write failure");
}

int LoadFile(char *filename, byte **bufferptr)
{
	FILE *f;
	int length;
	byte *buffer;

	f = SafeOpen(filename, "rb");
	length = filelength(f);
	buffer = (byte *)mem_alloc(length+1);
	((char *)buffer)[length] = 0;
	SafeRead (f, buffer, length);
	fclose(f);

	*bufferptr = buffer;
	return length;
}

size_t LoadFileUnsafe(char *filename, byte **bufferptr)
{
	FILE *f;
	size_t length;
	byte *buffer;

	f = fopen(filename, "rb");
	if (!f)
	{
		*bufferptr = NULL;
		return 0;
	}
	length = filelength (f);
	buffer = (byte *)mem_alloc(length+1);
	((char *)buffer)[length] = 0;
	SafeRead (f, buffer, length);
	fclose(f);
	*bufferptr = buffer;
	return length;
}

void SaveFile(char *filename, byte *buffer, size_t size)
{
	FILE *f;

	f = SafeOpen(filename, "wb");
	fwrite(buffer, size, 1, f);
	fclose(f);
}

/*
====================
 File path
====================
*/

void DefaultPath (char *path, char *basepath)
{
	char temp[128];

	if (path[0] == '/' || path[0] == '\\')
		return;
	strcpy(temp, path);
	strcpy(path, basepath);
	strcat(path, temp);
}

void ReplaceExtension (char *path, char *oldextension, char *replacementextension, char *missingextension)
{
	char *src;
	// if path has a .EXT, replace it with extension
	// if path doesn't have a .EXT, append extension
	// (extension should include the .)
	for (src = path + strlen(path) - 1;src >= path && *src != '/' && *src != '\\' && *src != ':';src--)
	{
		if (*src == '.')
		{
			if (!oldextension || !strncasecmp(src, oldextension, strlen(oldextension)))
			{
				*src = 0;
				strcat (path, replacementextension);
			}
			return;
		}
	}
	strcat (path, missingextension);
}

void DefaultExtension (char *path, const char *extension, size_t size_path)
{
	const char *src = path + strlen(path) - 1;
	while (*src != '/' && src != path)
	{
		if (*src == '.')
			return;
		src--;
	}
	strlcat (path, extension, size_path);
}

void ExtractFilePath (char *path, char *dest)
{
	char *src = path + strlen(path) - 1;
	while (src != path && (*(src-1) != '/' && *(src-1) != '\\'))
		src--;
	if (path == dest)
		*src = 0;
	else
	{
		memcpy(dest, path, src-path);
		dest[src-path] = 0;
	}
}

void ExtractFileBase (char *path, char *dest)
{
	char *src, *ext = NULL;

	src = path + strlen(path) - 1;
	while(src != path && (*(src-1) != '/' && *(src-1) != '\\'))
	src--;
	while(*src)
	{
		if (*src == '.')
			ext = dest;
		*dest++ = *src++;
	}
	if (ext)
		*ext = 0;
	else
		*dest = 0;
}

void ExtractFileName(char *path, char *dest)
{
	char *src;

	src = path + strlen(path) - 1;
	while (src != path && (*(src-1) != '/' && *(src-1) != '\\'))
		src--;
	while(*src)
		*dest++ = *src++;
	*dest = 0;
}

void ExtractFileSuffix(char *path, char *dest, char suffix_sym)
{
	char *src;

	src = path + strlen(path) - 1;
	// back up until a _ or the start
	while (src != path && *(src-1) != suffix_sym)
		src--;
	if (src == path)
	{
		*dest = 0;
		return;
	}
	strcpy(dest, src);
}

void StripFileExtension(char *path, char *dest)
{
	int l;

	l = strlen(path) - 1;
	while(l >= 0 && path[l] != '.')
		l--;
	if (l == 0)
		return;
	if (path == dest)
		path[l] = 0;
	else
		strlcpy(dest, path, l+1);
}

void ExtractFileExtension (char *path, char *dest)
{
	char  *src;

	src = path + strlen(path) - 1;
	while(src != path && *(src-1) != '.')
		src--;
	if (src == path)
    {
		*dest = 0;
		return;
	}
	strcpy(dest, src);
}

void AddSlash(char *path)
{
	int l;

	l = strlen(path) - 1;
	if (l < 0)
		return;
	if (path[l] == '/' || path[l] == '\\')
		return;
	path[l+1] = '/';
	path[l+2] = 0;
}

void AddSlash(string &path)
{
	int l;

	l = strlen(path.c_str()) - 1;
	if (l < 0)
		return;
	if (path.c_str()[l] == '/' || path.c_str()[l] == '\\')
		return;
	path.append("/");
}

/*
==============
 Parse
==============
*/

int ParseHex (char *hex)
{
	char *str;
	int num;

	num = 0;
	str = hex;
	while (*str && *str != '\n')
	{
		num <<= 4;
		if (*str >= '0' && *str <= '9')
			num += *str-'0';
		else if (*str >= 'a' && *str <= 'f')
			num += 10 + *str-'a';
		else if (*str >= 'A' && *str <= 'F')
			num += 10 + *str-'A';
		else
			Error ("Bad hex number: '%s'", hex);
		str++;
	}
	return num;
}

int ParseNum (char *str)
{
	if (str[0] == '$')
		return ParseHex (str+1);
	if (str[0] == '0' && str[1] == 'x')
		return ParseHex (str+2);
	return atol (str);
}

void ParseTokens(const char *tokenize_string, vector<string> &separators, vector<string> &tokens, size_t max_tokens, bool clear_tokens_array, bool trim_tokens)
{
	int i, k, numseparators;
	const char *p, *p0;
	char tokentext[32768], *token;
	int *separatorlen;
	bool foundseparator;

	if (clear_tokens_array)
		tokens.clear();
	separatorlen = (int *)mem_alloc(separators.size());
	numseparators = separators.size();
	p = tokenize_string;
	for (i = 0; i < numseparators; i++)
	{
		separatorlen[i] = strlen(separators[i].c_str());
		if (!separatorlen[i])
			Error("Tokenize: blank separators are not allowed!");
	}
	i = 0;
	while(1)
	{
		token = tokentext + i;
		foundseparator = false;
		p0 = p;
		while(*p)
		{
			for (k = 0; k < numseparators; k++)
			{
				if (!strncmp(p, separators[k].c_str(), separatorlen[k]))
				{
					p += separatorlen[k];
					break;
				}
			}
			if (k < numseparators)
			{
				foundseparator = true;
				break;
			}
			if (i < (int)sizeof(tokentext)-1)
				tokentext[i++] = *p;
			p++;
			p0 = p;
		}
		if (i >= (int)sizeof(tokentext))
			break;
		tokentext[i++] = 0;
		p0 = token;
		if (trim_tokens)
		{
			char *p1 = token;
			while(*p1 <= ' ' && *p1 > 0)
				p1++;
			for(k = strlen(p1) -1; k >= 0 && p1[k] <= ' '; k--)
				p1[k] = 0;
			p0 = p1;
		}
		string token_str = p0;
		tokens.push_back(token_str);
		if (!*p)
		{
			if (foundseparator)
				tokens.push_back("");
			break;
		}
		if (max_tokens > 0 && (tokens.size()-1) >= max_tokens)
		{
			string token_str = p;
			tokens.push_back(token_str);
		}
	}
}

void ParseTokens(const char *tokenize_string, const char *separator, vector<string> &tokens, size_t max_tokens, bool clear_tokens_array, bool trim_tokens)
{
	int i, k;
	const char *p, *p0;
	char tokentext[32768], *token;
	int separatorlen;
	bool foundseparator;

	if (clear_tokens_array)
		tokens.clear();
	separatorlen = strlen(separator);
	if (!separatorlen)
		Error("Tokenize: blank separator are not allowed!");
	p = tokenize_string;
	i = 0;
	while(1)
	{
		token = tokentext + i;
		foundseparator = false;
		p0 = p;
		while(*p)
		{
			if (!strncmp(p, separator, separatorlen))
			{
				p += separatorlen;
				foundseparator = true;
				break;
			}
			if (i < (int)sizeof(tokentext)-1)
				tokentext[i++] = *p;
			p++;
			p0 = p;
		}
		if (i >= (int)sizeof(tokentext))
			break;
		tokentext[i++] = 0;
		p0 = token;
		if (trim_tokens)
		{
			char *p1 = token;
			while(*p1 <= ' ' && *p1 > 0)
				p1++;
			for(k = strlen(p1)-1; k >= 0 && p1[k] <= ' '; k--)
				p1[k] = 0;
			p0 = p1;
		}
		string token_str = p0;
		tokens.push_back(token_str);
		if (!*p)
		{
			if (foundseparator)
				tokens.push_back("");
			break;
		}
		if (max_tokens > 0 && (tokens.size()-1) >= max_tokens)
		{
			string token_str = p;
			tokens.push_back(token_str);
		}
	}
}