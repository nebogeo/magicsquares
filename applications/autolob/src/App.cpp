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

#include <assert.h>
#include <unistd.h>
#include <dirent.h>
#include <list>
#include "App.h"
#include "FileTools.h"
#include "PCA.h"
//#include "Blob.h"
//#include "BlobResult.h"

//#define SAVE_FRAMES

using namespace std;

//int w=50;
//int h=80;

int w=20;
int h=30;

App::App(const string &filename) :
m_Capture(NULL),
m_Frame(NULL),
m_FrameCopy(NULL),
m_FrameNum(0),
m_SingleImage(false),
m_CurrentFile(0)
{
	if (filename=="")
	{

        //int ncams = cvcamGetCamerasCount( );    //returns the number of available cameras in the system
        //int* out; int nselected = cvcamSelectCamera(&out);

		m_Capture = cvCaptureFromCAM(1);
	}
	else
	{
        cerr<<"loading from "<<filename<<endl;

        DIR *dir;
        struct dirent *ent;
        if ((dir = opendir(filename.c_str())) != NULL) {
          while ((ent = readdir (dir)) != NULL) {
            if (ent->d_name[0]!='.') {
              //cerr<<"adding "<<ent->d_name<<endl;
              m_Files.push_back(filename+string("/")+string(ent->d_name));
            }
          }
          closedir (dir);
        }


        m_CurrentFile=0;
        m_Frame = cvLoadImage(m_Files[m_CurrentFile].c_str());
        cerr<<"loaded "<<m_Files[m_CurrentFile]<<endl;


        m_SingleImage=true;
	}

	cvInitFont( &m_Font, CV_FONT_HERSHEY_PLAIN, 1, 1, 0, 1, CV_AA );
	cvInitFont( &m_LargeFont, CV_FONT_HERSHEY_PLAIN, 5, 5, 0, 10, CV_AA );
	cvNamedWindow( "harvest", 1 );
}

App::~App()
{
}

static CvScalar colors[] =
    {
        {{0,0,255}},
        {{0,0,0}},
        {{0,255,255}},
        {{0,255,0}},
        {{255,128,0}},
        {{255,255,0}},
        {{255,0,0}},
        {{255,0,255}},
    };


 void App::CallBackFunc(int event, int x, int y, int flags, void* userdata) {
  App *ctx = (App*)userdata;
  if (event==1) {
    ctx->m_Fillposx.push_back(x*2);
    ctx->m_Fillposy.push_back(y*2);
  }
  if (event==2) {
    ctx->m_Circleposx.push_back(x*2);
    ctx->m_Circleposy.push_back(y*2);
  }
}

void App::Run()
{
	Image camera(m_Frame);
	Update(camera);
    Image resized = camera.Scale(camera.m_Image->width/2,
                                 camera.m_Image->height/2);
	cvShowImage("harvest", resized.m_Image);
    cvSetMouseCallback("harvest", CallBackFunc, this);
}

char *spirits[]={"TreeSpirit","ShrubSpirit","CoverSpirit"};

bool PointInside(int x, int y, CvRect b)
{
    return (x>b.x && x<b.x+b.width &&
            y>b.y && y<b.y+b.height);
}

bool Inside(CvRect a, CvRect b)
{
    return PointInside(a.x,a.y,b) ||
        PointInside(a.x+a.width,a.y,b) ||
        PointInside(a.x,a.y+a.height,b) ||
        PointInside(a.x+a.width,
                    a.y+a.height,b);
}

void App::Update(Image &camera)
{
    /*camera=camera.Scale(camera.m_Image->width/2,
                        camera.m_Image->height/2);
    */
    //cvFlip(camera.m_Image, NULL, 0);

	///////////////////////////////////
	// dispatch from input

	int key=cvWaitKey(10);

//    usleep(500);

	static int t=35;
    static bool viewthresh=true;
    static bool off=false;
    static int spirit=0;
    static int crop_x=0;
    static int crop_y=0;
    static int crop_w=camera.m_Image->width;
    static int crop_h=camera.m_Image->height;

    crop_x=0;
    crop_y=0;
    crop_w=camera.m_Image->width;
    crop_h=camera.m_Image->height;

	switch (key)
	{
    case 't': viewthresh=!viewthresh; break;
    case 'f': m_Fillposy.clear(); m_Fillposx.clear();
      m_Circleposy.clear(); m_Circleposx.clear(); break;
    case 'q': t--; break;
    case 'w': t++; break;
    case 'e': t-=20; break;
    case 'r': t+=20; break;
    case 'u': m_CurrentFile--;
      m_Fillposy.clear(); m_Fillposx.clear();
      m_Circleposy.clear(); m_Circleposx.clear();
      m_Frame = cvLoadImage(m_Files[m_CurrentFile].c_str());
      break;
    case 'c':
      break;
	}

    if (off)
    {
        sleep(1);
        cerr<<"off..."<<endl;
        return;
    }

    Image thresh=camera.RGB2GRAY();
    cvThreshold(thresh.m_Image,thresh.m_Image,t,255,CV_THRESH_BINARY);
    // copy the threshold into a colour image
    Image tofill=thresh.GRAY2RGB();

    for (int n=0; n<m_Fillposx.size(); n++) {
      cvFloodFill(tofill.m_Image,cvPoint(m_Fillposx[n],m_Fillposy[n]),
                  CV_RGB(0,255,0),cvScalar(0),cvScalar(255));
    }

    for (int n=0; n<m_Circleposx.size(); n++) {
      cvCircle(tofill.m_Image,cvPoint(m_Circleposx[n],m_Circleposy[n]),
               50,CV_RGB(0,255,0),-1);
    }


    //CBlobResult blobs;
    //blobs = CBlobResult( tofill.m_Image, NULL, 255 );
    // exclude the ones smaller than param2 value
    //blobs.Filter( blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, 10);

    //CBlob *currentBlob;
    Image *out=NULL;

    if (key=='s') {
      // add the alpha channel
      Image src=camera.SubImage(crop_x,crop_y,crop_w,crop_h);
      out = new Image(src.m_Image->width,
                      src.m_Image->height, 8, 4);
      Image *for_cont = new Image(src.m_Image->width,
                                  src.m_Image->height, 8, 1);
      for(int y=0; y<src.m_Image->height; y++) {
        for(int x=0; x<src.m_Image->width; x++) {
          CvScalar col = cvGet2D(src.m_Image,y,x);
          CvScalar alpha = cvGet2D(tofill.m_Image,y,x);

          if (alpha.val[0]==0 &&
              alpha.val[1]==255 &&
              alpha.val[2]==0) {
            col.val[3]=255;
            cvSet2D(for_cont->m_Image,y,x,cvScalar(1));
          } else {
            col.val[3]=0;
            cvSet2D(for_cont->m_Image,y,x,cvScalar(0));
          }
          cvSet2D(out->m_Image,y,x,col);
        }
      }

      //Search countours in preprocesed image
      CvMemStorage* storage = cvCreateMemStorage(0);
      CvSeq* contour;
      CvSeq* contourLow;

      cvFindContours( for_cont->m_Image, storage, &contour, sizeof(CvContour),
                      CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0) );

      contourLow=cvApproxPoly(contour, sizeof(CvContour), storage,CV_POLY_APPROX_DP,1,1);
      if (contourLow!=0) {
        int xmin=99999,ymin=99999,xmax=0,ymax=0;

        for(; contourLow != 0; contourLow = contourLow->h_next) {
          CvRect rect;
          rect=cvBoundingRect(contourLow, NULL);
          if (rect.x<xmin) xmin = rect.x;
          if (rect.y<ymin) ymin = rect.y;
          if (rect.x+rect.width>xmax) xmax = rect.x+rect.width;
          if (rect.y+rect.height>ymax) ymax = rect.y+rect.height;
        }

        Image island = out->SubImage(xmin,ymin,
                                     xmax-xmin,ymax-ymin);

        char buf[256];
        sprintf(buf,"dump/autocrab-%d.png",m_CurrentFile);
        cerr<<"saving "<<buf<<endl;
        island.Save(buf);
      } else {
        cerr<<"no contour found..."<<endl;
      }

      //m_Capture = cvCaptureFromFile(filename.c_str());
      m_CurrentFile++;
      m_Frame = cvLoadImage(m_Files[m_CurrentFile].c_str());
      m_Fillposy.clear(); m_Fillposx.clear();
      m_Circleposy.clear(); m_Circleposx.clear();
    }

    if (viewthresh) camera=tofill;

    char buf[256];
    sprintf(buf,"spirit: %s thresh: %d", spirits[spirit%3], t);
    cvPutText(camera.m_Image, buf, cvPoint(10,20),
              &m_Font, colors[0]);

    if (out!=NULL) delete out;
}
