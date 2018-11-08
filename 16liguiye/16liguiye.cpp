// 16liguiye.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include<stdio.h>
#include<windows.h>
#include "stdlib.h"

//如果是彩色的话就有rgb三个波段，没颜色的话就rgb没区别
typedef struct
{
	BYTE b;
	BYTE g;
	BYTE r;
}RGB;
BITMAPFILEHEADER fileHeader;
BITMAPINFOHEADER infoHeader;

int bmpBit,bmpSize,bmpHeight,bmpWidth;
RGB img[512][512]; //暂时只会写死，因为要求输入常量

//封装的读取BMP图像的函数
void readbmp(const char *str) {
	
	FILE *pfin = fopen(str, "rb");
	//读取bmp影像的文件头
	fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, pfin);
	//读取bmp影像的信息头;
	fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, pfin);
	//位深度
	bmpBit = infoHeader.biBitCount;
	//位图的宽度*高度
	bmpSize = infoHeader.biWidth*infoHeader.biHeight;
	//位图的高
	bmpHeight = infoHeader.biHeight;
	//位图的宽
	bmpWidth = infoHeader.biWidth;
	//读取位图数据
	fread(img, sizeof(RGB), bmpSize, pfin);
}
//统计灰度值并生成直方图
void GrayCount()
{
	//定义统计灰度值的数组
	int grayCount[256];
	for (int x = 0; x < 256; x++)
	{
		grayCount[x] = 0;
	}
	//遍历所有的行
	for (int i = 0; i < bmpHeight; i++)
	{
		//遍历所有的列
		for (int j = 0; j < bmpWidth; j++)
		{
			int grayvalue = img[i][j].b;
			/*printf("第%d行第%d列grayvalue:", i, j);
			printf("%d\n", grayvalue);*/
			grayCount[grayvalue]++;
		}
	}
	//命令行里用*生成灰度直方图
	for (int i = 0; i < 256; i++)
	{
		printf("%d:",i);
		for (int j = 0; j < grayCount[i] / 10; j++)
		{
			printf("*");
		}
		printf(" %d\n", grayCount[i]);
	}
}
//保存bmp影像
void savebmp(const char *lujing) {
	FILE*pfout = fopen(lujing, "wb");
	//将修改后的图片保存到文件
	fwrite(&fileHeader, sizeof(fileHeader), 1, pfout);
	fwrite(&infoHeader, sizeof(infoHeader), 1, pfout);
	fwrite(img, sizeof(RGB), bmpSize, pfout);
	fclose(pfout);
}

void main(void)
{
	readbmp("lady.bmp");
	if ( bmpBit == 24)
	{
		GrayCount();
	}
	savebmp("保存.bmp");

}


