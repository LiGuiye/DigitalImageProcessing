// 16liguiye.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include<stdio.h>
#include<windows.h>
#include "stdlib.h"


typedef struct
{
	//命名规则:int型开头就是n
	int nWidth;
	int nHeight;
	int nBands;//波段数
	unsigned char *pData;//像元灰度值
	float fMax;//最大值
	float fMin;//最小值
	float fMean;//均值
	float fStd;//标准差
}struct_dib;
BITMAPFILEHEADER fileHeader;
BITMAPINFOHEADER infoHeader;
RGBQUAD rgb[256];

//读取BMP影像的信息到dib结构体里
void readbmp(const char *path, struct_dib &dib) {

	FILE *pfin = fopen(path, "rb");
	//读取bmp影像的文件头
	fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, pfin);
	//读取bmp影像的信息头;
	fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, pfin);
	dib.nWidth = infoHeader.biWidth;
	dib.nHeight = infoHeader.biHeight;
	if (infoHeader.biBitCount == 8)
	{
		dib.nBands = 1;
		fread(rgb, sizeof(RGBQUAD), 256, pfin);
		printf("8位影像读取成功");
	}
	if (infoHeader.biBitCount == 24)
	{
		dib.nBands = 3;
		fread(rgb, sizeof(RGBQUAD), 256, pfin);
		printf("24位影像读取成功");
	}
	dib.pData = new unsigned char[dib.nWidth*dib.nHeight*dib.nBands];
	fread(dib.pData, sizeof(unsigned char), dib.nWidth*dib.nHeight*dib.nBands, pfin);
	fclose(pfin);//与上面的fopen对应
}
//保存bmp影像
void savebmp(const char *path, struct_dib &dib) {
	/////////////////////////////////////////////////////////
	FILE *pfout = fopen(path, "wb");
	fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, pfout);
	fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, pfout);
	//////////////////////////////////////////////////////////
	//给颜色表赋值
		for (int i = 0; i < 256; i++)
		{
			rgb[i].rgbBlue = rgb[i].rgbGreen = rgb[i].rgbRed = i;
			rgb[i].rgbReserved = 0;
		}
		fwrite(rgb, sizeof(RGBQUAD), 256, pfout);
	///////////////////////////////////////////////////////////////////
	fwrite(dib.pData, sizeof(unsigned char), dib.nWidth*dib.nHeight*dib.nBands, pfout);
	fclose(pfout);
}

//bmp影像统计（最大值、最小值、均值、方差）
void bmpstatistics(struct_dib &dib)
{
	float fMax = -9999999;
	float fMin = 99999999;
	double dMean = 0;
	double dStd = 0;
	double dSumbysum = 0;
	for (int i = 0; i < dib.nHeight; i++) //一行一行遍历
	{
		for (int j = 0; j < dib.nWidth; j++)
		{
			unsigned char ctemp = dib.pData[i*dib.nWidth + j];//第i行第j列的数值
			if (ctemp > fMax) fMax = ctemp;
			if (ctemp < fMin) fMin = ctemp;
			dMean += ctemp;
			dSumbysum += ctemp * ctemp;//方差 = 平方的均值-均值的平方	
		}
	}
	dMean /= dib.nHeight*dib.nWidth;//均值
	dSumbysum /= dib.nHeight*dib.nWidth;
	dStd = sqrt(dSumbysum - dMean * dMean);//标准差=sqrt(方差)
	/////////////////////////////////////////////////////////////
	dib.fMax = fMax;
	dib.fMin = fMin;
	dib.fMean = dMean;
	dib.fStd = dStd;
}
//统计灰度值并生成直方图
void GrayCount(struct_dib &dib)
{
	//定义统计灰度值的数组
	int grayCount[256];
	for (int x = 0; x < 256; x++)
	{
		grayCount[x] = 0;
	}
	//遍历所有的行
	for (int i = 0; i < dib.nHeight; i++)
	{
		//遍历所有的列
		for (int j = 0; j < dib.nWidth; j++)
		{
			int grayvalue = dib.pData[i*dib.nWidth + j];
			grayCount[grayvalue]++;
		}
	}
	//命令行里用*生成灰度直方图
	for (int i = 0; i < 256; i++)
	{
		printf("%d:", i);
		for (int j = 0; j < grayCount[i]; j++)
		{
			printf("*");
		}
		printf(" %d\n", grayCount[i]);
	}
}
//影像拉伸
void strech(struct_dib &dib , float A, float B){
	float a,b;
	a = dib.fMin;
	b = dib.fMax;
	for (int i = 0; i < dib.nHeight; i++) //一行一行遍历
	{
		for (int j = 0; j < dib.nWidth; j++)
		{
			unsigned char ctemp = dib.pData[i*dib.nWidth + j];//第i行第j列的数值
			dib.pData[i*dib.nWidth + j] = A + (B - A)*(dib.pData[i*dib.nWidth + j] - a)/(b - a);
			//防止拉伸后越界
			if (dib.pData[i*dib.nWidth + j] > 255) dib.pData[i*dib.nWidth + j] = 255;
			if (dib.pData[i*dib.nWidth + j] < 0) dib.pData[i*dib.nWidth + j] = 0;
		}
	}
}


void main(void)
{
	struct_dib mydib;
	readbmp("lady.bmp", mydib);
	//GrayCount(mydib);
	bmpstatistics(mydib);
	printf("The Result of bmp statistics as follows:\n");
	printf("max = %f, min = %f, mean = %f, std = %f\n", mydib.fMax, mydib.fMin, mydib.fMean, mydib.fStd);
	savebmp("savetest.bmp", mydib);
	strech(mydib,0,255);
	bmpstatistics(mydib);
	printf("The Result of strech bmp statistics as follows:\n");
	printf("max = %f, min = %f, mean = %f, std = %f\n", mydib.fMax, mydib.fMin, mydib.fMean, mydib.fStd);
	savebmp("strech.bmp",mydib);
}


