// ftaglib.cpp : Defines the exported functions for the DLL application.
//

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
	_tgroup* group;
};

class tag;

class _tgroup
{
public:
	wchar_t name[256];
	int tag_count;
	virtual LPWSTR get_name() = 0;
	virtual void add_tag(LPWSTR) = 0;
	virtual int get_tag_count() = 0;
	virtual vector<tag> get_tags() = 0;
};

class tag : public _tag
{
public:
	tag(LPWSTR n, _tgroup* grp)
	{
		lstrcpyW(name, n);
		group = grp;
	}
};

class tag_group : public _tgroup
{
public:
	vector<tag> tags;

	tag_group(LPWSTR n)
	{
		lstrcpyW(name, n);
		tag_count = 0;
	}

	LPWSTR get_name()
	{
		return name;
	}

	void add_tag(LPWSTR n)
	{
		tag* t = new tag(n, this);
		tags.push_back(*t);
		tag_count++;
	}

	int get_tag_count()
	{
		return tag_count;
	}

	vector<tag> get_tags()
	{
		return tags;
	}
};

extern "C" EXPORT _tag* CreateTagInstance(LPWSTR name, _tgroup* grp)
{
	return new tag(name,grp);
}

extern "C" EXPORT _tgroup* CreateTagGroupInstance(LPWSTR name)
{
	return new tag_group(name);
}

extern "C" EXPORT void DestroyTagGroupInstance(_tgroup* inst)
{
	delete inst;
}