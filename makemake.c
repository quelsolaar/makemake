#define _CRT_SECURE_NO_WARNINGS
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define FALSE 0
#define TRUE !FALSE
#ifdef _WIN32
#define MAKE_MAKE_WINDOWS_PATH_LENGHT_MAX (256 * 128 - 1)
#include <string.h> 
#include <windows.h>
#include <stdlib.h>
#include <tchar.h>
#include <wchar.h>
#define DIR_ROOT_PATH "/"
#define DIR_HOME_PATH "."
#define DIR_SLASH '\\'
#else
#include <sys/types.h>
#include <sys/dir.h>
#include <dirent.h>
#include <sys/statvfs.h>
#define DIR_ROOT_PATH "/"
#define DIR_HOME_PATH "."
#define DIR_SLASH '/'
#define idirent struct dirent
#endif
//#include "forge.h"
//#include "imagine.h"

#define MAKE_MAKE_TEXT_ALLOCATION_SIZE 1024
#define MAKE_MAKE_TOKEN_LENGTH_MAX 256

typedef struct{
	char *array;
	unsigned int array_length;
}MakeMakeList;

typedef enum{
	MM_LT_INCLUDES,
	MM_LT_DECLARATIONS,
	MM_LT_DEFINITIONS,
	MM_LT_DEPENDENCY,
	MM_LT_PRAGMA,
	MM_LT_COUNT
}MakeMakeListType;

typedef struct{
	MakeMakeList lists[MM_LT_COUNT];
	char file_name[MAKE_MAKE_TOKEN_LENGTH_MAX];
	int added;
	int header;
}MakeMakeFile;

typedef struct{
	MakeMakeFile *files;
	unsigned int file_count;
	char program_name[MAKE_MAKE_TOKEN_LENGTH_MAX];
	char path[MAKE_MAKE_TOKEN_LENGTH_MAX];
	MakeMakeList options;
}MakeMakeProject;

typedef struct{
	const char* sdkVersion;
	const char* ndkVersion;
	const char* packageName;
	const char* packageVersion;
	const char* architectures[4];
}MakeMakeAndroidOptions;

typedef struct{
	char sdkBaseDirectory[MAKE_MAKE_TOKEN_LENGTH_MAX];
	char sdkBuildToolsPath[MAKE_MAKE_TOKEN_LENGTH_MAX];
	char sdkPlatformsPath[MAKE_MAKE_TOKEN_LENGTH_MAX];
	char sdkSysRootLibraryPath[MAKE_MAKE_TOKEN_LENGTH_MAX];
	char sdkSysRootIncludePath[MAKE_MAKE_TOKEN_LENGTH_MAX];
	char ndkNativeAppGlueSourcePath[MAKE_MAKE_TOKEN_LENGTH_MAX];
	char ndkLinkerExecutablePath[MAKE_MAKE_TOKEN_LENGTH_MAX];
	char ndkCompilerExecutablePath[MAKE_MAKE_TOKEN_LENGTH_MAX];
	char sdkDownloadLink[MAKE_MAKE_TOKEN_LENGTH_MAX];
} MakeMakeAndroidToolchain;

#ifdef _WIN32 /* Windows implementation of UNIX directory functionality. Copied form Imagine library. */

typedef struct{
	char			d_name[MAKE_MAKE_WINDOWS_PATH_LENGHT_MAX];
}idirent;
 
typedef struct{
	WIN32_FIND_DATAW data; 
	HANDLE			handle;
	idirent			ent;
	unsigned int	drive_letter;
}DIR;

#define MAKE_MAKE_NXT	0x80
#define MAKE_MAKE_SEQ2	0xc0
#define MAKE_MAKE_SEQ3	0xe0
#define MAKE_MAKE_SEQ4	0xf0
#define MAKE_MAKE_SEQ5	0xf8
#define MAKE_MAKE_SEQ6	0xfc
#define MAKE_MAKE_BOM	0xfeff


unsigned int make_make_utf8_to_uint32(char *c, unsigned int *pos)
{
	unsigned int i, bits, character_count = 1, high;
	unsigned int out;

	if((c[*pos] & 0x80) == 0)
		high = (wchar_t)c[*pos];
	else if((c[*pos] & 0xe0) == MAKE_MAKE_SEQ2)
	{
		character_count = 2;
		high = (wchar_t)(c[*pos] & 0x1f);
	}else if((c[*pos] & 0xf0) == MAKE_MAKE_SEQ3)
	{
		character_count = 3;
		high = (wchar_t)(c[*pos] & 0x0f);
	}else if((c[*pos] & 0xf8) == MAKE_MAKE_SEQ4)
	{
		character_count = 4;
		high = (wchar_t)(c[*pos] & 0x07);
	}else if((c[*pos] & 0xfc) == MAKE_MAKE_SEQ5)
	{
		character_count = 5;
		high = (wchar_t)(c[*pos] & 0x03);
	}else if((c[*pos] & 0xfe) == MAKE_MAKE_SEQ6)
	{
		character_count = 6;
		high = (wchar_t)(c[*pos] & 0x01);
	}else
	{
		(*pos)++;
		return 0;
	}
	out = 0;
	bits = 0;
	for(i = 1; i < character_count; i++)
	{
		out |= (wchar_t)(c[character_count - i] & 0x3f) << bits;
		bits += 6;		/* 6 low bits in every byte */
	}
	out |= high << bits;
	(*pos) += character_count;
	return out;
}

unsigned int make_make_uint32_to_utf8(unsigned int character, char *out) /* Copied form Forge library. */
{
	unsigned char bytes[4];
	bytes[0] = (character >> 24) & 0xFF;
	bytes[1] = (character >> 16) & 0xFF;
	bytes[2] = (character >> 8) & 0xFF;
	bytes[3] = character & 0xFF;
	if(character <= 0x0000007f)
	{
		out[0] = bytes[3];
		return 1;
	}else if(character <= 0x000007ff)
	{
		out[1] = MAKE_MAKE_NXT | bytes[3] & 0x3f;
		out[0] = MAKE_MAKE_SEQ2 | (bytes[3] >> 6) | ((bytes[2] & 0x07) << 2);
		return 2;
	}else if(character <= 0x0000ffff)
	{
		out[2] = MAKE_MAKE_NXT | bytes[3] & 0x3f;
		out[1] = MAKE_MAKE_NXT | (bytes[3] >> 6) | ((bytes[2] & 0x0f) << 2);
		out[0] = MAKE_MAKE_SEQ3 | ((bytes[2] & 0xf0) >> 4);
		return 3;
	}else if(character <= 0x001fffff)
	{
		out[3] = MAKE_MAKE_NXT | bytes[3] & 0x3f;
		out[2] = MAKE_MAKE_NXT | (bytes[3] >> 6) | ((bytes[2] & 0x0f) << 2);
		out[1] = MAKE_MAKE_NXT | ((bytes[2] & 0xf0) >> 4) | ((bytes[1] & 0x03) << 4);
		out[0] = MAKE_MAKE_SEQ4 | ((bytes[1] & 0x1f) >> 2);
		return 4;
	}else if(character <= 0x03ffffff)
	{
		out[4] = MAKE_MAKE_NXT | bytes[3] & 0x3f;
		out[3] = MAKE_MAKE_NXT | (bytes[3] >> 6) | ((bytes[2] & 0x0f) << 2);
		out[2] = MAKE_MAKE_NXT | ((bytes[2] & 0xf0) >> 4) | ((bytes[1] & 0x03) << 4);
		out[1] = MAKE_MAKE_NXT | (bytes[1] >> 2);
		out[0] = MAKE_MAKE_SEQ5 | bytes[0] & 0x03;
		return 5;
	}else /* if (*w <= 0x7fffffff) */
	{
		out[5] = MAKE_MAKE_NXT | bytes[3] & 0x3f;
		out[4] = MAKE_MAKE_NXT | (bytes[3] >> 6) | ((bytes[2] & 0x0f) << 2);
		out[3] = MAKE_MAKE_NXT | (bytes[2] >> 4) | ((bytes[1] & 0x03) << 4);
		out[2] = MAKE_MAKE_NXT | (bytes[1] >> 2);
		out[1] = MAKE_MAKE_NXT | bytes[0] & 0x3f;
		out[0] = MAKE_MAKE_SEQ6 | ((bytes[0] & 0x40) >> 6);
		return 6;
	}
}


DIR *opendir(char *path) 
{ 
	DIR *dir;
	if(path != NULL && path[0] != 0)
	{
		unsigned int pos, i;
		WIN32_FIND_DATAW data; 
		HANDLE handle; 
		wchar_t unicode_path[MAKE_MAKE_WINDOWS_PATH_LENGHT_MAX];
		char *post_fix = "*.*";
		for(i = pos = 0; i < MAKE_MAKE_WINDOWS_PATH_LENGHT_MAX - 5 && path[pos] != 0; i++)
			unicode_path[i] = make_make_utf8_to_uint32(path, &pos);
		for(pos = 0; post_fix[pos] != 0; i++)
			unicode_path[i] = (wchar_t)post_fix[pos++];
		unicode_path[i] = 0;
		if((handle = FindFirstFileW(unicode_path, &data)) != INVALID_HANDLE_VALUE)
		{ 
			dir = (DIR*)malloc(sizeof *dir);
			dir->handle = handle;
			dir->data = data;
			dir->ent.d_name[0] = 0;
			dir->drive_letter = -1;
			return dir;
		}
		return NULL;
	}else
	{
		dir = (DIR*)malloc(sizeof *dir);
		dir->handle = NULL;
		dir->ent.d_name[0] = 64;
		dir->ent.d_name[1] = ':';
		dir->ent.d_name[2] = '/';
		dir->ent.d_name[3] = 0;
		dir->drive_letter = GetLogicalDrives() * 2;
		return dir;
	}
}

void closedir(DIR *dir) 
{
	if(dir->drive_letter == -1)
		FindClose(dir->handle);
	free(dir);
}

idirent *readdir(DIR *dir) 
{
	if(dir->drive_letter == -1)
	{
		unsigned int i, pos;
		if(FindNextFileW(dir->handle, &dir->data) != TRUE)
			return NULL;
		for(i = pos = 0; dir->data.cFileName[i] != 0 && pos < MAKE_MAKE_WINDOWS_PATH_LENGHT_MAX - 6; i++)
			pos += make_make_uint32_to_utf8(dir->data.cFileName[i], &dir->ent.d_name[pos]);
		dir->ent.d_name[pos] = 0;
		return &dir->ent;
	}else
	{
		unsigned int letter, drive, i;
		drive = dir->drive_letter;
		letter = dir->ent.d_name[0] - 64;
		letter++;
		for(i = 0; i < letter; i++)
			drive /= 2;

		for(i = letter; i < 32; i++)
		{
			if(drive % 2 == 1)
			{
				dir->ent.d_name[0] = 64 + i;
				return &dir->ent;
			}
			drive /= 2;
		}
		return NULL;
	}
}


#endif

/* Function for parsing directories, copied form Imagine. */

int make_make_path_search(char *file, int partial, char *path, int folders, unsigned int number, char *out_buffer, unsigned int out_buffer_size)
{
	unsigned int i, j, found = 0;
	DIR *d;
	idirent *ent;
	d = opendir(path);
	if(d != NULL)
	{
		ent = readdir(d);
		if(ent != NULL)
		{		
			out_buffer_size--;
			while(TRUE)
			{
				if(file == NULL)
				{
					if(found == number)
					{
						for(i = 0; ent->d_name[i] != 0 && i < out_buffer_size; i++)
							out_buffer[i] = ent->d_name[i];
						out_buffer[i] = 0;
						closedir(d);
						return TRUE;
					}
					found++;
				}else
				{
					if(partial)
					{
						for(i = 0; ent->d_name[i] != 0; i++)
						{
							if(ent->d_name[i] == file[0])
							{
								for(j = 0; ent->d_name[i + j] != 0 && file[j] != 0 && (ent->d_name[i + j] == file[j]); j++);
								if(file[j] == 0)
								{
									if(found == number)
									{
										for(i = 0; ent->d_name[i] != 0 && i < out_buffer_size; i++)
											out_buffer[i] = ent->d_name[i];
										out_buffer[i] = 0;
										closedir(d);
										return TRUE;
									}
									found++;
									break;
								}
							}
						}
					}else
					{
						for(i = 0; ent->d_name[i] != 0 && ent->d_name[i] == file[i]; i++);
						if(ent->d_name[i] == file[i])
						{
							if(found == number)
							{
								for(i = 0; ent->d_name[i] != 0 && i < out_buffer_size; i++)
									out_buffer[i] = ent->d_name[i];
								out_buffer[i] = 0;
								closedir(d);
								return TRUE;
							}
							found++;
						}
					}
				}
				ent = readdir(d);
				if(ent == NULL)
					break;
			}
		}
		closedir(d);
	}
	return FALSE;
}



void make_make_list_add(MakeMakeList *list, char *token)
{
	unsigned int i;
//	printf("adding to list %s\n", token);
	for(i = 0; TRUE; i++)
	{
		if(list->array_length % MAKE_MAKE_TEXT_ALLOCATION_SIZE == 0)
			list->array = (char*)realloc(list->array, MAKE_MAKE_TEXT_ALLOCATION_SIZE + list->array_length);
		list->array[list->array_length++] = token[i];
		if(token[i] == 0)
			break;
	}
}

unsigned int make_make_list_next(MakeMakeList *list, unsigned int pos)
{	
	if(pos < list->array_length)
	{
		while(list->array[pos] != 0)
			pos++;
		pos++;
	}
	if(pos >= list->array_length)
		return ~0;
	return pos;
}


int make_make_list_test(MakeMakeList *list, char *token)
{
	unsigned int i, j;
	for(i = 0; i < list->array_length; i++)
	{
		for(j = 0; list->array[i] != 0 && token[j] == list->array[i]; i++)
			j++;
		if(token[j] == list->array[i])
			return TRUE;
		while(list->array[i] != 0)
			i++;
	}
	return FALSE;
}

void make_make_definition_list(MakeMakeFile *file, char *buffer, unsigned int file_length)
{
	char b[MAKE_MAKE_TOKEN_LENGTH_MAX];
	unsigned int i, j, k;
	char end;
	for(i = 0; i < file_length; i++)
	{
		if(buffer[i] == '(')
		{
			for(j = i + 1; j < file_length && buffer[j] != ';' && buffer[j] != '{' && buffer[j] != '(' && buffer[j] != '='; j++);
			end = buffer[j];
			for(j = i - 1; j != 0 && buffer[j] <= ' '; j--); // remove white space;
			for(; j != 0 && ((buffer[j] >= 'a' && buffer[j] <= 'z') ||
							(buffer[j] >= 'A' && buffer[j] <= 'Z') ||
							(buffer[j] >= '0' && buffer[j] <= '9') ||
							buffer[j] == '_'); j--); // parse the name;
			j++;
			for(k = 0; j < file_length && k < MAKE_MAKE_TOKEN_LENGTH_MAX && buffer[j] >= ' ' && buffer[j] != '('; k++)
				b[k] = buffer[j++];
			if(k > 2 && k < MAKE_MAKE_TOKEN_LENGTH_MAX)
			{
				b[k] = 0;
				if(end == '{')
				{
				//	printf("Defining %s: %s\n", file->file_name, b);
					make_make_list_add(&file->lists[MM_LT_DEFINITIONS], b);
				}
			}
		}
	}
}

void make_make_declaration_list(MakeMakeFile *file, char *buffer, unsigned int file_length)
{
	char b[MAKE_MAKE_TOKEN_LENGTH_MAX], *key_word = "extern", *exception_main = "main";
	unsigned int i, j, k;
	for(i = 0; i < file_length; i++)
	{
	//	if(buffer[i] <= ' ')
		{
	//		i++;
			for(j = 0; i + j < file_length && buffer[i + j] == key_word[j]; j++);
			if(j > 2)
				j += 0;
			if(0 == key_word[j] && buffer[i + j] <= ' ')
			{
				i += j;
				for(j = i + 1; j < file_length && buffer[j] != ';' && buffer[j] != '{' && buffer[j] != '('; j++);
				if(buffer[j] == '(')
				{
					i = j;
					for(j++; j < file_length && buffer[j] != ';' && buffer[j] != '(' && buffer[j] != '='; j++);
						
					if(buffer[j] == ';') /* ignore function pointers*/
					{
						while(i > 1 && buffer[i - 1] <= ' ') // remove white space;
							i++;
						for(j = i - 1; j != 0 && ((buffer[j] >= 'a' && buffer[j] <= 'z') ||
												(buffer[j] >= 'A' && buffer[j] <= 'Z') ||
												(buffer[j] >= '0' && buffer[j] <= '9') ||
												buffer[j] == '_'); j--); // parse the name;
						j++;
						for(k = 0; j < file_length && k < MAKE_MAKE_TOKEN_LENGTH_MAX && buffer[j] >= ' ' && buffer[j] != '('; k++)
							b[k] = buffer[j++];
						if(k > 2 && k < MAKE_MAKE_TOKEN_LENGTH_MAX)
						{
							b[k] = 0;							
							for(k = 0; b[k] == exception_main[k] && b[k] != 0; k++);
							if(b[k] != exception_main[k])
								make_make_list_add(&file->lists[MM_LT_DECLARATIONS], b);
						}
					}
				}
			}
		}
	}
}



unsigned int make_make_option_list(MakeMakeProject *project, char *buffer, unsigned int buffer_length, FILE *f, char *property, char *separator)
{
	unsigned int i, offset, used = 0, separator_size;
	int first = TRUE;
	for(separator_size = 0; separator[separator_size] != 0; separator_size++);
	for(i = 0; i < project->options.array_length; i = make_make_list_next(&project->options, i))
	{
		for(offset = 0; property[offset] == project->options.array[i + offset]; offset++);
		if(property[offset++] == 0)
		{
			if(!first)
			{
				memcpy(&buffer[used], separator, separator_size);
				used += separator_size;
			}
			first = FALSE;
			for(offset++; project->options.array[i + offset] != 0; offset++)
				buffer[used++] = project->options.array[i + offset];
			if(f != NULL)
			{
				buffer[used] = 0;
				fprintf(f, "%s", buffer);
				used = 0;
			}
		}
	}
	buffer[used] = 0;
	return used;
}

char *make_make_option_get(MakeMakeProject *project, char *property)
{
	unsigned int i, offset, used = 0;
	int first = TRUE;
	for(i = 0; i < project->options.array_length; i = make_make_list_next(&project->options, i))
	{
		for(offset = 0; property[offset] == project->options.array[i + offset]; offset++);
		if(property[offset++] == 0)
		{
			return &project->options.array[i + offset];
		}
	}
	return NULL;
}

int make_make_option_test(MakeMakeProject *project, char *property, char *value)
{
	unsigned int i, j, offset, used = 0;
	int first = TRUE;
	for(i = 0; i < project->options.array_length; i = make_make_list_next(&project->options, i))
	{
		for(offset = 0; property[offset] == project->options.array[i + offset]; offset++);
		if(property[offset++] == 0)
		{
			for(j = 0; project->options.array[i + offset + j] == value[j] && project->options.array[i + offset + j] != 0; j++);
			if(project->options.array[i + offset + j] == 0)
				return TRUE;
		}
	}
	return FALSE;
}


char *make_make_text_load(char *file_name, size_t *size)
{
	char *buffer;
	unsigned int allocation;
	FILE *f;
	f = fopen(file_name, "rb");
	if(f == NULL)
	{
		return NULL;
	}
	fseek(f, 0, SEEK_END);
	allocation = ftell(f);
	if(allocation == 0)
	{
		fclose(f);
		return NULL;
	}
	rewind(f);
	buffer = (char*)malloc(allocation + 1);
	memset(buffer, 0, allocation + 1);
	fread(buffer, 1, allocation, f);
	fclose(f);
	buffer[allocation] = 0;
	if(size != NULL)
		*size = allocation;
	return buffer;
}

void make_make_include(MakeMakeList *list, char *buffer, unsigned int buffer_length)
{
	char b[64];
	char *include = "include";
	unsigned int i, j;
	for(i = 0; i < buffer_length; i++)
	{
		if(buffer[i] == '#')
		{
			i++;
			for(j = 0; i + j < buffer_length && buffer[i + j] == include[j]; j++);
			i += j;
			if(include[j] == 0)
			{
				while(i < buffer_length && buffer[i] <= ' ')
					i++;
				if(i < buffer_length && buffer[i] == '\"')
				{
					i++;
					for(j = 0; j < 63 && buffer[i + j] != '\"'; j++)
						b[j] = buffer[i + j];
					b[j] = 0;
					make_make_list_add(list, b);
				}
			}
		}
	}
}



void make_make_find_next(char *file, unsigned int file_length, char start, char end)
{
		unsigned int i, count;
	for(i = 0; i < file_length; i++)
	{
		if(file[i] == start)
		{
			count = 1;
			for(i++; TRUE; i++)
			{
				if(file[i] == start)
					count++; 
				if(file[i] == end)
					if(--count == 0)
						break;
				file[i] = ' '; 
			}
		}
	}
}

void make_make_remove_literals(char *file, unsigned int file_length)
{
	unsigned int i, j;
	for(i = 0; i < file_length -1; i++)
	{
		if(file[i] == '\"')
		{
			i++;
			for(j = i + 1; j < file_length && (file[j] != '\"' || file[j - 1] == '\\'); j++);
			memset(&file[i], ' ', j - i);
			i = j + 1;
		}
	}
}

void make_make_remove_span(char *file, unsigned int file_length, char start, char end)
{
	unsigned int i, count;
	for(i = 0; i < file_length; i++)
	{
		if(file[i] == start)
		{
			count = 1;
			for(i++; i < file_length; i++)
			{
				if(file[i] == start)
					count++; 
				if(file[i] == end)
					if(--count == 0)
						break;
				file[i] = ' '; 
			}
		}
	}
}

void make_make_remove(char *file, unsigned int file_length, char *start, char *end)
{
	unsigned int i, j;
	for(i = 0; i < file_length;)
	{
		if(file[i] == '"' && file[i - 1] != '\'')
			for(i++; i < file_length && (file[i] != '"' || file[i - 1] == '\\'); i++);
		for(j = 0; i + j < file_length && file[i + j] == start[j] && start[j] != 0; j++);
		if(start[j] == 0)
		{
			for(j = 0; i < file_length && start[j] != 0; j++)
				file[i++] = ' '; 
			while(i < file_length)
			{
				for(j = 0; file[i + j] == end[j] && end[j] != 0; j++);
				if(end[j] == 0)
				{
					for(j = 0; end[j] != 0; j++)
						file[i++] = ' ';
					break;
				}
				if(file[i] == '\\')
				{
					file[i++] = ' ';
					while(file[i] <= ' ')
						file[i++] = ' ';
				}
				file[i++] = ' ';
			}
		}else
			i++;
	}
}
unsigned int make_make_compact(char *file, unsigned int file_length)
{
	unsigned int i, breaks = 0;
	char *r, *w;
	for(i = 0; i < file_length; i++)
		if(file[i] == '\t')
			file[i] = ' ';
	for(i = 0; i < file_length; i++)
	{
		if(file[i] != ' ')
		{
			if(file[i] != '\n' && file[i] != '\r')
				breaks = 0;
			else
				if(++breaks > 2)
					file[i] = ' ';
		}
	}
	r = w = file;
	for(i = 0; i < file_length; i++)
	{
		*w++ = *r;
		if(*r++ == ' ')
		{
			while(*r == ' ' && i < file_length)
			{
				r++;
				i++;
			}
		}
	}
	*w = '\0';
	return w - file;
}

void make_make_pragma(MakeMakeProject *project, char *file, unsigned int file_length, MakeMakeList *list)
{
	char *pragma = "#pragma", *makemake = "makemake", buffer_a[MAKE_MAKE_TOKEN_LENGTH_MAX], buffer_b[MAKE_MAKE_TOKEN_LENGTH_MAX];
	unsigned int i, j;
	for(i = 0; i < file_length; i++)
	{
		if(file[i] == '#')
		{
			for(j = 0; i + j < file_length && pragma[j] == file[i + j]; j++);
			if(pragma[j] == 0)
			{
				for(i += j; i < file_length && file[i] != '\n' && file[i] <= ' '; i++); /* skip white space */
				if(file[i] != '\n')
				{
					for(j = 0; i + j < file_length && makemake[j] == file[i + j]; j++);
					if(makemake[j] == 0)
					{
						for(i += j; i < file_length && file[i] != '\n' && file[i] <= ' '; i++); /* skip white space */
						if(file[i] != '\n')
						{
							for(j = 0; i + j < file_length && file[i + j] > ' ' && j < MAKE_MAKE_TOKEN_LENGTH_MAX - 1; j++)
								buffer_a[j] = file[i + j];
							buffer_a[j] = ' ';
							for(i += j; i < file_length && file[i] != '\n' && file[i] <= ' '; i++); /* skip white space */
							if(file[i] != '\n')
							{
								buffer_a[j++] = ' ';
								for(; i < file_length && file[i] > ' ' && j < MAKE_MAKE_TOKEN_LENGTH_MAX - 1; j++)
									buffer_a[j] = file[i++];
								buffer_a[j] = 0;
								for(; i < file_length && file[i] != '\n' && file[i] <= ' '; i++); /* skip white space */
					
								if(i < file_length && file[i] != '\n') /* has platform */
								{
									if(make_make_option_test(project, "platform", &file[i]))
										make_make_list_add(list, buffer_a);
								}else
									make_make_list_add(list, buffer_a);
							}
						}
					}
				}
			}
		}
	}
}

void make_make_parse_file(MakeMakeProject *project, char *full_path, char *file_name, int header, char *platform)
{
	MakeMakeFile *file;
	size_t size;
	char *buffer;
	unsigned int i;
	buffer = make_make_text_load(full_path, &size);
	if(buffer == NULL)
		return;
	if(project->file_count % 128 == 0)
		project->files = (MakeMakeFile*)realloc(project->files, (sizeof *project->files) * (project->file_count + 128));
	file = &project->files[project->file_count++];
	for(i = 0; i < MM_LT_COUNT; i++)
	{
		file->lists[i].array = NULL;
		file->lists[i].array_length = 0;
	}
	for(i = 0; file_name[i] != 0; i++)
		file->file_name[i] = file_name[i];
	file->file_name[i] = 0;
	file->added = FALSE;
	file->header = header;
	make_make_remove(buffer, size, "//", "\n");
	make_make_remove(buffer, size, "/*", "*/");
	make_make_pragma(project, buffer, size, &file->lists[MM_LT_PRAGMA]);
	make_make_include(&file->lists[MM_LT_INCLUDES], buffer, size);
	make_make_remove_literals(buffer, size);
	make_make_remove(buffer, size, "#", "\n");
//	make_make_remove(buffer, size, "(*", ")");
	make_make_remove_span(buffer, size, '{', '}');
	make_make_remove_span(buffer, size, '(', ')');
	size = make_make_compact(buffer, size);
	make_make_declaration_list(file, buffer, size);
	make_make_definition_list(file, buffer, size);
	buffer[size] = 0;
	free(buffer);
}

MakeMakeFile *make_make_file_find(MakeMakeFile *files, unsigned int file_count, char *file_name, MakeMakeFile *parent)
{
	unsigned int i, j;
	for(i = 0; TRUE; i++)
	{
		if(i == file_count)
			return NULL;
		for(j = 0; file_name[j] == files[i].file_name[j] && file_name[j] != 0; j++);
		if(file_name[j] == files[i].file_name[j])
		{
			if(parent != NULL && !files[i].added)
				printf(" - %s : Is included in %s\n", files[i].file_name, parent->file_name);
			return &files[i];
		}
	}	
}

MakeMakeFile *make_make_definition_find(MakeMakeFile *files, unsigned int file_count, char *declaration, MakeMakeFile *parent)
{
	unsigned int i, j, k;
	for(i = 0; i < file_count; i++)
	{
		if(!files[i].added)
		{
			if(make_make_list_test(&files[i].lists[MM_LT_DEFINITIONS], declaration))
			{
				printf(" - %s : Added because it declares %s required by %s\n", files[i].file_name, declaration, parent->file_name);
				return &files[i];
			}
		}
	}	
//	printf("Didnt find %s from %s\n", declaration, parent->file_name);
	return NULL;
}


void make_make_file_add(MakeMakeFile *files, unsigned int file_count, MakeMakeFile *f)
{
	unsigned int i;
	if(f == NULL || f->added)
		return;
	f->added = TRUE;

	for(i = 0; i < f->lists[MM_LT_DECLARATIONS].array_length; i = make_make_list_next(&f->lists[MM_LT_DECLARATIONS], i))
		make_make_file_add(files, file_count, make_make_definition_find(files, file_count, &f->lists[MM_LT_DECLARATIONS].array[i], f));
	for(i = 0; i < f->lists[MM_LT_INCLUDES].array_length; i = make_make_list_next(&f->lists[MM_LT_INCLUDES], i))
		make_make_file_add(files, file_count, make_make_file_find(files, file_count, &f->lists[MM_LT_INCLUDES].array[i], f));
}


void make_make_gather_options(MakeMakeProject *project)
{
	unsigned int i, j, k;
	for(i = 0; i < project->file_count; i++)
		if(project->files[i].added && project->files[i].lists[MM_LT_PRAGMA].array_length != 0)
			for(j = 0; j < project->files[i].lists[MM_LT_PRAGMA].array_length; j = make_make_list_next(&project->files[i].lists[MM_LT_PRAGMA], j))
				make_make_list_add(&project->options, &project->files[i].lists[MM_LT_PRAGMA].array[j]);

}

/*
Function that generates a make file:
Use "make_make_option_get" and "make_make_option_list" to retrive any options needed.
options are added to .c files using the following syntax:
#pragma makamake <option name> <option> <optional platform>
*/

void make_make_posix(MakeMakeProject *project, FILE *f)
{
	char buffer[1024];
	unsigned int i, j;
	fprintf(f, "# Makefile generated by makemake, written by Eskil Steenberg Hald www.quelsolaar.com\n");
	fprintf(f, "# Compiler and compiler flags\n");
	fprintf(f, "CC = gcc\n");
	fprintf(f, "CFLAGS = -Wall -g -Wno-unknown-pragmas");
	if(make_make_option_list(project, buffer, 1024, NULL, "compilerflag", " -"))
		fprintf(f, "-%s", buffer);
	if(make_make_option_list(project, buffer, 1024, NULL, "define", " -D"))
		fprintf(f, "-D%s", buffer);
	if(NULL != make_make_option_get(project, "dynamiclib"))
		fprintf(f, "-fPIC -shared");
	fprintf(f, "\n\n");
	
	fprintf(f, "# Libraries\n");
	fprintf(f, "LIBS = -lm");
	if(make_make_option_list(project, buffer, 1024, NULL, "lib", " -"))
	{
		fprintf(f, " -%s", buffer);
	}
	fprintf(f, "\n\n");

	fprintf(f, "# Source files\n");
	fprintf(f, "SRCS = ");
	for(i = 0; i < project->file_count; i++)
		if(project->files[i].added && !project->files[i].header)
			fprintf(f, "%s ", project->files[i].file_name);
	fprintf(f, "\n\n");

	fprintf(f, "# target executable name\n");
	fprintf(f, "TARGET := %s\n\n", make_make_option_get(project, "name"));

	fprintf(f, "# Default target\n");
	fprintf(f, "all: $(TARGET)\n\n");

	fprintf(f, "# Linking the object files to create the executable\n");
	fprintf(f, "$(TARGET): $(SRCS:.c=.o)\n");
	fprintf(f, "\t$(CC) $(CFLAGS) -o $@ $^ $(LIBS)\n\n");

	fprintf(f, "# Compiling individual source files into object files\n");
	fprintf(f, "%%.o: %%.c\n");
	fprintf(f, "\t$(CC) $(CFLAGS) -c -o $@ $<\n\n");

	fprintf(f, "# Clean up the object files and executable\n");
	fprintf(f, "clean:\n");
	fprintf(f, "\trm -f $(SRCS:.c=.o) $(TARGET)\n");
}

int make_make_android_options_get(MakeMakeProject *project, MakeMakeAndroidOptions *androidOptions)
{
	androidOptions->sdkVersion 		= make_make_option_get(project, "sdk_version");
	androidOptions->ndkVersion 		= make_make_option_get(project, "ndk_version");
	androidOptions->packageName 	= make_make_option_get(project, "package_name");
	androidOptions->packageVersion 	= make_make_option_get(project, "package_version");

	if( androidOptions->sdkVersion == NULL )
	{
		printf("MakeMake Error: missing makemake option 'sdk_version' for android build\n");
		return 0;
	}

	if( androidOptions->ndkVersion == NULL )
	{
		printf("MakeMake Error: missing makemake option 'ndk_version' for android build\n");
		return 0;
	}

	if( androidOptions->packageName == NULL )
	{
		printf("MakeMake Error: missing makemake option 'package_name' for android build\n");
		return 0;
	}

	if( androidOptions->packageVersion == NULL )
	{
		printf("MakeMake Error: missing makemake option 'package_version' for android build\n");
		return 0;
	}

	//FK: set all architectures 
	//    TODO: make this an option
	androidOptions->architectures[0] = "arm64";
	androidOptions->architectures[1] = "arm32";
	androidOptions->architectures[2] = "x64";
	androidOptions->architectures[3] = "x86";
	return 1;
}

const char* make_make_android_sysroot_architecture_get(const char* architecture)
{
	if(strcmp(architecture, "arm64") == 0)
	{
		return "aarch64-linux-android";
	}
	else if(strcmp(architecture, "arm32") == 0)
	{
		return "arm-linux-androideabi";
	}
	else if(strcmp(architecture, "x86") == 0)
	{
		return "i686-linux-android";
	}

	return "x86_64-linux-android";
}

const char* make_make_android_clang_architecture_get(const char* architecture)
{
	if(strcmp(architecture, "arm64") == 0)
	{
		return "aarch64-linux-android";
	}
	else if(strcmp(architecture, "arm32") == 0)
	{
		return "armv7a-linux-androideabi";
	}
	else if(strcmp(architecture, "x86") == 0)
	{
		return "i686-linux-android";
	}

	return "x86_64-linux-android";
}

const char* make_make_android_lib_folder_get(const char* architecture)
{
	if(strcmp(architecture, "arm64") == 0)
	{
		return "arm64-v8a";
	}
	else if(strcmp(architecture, "arm32") == 0)
	{
		return "armeabi-v7a";
	}
	else if(strcmp(architecture, "x86") == 0)
	{
		return "x86";
	}

	return "x86_64";
}

const char* make_make_android_generate_manifest(const MakeMakeAndroidOptions* options, const char* project_name)
{
	const char* manifestTemplate = 
	      "<?xml version=\"1.0\" encoding=\"utf-8\"?>\\n"
	      "<manifest xmlns:android=\"http://schemas.android.com/apk/res/android\" package=\"%s\" android:versionCode=\"%s\" android:versionName=\"%s\">\\n"
        "\\t<uses-sdk android:minSdkVersion=\"9\" android:targetSdkVersion=\"%s\"/>\\n"
        "\\t<uses-permission android:name=\"android.permission.INTERNET\" />\\n"
        "\\t<uses-permission android:name=\"android.permission.ACCESS_NETWORK_STATE\" />\\n"
        "\\t<application android:hasCode=\"false\" android:debuggable=\"false\">\\n"
      "\\t\\t<meta-data android:name=\"android.max_aspect\" android:value=\"9.1\" />\\n"
      "\\t\\t<activity android:name=\"android.app.NativeActivity\" android:label=\"%s\" android:screenOrientation=\"portrait\" android:exported=\"true\" android:configChanges=\"orientation|keyboardHidden\">\\n"
    "\\t\\t\\t<meta-data android:name=\"android.app.lib_name\" android:value=\"%s\" />\\n"
    "\\t\\t\\t<meta-data android:name=\"android.max_aspect\" android:value=\"9.1\" />\\n"
  "\\t\\t\\t\\t<intent-filter>\\n"
"\\t\\t\\t\\t\\t<action android:name=\"android.intent.action.MAIN\" />\\n"
"\\t\\t\\t\\t\\t<category android:name=\"android.intent.category.LAUNCHER\" />\\n"
"\\t\\t\\t\\t</intent-filter>\\n"
"\\t\\t\\t</activity>\\n"
"\\t\\t</application>\\n"
"\\t</manifest>\\n";

	char* manifest = (char*)malloc(1024*1024);
	sprintf(manifest, manifestTemplate, options->packageName, options->packageVersion, options->packageVersion, options->sdkVersion, project_name, project_name);
	return manifest;
}

void make_make_android_toolchain_get(const MakeMakeAndroidOptions* androidOptions, MakeMakeAndroidToolchain* androidToolchain, const char* hostPlatform)
{
	//FK: TODO: add support for 'host_platform'
	const char* llvmPrebuiltPlatform = "linux-x86_64";

	char androidNdkDirectory[MAKE_MAKE_TOKEN_LENGTH_MAX];
	char androidNativeAppGluePath[MAKE_MAKE_TOKEN_LENGTH_MAX];
	char androidSysRootPath[MAKE_MAKE_TOKEN_LENGTH_MAX];

	sprintf(androidToolchain->sdkBaseDirectory, "$(HOME)/.android_sdk_%s", androidOptions->sdkVersion);
	sprintf(androidNdkDirectory, "%s/ndk/%s", androidToolchain->sdkBaseDirectory, androidOptions->ndkVersion);
	sprintf(androidNativeAppGluePath, "%s/sources/android/native_app_glue", androidNdkDirectory);
	sprintf(androidSysRootPath, "%s/toolchains/llvm/prebuilt/%s/sysroot", androidNdkDirectory, llvmPrebuiltPlatform);

	sprintf(androidToolchain->ndkCompilerExecutablePath, "%s/toolchains/llvm/prebuilt/%s/bin/clang", androidNdkDirectory, llvmPrebuiltPlatform);
	sprintf(androidToolchain->ndkLinkerExecutablePath, "%s/toolchains/llvm/prebuilt/%s/bin/ld", androidNdkDirectory, llvmPrebuiltPlatform);
	sprintf(androidToolchain->ndkNativeAppGlueSourcePath, "%s/android_native_app_glue.c", androidNativeAppGluePath );
	sprintf(androidToolchain->sdkSysRootIncludePath, "%s/usr/include", androidSysRootPath);
	sprintf(androidToolchain->sdkSysRootLibraryPath, "%s/usr/lib", androidSysRootPath);
	sprintf(androidToolchain->sdkBuildToolsPath, "%s/build-tools/%s.0.0", androidToolchain->sdkBaseDirectory, androidOptions->sdkVersion);
	sprintf(androidToolchain->sdkPlatformsPath, "%s/platforms/android-%s", androidToolchain->sdkBaseDirectory, androidOptions->sdkVersion);

	const char* sdkDownloadArchiveName = "commandlinetools-linux-7583922_latest.zip";
	sprintf(androidToolchain->sdkDownloadLink, "https://dl.google.com/android/repository/%s", sdkDownloadArchiveName);
}

void make_make_android_sdk_add(FILE* f, const MakeMakeAndroidOptions* androidOptions, const MakeMakeAndroidToolchain* androidToolchain )
{
	char sdkManangerExecutable[MAKE_MAKE_TOKEN_LENGTH_MAX];
	char sdkBuildToolsVersion[10];

	sprintf(sdkBuildToolsVersion, "%s.0.0", androidOptions->sdkVersion);
	sprintf(sdkManangerExecutable, "%s/cmdline-tools/bin/sdkmanager", androidToolchain->sdkBaseDirectory);

	fprintf(f, "$(android_sdk):\n");
	fprintf(f, "\tmkdir -p %s\n", androidToolchain->sdkBaseDirectory);
	fprintf(f, "\twget -O %s/sdk_archive.zip %s\n", androidToolchain->sdkBaseDirectory, androidToolchain->sdkDownloadLink);
	fprintf(f, "\tunzip -o -q %s/sdk_archive.zip -d %s\n", androidToolchain->sdkBaseDirectory, androidToolchain->sdkBaseDirectory);
	fprintf(f, "\tyes | %s --sdk_root=%s --licenses &> /dev/null\n", sdkManangerExecutable, androidToolchain->sdkBaseDirectory);
	fprintf(f, "\t%s --sdk_root=%s 'platforms;android-%s'\n", sdkManangerExecutable, androidToolchain->sdkBaseDirectory, androidOptions->sdkVersion);
	fprintf(f, "\t%s --sdk_root=%s 'ndk;%s'\n", sdkManangerExecutable, androidToolchain->sdkBaseDirectory, androidOptions->ndkVersion);
	fprintf(f, "\t%s --sdk_root=%s 'build-tools;%s'\n", sdkManangerExecutable, androidToolchain->sdkBaseDirectory, sdkBuildToolsVersion);
	fprintf(f, "\ttouch %s/.sentinel\n\n", androidToolchain->sdkBaseDirectory);
}

void make_make_android_keystore_add(FILE* f)
{
	fprintf(f, "$(keystore_file):\n");
	fprintf(f, "\tkeytool -validity 20000 -keystore $(keystore_file) -genkey -noprompt -keypass ABCD1234 -storepass ABCD1234 -keyalg RSA -dname \"CN=mqttserver.ibm.com, OU=ID, O=IBM, L=Hursley, S=Hants, C=GB\"\n\n");
}

void make_make_android_manifest_add(FILE* f, const MakeMakeAndroidOptions* androidOptions, const char* projectName)
{
	fprintf(f, "$(android_manifest):\n");
	fprintf(f, "\tprintf '%s' > $(android_manifest)\n\n", make_make_android_generate_manifest(androidOptions, projectName));
}

void make_make_android_unaligned_apk_add(FILE* f, const MakeMakeAndroidOptions* androidOptions, const MakeMakeAndroidToolchain* androidToolchain)
{
	int i;
	fprintf(f, "$(unaligned_apk): $(android_manifest)\n");
	for(i = 0; i < 4u; ++i)
		if(androidOptions->architectures[i] != NULL)
			fprintf(f, "\tcp android_artifacts/%s/*.so android_artifacts/apk_content/lib/%s/\n", androidOptions->architectures[i], make_make_android_lib_folder_get(androidOptions->architectures[i]));
	fprintf(f, "\t%s/aapt p -f -M $(android_manifest) -F $(unaligned_apk) -I %s/android.jar android_artifacts/apk_content", androidToolchain->sdkBuildToolsPath, androidToolchain->sdkPlatformsPath);
	fprintf(f, "\n\n");
}

void make_make_android_apk_add(FILE* f, const MakeMakeAndroidToolchain* androidToolchain)
{
	fprintf(f, "$(apk): $(native_libraries) $(unaligned_apk) $(keystore_file)\n");
	fprintf(f, "\t%s/zipalign -p -f -v 4 $(unaligned_apk) $(apk)\n", androidToolchain->sdkBuildToolsPath);
	fprintf(f, "\t%s/apksigner sign --ks $(keystore_file) --ks-pass pass:ABCD1234 $(apk)\n\n", androidToolchain->sdkBuildToolsPath);
}

void make_make_android(MakeMakeProject *project, FILE *f)
{
	MakeMakeAndroidOptions androidOptions;
	if( !make_make_android_options_get(project, &androidOptions) )
	{
		return;
	}

	const char* hostPlatform = make_make_option_get(project, "host_platform");
	if (hostPlatform != NULL)
	{
		//TODO...
	}

	MakeMakeAndroidToolchain androidToolchain;
	make_make_android_toolchain_get(&androidOptions, &androidToolchain, hostPlatform);

	const char* project_name = make_make_option_get(project, "name");

	unsigned int i;
	fprintf(f, "# Compiler and compiler flags\n");
	fprintf(f, "CC = %s\n", androidToolchain.ndkCompilerExecutablePath);
	fprintf(f, "LD = %s\n\n", androidToolchain.ndkLinkerExecutablePath);

	fprintf(f, "# target executable name(s)\n");
	for(i = 0; i < 4u; ++i)
		if(androidOptions.architectures[i] != NULL)
			fprintf(f, "library_%s := android_artifacts/%s/lib%s.so\n", androidOptions.architectures[i], androidOptions.architectures[i], project_name);
	fprintf(f, "\n");
	fprintf(f, "apk := %s.apk\n", project_name);
	fprintf(f, "unaligned_apk := %s.unaligned.apk\n", project_name);
	fprintf(f, "android_manifest := android_artifacts/AndroidManifest.xml\n");
	fprintf(f, "keystore_file := android_artifacts/signature.keystore\n");
	fprintf(f, "android_sdk := %s/.sentinel\n", androidToolchain.sdkBaseDirectory);
	fprintf(f, "# Libraries\n");
	fprintf(f, "LIBS = -lc -landroid -llog\n\n");
	fprintf(f, "# Source files\n");
	fprintf(f, "SRCS = ");
	for(unsigned int i = 0; i < project->file_count; i++)
		if(project->files[i].added && !project->files[i].header)
			fprintf(f, "%s ", project->files[i].file_name);

	fprintf(f, "%s ", androidToolchain.ndkNativeAppGlueSourcePath);
	fprintf(f, "\n\n");

	fprintf(f, "native_libraries := ");
	for(i = 0; i < 4u; ++i)
		if(androidOptions.architectures[i] != NULL)
			fprintf(f, "android_artifacts/%s/lib%s.so ", androidOptions.architectures[i], make_make_option_get(project, "name"));
	fprintf(f, "\n\n");

	fprintf(f, "all: $(android_sdk) setup obj_files $(apk)\n\n");

	fprintf(f, "setup:\n");
	for(i = 0; i < 4u; ++i)
	{
		if(androidOptions.architectures[i] != NULL)
		{
			fprintf(f, "\tmkdir -p android_artifacts/%s/\n", androidOptions.architectures[i]);
			fprintf(f, "\tmkdir -p android_artifacts/apk_content/lib/%s/\n", make_make_android_lib_folder_get(androidOptions.architectures[i]));
		}
	}	
	fprintf(f, "\n");

	make_make_android_sdk_add(f, &androidOptions, &androidToolchain);
	make_make_android_keystore_add(f);
	make_make_android_manifest_add(f, &androidOptions, project_name);
	make_make_android_unaligned_apk_add(f, &androidOptions, &androidToolchain);
	make_make_android_apk_add(f, &androidToolchain);
	
	for(i = 0; i < 4u; ++i)
	{
		if(androidOptions.architectures[i] != NULL)
		{
			const char* clangArchitectureName = make_make_android_clang_architecture_get(androidOptions.architectures[i]);
			const char* sysrootArchitectureName = make_make_android_sysroot_architecture_get(androidOptions.architectures[i]);

			fprintf(f, "CFLAGS_%s = -Wall -g -fPIC -Wno-unknown-pragmas -target %s%s -I%s -isystem%s -isystem%s/%s\n", androidOptions.architectures[i], clangArchitectureName, androidOptions.sdkVersion, androidToolchain.ndkNativeAppGlueSourcePath, androidToolchain.sdkBuildToolsPath, androidToolchain.sdkSysRootIncludePath, sysrootArchitectureName);
			fprintf(f, "LDFLAGS_%s = $(LIBS) -shared -L%s/%s/%s/", androidOptions.architectures[i], androidToolchain.sdkSysRootLibraryPath, sysrootArchitectureName, androidOptions.sdkVersion);
			fprintf(f, "\n");
		}
	}
	fprintf(f, "\n");

	fprintf(f, "obj_files: $(SRCS:.c=.o)\n");
	fprintf(f, "\n\n");

	for(i = 0; i < 4u; ++i)
	{
		if(androidOptions.architectures[i] != NULL)
		{
			fprintf(f, "obj_files_%s = $(wildcard android_artifacts/%s/*.o)\n\n", androidOptions.architectures[i], androidOptions.architectures[i]);
			fprintf(f, "$(library_%s): $(obj_files_%s)\n", androidOptions.architectures[i], androidOptions.architectures[i]);
			fprintf(f, "\t$(LD) $(LDFLAGS_%s) -o $@ $(obj_files_%s)\n\n", androidOptions.architectures[i], androidOptions.architectures[i]);
		}
	}

	fprintf(f, "# Compiling individual source files into object files\n");
	fprintf(f, "%%.o: %%.c\n");
	for(i = 0; i < 4u; ++i)
		if(androidOptions.architectures[i] != NULL)
			fprintf(f, "\t$(CC) $(CFLAGS_%s) -c -o android_artifacts/%s/$(notdir $@) $<\n", androidOptions.architectures[i], androidOptions.architectures[i]);
	fprintf(f, "\n");

	fprintf(f, "# Clean up the object files and executable\n");
	fprintf(f, "clean:\n");
	fprintf(f, "\trm -rf android_artifacts\n");
}

void make_make_ios(MakeMakeProject *project, FILE *f)
{

}

void makemake_gen_makemakemake(char *source_file)
{
	size_t i, size;
	FILE *f;
	char *text;
	f = fopen("makemakemake.sh", "w");
	if(f == NULL)
	{
		printf("Error: Failed to open makemakemake.sh");
		return;
	}
	if(source_file == NULL)
		source_file = "makemake.c";
	text = make_make_text_load(source_file, &size);
	if(text == NULL)
	{
	//	printf("Error: Failed to open makemake.c");
		fclose(f);
		return;
	}
	fprintf(f, "echo Running MakeMakeMake by Eskil Steenberg Hald\n");	
	fprintf(f, "source=\"");
	for(i = 0; i < size; i++)
	{
		switch(text[i])
		{
			case '\"' :
				fprintf(f, "\\\"");
			break;
			case '\n' :
				fprintf(f, "\\n");
			break;
			case '\r' :
				fprintf(f, "\\r");
			break;
			case '\'' :
				fprintf(f, "\\\'");
			break;
			default :
				fprintf(f, "%c", text[i]);
			break;
		}
	}
	fprintf(f, "\"\n");
	fprintf(f, "echo \"$source\" > \"makemake.c\"\n");
	fprintf(f, "gcc makemake.c -o makemake\n\n"
				"set -e\n"
				"cmd=`basename $0`\n"
				"echo \"Running $cmd\" 1>&2\n\n"

				"if [ $# -gt 3 ]; then\n"
				"    echo \"[$cmd] takes up to three args\" 1>&2\n"
				"    exit 1\n"
				"fi\n"
				"./makemake $*  1>&2\n"
				"make\n");
	fclose(f);
}

int main(int argc, char **argv)
{
	char file_name[1024], option_buffer[MAKE_MAKE_TOKEN_LENGTH_MAX], path_buffer[1024], *s, *platform = "", *home = DIR_HOME_PATH, *makemakemakemake = "-makemakemakemake";
	unsigned int i, j, k, path_length;
	MakeMakeFile *file;
	MakeMakeProject project;
	FILE *f;
	char *option, *stdout_string = "stdout";
//	char *argv_test[] = {"something", "C:\\Mergesource\\t_test.c", "-name", "test", "-makefile", "stdout"};
//	char *argv_test[] = {"something", "-makemakemakemake", "C:\\Mergesource\\makemake.c"};
//	argv = argv_test;
//	argc = 3;

	project.files = NULL;
	project.file_count = 0;
	project.options.array = NULL;
	project.options.array_length = 0;
	if(argc <= 1)
	{
		printf("MakeMake\n--------\n");
		printf("MakeMake is a command line tool that generates a make file form a single .c file by scanning and traversing includes and defines in files in the same directory.\n\n");
		printf("\nMakeMake is free and opensource under the MIT License.\nWritten by Eskil Steenberg Hald eskil@quelsolaar.com www.quelsolaar.com eskil@quelsolaar.com.\n\n");
		printf("Usage:\n------\n");
		printf("\t-makemake <c file> <options>\n\nOptions:\n--------\n");
		printf("\t-name <name> Name of the executable \n");
		printf("\t-makefile <name> Name of the make file (or stdout)\n");
		printf("\t-compilerflag <flag> Custom compiler flags.\n");
		printf("\t-platform <name> Platform define\n");
		printf("\t-lib <name> Library dependency\n");
		printf("\t-define <name> Adds a define to the build\n");
		printf("Options can be added in .c and .h files using the following pragma:\n");
		printf("\t#pragma makemake <option name> <option> <optional platform>\n\n");
		printf("If a platform is defined, the option is only active when that platform is defined. More than one platform can be defined simultaneously (Example \"linux\" and \"posix\")\n");
		printf("\tGenerate:\n--------\n");
		printf("\t-makemakemakemake <optional path to makemake.c> generates makemakemake.sh portable standalone shellscript\n\n");
	
		
		return TRUE;
	}
	for(i = 0; argv[1][i] != 0 && makemakemakemake[i] == argv[1][i]; i++);
	if(makemakemakemake[i] == argv[1][i])
	{
		printf("Generate makemakemake.sh portable standalone shellscript\n...");
		if(argc > 2)
			makemake_gen_makemakemake(argv[2]);
		else
			makemake_gen_makemakemake(NULL);
		printf("Done\n");
		return 1;
	}
	

	j = ~0;
	for(i = 0; i < 1024 && argv[1][i] != 0; i++)
		if(argv[1][i] == DIR_SLASH)
			j = i;
	if(i == 1024)
	{
		for(i = 0; argv[1][i] != 0; i++);
		printf("MakeMake Error: Path is too long: %u bytes (MAX: 1024)\n", i);
		return FALSE;
	}
	if(j == ~0)
	{
		for(i = path_length = 0; i < 1024 && argv[0][i] != 0; i++)
		{
			path_buffer[i] = project.path[i] = argv[0][i];
			if(argv[0][i] == DIR_SLASH)
				path_length = i + 1;
		}
		path_buffer[path_length] = project.path[path_length] = 0;
		for(i = 0; i < 1024 && argv[1][i] != 0; i++)
			file_name[i] = argv[1][i];
		file_name[i] = 0;
	}else
	{
		j++;
		for(path_length = 0; path_length < j; path_length++)
			path_buffer[path_length] = project.path[path_length] = argv[1][path_length];
		path_buffer[path_length] = project.path[path_length] = 0;
		for(i = 0; i < 1024 && argv[1][i + j] != 0; i++)
			file_name[i] = argv[1][i + j];
		file_name[i] = 0;
	}
	for(i = 2; i < argc - 1; i++)
	{                  
		if(argv[i][0] == '-')
		{
			for(j = 0; j < MAKE_MAKE_TOKEN_LENGTH_MAX && argv[i][j + 1] != 0; j++)
				option_buffer[j] = argv[i][j + 1];
			if(j < MAKE_MAKE_TOKEN_LENGTH_MAX - 1)
			{
				i++;
				option_buffer[j++] = ' ';
				for(k = 0; j < MAKE_MAKE_TOKEN_LENGTH_MAX - 1 && argv[i][k] != 0; k++)
					option_buffer[j++] = argv[i][k];
				option_buffer[j] = 0;
				make_make_list_add(&project.options, option_buffer);
			}
		}
	}
	if(NULL == make_make_option_get(&project, "platform"))
	{
#ifdef	__linux__
		make_make_list_add(&project.options, "platform linux");
		make_make_list_add(&project.options, "platform posix");
#endif 
#ifdef	_WIN32
		make_make_list_add(&project.options, "platform win32");
#endif 
#ifdef	__ANDROID__
		make_make_list_add(&project.options, "platform android");
		make_make_list_add(&project.options, "platform posix");
#endif 
#ifdef	MACOSX
		make_make_list_add(&project.options, "platform macosx");
		make_make_list_add(&project.options, "platform posix");
#endif 
	}

	//make_make_parse_file(&project, argv[1], file_name, FALSE, platform);

	printf("Parsing files...\n");
	for(i = 0; make_make_path_search(".c", TRUE, project.path, FALSE, i, &path_buffer[path_length], 1024 - path_length); i++)
		make_make_parse_file(&project, path_buffer, &path_buffer[path_length], FALSE, platform);
	for(i = 0; make_make_path_search(".h", TRUE, project.path, FALSE, i, &path_buffer[path_length], 1024 - path_length); i++)
		make_make_parse_file(&project, path_buffer, &path_buffer[path_length], TRUE, platform);
/*
	for(i = 0; i < project.file_count; i++)
		printf("%s ", project.files[i].file_name);
	printf("\n");
*/
	file = make_make_file_find(project.files, project.file_count, file_name, NULL);
	if(file == NULL)
	{
		printf("MakeMake Error: %s not found\n", file_name);
		return FALSE;
	}
	
	printf("Traversing files...\n");
	make_make_file_add(project.files, project.file_count, file);
	printf("Gather options...\n");
	make_make_gather_options(&project);
	if(NULL == make_make_option_get(&project, "name"))
	{
		option_buffer[0] = 'n';
		option_buffer[1] = 'a';
		option_buffer[2] = 'm';
		option_buffer[3] = 'e';
		option_buffer[4] = ' ';
		for(i = 0; i < 63 && file_name[i] != 0 && file_name[i] != '.' && i + 6 < MAKE_MAKE_TOKEN_LENGTH_MAX; i++)
			option_buffer[i + 5] = file_name[i];
		option_buffer[i + 5] = 0;
		make_make_list_add(&project.options, option_buffer);
	}

	for(i = 0; i < project.options.array_length; i = make_make_list_next(&project.options, i))
		printf(" - %s\n", &project.options.array[i]);
	printf("Generating makefile...\n");
	option = make_make_option_get(&project, "makefile");


	if(option == NULL)
		option = "Makefile";
	for(i = 0; option[i] != 0 && option[i] == stdout_string[i]; i++);
	if(option[i] == stdout_string[i])
		f = stdout;
	else
		f = fopen(option, "w");
	if(f == NULL)
	{
		printf("MakeMake Error: Failed to open file %s for writing", file_name);
		return 1;
	}
	
	if(make_make_list_test(&project.options, "platform android"))
		make_make_android(&project, f); /* write different function for different OS */
	else if(make_make_list_test(&project.options, "platform ios"))
		make_make_ios(&project, f);
	else
//	if(make_make_list_test(&project.options, "platform posix"))
		make_make_posix(&project, f);
		
	if(f != stdout)
		fclose(f);
	printf("MakeMake Sucsess!");
	return TRUE;
}
