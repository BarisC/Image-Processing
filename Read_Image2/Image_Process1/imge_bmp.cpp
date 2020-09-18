#include "stdafx.h"
#include <windows.h>
#include <math.h>
#include <iostream>
using namespace std;
int Sort(int Array[])
{
	int x, y, tut;
	for (y = 1; y < 9; y++)
	{
		for (x = 0; x < (9 - y); x++)
		{
			if (Array[x] > Array[x + 1])
			{
				tut = Array[x];
				Array[x] = Array[x + 1];
				Array[x + 1] = tut;
			}
		}
	}
	return Array[5];

}

BYTE* Histogram(BYTE* Buffer, int width, int height)
{
	int i;
	BYTE* t = new BYTE[256];

	for (i = 0; i < 255; i++)
		t[i] = 0;
	for (i = 0; i < height*width; i++)
		t[Buffer[i]]++;
	return t;
}

BYTE* LoadBMP(int* width, int* height, long* size, LPCTSTR bmpfile)
{
	// declare bitmap structures
	BITMAPFILEHEADER bmpheader;
	BITMAPINFOHEADER bmpinfo;
	// value to be used in ReadFile funcs
	DWORD bytesread;
	// open file to read from
	HANDLE file = CreateFile(bmpfile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (NULL == file)
		return NULL; // coudn't open file

	// read file header
	if (ReadFile(file, &bmpheader, sizeof (BITMAPFILEHEADER), &bytesread, NULL) == false)  {
		       CloseHandle(file);
		       return NULL;
	      }
	//read bitmap info
	if (ReadFile(file, &bmpinfo, sizeof (BITMAPINFOHEADER), &bytesread, NULL) == false) {
		        CloseHandle(file);
		        return NULL;
	      }
	// check if file is actually a bmp
	if (bmpheader.bfType != 'MB')  	{
		       CloseHandle(file);
		       return NULL;
	      }
	// get image measurements
	*width = bmpinfo.biWidth;
	*height = abs(bmpinfo.biHeight);

	// check if bmp is uncompressed
	if (bmpinfo.biCompression != BI_RGB)  {
		      CloseHandle(file);
		      return NULL;
	      }
	// check if we have 24 bit bmp
	if (bmpinfo.biBitCount != 24) {
		      CloseHandle(file);
		      return NULL;
	     }

	// create buffer to hold the data
	*size = bmpheader.bfSize - bmpheader.bfOffBits;
	BYTE* Buffer = new BYTE[*size];
	// move file pointer to start of bitmap data
	SetFilePointer(file, bmpheader.bfOffBits, NULL, FILE_BEGIN);
	// read bmp data
	if (ReadFile(file, Buffer, *size, &bytesread, NULL) == false)  {
		     delete[] Buffer;
		     CloseHandle(file);
		     return NULL;
	      }
	// everything successful here: close file and return buffer
	CloseHandle(file);

	return Buffer;
}//LOADPMB

bool SaveBMP(BYTE* Buffer, int width, int height, long paddedsize, LPCTSTR bmpfile)
{
	// declare bmp structures 
	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER info;

	// andinitialize them to zero
	memset(&bmfh, 0, sizeof (BITMAPFILEHEADER));
	memset(&info, 0, sizeof (BITMAPINFOHEADER));

	// fill the fileheader with data
	bmfh.bfType = 0x4d42;       // 0x4d42 = 'BM'
	bmfh.bfReserved1 = 0;
	bmfh.bfReserved2 = 0;
	bmfh.bfSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+paddedsize;
	bmfh.bfOffBits = 0x36;		// number of bytes to start of bitmap bits

	// fill the infoheader

	info.biSize = sizeof(BITMAPINFOHEADER);
	info.biWidth = width;
	info.biHeight = height;
	info.biPlanes = 1;			// we only have one bitplane
	info.biBitCount = 24;		// RGB mode is 24 bits
	info.biCompression = BI_RGB;
	info.biSizeImage = 0;		// can be 0 for 24 bit images
	info.biXPelsPerMeter = 0x0ec4;     // paint and PSP use this values
	info.biYPelsPerMeter = 0x0ec4;
	info.biClrUsed = 0;			// we are in RGB mode and have no palette
	info.biClrImportant = 0;    // all colors are important

	// now we open the file to write to
	HANDLE file = CreateFile(bmpfile, GENERIC_WRITE, FILE_SHARE_READ,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file == NULL)  	{
		    CloseHandle(file);
		    return false;
	   }
	// write file header
	unsigned long bwritten;
	if (WriteFile(file, &bmfh, sizeof (BITMAPFILEHEADER), &bwritten, NULL) == false)  {
		       CloseHandle(file);
		       return false;
	      }
	// write infoheader
	if (WriteFile(file, &info, sizeof (BITMAPINFOHEADER), &bwritten, NULL) == false)  {
		     CloseHandle(file);
		     return false;
	      }
	// write image data
	if (WriteFile(file, Buffer, paddedsize, &bwritten, NULL) == false)  {
		      CloseHandle(file);
		      return false;
	     }

	// and close file
	CloseHandle(file);

	return true;
} // SaveBMP

BYTE* ConvertBMPToIntensity(BYTE* Buffer, int width, int height)
{
	// first make sure the parameters are valid
	if ((NULL == Buffer) || (width == 0) || (height == 0))
		return NULL;

	// find the number of padding bytes

	int padding = 0;
	int scanlinebytes = width * 3;
	while ((scanlinebytes + padding) % 4 != 0)     // DWORD = 4 bytes
		padding++;
	// get the padded scanline width
	int psw = scanlinebytes + padding;

	// create new buffer
	BYTE* newbuf = new BYTE[width*height];

	// now we loop trough all bytes of the original buffer, 
	// swap the R and B bytes and the scanlines
	long bufpos = 0;
	long newpos = 0;
	for (int row = 0; row < height; row++)
	for (int column = 0; column < width; column++)  {
		      newpos = row * width + column;
		      bufpos = (height - row - 1) * psw + column * 3;
		      newbuf[newpos] = BYTE(0.11*Buffer[bufpos + 2] + 0.59*Buffer[bufpos + 1] + 0.3*Buffer[bufpos]);
	      }

	return newbuf;
}//ConvetBMPToIntensity

BYTE* ConvertIntensityToBMP(BYTE* Buffer, int width, int height, long* newsize)
{
	if ((NULL == Buffer) || (width == 0) || (height == 0))
		return NULL;
	int padding = 0;
	int scanlinebytes = width * 3;
	while ((scanlinebytes + padding) % 4 != 0)
		padding++;
	int psw = scanlinebytes + padding;
	*newsize = height * psw;

	BYTE* newbuf = new BYTE[*newsize];

	memset(newbuf, 0, *newsize);

	long bufpos = 0;
	long newpos = 0;
	for (int row = 0; row < height; row++)
	for (int column = 0; column < width; column++)  	
	{
		bufpos = row * width + column;// position in original buffer
		newpos = (height - row - 1) * psw + column * 3;// position in padded buffer
		newbuf[newpos] = Buffer[bufpos];//  blue
		newbuf[newpos + 1] = Buffer[bufpos];//  green
		newbuf[newpos + 2] = Buffer[bufpos];//  red
	}

	return newbuf;
} //ConvertIntensityToBMP

BYTE* ConvertMaskMean(BYTE* Buffer, int width, int height, long* newsize)
{
	if ((NULL == Buffer) || (width == 0) || (height == 0))
		return NULL;

	int padding = 0;
	int scanlinebytes = width * 3;
	while ((scanlinebytes + padding) % 4 != 0)
		padding++;
	int psw = scanlinebytes + padding;
	*newsize = height * psw;
	BYTE* yenibuf = new BYTE[*newsize];
	
	for (int i = 0; i < height-2; i++)
		for (int j = 0; j < width-2; j++)
			yenibuf[width*(i + 1) + width + j + 1] = (Buffer[width*i + j] + Buffer[width*i + j + 1] + Buffer[width*i + j + 2] +
				Buffer[width*(i + 1) + j] + Buffer[width*(i + 1) + j + 1] + Buffer[width*(i + 1) + j + 2] +
				Buffer[width*(i + 2) + j] + Buffer[width*(i + 2) + j + 1] + Buffer[width*(i + 2) + j + 2]) / 9;

	return yenibuf;
}

BYTE* ConvertMaskMedyan(BYTE* Buffer, int width, int height, long* newsize)
{
	BYTE *ptr1, *ptr2, *ptr3;
	int i, j, sayi;
	int Medyan[9];

	if ((NULL == Buffer) || (width == 0) || (height == 0))
		return NULL;

	int padding = 0;
	int scanlinebytes = width * 3;
	while ((scanlinebytes + padding) % 4 != 0)
		padding++;
	int psw = scanlinebytes + padding;

	*newsize = height * psw;

	ptr2 = Buffer + width;
	ptr3 = ptr2 + width;

	BYTE* yenibuf = new BYTE[*newsize];

	for (i = 0; i < height-2; i++)
		for (j = 0; j < width-2; j++)
		{
			Medyan[0] = Buffer[width*i + j];	   Medyan[1] = Buffer[width*i + j + 1];       Medyan[2] = Buffer[width*i + j + 2];
			Medyan[3] = Buffer[width*(i + 1) + j]; Medyan[4] = Buffer[width*(i + 1) + j + 1]; Medyan[5] = Buffer[width*(i + 1) + j + 2];
			Medyan[6] = Buffer[width*(i + 2) + j]; Medyan[7] = Buffer[width*(i + 2) + j + 1]; Medyan[8] = Buffer[width*(i + 2) + j + 2];
			sayi = Sort(Medyan);
			yenibuf[width*i + j + 1] = sayi;
		}


	return yenibuf;
}

BYTE* ConvertMaskGaussian(BYTE* Buffer, int width, int height, long* newsize)
{
	if ((NULL == Buffer) || (width == 0) || (height == 0))
		return NULL;

	int padding = 0;
	int scanlinebytes = width * 3;
	while ((scanlinebytes + padding) % 4 != 0)
		padding++;
	int psw = scanlinebytes + padding;
	*newsize = height * psw;
	BYTE* yenibuf = new BYTE[*newsize];

	for (int i = 0; i < height-2; i++)
		for (int j = 0; j < width-2; j++)
			yenibuf[width*(i + 1) + j + 1] = (Buffer[width*i + j] / 4 +	    Buffer[width*i + j + 1] / 2 +        Buffer[width*i + j + 2] / 4 +
												Buffer[width*(i + 1) + j] / 2 + Buffer[width*(i + 1) + j + 1] +      Buffer[width*(i + 1) + j + 2] / 2 +
												Buffer[width*(i + 2) + j] / 4 + Buffer[width*(i + 2)  + j + 1] / 2 + Buffer[width*(i + 2) + j + 2] / 4) / 9;

	return yenibuf;
}

BYTE* Threshold(BYTE* Buffer, int width, int height, long* newsize)
{
	int t1 = 0, t2 = 255, t12 = 1, t22 = 1;
	int a = 0, b = 0;
	*newsize = width*height;

	BYTE* Histg = new BYTE[256];
	BYTE* binary = new BYTE[*newsize];

	Histg = Histogram(Buffer, width, height);

	while (1)
	{
		for (int i = 0; i < 255; i++)
		{
			if (-(t1 - i) < (t2 - i))
				Histg[i] = 1;
			else
				Histg[i] = 2;
		}
		for (int j = 0; j < 255; j++)
			if (Histg[j] == 1)
				a = a + j, b++;
		t12 = a / b, a = 0, b = 0;

		for (int k = 0; k < 255; k++)
			if (Histg[k] == 2)
				a = a + k, b++;
		t22 = a / b, a = 0, b = 0;

		if (t1 == t12&&t2 == t22)
			break;
		else
		{
			t1 = t12, t12 = 0;
			t2 = t22, t22 = 0;
		}
	}

	int T = t12 + abs(t12 - t22) / 2;

	for (int x = 0; x < width*height; x++)
		binary[x] = 0;
	for (int i = 0; i < width*height; i++)
		if (Buffer[i] >= T)
			binary[i] = 255;
		else
			binary[i] = 0;

	return binary;
}

BYTE* Dilation(BYTE* Buffer, int width, int height, long* newsize)
{
	if ((NULL == Buffer) || (width == 0) || (height == 0))
		return NULL;

	*newsize = height * width;

	BYTE* yenibuf = new BYTE[*newsize];

	for (int k = 0; k < width*height; k++)
		yenibuf[k] = 255;

	for (int i = 0; i < width*height; i++)
		if (Buffer[i] == 0)
		{
			if (i == 0)  // sol ust kose
			{
				yenibuf[i] = 0;
				yenibuf[i + 1] = 0;
				yenibuf[i + width] = 0;
			}
			if (i == width - 1)  // sag ust kose
			{
				yenibuf[i - 1] = 0;
				yenibuf[i] = 0;
				yenibuf[i + width] = 0;
			}
			if (i == width*height - width)  // sol alt kose
			{
				yenibuf[i - height];
				yenibuf[i] = 0;
				yenibuf[i + 1] = 0;
			}
			if (i == width*height - 1)  //sag alt kose
			{
				yenibuf[i - height];
				yenibuf[i - 1] = 0;
				yenibuf[i] = 0;
			}
			if (i%width == 0 && i != 0) // sol kenar
			{
				yenibuf[i - height];
				yenibuf[i] = 0;
				yenibuf[i + 1] = 0;
				yenibuf[i + height] = 0;
			}
			if ((i + 1) % width == 0 && i > width)  //sag kenar
			{
				yenibuf[i - height];
				yenibuf[i - 1] = 0;
				yenibuf[i] = 0;
				yenibuf[i + height] = 0;
			}
			if (i > 0 && i < width - 1) // üst kenar
			{
				yenibuf[i - 1] = 0;
				yenibuf[i] = 0;
				yenibuf[i + 1] = 0;
				yenibuf[i + height] = 0;
			}
			if (i >(width*height) - width && i < (width*height) - 1)  // alt kenar
			{
				yenibuf[i - height] = 0;
				yenibuf[i - 1] = 0;
				yenibuf[i] = 0;
				yenibuf[i + 1] = 0;
			}
			if (i > width && i < width*height - width - 1 && i % width != 0 && (i + 1) % width != 0)  //orta kýsým
			{
				yenibuf[i - height] = 0;
				yenibuf[i - 1] = 0;
				yenibuf[i] = 0;
				yenibuf[i + 1] = 0;
				yenibuf[i + height] = 0;
			}
		}
	return yenibuf;
}

BYTE* EdgeDetection(BYTE* Buffer, int width, int height, long* newsize)
{
	
	if ((NULL == Buffer) || (width == 0) || (height == 0))
		return NULL;
	int padding = 0;
	int scanlinebytes = width * 3;
	while ((scanlinebytes + padding) % 4 != 0)
		padding++;
	int psw = scanlinebytes + padding;




	*newsize = height * width;

	BYTE* buffer1 = new BYTE[*newsize];


	for (int i = 0; i < (width-2); i++)
		for (int j = 0; j < (height-2); j++)
		{
			int a = (Buffer[width*i + j] + 2 * Buffer[width*(i + 1) + j] + Buffer[width*(i + 2) + j])*(-1) +
				(Buffer[width*i + j + 2] + 2 * Buffer[width*(i + 1) + j + 2] + Buffer[width*(i + 2) + j + 2]);

			int b = (Buffer[width*i + j] + 2 * Buffer[width*i + j + 1] + Buffer[width*i + j + 2]) +
				(Buffer[width*(i + 2) + j] + 2 * Buffer[width*(i + 2) + j + 1] + Buffer[width*(i + 2) + j + 2])*(-1);



			buffer1[i*width + j] = abs(a) + abs(b);
			
		}
	
	return buffer1;
}

BYTE* CannyÝmage(BYTE* Buffer, int width, int height, long* newsize)
{
	int deger1;
	BYTE *ptr4, *ptr5, *ptr6;

	if ((NULL == Buffer) || (width == 0) || (height == 0))
		return NULL;
	*newsize = height * width;

	BYTE* yenibuf = new BYTE[*newsize];
	BYTE* buffer1 = new BYTE[(width + 2)*(height + 2)];
	ptr4 = Buffer;
	ptr5 = ptr4 + width;
	ptr6 = ptr5 + width;
	
	for (int i = 0; i < width - 1; i++)
		for (int j = 0; j < height - 1; j++)
		{
			deger1 = ptr6[i*width + j + 2] - ptr4[i*width + j] +
					 ptr5[i*width + j + 2] - ptr4[i*width + j + 1] +
					 ptr4[i*width + j + 1] - ptr5[i*width + j];
				
			yenibuf[i*width + j] = abs(deger1);
		}

	ptr4 = yenibuf;
	ptr5 = ptr4 + width;
	ptr6 = ptr5 + width;
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
		{
			if (ptr5[i*width + j + 1] < ptr4[i*width + j] || ptr5[i*width + j + 1] < ptr6[i*width + j + 2])
				buffer1[i*width + j] = 0;
			else
				buffer1[i*width + j] = ptr5[i*width + j + 1];
		}
	



	return buffer1;
}

/*BYTE* HoughÝmage(BYTE* Buffer, int width, int height, long* newsize)
{
	BYTE* buffer1 = new BYTE[*newsize];
	buffer1 = Threshold(Buffer, width, height, newsize);



	return buffer1;
}*/