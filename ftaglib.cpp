#include "stdafx.h"
#include <string>

#include "ftaglib.h"

using namespace std;

Tag::Tag()
{}

Tag::Tag(long uid, LPWSTR n, long tgid)
{
	id = uid;
	lstrcpyW(name, n);
	tagGroupId = tgid;
}

long Tag::get_id()
{
	return id;
}

long Tag::get_group_id()
{
	return tagGroupId;
}

LPWSTR Tag::get_name()
{
	return name;
}

void Tag::ungroup()
{
	tagGroupId = 0;
}

TagGroup::TagGroup()
{}

TagGroup::TagGroup(long uid, LPWSTR n, int i)
{
	id = uid;
	lstrcpyW(name, n);
	tag_count = 0;
}

LPWSTR TagGroup::get_name()
{
	return name;
}

long TagGroup::get_id()
{
	return id;
}

void TagGroup::add_tag(long uid, LPWSTR tname)
{
	Tag* t = new Tag(uid, tname, id);
	tags.push_back(*t);
	tag_count++;
}

void TagGroup::remove_tag(long uid)
{
	for (vector<Tag>::iterator iter = tags.begin(); iter != tags.end(); iter++)
	{
		if (iter->get_id() == uid)
		{
			tags.erase(iter);
			tag_count--;
			break;
		}
	}
}

void TagGroup::remove_tag(int tpos)
{
	tags.erase(tags.begin() + tpos);
	tag_count--;
}

int TagGroup::get_tag_count()
{
	return tag_count;
}

const vector<Tag>& TagGroup::get_tags()
{
	return tags;
}

int TagGroup::find_tag(LPWSTR tname)
{
	for (int i = 0; i < tag_count; i++)
	{
		if (!lstrcmpiW(tags[i].get_name(), tname))
		{
			return i;
		}
	}
	return -1;
}

Tag* TagGroup::get_tag(long tid)
{
	for (int i = 0; i < tag_count; i++)
	{
		if (tags[i].get_id() == tid)
			return &tags[i];
	}
	return nullptr;
}

void TagGroup::save(FILE* file)
{
	fwrite(&id, sizeof(id), 1, file);
	fwrite(name, sizeof(name), 1, file);
	fwrite(&tag_count, sizeof(tag_count), 1, file);
	for (int i = 0; i < tag_count; i++)
	{
		fwrite(&(tags[i]), sizeof(Tag), 1, file);
	}
}

void TagGroup::load(FILE* file)
{
	fread(&id, sizeof(id), 1, file);
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

TaggedFile::TaggedFile(long uid, LPWSTR n)
{
	id = uid;
	lstrcpyW(name, n);
	tag_count = 0;
}

long TaggedFile::get_id()
{
	return id;
}

LPWSTR TaggedFile::get_name()
{
	return name;
}

int TaggedFile::get_tag_count()
{
	return tag_count;
}

vector<long> TaggedFile::get_tags()
{
	vector<long> tags;
	for (vector<TagGroupSkel>::iterator iter = tag_groups.begin(); iter != tag_groups.end(); iter++)
	{
		for (vector<long>::iterator it = iter->tids.begin(); it != iter->tids.end(); it++)
		{
			tags.push_back(*it);
		}
	}
	return tags;
}

const vector<TagGroupSkel>& TaggedFile::get_tag_groups()
{
	return tag_groups;
}


void TaggedFile::add_tag(long gid, long tid)
{
	bool groupExists = false;
	for (vector<TagGroupSkel>::iterator iter = tag_groups.begin(); iter != tag_groups.end(); iter++)
	{
		if (iter->gid == gid)
		{
			iter->tids.push_back(tid);
			groupExists = true;
			break;
		}
	}

	if (!groupExists)
	{
		TagGroupSkel* tgs = new TagGroupSkel(gid);
		tgs->tids.push_back(tid);
		tag_groups.push_back(*tgs);
	}

	tag_count++;
}

void TaggedFile::remove_tag(long gid, long tid)
{
	for (vector<TagGroupSkel>::iterator iter = tag_groups.begin(); iter != tag_groups.end(); iter++)
	{
		if (iter->gid == gid)
		{
			for (vector<long>::iterator it = iter->tids.begin(); it != iter->tids.end(); it++)
			{
				if (*it == tid)
				{
					tag_groups.erase(iter);
					tag_count--;
					return;
				}
			}			
		}
	}
}

void TaggedFile::remove_group(long gid)
{
	for (vector<TagGroupSkel>::iterator iter = tag_groups.begin(); iter != tag_groups.end(); iter++)
	{
		if (iter->gid == gid)
		{
			tag_groups.erase(iter);
			break;
		}
	}
}

bool TaggedFile::check_tag(long gid, long tid)
{
	for (vector<TagGroupSkel>::iterator iter = tag_groups.begin(); iter != tag_groups.end(); iter++)
	{
		if (iter->gid == gid)
		{
			for (vector<long>::iterator it = iter->tids.begin(); it != iter->tids.end(); it++)
			{
				if (*it == tid)
				{
					return true;
				}
			}
		}		
	}
	return false;
}

void TaggedFile::save(FILE* file)
{
	fwrite(&id, sizeof(id), 1, file);
	fwrite(name, sizeof(name), 1, file);
	fwrite(&tag_count, sizeof(tag_count), 1, file);
	int size = tag_groups.size();
	fwrite(&size, sizeof(int), 1, file);
	for (vector<TagGroupSkel>::iterator iter = tag_groups.begin(); iter != tag_groups.end(); iter++)
	{
		fwrite(&(iter->gid), sizeof(long), 1, file);
		int size = iter->tids.size();
		fwrite(&size, sizeof(int), 1, file);
		for (vector<long>::iterator it = iter->tids.begin(); it != iter->tids.end(); it++)
		{
			fwrite(&(*it), sizeof(long), 1, file);
		}
	}
}

void TaggedFile::load(FILE* file)
{
	fread(&id, sizeof(id), 1, file);
	fread(name, sizeof(name), 1, file);
	fread(&tag_count, sizeof(tag_count), 1, file);
	int size;
	fread(&size, sizeof(int), 1, file);
	for (int i = 0; i < size; i++)
	{
		long gid;
		fread(&gid, sizeof(long), 1, file);
		TagGroupSkel* tgs = new TagGroupSkel(gid);
		int size;
		fread(&size, sizeof(int), 1, file);
		for (int j = 0; j < size; j++)
		{
			long tid;
			fread(&tid, sizeof(long), 1, file);
			tgs->tids.push_back(tid);
		}
		tag_groups.push_back(*tgs);
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

	TagGroup* ungrp = new TagGroup(0, (LPWSTR)L"Ungrouped", 0);
	tag_groups.push_back(*ungrp);
	group_count = 1;
}

long TagDB::gen_id()
{
	long id = (long)time(0);
	while (id <= last_id)
		id++;
	last_id = id;
	return last_id;
}

TagGroup* TagDB::get_tag_group(LPWSTR groupName)
{
	return &tag_groups[find_tag_group(groupName)];
}

TagGroup* TagDB::get_tag_group(long gid)
{
	return &tag_groups[find_tag_group(gid)];
}

TaggedFile* TagDB::get_file(LPWSTR fileName)
{
	return &files[find_file(fileName)];
}

void TagDB::add_group(LPWSTR name)
{
	TagGroup* grp = new TagGroup(gen_id(), name, group_count);
	tag_groups.push_back(*grp);
	group_count++;
}

int TagDB::add_file(LPWSTR fname)
{
	TaggedFile* file = new TaggedFile(gen_id(), fname);
	files.push_back(*file);
	file_count++;
	return file_count - 1;
}

void TagDB::remove_group(LPWSTR groupName)
{
	int gpos = find_tag_group(groupName);
	if (gpos > 0)
	{
		TagGroup* grp = &tag_groups[gpos];

		for (vector<TaggedFile>::iterator iter = files.begin(); iter != files.end(); iter++)
		{
			iter->remove_group(grp->get_id());
		}

		TagGroup* ungrp = &tag_groups[0];
		vector<Tag> tags = grp->get_tags();
		vector<Tag> utags = ungrp->get_tags();
		for (vector<Tag>::iterator iter = tags.begin(); iter != tags.end(); iter++)
		{
			iter->ungroup();
			utags.push_back(*iter);
		}
		tag_groups.erase(tag_groups.begin() + gpos);
	}

	group_count--;
}

int TagDB::find_tag_group(LPWSTR grpname)
{
	for (int i = 0; i < group_count; i++)
	{
		if (!lstrcmpiW(tag_groups[i].get_name(), grpname))
		{
			return i;
		}
	}
	return -1;
}

int TagDB::find_tag_group(long gid)
{
	for (int i = 0; i < group_count; i++)
	{
		if (tag_groups[i].get_id() == gid)
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
		if (!lstrcmpiW(files[i].get_name(), fname))
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
			tgrp->add_tag(gen_id(), tname);
	}
}

void TagDB::remove_tag_from_group(LPWSTR grpname, LPWSTR tname)
{
	int gid = find_tag_group(grpname);
	if (gid != -1)
	{
		TagGroup* tgrp = &tag_groups[gid];
		int tpos = tgrp->find_tag(tname);
		vector<Tag> tags = tgrp->get_tags();
		if (tpos != -1)
		{
			long tid = tags[tpos].get_id();
			long gid = tgrp->get_id();
			for (int i = 0; i < file_count; i++)
			{
				files[i].remove_tag(gid,tid);
			}
			tgrp->remove_tag(tpos);
		}
	}
}

void TagDB::add_tag_to_file(LPWSTR fname, LPWSTR grpname, LPWSTR tname)
{
	int gpos = find_tag_group(grpname);
	if (gpos != -1)
	{
		TagGroup* grp = &tag_groups[gpos];
		int tpos = grp->find_tag(tname);
		vector<Tag> tags = grp->get_tags();
		if (tpos != -1)
		{
			long tid = tags[tpos].get_id();
			long gid = grp->get_id();
			int fid = find_file(fname);
			if (fid != -1)
			{
				files[fid].add_tag(gid,tid);
			}
			else
			{
				//check if file exists in dir
				fid = add_file(fname);
				files[fid].add_tag(gid,tid);
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
		int tpos = grp->find_tag(tname);
		vector<Tag> tags = grp->get_tags();
		if (tpos != -1)
		{
			long tid = tags[tpos].get_id();
			long gid = grp->get_id();
			int fid = find_file(fname);
			if (fid != -1)
			{
				files[fid].remove_tag(gid, tid);
			}
		}
	}
}

vector<LPWSTR> TagDB::filter(LPWSTR grpname, LPWSTR tname)
{
	vector<LPWSTR> fnames;
	LPWSTR fname;

	int gpos = find_tag_group(grpname);
	if (gpos != -1)
	{
		int tpos = tag_groups[gpos].find_tag(tname);
		vector<Tag> tags = tag_groups[gpos].get_tags();
		if (tpos != -1)
		{
			long tid = tags[tpos].get_id();
			long gid = tag_groups[gpos].get_id();
			for (int i = 0; i < file_count; i++)
			{
				if (files[i].check_tag(gid, tid))
				{
					fname = files[i].get_name();
					fnames.push_back(fname);
				}
			}
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
		wprintf_s(L"fname: %s\n", file->get_name());
		vector<TagGroupSkel> fgroups = file->get_tag_groups();
		for (vector<TagGroupSkel>::iterator iter = fgroups.begin(); iter != fgroups.end(); iter++)
		{
			for (int i = 0; i < group_count; i++)
			{
				if (iter->gid == tag_groups[i].get_id())
				{
					TagGroup* grp = &tag_groups[i];
					wprintf_s(L"group: %s\n", grp->get_name());
					for (vector<long>::iterator it = iter->tids.begin(); it != iter->tids.end(); it++)
					{
						for (int j = 0; j < grp->get_tag_count(); j++)		//TODO: replace with binsrch
						{
							vector<Tag> tags = grp->get_tags();
							if (*it == tags[j].get_id())
							{
								Tag* tag = &tags[j];
								wprintf_s(L"tname: %s\t", tag->get_name());
								break;
							}
						}
					}
					wprintf_s(L"\n\n");
					break;
				}
			}
			
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