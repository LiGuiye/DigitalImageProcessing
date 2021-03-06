// 16liguiye.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "pch.h"
//#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>
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

//定义一个颜色数组把所需的颜色放进去，根据需要取用
unsigned char colorarry[10][3] = { { 0, 255, 0 }, { 0,0,255 }, { 255,0,0 }, { 0,255,255 }, { 255,255,0 }, { 255,0,255 } };

//读取BMP影像的信息到dib结构体里
void readbmp(const char *path, struct_dib &mydib)
{
	BITMAPFILEHEADER header;
	BITMAPINFOHEADER info;
	RGBQUAD rgb[256];
	FILE *fp = fopen(path, "rb");
	fread(&header, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&info, sizeof(BITMAPINFOHEADER), 1, fp);
	mydib.nWidth = info.biWidth;
	mydib.nHeight = info.biHeight;
	if (info.biBitCount == 8)
	{
		mydib.nBands = 1;
		fread(rgb, sizeof(RGBQUAD), 256, fp);
		printf("8位影像读取成功");
	}
	if (info.biBitCount == 24)
	{
		mydib.nBands = 3;
		fread(rgb, sizeof(RGBQUAD), 256, fp);
		printf("24位影像读取成功");
	}
	mydib.pData = new unsigned char[mydib.nWidth*mydib.nHeight*mydib.nBands];
	fread(mydib.pData, sizeof(unsigned char), mydib.nWidth*mydib.nHeight*mydib.nBands, fp);
	fclose(fp);//与上面的fopen对应
}
//保存bmp影像
void writebmp(const char *path, struct_dib &dib)
{
	BITMAPFILEHEADER header;
	BITMAPINFOHEADER info;
	RGBQUAD rgb[256];
	FILE *fp = fopen(path, "wb");
	/////////////////////////////////////////////////
	header.bfType = 19778;
	header.bfReserved1 = 0;
	header.bfReserved2 = 0;
	if (dib.nBands == 1)
	{
		info.biBitCount = 8;
		header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256;
	}
	if (dib.nBands == 3)
	{
		info.biBitCount = 24;
		header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	}
	header.bfSize = header.bfOffBits + dib.nWidth*dib.nHeight*dib.nBands;
	////////////////////////////////////////////////
	info.biClrImportant = 256;
	info.biClrUsed = 256;
	info.biCompression = 0;
	info.biHeight = dib.nHeight;
	info.biPlanes = 1;
	info.biSize = sizeof(BITMAPINFOHEADER);
	info.biSizeImage = dib.nWidth*dib.nHeight*dib.nBands;
	info.biWidth = dib.nWidth;
	info.biXPelsPerMeter = 0;
	info.biYPelsPerMeter = 0;
	////////////////////////////////////////////////
	for (int i = 0; i < 256; i++)
	{
		rgb[i].rgbRed = i;
		rgb[i].rgbGreen = i;
		rgb[i].rgbBlue = i;
		rgb[i].rgbReserved = 0;
	}
	////////////////////////////////////////////////
	fwrite(&header, sizeof(BITMAPFILEHEADER), 1, fp);
	fwrite(&info, sizeof(BITMAPINFOHEADER), 1, fp);
	if (info.biBitCount == 8)
	{
		fwrite(rgb, sizeof(RGBQUAD), 256, fp);
	}
	fwrite(dib.pData, sizeof(unsigned char), dib.nWidth*dib.nHeight*dib.nBands, fp);
	fclose(fp);
}
//bmp影像统计（最大值、最小值、均值、方差）
void bmpstatistics(struct_dib &dib)
{
	float fMax = -999999;
	float fMin = 9999999;
	double dMean = 0;
	double dStd = 0;
	double dSumbysum = 0;;
	for (int j = 0; j < dib.nHeight; j++)//一行一行遍历
	{
		for (int i = 0; i < dib.nWidth; i++)
		{
			unsigned char ctemp = dib.pData[j*dib.nWidth + i];//第j行第i列的数值
			if (ctemp > fMax) fMax = ctemp;
			if (ctemp < fMin) fMin = ctemp;
			dMean += ctemp;
			dSumbysum += ctemp * ctemp;//方差 = 平方的均值-均值的平方	
		}
	}
	dMean /= dib.nHeight*dib.nWidth;
	dSumbysum /= dib.nHeight*dib.nWidth;
	dStd = sqrt(dSumbysum - dMean * dMean);//标准差=sqrt(方差)
	/////////////////////////////////////
	dib.fMax = fMax;
	dib.fMin = fMin;
	dib.fMean = dMean;
	dib.fStd = dStd;
}
//统计灰度值并生成直方图
void GrayCount(struct_dib &mydib)
{
	//定义统计灰度值的数组
	int grayCount[256];
	for (int x = 0; x < 256; x++)
	{
		grayCount[x] = 0;
	}
	//遍历所有的行
	for (int i = 0; i < mydib.nHeight; i++)
	{
		//遍历所有的列
		for (int j = 0; j < mydib.nWidth; j++)
		{
			int grayvalue = mydib.pData[i*mydib.nWidth + j];
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
void stretch(struct_dib &mydib, float A, float B) {
	float a, b;
	a = mydib.fMin;
	b = mydib.fMax;
	if (A < B)
	{
		for (int i = 0; i < mydib.nHeight; i++) //一行一行遍历
		{
			for (int j = 0; j < mydib.nWidth; j++)
			{
				unsigned char ctemp = mydib.pData[i*mydib.nWidth + j];//第i行第j列的数值
				mydib.pData[i*mydib.nWidth + j] = A + (B - A)*(mydib.pData[i*mydib.nWidth + j] - a) / (b - a);
				//防止拉伸后越界
				if (mydib.pData[i*mydib.nWidth + j] > 255) mydib.pData[i*mydib.nWidth + j] = 255;
				if (mydib.pData[i*mydib.nWidth + j] < 0) mydib.pData[i*mydib.nWidth + j] = 0;
			}
		}
	}
	else
		printf("A必须小于B");
}
//k均值拉伸
void meanstretch(struct_dib &mydib, float k, float A, float B) {

	float a, b;
	a = mydib.fMean - k * mydib.fStd;
	b = mydib.fMean + k * mydib.fStd;

	if (A < B)
	{
		for (int i = 0; i < mydib.nHeight; i++) //一行一行遍历
		{
			for (int j = 0; j < mydib.nWidth; j++)
			{
				unsigned char ctemp = mydib.pData[i*mydib.nWidth + j];//第i行第j列的数值
				mydib.pData[i*mydib.nWidth + j] = A + (B - A)*(mydib.pData[i*mydib.nWidth + j] - a) / (b - a);
				//防止拉伸后越界
				if (mydib.pData[i*mydib.nWidth + j] > 255) mydib.pData[i*mydib.nWidth + j] = 255;
				if (mydib.pData[i*mydib.nWidth + j] < 0) mydib.pData[i*mydib.nWidth + j] = 0;
			}
		}
	}
	else
		printf("A必须小于B");

}
//假彩色合成
void pseudocolor(struct_dib &gray, struct_dib &color, int nClass) {
	/*其他信息暂不需要，所以只用改以下三个值*/
	color.nWidth = gray.nWidth;
	color.nHeight = gray.nHeight;
	color.nBands = 3;
	/*新开辟一块内存*/
	color.pData = new unsigned char[color.nWidth*color.nHeight*color.nBands];
	/*定义步长，分类赋颜色值*/
	float fStep = 255 / nClass;
	for (int j = 0; j < color.nHeight; j++)
	{
		for (int i = 0; i < color.nWidth; i++)
		{
			/*把以前的灰度信息值按照索引存进新的颜色表*/
			unsigned char cTemp = gray.pData[j*color.nWidth + i];
			/*定义索引*/
			int nindex = cTemp / fStep;
			/*颜色的存储模式为rgbrgbrgb...所以在行和列*3的基础上还要加上0(r)1(g)2(b)*/
			/*r赋成颜色数组里的第一种颜色，g第二种，b第三种*/
			color.pData[j*color.nWidth * 3 + i * 3 + 0] = colorarry[nindex][0];
			color.pData[j*color.nWidth * 3 + i * 3 + 1] = colorarry[nindex][1];
			color.pData[j*color.nWidth * 3 + i * 3 + 2] = colorarry[nindex][2];
		}
	}

}
//画十字叉叉
//用数组传位置
void newdraw(struct_dib &gray, struct_dib &color, int *hang, int *lie, int length) {
	/*其他信息暂不需要，所以只用改以下三个值*/
	color.nWidth = gray.nWidth;
	color.nHeight = gray.nHeight;
	color.nBands = 3;
	/*新开辟一块内存*/
	color.pData = new unsigned char[color.nWidth*color.nHeight*color.nBands];
	
	for (int j = 0; j < color.nHeight; j++)
	{
		for (int i = 0; i < color.nWidth; i++)
		{
			/*把以前的灰度信息值按照索引存进新的颜色表*/
			unsigned char cTemp = gray.pData[j*color.nWidth + i];

			/*颜色的存储模式为rgbrgbrgb...所以在行和列*3的基础上还要加上0(r)1(g)2(b)*/
			/*r赋成颜色数组里的第一种颜色，g第二种，b第三种*/
			color.pData[j*color.nWidth * 3 + i * 3 + 0] = gray.pData[j*gray.nWidth + i];
			color.pData[j*color.nWidth * 3 + i * 3 + 1] = gray.pData[j*gray.nWidth + i];
			color.pData[j*color.nWidth * 3 + i * 3 + 2] = gray.pData[j*gray.nWidth + i];
		}
	}
	for (int s = 0; s < length; s++)
	{
		//竖着1个像素
		for (int j = hang[s] - 0; j < hang[s] + 1; j++)
		{
			color.pData[j*color.nWidth * 3 + lie[s] * 3 + 0] = colorarry[1][0];
			color.pData[j*color.nWidth * 3 + lie[s] * 3 + 1] = colorarry[1][1];
			color.pData[j*color.nWidth * 3 + lie[s] * 3 + 2] = colorarry[1][2];
		}
		//横着1个像素
		for (int i = lie[s] - 0; i < lie[s] + 1; i++)
		{
			color.pData[hang[s] * color.nWidth * 3 + i * 3 + 0] = colorarry[1][0];
			color.pData[hang[s] * color.nWidth * 3 + i * 3 + 1] = colorarry[1][1];
			color.pData[hang[s] * color.nWidth * 3 + i * 3 + 2] = colorarry[1][2];
		}
	}

}
//直接给行和列
void draw(struct_dib &gray, struct_dib &color, int hang, int lie) {
	/*其他信息暂不需要，所以只用改以下三个值*/
	color.nWidth = gray.nWidth;
	color.nHeight = gray.nHeight;
	color.nBands = 3;
	/*新开辟一块内存*/
	color.pData = new unsigned char[color.nWidth*color.nHeight*color.nBands];

	for (int j = 0; j < color.nHeight; j++)
	{
		for (int i = 0; i < color.nWidth; i++)
		{
			/*把以前的灰度信息值按照索引存进新的颜色表*/
			unsigned char cTemp = gray.pData[j*color.nWidth + i];

			/*颜色的存储模式为rgbrgbrgb...所以在行和列*3的基础上还要加上0(r)1(g)2(b)*/
			/*r赋成颜色数组里的第一种颜色，g第二种，b第三种*/
			color.pData[j*color.nWidth * 3 + i * 3 + 0] = gray.pData[j*gray.nWidth + i];
			color.pData[j*color.nWidth * 3 + i * 3 + 1] = gray.pData[j*gray.nWidth + i];
			color.pData[j*color.nWidth * 3 + i * 3 + 2] = gray.pData[j*gray.nWidth + i];
		}
	}
	//竖着2个像素
	for (int j = hang - 1; j < hang + 2; j++)
	{
		color.pData[j*color.nWidth * 3 + lie * 3 + 0] = colorarry[1][0];
		color.pData[j*color.nWidth * 3 + lie * 3 + 1] = colorarry[1][1];
		color.pData[j*color.nWidth * 3 + lie * 3 + 2] = colorarry[1][2];
	}
	//横着2个像素
	for (int i = lie - 1; i < lie + 2; i++)
	{
		color.pData[hang*color.nWidth * 3 + i * 3 + 0] = colorarry[1][0];
		color.pData[hang*color.nWidth * 3 + i * 3 + 1] = colorarry[1][1];
		color.pData[hang*color.nWidth * 3 + i * 3 + 2] = colorarry[1][2];
	}
}
//空间域平滑
void smooth(struct_dib &mydib, int nStep, float *h) {
	if (mydib.nBands > 1)
	{
		printf("Error!\n");
		return;
	}
	/*新开辟一块内存*/
	unsigned char *pTempData = new unsigned char[mydib.nWidth*mydib.nHeight*mydib.nBands];
	//for (int j = 0; j < mydib.nHeight; j++)//一行一行遍历
	//{
	//	for (int i = 0; i < mydib.nWidth; i++)
	//	{
	//		pTempData[j*mydib.nWidth + i] = mydib.pData[j*mydib.nWidth + i];
	//	}
	//}
	memcpy(pTempData, mydib.pData, mydib.nWidth*mydib.nHeight);
	//for (int j = 1; j < mydib.nHeight-1; j++)
	//{
	//	for (int i = 1; i < mydib.nWidth-1; i++)
	//	{
	//		float fTemp[9];
	//		float fSum = 0.0;
	//		fTemp[0] = pTempData[(j + 1)*mydib.nWidth + i - 1]*1;
	//		fTemp[1] = pTempData[(j + 1)*mydib.nWidth + i]*1;
	//		fTemp[2] = pTempData[(j + 1)*mydib.nWidth + i + 1]*1;
	//		fTemp[3] = pTempData[j*mydib.nWidth + i - 1]*1;
	//		fTemp[4] = pTempData[j*mydib.nWidth + i]*1;
	//		fTemp[5] = pTempData[j*mydib.nWidth + i + 1]*1;
	//		fTemp[6] = pTempData[(j - 1)*mydib.nWidth + i - 1]*1;
	//		fTemp[7] = pTempData[(j - 1)*mydib.nWidth + i]*1;
	//		fTemp[8] = pTempData[(j - 1)*mydib.nWidth + i + 1]*1;
	//		for (int i = 0; i < 9 ; i++)
	//		{
	//			fSum += fTemp[i];
	//		}
	//		if (fSum / 9 > 255)
	//		{
	//			mydib.pData[j*mydib.nWidth + i] = 255;
	//		}
	//		else if (fSum / 9 < 0)
	//		{
	//			mydib.pData[j*mydib.nWidth + i] = 0;
	//		}
	//		else
	//		{
	//			mydib.pData[j*mydib.nWidth + i] = fSum / 9;
	//		}
	//		
	//	}
	//}
	/*nStep几乘几的矩阵*/
	/*nStep/2表示从第几行开始处理*/
	for (int j = nStep / 2; j < mydib.nHeight - nStep / 2; j++)
	{
		for (int i = nStep / 2; i < mydib.nWidth - nStep / 2; i++)
		{
			float fSum = 0.0;
			for (int m = -nStep / 2; m <= nStep / 2; m++)
			{
				for (int n = -nStep / 2; n <= nStep / 2; n++)
				{
					/*两个循环遍历:计算像素单元的新值*/
					fSum += pTempData[(j + m)*mydib.nWidth + i + n] * h[(m + nStep / 2)*nStep + (n + nStep / 2)];
				}
			}
			/*防止计算后的值越界*/
			if (fSum / 9 > 255)
			{
				mydib.pData[j*mydib.nWidth + i] = 255;
			}
			else if (fSum / 9 < 0)
			{
				mydib.pData[j*mydib.nWidth + i] = 0;
			}
			else
				mydib.pData[j*mydib.nWidth + i] = fSum / 9;
		}
	}
}
//边缘检测
void EdgeExtraction(struct_dib &mydib, int nStep, float *h1, float *h2, float ss) {
	if (mydib.nBands > 1)
	{
		printf("Error!\n");
		return;
	}
	/*新开辟一块内存*/
	unsigned char *pTempData = new unsigned char[mydib.nWidth*mydib.nHeight*mydib.nBands];
	memcpy(pTempData, mydib.pData, mydib.nWidth*mydib.nHeight);

	/*nStep几乘几的矩阵*/
	/*nStep/2表示从第几行开始处理*/
	for (int j = nStep / 2; j < mydib.nHeight - nStep / 2; j++)
	{
		for (int i = nStep / 2; i < mydib.nWidth - nStep / 2; i++)
		{
			float fSum1 = 0.0;
			float fSum2 = 0.0;
			for (int m = -nStep / 2; m <= nStep / 2; m++)
			{
				for (int n = -nStep / 2; n <= nStep / 2; n++)
				{
					fSum1 += pTempData[(j + m)*mydib.nWidth + i + n] * h1[(m + nStep / 2)*nStep + (n + nStep / 2)];
				}
			}
			for (int m = -nStep / 2; m <= nStep / 2; m++)
			{
				for (int n = -nStep / 2; n <= nStep / 2; n++)
				{
					fSum2 += pTempData[(j + m)*mydib.nWidth + i + n] * h1[(m + nStep / 2)*nStep + (n + nStep / 2)];
				}
			}
			if (fSum1 + fSum2 > ss)
			{
				mydib.pData[j*mydib.nWidth + i] = 255;
			}
			else
			{
				mydib.pData[j*mydib.nWidth + i] = 0;
			}

		}
	}

}
//二值化（未测试）
//void Binarization(struct_dib &dib1, struct_dib &dib2, float Threshold) {
//	/*其他信息暂不需要，所以只用改以下三个值*/
//	dib2.nWidth = dib1.nWidth;
//	dib2.nHeight = dib1.nHeight;
//	dib2.nBands = 1;
//	dib2.pData = new unsigned char[dib2.nWidth*dib2.nHeight*dib2.nBands];
//	for (int j = 0; j < dib2.nHeight; j++)
//	{
//		for (int i = 0; i < dib2.nWidth; i++)
//		{
//			if (dib1.pData[j*dib2.nWidth + i] >= Threshold)
//			{
//				dib2.pData[j*dib2.nWidth + i] = 255;
//			}
//			else {
//				dib2.pData[j*dib2.nWidth + i] = 1;
//			}
//		}
//	}
//
//}
//void CopyBmp(struct_dib &dib1, struct_dib &dib2) {
//	/*其他信息暂不需要，所以只用改以下三个值*/
//	dib2.nWidth = dib1.nWidth;
//	dib2.nHeight = dib1.nHeight;
//	dib2.nBands = 1;
//	dib2.pData = new unsigned char[dib2.nWidth*dib2.nHeight*dib2.nBands];
//	for (int j = 0; j < dib2.nHeight; j++)
//	{
//		for (int i = 0; i < dib2.nWidth; i++)
//		{
//
//			dib2.pData[j*dib2.nWidth + i] = dib1.pData[j*dib2.nWidth + i];
//
//		}
//	}
//
//}

//Moravec
void Moravec(struct_dib mydib, int nW, int Threshold, int nStep) {
	if (mydib.nBands > 1)
	{
		printf("Error!\n");
		return;
	}
	//把读取进来的影像复制一份
	//算子只对单波段有效
	unsigned char *pTempData = new unsigned char[mydib.nWidth*mydib.nHeight];
	//memcpy(pTempData, mydib.pData, mydib.nWidth*mydib.nHeight);

	int k = nW / 2;
	//计算各像元兴趣值fIV
	for (int c = k; c < mydib.nHeight - k; c++)
	{
		for (int r = k; r < mydib.nWidth - k; r++)
		{
			float fV1 = 0, fV2 = 0, fV3 = 0, fV4 = 0, fIV = 10000;
			for (int i = -k; i < k; i++)
			{
				fV1 += pow(mydib.pData[(c + i)*mydib.nWidth + r] - mydib.pData[(c + i + 1)*mydib.nWidth + r],2);
				fV2 += pow(mydib.pData[(c + i)*mydib.nWidth + r + i] - mydib.pData[(c + i + 1)*mydib.nWidth + r + i + 1],2);
				fV3 += pow(mydib.pData[(c)*mydib.nWidth + r + i] - mydib.pData[(c)*mydib.nWidth + r + i + 1],2);
				fV4 += pow(mydib.pData[(c + i)*mydib.nWidth + r - i] - mydib.pData[(c + i + 1)*mydib.nWidth + r - i - 1],2);
			}
			fIV = min(min(fV1, fV2), min(fV3, fV4));
			//兴趣值大于阈值的点作为候选点,小于阈值的设为0（提取特征点时可以直接舍弃）
			if (fIV > Threshold)
			{
				pTempData[(c*mydib.nHeight + r)] = fIV;
			}
			else
			{
				pTempData[(c*mydib.nHeight + r)] = 0;
			}
		}
	}
	//定义保存红叉叉位置的数组和长度变量
	int length = 0;
	int hang[20000];
	int lie[20000];
	//提取特征点
	for (int j = nStep / 2; j < mydib.nHeight - nStep / 2; j = j + nStep)
	{
		for (int i = nStep / 2; i < mydib.nWidth - nStep / 2; i = i + nStep)
		{
			//循环遍历窗口中的每一个像素值
			int fMax = 0;
			int c = 0, r = 0;
			for (int m = -nStep / 2; m <= nStep / 2; m++)
			{
				for (int n = -nStep / 2; n <= nStep / 2; n++)
				{
					if (pTempData[(j + m)*mydib.nWidth + i + n] > fMax) {
						fMax = pTempData[(j + m)*mydib.nWidth + i + n];
						c = j + m;//行
						r = i + n;//列
					}
				}
			}
			//把特征点的行和列记录在数组里，方便画红叉叉
			hang[length] = c;
			lie[length] = r;
			length++;
		}
	}
	struct_dib color;
	printf("length = %d", length);//数组当参数传过去之后sizeof就用不了了，所以再搞个length一起传过去
	newdraw(mydib, color, hang, lie, length);
	writebmp("lena_xx.bmp", color);
}


void main()
{
	struct_dib mydib;
	readbmp("lena.bmp", mydib);
	//bmpstatistics(mydib);
//printf("The Result of bmp statistics as follows:\n");
//printf("max = %f, min = %f, mean = %f, std = %f\n", mydib.fMax, mydib.fMin, mydib.fMean, mydib.fStd);
	//struct_dib color;
	//int hang[9] = { 4, 9, 11, 22, 44, 33, 66, 77, 100 };
	//int lie[9] = { 4, 9, 11, 22, 44, 33, 66, 77, 100 };
	//newdraw(mydib, color, hang, lie,9);
	//writebmp("lena_xx.bmp", color);
	///*定义卷积算子*/
	//float mean  [9] = { 1, 1, 1, 1, 1, 1, 1, 1, 1 };
	//float Prewitt1[9] = { -1, 0, 1, -1, 0, 1, -1, 0, 1 };
	//float Prewitt2[9] = { -1, -1, -1, 0, 0, 0, 1, 1, 1 };
	//float Sobel1[9] = { -1, 0, 1, -2, 0, 2, -1, 0, 1 };
	//float Sobel2[9] = { -1, -2, 1, 0, 0, 0, 1, 2, 1 };
	//float Laplace[9] = { 0, 1, 0, 1, -4, 1, 0, 1, 0 };

	//smooth(mydib, 3, mean);
	////EdgeExtraction(mydib,3, Prewitt1, Prewitt2,80);
	////writebmp("Prewitt_lena.bmp", mydib);
	//EdgeExtraction(mydib, 3, Sobel1, Sobel2, 110);
	//writebmp("Sobel_lena.bmp", mydib);
	//bmpstatistics(mydib);
	//printf("The Result of bmp statistics as follows:\n");
	//printf("max = %f, min = %f, mean = %f, std = %f\n", mydib.fMax, mydib.fMin, mydib.fMean, mydib.fStd);

	//第一个变量为读取的图像，第二个为计算兴趣值的窗口大小，第三个为阈值，第四个为提取特征点的窗口大小
	Moravec(mydib, 5, 250, 5);
}
