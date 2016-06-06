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

#include "cv.h"
#include "highgui.h"
//#include <yarp/os/all.h>
#include <string>
#include <vector>
#include <map>
#include <Image.h>

//using namespace yarp::os;
using namespace std;

class App
{
public:
	App(const string &filename);
	~App();

	void Run();
	void Update(Image &camera);

    vector<int> m_Fillposx;
    vector<int> m_Fillposy;
    vector<int> m_Circleposx;
    vector<int> m_Circleposy;
    vector<int> m_Circlesize;

    int m_CurrentCircleSize;

private:
    void ClearStuff() {
      m_Fillposy.clear();
      m_Fillposx.clear();
      m_Circleposy.clear();
      m_Circleposx.clear();
      m_Circlesize.clear();
    }

    static void CallBackFunc(int event, int x, int y, int flags, void* userdata);

	CvCapture* m_Capture;

	CvFont m_Font;
	CvFont m_LargeFont;

	IplImage *m_Frame;
	IplImage *m_FrameCopy;
	int m_FrameNum;
    bool m_SingleImage;

    vector<string> m_Files;
    int m_CurrentFile;
};
