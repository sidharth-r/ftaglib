#include "stdafx.h"
#include <string>

#include "ftaglib.h"

using namespace std;

Tag::Tag()
{}

Tag::Tag(LPWSTR n, int i)
{
	lstrcpyW(name, n);
	tagGroupIndex = i;
}

TagGroup::TagGroup()
{}

TagGroup::TagGroup(LPWSTR n, int i)
{
	lstrcpyW(name, n);
	tag_count = 0;
	index = i;
}

LPWSTR TagGroup::get_name()
{
	return name;
}

void TagGroup::add_tag(LPWSTR tname)
{
	Tag* t = new Tag(tname, index);
	tags.push_back(*t);
	tag_count++;
}

void TagGroup::remove_tag(LPWSTR tname)
{
	for (vector<Tag>::iterator iter = tags.begin(); iter != tags.end(); iter++)
	{
		if (!lstrcmpiW((*iter).name, tname))
		{
			tags.erase(iter);
			tag_count--;
			break;
		}
	}
}

void TagGroup::remove_tag(int tid)
{
	tags.erase(tags.begin() + tid);
	tag_count--;
}

int TagGroup::get_tag_count()
{
	return tag_count;
}

vector<Tag> TagGroup::get_tags()
{
	return tags;
}

int TagGroup::find_tag(LPWSTR tname)
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

void TagGroup::save(FILE* file)
{
	fwrite(&index, sizeof(index), 1, file);
	fwrite(name, sizeof(name), 1, file);
	fwrite(&tag_count, sizeof(tag_count), 1, file);
	for (int i = 0; i < tag_count; i++)
	{
		fwrite(&(tags[i]), sizeof(Tag), 1, file);
	}
}

void TagGroup::load(FILE* file)
{
	fread(&index, sizeof(index), 1, file);
	fread(name, sizeof(name), 1, file);
	fread(&tag_count, sizeof(tag_count), 1, file);
	for (int i = 0; i < tag_count; i++)
	{
		Tag t;
		fread(&t, sizeof(Tag), 1, file);
		tags.push_back(t);
	}
}

TaggedFile::TaggedFile()
{}

TaggedFile::TaggedFile(LPWSTR n)
{
	lstrcpyW(name, n);
	tag_count = 0;
}

LPWSTR TaggedFile::get_name()
{
	return name;
}

void TaggedFile::add_tag(Tag* t)
{
	tags.push_back(*t);
	tag_count++;
}

void TaggedFile::remove_tag(Tag* t)
{
	for (vector<Tag>::iterator iter = tags.begin(); iter != tags.end(); iter++)
	{
		if ((*iter).tagGroupIndex == t->tagGroupIndex)
		{
			if (!lstrcmpiW((*iter).name, t->name))
			{
				tags.erase(iter);
				tag_count--;
				break;
			}
		}
	}

}

int TaggedFile::get_tag_count()
{
	return tag_count;
}

vector<Tag> TaggedFile::get_tags()
{
	return tags;
}

int TaggedFile::find_tag(LPWSTR tname, int gid)
{
	for (int i = 0; i < tag_count; i++)
	{
		if (tags[i].tagGroupIndex == gid)
		{
			if (!lstrcmpiW(tags[i].name, tname))
				return i;
		}
	}
	return -1;
}

void TaggedFile::save(FILE* file)
{
	fwrite(name, sizeof(name), 1, file);
	fwrite(&tag_count, sizeof(tag_count), 1, file);
	for (int i = 0; i < tag_count; i++)
	{
		fwrite(&(tags[i]), sizeof(Tag), 1, file);
	}
}

void TaggedFile::load(FILE* file)
{
	fread(name, sizeof(name), 1, file);
	fread(&tag_count, sizeof(tag_count), 1, file);
	for (int i = 0; i < tag_count; i++)
	{
		Tag t;
		fread(&t, sizeof(Tag), 1, file);
		tags.push_back(t);
	}
}

TagDB::TagDB()
{
}

TagDB::TagDB(LPWSTR n, LPWSTR path)
{
	lstrcpyW(name, n);
	lstrcpyW(dir_path, path);
	file_count = 0;

	TagGroup* ungrp = new TagGroup((LPWSTR)L"Ungrouped", 0);
	tag_groups.push_back(*ungrp);
	group_count = 1;
}

void TagDB::add_group(LPWSTR name)
{
	TagGroup* grp = new TagGroup(name, group_count);
	tag_groups.push_back(*grp);
	group_count++;
}

int TagDB::add_file(LPWSTR fname)
{
	TaggedFile* file = new TaggedFile(fname);
	files.push_back(*file);
	file_count++;
	return file_count - 1;
}

int TagDB::find_tag_group(LPWSTR grpname)
{
	for (int i = 0; i < group_count; i++)
	{
		if (!lstrcmpiW(tag_groups[i].name, grpname))
		{
			return i;
		}
	}
	return -1;
}

int TagDB::find_file(LPWSTR fname)
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

void TagDB::add_tag_to_group(LPWSTR grpname, LPWSTR tname)
{
	int gid = find_tag_group(grpname);
	if (gid != -1)
	{
		TagGroup* tgrp = &tag_groups[gid];
		if (tgrp->find_tag(tname) == -1)
			tgrp->add_tag(tname);
	}
}

void TagDB::remove_tag_from_group(LPWSTR grpname, LPWSTR tname)
{
	int gid = find_tag_group(grpname);
	if (gid != -1)
	{
		TagGroup* tgrp = &tag_groups[gid];
		int tid = tgrp->find_tag(tname);
		if (tid != -1)
		{
			tgrp->remove_tag(tid);
			Tag* t = new Tag(tname, gid);
			for (int i = 0; i < file_count; i++)
			{
				files[i].remove_tag(t);
			}
		}
	}
}

void TagDB::add_tag_to_file(LPWSTR fname, LPWSTR grpname, LPWSTR tname)
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

void TagDB::remove_tag_from_file(LPWSTR fname, LPWSTR grpname, LPWSTR tname)
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
				files[fid].remove_tag(t);
			}
		}
	}
}

vector<LPWSTR> TagDB::filter(LPWSTR grpname, LPWSTR tname)
{
	vector<LPWSTR> fnames;
	LPWSTR fname;

	int gid = find_tag_group(grpname);
	for (int i = 0; i < file_count; i++)
	{
		if (files[i].find_tag(tname, gid) != -1)
		{
			fname = files[i].name;
			fnames.push_back(fname);
		}
	}

	return fnames;
}

void TagDB::DEBUG_info()
{
	//vector<TaggedFile> files = tagdb->files;
	for (unsigned int i = 0; i < files.size(); i++)
	{
		TaggedFile* file = &(files[i]);
		wprintf_s(L"\nfname: %s\n", file->name);
		vector<Tag> tags = file->tags;
		for (int j = 0; j < file->tag_count; j++)
		{
			wprintf_s(L"tagname: %s\ngrp: %s\n", tags[j].name, tag_groups[tags[j].tagGroupIndex].name);
		}
	}
}

void TagDB::save_db()
{
	FILE* dbfile;
	fopen_s(&dbfile, DB_NAME, "wb");

	fwrite(name, sizeof(name), 1, dbfile);
	fwrite(dir_path, sizeof(dir_path), 1, dbfile);
	fwrite(&file_count, sizeof(file_count), 1, dbfile);
	fwrite(&group_count, sizeof(group_count), 1, dbfile);
	
	for (int i = 0; i < group_count; i++)
	{
		tag_groups[i].save(dbfile);
	}

	for (int i = 0; i < file_count; i++)
	{
		files[i].save(dbfile);
	}

	fclose(dbfile);
}

bool TagDB::load_db()
{
	FILE* dbfile = nullptr;
	fopen_s(&dbfile, DB_NAME, "rb");
	if (dbfile == nullptr)
		return false;

	fread(name, sizeof(name), 1, dbfile);
	fread(dir_path, sizeof(dir_path), 1, dbfile);
	fread(&file_count, sizeof(file_count), 1, dbfile);
	fread(&group_count, sizeof(group_count), 1, dbfile);

	for (int i = 0; i < group_count; i++)
	{
		TagGroup tg;
		tg.load(dbfile);
		tag_groups.push_back(tg);
	}

	for (int i = 0; i < file_count; i++)
	{
		TaggedFile tf;
		tf.load(dbfile);
		files.push_back(tf);
	}

	fclose(dbfile);
	return true;
}

extern "C" EXPIMP TagDB* CreateTagDbInstance(LPWSTR name, LPWSTR path)
{
	return new TagDB(name, path);
}

extern "C" EXPIMP TagDB* CreateTagDbEmptyInstance()
{
	return new TagDB();
}

extern "C" EXPIMP void DestroyTagDbInstance(TagDB* inst)
{
	delete inst;
}