//*================================================================================================
//SingleFingerMatch.h match dynamic link library header file
//constants defination, struct definations, and functions declaration
//all rights reserved (c) 2001.09.25
//===============================================================================================*/

#ifndef _AFIS_LIVE_H_
#define _AFIS_LIVE_H_

#define PAI				 3.141592653589793
#define HPAI			 1.570796326794897
#define DPAI			 6.283185307179586	//@. = 2*PAI			
#define NPAI           ��3.141592653589793

#define ATR				 0.017453292519943	//@. = PAI/180 
#define RTA				57.295779513082321	//@. = 180/PAI

#define MAX_MINU_COUNT				   200	//@, �����������Ŀ

typedef struct  _MinutiaeInfo
{
	short iSita;
	short ix;	 
	short iy;
}MINUINFO;

typedef struct  _SPInfo
{
	unsigned char	cFid;			//@. �������Ŷ�
	short	cx;						//@. ��������
	short	cy;						//@. ��������
	short	cSita;					//@. ���ķ���
}CENTERINFO;

typedef struct  _FeatureInfo
{
	short		minu_num;				//@. ��������Ŀ
	CENTERINFO		sp[4];
	MINUINFO	minu[MAX_MINU_COUNT];	//@. ��������Ϣ
}FEATUREINFO;

#endif	//#ifndef _AFIS_LIVE_H_
