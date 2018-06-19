#pragma once

#include <stdlib.h>
#include <vector>
#include <string>
#include <chrono>

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
	long id;
	wchar_t name[256];
	long tagGroupId;
public:
	Tag();
	Tag(long, LPWSTR, long);
	long get_id();
	long get_group_id();
	LPWSTR get_name();
	void ungroup();
};

EXPIMP_STL template class EXPIMP vector<Tag>;

class EXPIMP TagGroup
{
	long id;
	wchar_t name[255];
	int tag_count;
	vector<Tag> tags;
public:
	TagGroup();
	TagGroup(long, LPWSTR n, int i);
	LPWSTR get_name();
	long get_id();
	void add_tag(long uid,LPWSTR tagName);
	void remove_tag(long);
	void remove_tag(int);
	int get_tag_count();
	const vector<Tag>& get_tags();
	int find_tag(LPWSTR tagName);
	Tag* get_tag(long tid);
	void save(FILE* file);
	void load(FILE* file);
};

class TagGroupSkel
{
public:
	long gid;
	vector<long> tids;
	TagGroupSkel(long gid)
	{
		this->gid = gid;
	}
};

class EXPIMP TaggedFile
{
	long id;
	wchar_t name[255];
	int tag_count;
	vector<TagGroupSkel> tag_groups;
public:
	TaggedFile();
	TaggedFile(long, LPWSTR n);
	long get_id();
	LPWSTR get_name();
	int get_tag_count();
	const vector<TagGroupSkel>& get_tag_groups();
	void add_tag(long gid,long tid);
	void remove_tag(long gid, long tid);
	void remove_group(long gid);
	vector<long> get_tags();
	bool check_tag(long gid, long tid);
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
	long last_id;
	TagDB();
	TagDB(LPWSTR n, LPWSTR path);
	long gen_id();
	TagGroup* get_tag_group(LPWSTR groupName);
	TagGroup* get_tag_group(long gid);
	TaggedFile* get_file(LPWSTR fileName);
	void add_group(LPWSTR groupName);
	//Returns index of added file
	int add_file(LPWSTR fileName);
	void remove_group(LPWSTR groupName);
	//Return index of tag group
	int find_tag_group(LPWSTR groupName);
	int find_tag_group(long gid);
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