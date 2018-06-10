#pragma once

#include "stdafx.h"
#include <string>

#ifdef FTAGLIB_EXPORTS
#    define EXPIMP __declspec(dllexport)
#    define EXPIMP_STL
#else
#    define EXPIMP __declspec(dllimport)
#    define EXPIMP_STL extern
#endif

#define DB_NAME "ftag.db"

using namespace std;

class EXPIMP Tag
{
public:
	wchar_t name[256];
	int tagGroupIndex;
	Tag();
	Tag(LPWSTR, int);
};

EXPIMP_STL template class EXPIMP vector<Tag>;

class EXPIMP TagGroup
{
public:
	int index;
	wchar_t name[255];
	int tag_count;
	vector<Tag> tags;
	TagGroup();
	TagGroup(LPWSTR n, int i);
	LPWSTR get_name();
	void add_tag(LPWSTR tagName);
	void remove_tag(LPWSTR);
	void remove_tag(int);
	int get_tag_count();
	vector<Tag> get_tags();
	int find_tag(LPWSTR tagName);
	void save(FILE* file);
	void load(FILE* file);
};

class EXPIMP TaggedFile
{
public:
	wchar_t name[255];
	int tag_count;
	vector<Tag> tags;
	TaggedFile();
	TaggedFile(LPWSTR n);
	LPWSTR get_name();
	void add_tag(Tag* tag);
	void remove_tag(Tag* tag);
	int get_tag_count();
	vector<Tag> get_tags();
	int find_tag(LPWSTR tagName, int gid);
	void save(FILE* file);
	void load(FILE* file);
};

EXPIMP_STL template class EXPIMP vector<TagGroup>;
EXPIMP_STL template class EXPIMP vector<TaggedFile>;

class EXPIMP TagDB
{
public:
	wchar_t name[256];
	wchar_t dir_path[MAX_PATH];
	vector<TagGroup> tag_groups;
	vector<TaggedFile> files;
	int file_count, group_count;
	TagDB();
	TagDB(LPWSTR n, LPWSTR path);
	void add_group(LPWSTR groupName);
	//Returns index of added file
	int add_file(LPWSTR fileName);
	//Return index of tag group
	int find_tag_group(LPWSTR groupName);
	//Returns index of file
	int find_file(LPWSTR fileName);
	void add_tag_to_group(LPWSTR groupName, LPWSTR tagName);
	void remove_tag_from_group(LPWSTR groupName, LPWSTR tagName);
	void add_tag_to_file(LPWSTR fileName, LPWSTR groupName, LPWSTR tagName);
	void remove_tag_from_file(LPWSTR fileName, LPWSTR groupName, LPWSTR tagName);
	//Returns list of filenames as vector of LPWSTRs
	vector<LPWSTR> filter(LPWSTR groupName, LPWSTR tagName);
	void save_db();
	bool load_db();
	void DEBUG_info();
};


extern "C" EXPIMP TagDB* CreateTagDbInstance(LPWSTR name, LPWSTR path);
extern "C" EXPIMP TagDB* CreateTagDbEmptyInstance();
extern "C" EXPIMP void DestroyTagDbInstance(TagDB* inst);