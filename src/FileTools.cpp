// Copyright (C) 2009 foam
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#include <glob.h>
#include <stdio.h>
#include "FileTools.h"

using namespace std;

FILE *OpenFile(char * filename, char *mode)
{
	return fopen(filename,mode);
}

void CloseFile(FILE *f)
{
	fclose(f);
}

vector<string> Glob(const string &path)
{
	// todo windoze version
	glob_t g;
	vector<string> ret;
	glob(path.c_str(),GLOB_PERIOD,NULL,&g);
	for (unsigned int n=0; n<g.gl_pathc; n++)
	{
		string path=g.gl_pathv[n];
		if (path[path.find_last_of("/")+1]!='.')
		{
			ret.push_back(path);
		}
	}
	globfree (&g);
	return ret;
}
