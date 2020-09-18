#include <windows.h>

BYTE* LoadBMP(int* width, int* height, long* size, LPCTSTR bmpfile);
BYTE* EdgeDetection(BYTE* Buffer, int width, int height, long* newsize);
bool  SaveBMP(BYTE* Buffer, int width, int height, long paddedsize, LPCTSTR bmpfile);
BYTE* ConvertBMPToIntensity(BYTE* Buffer, int width, int height);
BYTE* ConvertIntensityToBMP(BYTE* Buffer, int width, int height, long* newsize);
BYTE* ConvertMaskMean(BYTE* Buffer, int width, int height, long* newsize);
BYTE* ConvertMaskMedyan(BYTE* Buffer, int width, int height, long* newsize);
BYTE* ConvertMaskGaussian(BYTE* Buffer, int width, int height, long* newsize);
BYTE* Threshold(BYTE* Buffer, int width, int height, long* newsize);
int Sort(int Array[]);
BYTE* Histogram(BYTE* Buffer, int width, int height);
BYTE* Dilation(BYTE* Buffer, int width, int height, long* newsize);
BYTE* Canny›mage(BYTE* Buffer, int width, int height, long* newsize);
BYTE* Hough›mage(BYTE* Buffer, int width, int height, long* newsize);