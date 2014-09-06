// FingerBasicTest.cpp : �������̨Ӧ�ó������ڵ㡣
//

#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <string>
#include <vector>
#include <algorithm>
#define _USE_MATH_DEFINES
#include "math.h"
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "FingerBasic.h"
#include "MinuMatch.h"
#include "MinuDesc.h"
#include "MinuDescMCC.h"
#include "MinuMatchTico.h"
#include "MatchScoreFormula.h"

char g_fpPath[2][_MAX_PATH];
char g_featurePath[2][_MAX_PATH];

// Declaration of functions
void SetPath();
void DoSingle(char* title1, char* title2);
void WindowCallback(int k, int event, int x, int y, int flags);
void WindowCallback0(int event, int x, int y, int flags);
void WindowCallback1(int event, int x, int y, int flags);
void FeatureExtract(char* title);

int main(int argc, char* argv[])
{
	SetPath();
	FeatureExtract("1_1");
	DoSingle("1_1", "1_3");

	return 0;
}

void SetPath()
{
	// Get the path of current project
    char buffer[_MAX_PATH];
    ::GetModuleFileNameA(NULL, buffer, _MAX_PATH);
    string::size_type pos = string( buffer ).find_last_of( "\\/" );
    string root = string(buffer).substr(0, pos);
	pos = root.find_last_of( "\\/" );
	root = root.substr(0, pos);
	pos = root.find_last_of( "\\/" );
	root = root.substr(0, pos);
	root += "\\data";

	sprintf(g_fpPath[0], "%s\\image\\", root.c_str());
	strcpy(g_fpPath[1], g_fpPath[0]);
	sprintf(g_featurePath[0], "%s\\feature\\", root.c_str());
	strcpy(g_featurePath[1], g_featurePath[0]);
}

void FeatureExtract(char* title)
{
	// Load image
	char fname[1000];
	sprintf(fname, "%s%s.bmp", g_fpPath[0], title);
	IplImage* fpImg = cvLoadImage(fname, -1);
	IplImage* img = cvCreateImage(cvGetSize(fpImg), 8, 3);

	// Load and show orientation field by VeriFinger
	int block_size = 16;
	sprintf(fname, "%spd%s.bmp", g_featurePath[0], title);
	IplImage* pointOF = cvLoadImage(fname, -1);
	IplImage* blockOF = cvCreateImage(cvSize(pointOF->width/block_size, pointOF->height/block_size), 8, 1);
	cvResize(pointOF, blockOF, CV_INTER_NN);
	DrawImage(img, fpImg);
	DrawOF(img, blockOF, block_size);
	ShowImage("OF by VeriFinger", img);
	cvReleaseImage(&pointOF);
	cvReleaseImage(&blockOF);

	// Load and show minutiae by VeriFinger
	Minutiae minus;
	sprintf(fname, "%sm%s.txt", g_featurePath[0], title);
	LoadMinuTxt(fname, minus);
	DrawImage(img, fpImg);
	DrawMinutiae(img, minus);
	ShowImage("Minutiae by VeriFinger", img);

	// Compute orientation field using gradient-based method
	blockOF = ComputeOFGradient(fpImg, block_size);
	IplImage* blockMaskImg = ForegroundSegmentSimple(fpImg, block_size, 30);
	MaskOF(blockMaskImg, blockOF);
	DrawImage(img, fpImg);
	DrawOF(img, blockOF, block_size);
	ShowImage("OF by Gradient", img);

	// Gabor filtering
	GaborFilter* filter = GaborFilterCreate();
	IplImage* enhancedImage = RidgeEnhanceGabor(fpImg, blockOF, block_size, filter);
	ShowImage("Enhanced by Gabor", enhancedImage);
	GaborFilterRelease(filter);

	// Binarize and thinning
	IplImage* binaryImage = cvCloneImage(enhancedImage);
	RidgeBinarization(enhancedImage, binaryImage);
	ShowImage("Binary image", binaryImage);
	RidgeThinning(binaryImage);
	ShowImage("Thin image", binaryImage);

	cvReleaseImage(&fpImg);
	cvReleaseImage(&img);
	cvReleaseImage(&blockOF);
	cvReleaseImage(&enhancedImage);
	cvReleaseImage(&binaryImage);
}


////////////////////////////////////////////////////////
// MCC + Tico + Simple scoring
Minutiae minus[2];
MinuDescsMCC descs[2];
IplImage *fpImages[2], *dirImages[2];
void DoSingle(char* title0, char* title1)
{
	char fname[_MAX_PATH];
	char titles[2][_MAX_PATH];
	strcpy(titles[0], title0);
	strcpy(titles[1], title1);

	////////////////////////////////////////////////////////
	// Load features
	for(int k = 0; k < 2; k++)
	{
		sprintf(fname, "%s%s.bmp", g_fpPath[k], titles[k]);
		fpImages[k] = cvLoadImage(fname, -1);
		int height = fpImages[k]->height;
		int width = fpImages[k]->width;
		sprintf(fname, "%sm%s.txt", g_featurePath[k], titles[k]);
		LoadMinu(fname, minus[k], MINUFILE_TEXT3);

		// ROI based on orientation field
		sprintf(fname, "%spd%s.bmp", g_featurePath[k], titles[k]);
		dirImages[k] = cvLoadImage(fname, -1);
		BYTE* ROI = new BYTE[height * width];
		for(int y = 0; y < height; y++)
		{
			for(int x = 0; x < width; x++)
			{
				ROI[y*width+x] = (IMG(dirImages[k], y, x)==91) ? 0:1;
			}
		}

		// Create MCC
		descs[k].CreateDesc(minus[k], ROI, height, width);
		delete [] ROI;

		// Show images and features
		IplImage *img = cvCreateImage(cvGetSize(fpImages[k]), 8, 3);
		DrawImage(img, fpImages[k]);
		DrawMinutiae(img, minus[k]);
		sprintf(fname, "%d", k);
		ShowImage(fname, img, false);
		cvReleaseImage(&img);
	}
	cvSetMouseCallback("0", (CvMouseCallback)WindowCallback0);
	cvSetMouseCallback("1", (CvMouseCallback)WindowCallback1);
	cvWaitKey();

	////////////////////////////////////////////////////////
	// Minutiae matching
	Minutiae* pMinus[2];
	MinuDescs* pDescs[2];
	for(int k = 0; k < 2; k++)
	{
		pMinus[k] = minus+k;
		pDescs[k] = descs+k;
	}
	
	MinuMatchTico matcher(pMinus, pDescs);
	matcher.LocalMatch();
	matcher.ShowSimilarityMatrix();

	matcher.SortLocalMatch();
	IplImage *img0 = cvCreateImage(cvGetSize(fpImages[0]), 8, 3);
	DrawImage(img0, fpImages[0]);
	IplImage *img1 = cvCreateImage(cvGetSize(fpImages[1]), 8, 3);
	DrawImage(img1, fpImages[1]);
	matcher.DrawTopLocalMatch(img0, img1, 5);
	ShowImage("Top local match 0", img0, false);
	ShowImage("Top local match 1", img1);

	matcher.Pairing(1);
	DrawImage(img0, fpImages[0]);
	DrawImage(img1, fpImages[1]);
	matcher.DrawMatchPairs(img0, img1, matcher.match_pair_sets[0]);
	ShowImage("Global match 0", img0, false);
	ShowImage("Global match 1", img1);
	cvReleaseImage(&img0);
	cvReleaseImage(&img1);

	////////////////////////////////////////////////////////
	// Scoring
	MatchScoreFormula score(matcher, matcher.match_pair_sets[0]);
	score.ComputeFeatures();
	score.ComputeScore();
	cout<<score.score;
}

void WindowCallback(int k, int event, int x, int y, int flags)
{
	if(event==CV_EVENT_LBUTTONDOWN)
	{
		int mid = GetNearMinu(minus[k], x, y);
		if(mid >= 0)
		{
			MinuDescMCC* mcc = (MinuDescMCC*)(descs[k].pDescs)+mid;
			mcc->ShowDesc();
		}
	}
}

void WindowCallback0(int event, int x, int y, int flags)
{
	WindowCallback(0, event, x, y, flags);
}

void WindowCallback1(int event, int x, int y, int flags)
{
	WindowCallback(1, event, x, y, flags);
}
