////////////////////////////////////////////////////////////////
//
// DpOmniTool / Main File
// (c) Pavel [VorteX] Timofeyev
// See LICENSE text file for a license agreement
//
////////////////////////////////

#define F_DPOMNITOOL_C

#include "thread.h"
#include "dpomnilib.h"
#include "dpomnitool.h"

using namespace omnilib;

typedef enum
{
	STAGE_HELP,
	STAGE_CONVERT,
	STAGE_DECOMPILE,
	NUM_ACTIONS
}stage_t;

/*
==========================================================================================

  CONVERT

==========================================================================================
*/

int Convert(int argc, char **argv)
{
	char spritefile[MAX_FPATH], outname[MAX_FPATH], outfolder[MAX_FPATH];
	size_t bufsize;
	byte *buf;
	MetaSprite_t *sprite, *sprite2;
	bool debugborder, debugfill, forcesquare, nosort, npot, packed, single;
	int i, maxwidth, maxheight, border;

	// early out
	if (argc < 1)
		Error("not enough parms\n");

	// parse parms
	nosort = false;
	debugborder = false;
	debugfill = false;
	forcesquare = false;
	npot = false;
	maxwidth = maxheight = 32768;
	packed = false;
	single = false;
	border = 0;
	for (i =  1; i < argc; i++)
	{
		if (!_stricmp(argv[i], "-debug"))
		{
			debugborder = true;
			debugfill = true;
			Print("Option: debug mode\n");
			continue;
		}
		if (!_stricmp(argv[i], "-debugborder"))
		{
			debugborder = true;
			Print("Option: debug borders (show borders as magenta)\n");
			continue;
		}
		if (!_stricmp(argv[i], "-debugfill"))
		{
			debugfill = true;
			Print("Option: debug fill (show empty space as cyan)\n");
			continue;
		}
		if (!_stricmp(argv[i], "-square"))
		{
			forcesquare = true;
			Print("Option: force square textures\n");
			continue;
		}
		if (!_stricmp(argv[i], "-maxwidth"))
		{
			i++;
			if (i < argc)
			{
				maxwidth = atoi(argv[i]);
				Print("Option: max width of packed texture %i pixels\n", maxwidth);
			}
			continue;
		}
		if (!_stricmp(argv[i], "-maxheight"))
		{
			i++;
			if (i < argc)
			{
				maxheight = atoi(argv[i]);
				Print("Option: max height of packed texture %i pixels\n", maxheight);
			}
			continue;
		}
		if (!_stricmp(argv[i], "-border"))
		{
			i++;
			if (i < argc)
			{
				border = atoi(argv[i]);
				Print("Option: border %i pixels\n", border);
			}
			continue;
		}
		if (!_stricmp(argv[i], "-nosort"))
		{
			nosort = true;
			Print("Option: forcing normal frame order\n");
			continue;
		}
		if (!_stricmp(argv[i], "-npot"))
		{
			npot = true;
			Print("Option: allow non-power-of-two pictures\n");
			continue;
		}
		if (!_stricmp(argv[i], "-packed"))
		{
			packed = true;
			Print("Option: converting to packed sprite format\n");
			continue;
		}
		if (!_stricmp(argv[i], "-single"))
		{
			single = true;
			Print("Option: converting to single-frame sprite format\n");
			continue;
		}
	}

	// get file name
	strncpy(spritefile, argv[0], sizeof(spritefile));
	bufsize = LoadFile(spritefile, &buf);
	if (!bufsize)
		Error("failed to load file\n");

	// get default output names
	ExtractFilePath(spritefile, outfolder);
	if (outfolder[0] == 0)
		strcpy(outfolder, ".");
	ExtractFileName(spritefile, outname);
	strcat(outname, "_converted");

	// load sprite
	Print("Loading %s.\n", spritefile);
	sprite = olLoadSprite(buf, bufsize);
	if (sprite->errormsg[0])
		Error("olLoadSprite: %s", sprite->errormsg);
	mem_free(buf);

	// load sprite
	if (packed == true)
	{
		Print("Converting to packed format...\n");
		sprite2 = olSpriteConvertToPacked(sprite, border, maxwidth, maxheight, forcesquare, debugfill, debugborder, nosort, npot, SPR_PACK_NORMAL);
	}
	else if (single == true)
	{
		Print("Converting to single-frame format...\n");
		sprite2 = olSpriteConvertToSingle(sprite);
	}

	// save
	sprintf(spritefile, "%s/%s.spr", outfolder, outname);
	Print("Saving %s.\n", spritefile);
	bufsize = olSpriteSave(sprite2, &buf);
	SaveFile(spritefile, buf, bufsize);
	mem_free(buf);

	// quit
	olFreeSprite(sprite);
	olFreeSprite(sprite2);
	Print("Converting finished.\n");
	return 0;
}

/*
==========================================================================================

  DECOMPILE

==========================================================================================
*/

int Decompile(int argc, char **argv)
{
	char spritefile[MAX_FPATH], outname[MAX_FPATH], outfolder[MAX_FPATH];
	size_t bufsize;
	byte *buf;
	MetaSprite_t *sprite;

	// early out
	if (argc < 1)
		Error("not enough parms\n");

	// get file name
	strncpy(spritefile, argv[0], sizeof(spritefile));
	bufsize = LoadFile(spritefile, &buf);
	if (!bufsize)
		Error("failed to load file\n");

	// get default output names
	ExtractFilePath(spritefile, outfolder);
	if (outfolder[0] == 0)
		strcpy(outfolder, ".");
	ExtractFileName(spritefile, outname);
	strcat(outname, "_decompiled");

	// load sprite
	Print("Loading %s.\n", spritefile);
	sprite = olLoadSprite(buf, bufsize);
	if (sprite->errormsg[0])
		Error("olLoadSprite: %s", sprite->errormsg);

	// export
	Print("Exporting %s/%s.\n", outfolder, outname);
	olSpriteExport(sprite, outfolder, outname);

	// quit
	olFreeSprite(sprite);
	Print("Export finished.\n");
	return 0;
}

/*
==========================================================================================

  MAIN

==========================================================================================
*/

int Help(void)
{
	waitforkey = true;
	Print(
	"1.1 Usage:\n"
	"    dpomnitool [options] <action>\n"
	"\n"
	"1.2 Options:\n"
	"    -nc: print no caption\n"
	"    -w: wait for key when finished\n"
	"    -mem: show memstats\n"
	"    -v: show verbose messages\n"
	"    -sp: solid pacifier prints\n"
	"    -c: compact mode (only generic prints)\n"
	"    -f: function mode (only pacifier prints)\n"
	"    -errlog: write errlog.txt on error\n"
	"\n"
	"1.3 Actions:\n"
	"    sprdecompile: or sd - decompile sprite\n"
	"    sprconvert: or sc - convert sprite\n"
	"\n"
	"2.1 Sprite decompile:\n"
	"    Usage: dpomnitool sd \"filename\"\n"
	"\n"
	"2.2 Sprite convert:\n"
	"    Usage: dpomnitool sc \"filename\" [parms]\n"
	"    Parameters:\n"
	"      -packed: convert to a new packed sprite format\n"
	"      -single: convert to a single (one frame per image) sprite format\n"
	"      -square: force square texture (packed sprite only)\n"
	"      -maxwidth X: max width of packed texture (packed sprite only)\n"
	"      -maxheight X: max height of packed texture (packed sprite only)\n"
    "      -border X: set border of X pixels between sprites (packed sprite only)\n"
    "      -nosort: do not sort frames by size (packed sprite only)\n"
    "      -npot: allow non-power-of-two texture (packed sprite only)\n"
	"      -debugborder: show borders as magenta (packed32 sprite only)\n"
	"      -debugfill: show empty space as cyan (packed32 sprite only)\n"
	"      -debug: debugborder + debugfill\n"
	"\n"
	);
	return 0;
}

// sprlib message wrapper
void sprlib_print_message(int level, char *msg)
{
	if (level == 0)
		Print(msg);
	else if (level == 1)
		Verbose(msg);
	else if (level == 2)
		Warning(msg);
}

// sprlib error wrapper
void sprlib_error(char *msg)
{
	Error(msg);
}

// main function
int main(int argc, char **argv)
{
	bool printcap;
	int i, returncode;
	stage_t foundaction;

	// get program name
	myargv = argv;
	myargc = argc;
	memset(progname, 0, MAX_FPATH);
	memset(progpath, 0, MAX_FPATH);
	ExtractFileBase(argv[0], progname);
	ExtractFilePath(argv[0], progpath);

	// COMMANDLINEPARM: -nc: print no caption
	printcap = CheckParm("-nc") ? false : true;
	// COMMANDLINEPARM: -w: wait for key when finished
	waitforkey = (CheckParm("-w") || CheckParm("-"));
	// COMMANDLINEPARM: -mem: show memstats
	memstats = CheckParm("-mem");
	// COMMANDLINEPARM: -v: show verbose messages
	verbose = CheckParm("-v");
	// COMMANDLINEPARM: -sp: solid pacifier prints
	solidpacifier = CheckParm("-sp");
	// COMMANDLINEPARM: -errlog: write errlog.txt on error
	errorlog = CheckParm("-errlog");
	// COMMANDLINEPARM: -c: compact mode (only generic prints)
	if (CheckParm("-c")) 
	{
		verbose = false;
		printcap = false;
	}
	// COMMANDLINEPARM: -f: function mode (only pacifier prints)
	if (CheckParm("-f")) 
	{
		verbose = false;
		printcap = false;
		noprint = true;
	}
	// get action
	foundaction = STAGE_HELP;
	for (i = 1; i < argc; i++)
	{
		// c - convert spr file
		if (!_stricmp(argv[i], "sc") || !_stricmp(argv[i], "sprconvert"))
		{
			foundaction = STAGE_CONVERT;
			i++;
			break;
		}
		// d - decompile existing spr32 file
		if (!_stricmp(argv[i], "sd") || !_stricmp(argv[i], "sprdecompile"))
		{
			foundaction = STAGE_DECOMPILE;
			i++;
			break;
		}
	}

	Thread_Init();
	if (!numthreads)
		numthreads = num_cpu_cores;
	if (printcap)
	{
		Print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
		Print(" DpOmniTool v%s.%s by Pavel [VorteX] Timofeyev\n", DPOMNITOOL_VERSION_MAJOR, DPOMNITOOL_VERSION_MINOR);
		Print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
		//Print("%i threads", numthreads);
		if (memstats || waitforkey)
		{
			if (memstats)
				Print(", showing memstats");
			if (waitforkey)
				Print(", waiting for key");
			Print("\n");
		}
		Print("\n");
	}

	// init the sprite lib
	OmnilibSetMessageFunc(sprlib_print_message, sprlib_error);

	// do action
	if (foundaction == STAGE_CONVERT)
		returncode = Convert(argc-i, argv+i);
	else if (foundaction == STAGE_DECOMPILE)
		returncode = Decompile(argc-i, argv+i);
	else
		returncode = Help();

#if _MSC_VER
	if (waitforkey && !noprint)
	{
		printf("press any key\n");
		getchar();
	}
#endif

	return returncode;
}