#include "stdafx.h"
#include <string>

//#ifdef FTAGLIB_EXPORT
#define EXPORT __declspec(dllexport)
//#endif

using namespace std;

class _tgroup;

class _tag
{
public:
	wchar_t name[256];
	int tagGroupIndex;
};

class Tag;

class _tgroup
{
public:
	int index;
	wchar_t name[255];
	int tag_count;
	vector<Tag> tags;
	virtual LPWSTR get_name() = 0;
	virtual void add_tag(LPWSTR) = 0;
	virtual int get_tag_count() = 0;
	virtual vector<Tag> get_tags() = 0;
	virtual int find_tag(LPWSTR) = 0;
};

class Tag : public _tag
{
public:
	Tag(LPWSTR n, int i)
	{
		lstrcpyW(name, n);
		tagGroupIndex = i;
	}
};

class TagGroup : public _tgroup
{
public:
	TagGroup(LPWSTR n, int i)
	{
		lstrcpyW(name, n);
		tag_count = 0;
		index = i;
	}

	LPWSTR get_name()
	{
		return name;
	}

	void add_tag(LPWSTR n)
	{
		Tag* t = new Tag(n, index);
		tags.push_back(*t);
		tag_count++;
	}

	int get_tag_count()
	{
		return tag_count;
	}

	vector<Tag> get_tags()
	{
		return tags;
	}

	int find_tag(LPWSTR tname)
	{
		for (int i = 0; i < tag_count; i++)
		{
			if (!lstrcmpiW(tags[i].name,tname))
			{
				return i;
			}
		}
		return -1;
	}
};


class _tagged_file
{
public:
	wchar_t name[255];
	int tag_count;
	vector<Tag> tags;
	virtual LPWSTR get_name() = 0;
	virtual void add_tag(Tag*) = 0;
	virtual int get_tag_count() = 0;
	virtual vector<Tag> get_tags() = 0;
	virtual int find_tag(LPWSTR) = 0;
};


class TaggedFile : public _tagged_file
{
public:
	TaggedFile(LPWSTR n)
	{
		lstrcpyW(name, n);
		tag_count = 0;
	}

	LPWSTR get_name()
	{
		return name;
	}
	
	void add_tag(Tag* t)
	{
		tags.push_back(*t);
		tag_count++;
	}
	
	int get_tag_count()
	{
		return tag_count;
	}

	vector<Tag> get_tags()
	{
		return tags;
	}

	int find_tag(LPWSTR tname)
	{
		for (int i = 0; i < tag_count; i++)
		{
			if (!lstrcmpiW(tags[i].name, tname))
			{
				return i;
			}
		}
		return -1;
	}
};

class _tagdb
{
public:
	wchar_t name[256];
	wchar_t dir_path[MAX_PATH];
	vector<TagGroup> tag_groups;
	vector<TaggedFile> files;
	int file_count, group_count;
	virtual void add_group(LPWSTR) = 0;
	virtual int add_file(LPWSTR) = 0;
	virtual int find_tag_group(LPWSTR) = 0;
	virtual int find_file(LPWSTR) = 0;
	virtual void add_tag_to_group(LPWSTR, LPWSTR) = 0;
	virtual void add_tag_to_file(LPWSTR, LPWSTR, LPWSTR) = 0;
};

class TagDB : public _tagdb
{
public:
	TagDB(LPWSTR n, LPWSTR path)
	{
		lstrcpyW(name, n);
		lstrcpyW(dir_path, path);
		file_count = 0;

		TagGroup* ungrp = new TagGroup((LPWSTR)L"Ungrouped",0);
		tag_groups.push_back(*ungrp);
		group_count = 1;
	}

	void add_group(LPWSTR name)
	{
		TagGroup* grp = new TagGroup(name,group_count);
		tag_groups.push_back(*grp);
		group_count++;
	}

	int add_file(LPWSTR fname)
	{
		TaggedFile* file = new TaggedFile(fname);
		files.push_back(*file);
		file_count++;
		return file_count-1;
	}

	int find_tag_group(LPWSTR grpname)
	{
		for (int i = 0; i < group_count; i++)
		{
			if (!lstrcmpiW(tag_groups[i].name,grpname))
			{
				return i;
			}
		}
		return -1;
	}

	int find_file(LPWSTR fname)
	{
		for (int i = 0; i < file_count; i++)
		{
			if (!lstrcmpiW(files[i].name, fname))
			{
				return i;
			}
		}
		return -1;
	}

	void add_tag_to_group(LPWSTR grpname, LPWSTR t)
	{
		TagGroup* tgrp = &tag_groups[find_tag_group(grpname)];
		if (tgrp->find_tag(t) == -1)
			tgrp->add_tag(t);
	}

	void add_tag_to_file(LPWSTR fname, LPWSTR grpname, LPWSTR tname)
	{
		int gid = find_tag_group(grpname);
		if (gid != -1)
		{
			TagGroup* grp = &tag_groups[gid];
			int tid = grp->find_tag(tname);
			if (tid != -1)
			{
				Tag* t = &grp->tags[tid];
				int fid = find_file(fname);
				if (fid != -1)
				{
					files[fid].add_tag(t);
				}
				else
				{
					//check if file exists in dir
					fid = add_file(fname);
					files[fid].add_tag(t);
				}
			}
		}
	}
};

extern "C" EXPORT _tagdb* CreateTagDbInstance(LPWSTR name, LPWSTR path)
{
	return new TagDB(name, path);
}

extern "C" EXPORT void DestroyTagDbInstance(_tagdb* inst)
{
	delete inst;
}