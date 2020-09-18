// Image_Process1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <windows.h>
#include <math.h>

//User defined includes
#include "imge_bmp.h"

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	LPCTSTR input, output;
	int Width, Height;
	long Size, new_size;

	cout << "Haydi Bismillah" << endl;
	input = L"C://Users//Baris//Desktop//foto1.bmp";
	
	BYTE* buffer = LoadBMP(&Width, &Height, &Size, input);
	BYTE* raw_intensity = ConvertBMPToIntensity(buffer, Width, Height);
	
	BYTE* raw_intensityMean = ConvertMaskMean(raw_intensity, Width, Height, &new_size);
	BYTE* raw_intensityMedyan = ConvertMaskMedyan(raw_intensity, Width, Height, &new_size);
	BYTE* raw_intensityGaussian = ConvertMaskGaussian(raw_intensity, Width, Height, &new_size);
	//BYTE* raw_intensityDilation = Dilation(buffer, Width, Height, &new_size);
	BYTE* raw_intensityEdge = EdgeDetection(raw_intensityMean, Width, Height, &new_size);
	//BYTE* raw_intensityCanny= CannyÝmage(raw_intensityEdge, Width, Height, &new_size);
	//BYTE* raw_intensityHough = HoughÝmage(raw_intensityEdge, Width, Height, &new_size);
	BYTE* threshold1 = Threshold(raw_intensityMean, Width, Height, &new_size);
	char ch;
	cout << "Sonucu diske kaydetsin mi? E/H:"; cin >> ch;
	if ((ch == 'E') || (ch == 'e')) 
	{
		BYTE* display_imge = ConvertIntensityToBMP(raw_intensityEdge, Width, Height, &new_size);
		output = L"C://Users//Baris//Desktop//EdgeDetection.bmp";
		if (SaveBMP(display_imge, Width, Height, new_size, output))
			 cout << "Output Image was successfully saved" << endl;
		else cout << "Error on saving image" << endl;
		delete[] display_imge;
	}


	delete[] buffer;
	delete[] raw_intensity;
	delete[] raw_intensityMean;
	delete[] raw_intensityMedyan;
	delete[] raw_intensityGaussian;
	delete[] threshold1;
	delete[] raw_intensityEdge;
	/*delete[] raw_intensityCanny;
	delete[] raw_intensityHough;*/
	system("pause");
	return 0;
}







