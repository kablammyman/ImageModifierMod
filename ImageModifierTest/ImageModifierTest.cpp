// ImageModifierTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <string>
#include <vector>

#include "stb_image.h"
#include "stb_image_write.h"

#include "ImageModiferHelpers.h"
#include <time.h>

using namespace std;

int main()
{
	string inputFile, outputDir;
	string outputFileNameOnly;
	string destFile;
	string waterMarkText;
	string ret;

	PIXMAP *photo = nullptr;
	PIXMAP *border = nullptr;
	unsigned char *finalImageRawPixels;

	bool addBorder = true;
	bool addTint = true;
	bool error = false;

	char slash = '\\';
	int bw;
	int bh;
	int bpp;



	srand(time(0));
	string args = "input=E:\\documents\\land investing\\parcels\\Eureka county NV\\stockPhotos\\nelvelco003.jpg&output=C:\\temp\\9untAe\\TyTaD4IC.png&text=";
	vector<string> argTokens = StringUtils::Tokenize(args, "&");
	inputFile = GetArgValue(argTokens, "input");
	outputDir = GetArgValue(argTokens, "output");
	waterMarkText = GetArgValue(argTokens, "text");

	//ap_log_error(__FILE__, __LINE__, 0, APLOG_INFO, 0, req->server, "-------------MY TEST!!!! Entered Function = hello_handler, time = %d", req->finfo.mtime);
	if (inputFile.empty())
	{
		//ap_log_error(__FILE__, __LINE__, 0, APLOG_INFO, 0, req->server, "Entered Function = hello_handler, time = %d", req->finfo.mtime);
		ret += "specify a .png or .jpg image to modify. Use the param -i or --input\n";
		ret += "\nyou can optionally specify the fully qualified output path or path+filename using -o or --output\n";
		error = true;
	}
	unsigned char* imgData = stbi_load(inputFile.c_str(), &bw, &bh, &bpp, 4);
	if (!imgData)
	{
		ret += "\ncouldnt load image: " + inputFile;
		error = true;
	}


	if (outputDir.empty())
	{
		outputDir = inputFile.substr(0, inputFile.find_last_of("/\\") + 1);
	}

	if (outputDir.back() != slash && outputDir.find_last_of(".") == string::npos)
	{
		outputDir += slash;
	}



	if (outputDir.find_last_of(".") == string::npos)
		outputFileNameOnly = inputFile.substr(inputFile.find_last_of("/\\") + 1);
	else
	{
		outputFileNameOnly = outputDir.substr(outputDir.find_last_of("/\\") + 1);
		outputDir = outputDir.substr(0, outputDir.find_last_of("/\\") + 1);
	}
	outputFileNameOnly = outputFileNameOnly.substr(0, outputFileNameOnly.find_last_of("."));//remove file ext

																							//if the dest file is in a differnt dir than src file, dont add a modfied marker
	string inputDir = inputFile.substr(0, inputFile.find_last_of("/\\") + 1);

	if (outputDir == inputDir)
		destFile = outputDir + "modified_" + outputFileNameOnly + ".png";
	else
		destFile = outputDir + outputFileNameOnly + ".png";

	string inputFileExt = inputFile.substr(inputFile.find_last_of(".") + 1);//remove file ext 



	photo = new PIXMAP(imgData, bw, bh);
	//once the data ahs been copied into our class, get rid of raw data
	stbi_image_free(imgData);

	if (!photo)//sometimes the bmp fails to load, until i figure out why, ill just do this
	{
		photo = nullptr;
		ret += "\ncouldnt convert image into PIXMAP: " + inputFile;
	}


	bw *= GetRandomFloat(0.5f, 1.5f);
	bh *= GetRandomFloat(0.5f, 1.5f);

	photo->Scale(bw, bh);

	//do we want to add a color tint?
	if (GetRandomNum(0, 100) > 50)
		addTint = false;
	if (addTint)
	{
		int amt = GetRandomNum(10, 50);
		int gun = GetRandomNum(0, 90);

		for (int i = 0; i < (bw * bh); i++)
		{
			if (gun >= 0 && gun <= 30)
			{
				if (photo->pixels[i].r < (255 - amt))
					photo->pixels[i].r += amt;
				else
					photo->pixels[i].r = 255;
			}
			else if (gun >= 31 && gun <= 60)
			{
				if (photo->pixels[i].g < (255 - amt))
					photo->pixels[i].g += amt;
				else
					photo->pixels[i].g = 255;
			}
			else
			{
				if (photo->pixels[i].b < (255 - amt))
					photo->pixels[i].b += amt;
				else
					photo->pixels[i].b = 255;
			}
		}
	}
	//add the water mark if there is one
	if (!waterMarkText.empty())
	{
		for (int i = 0; i < 10; i++)
		{
			StockBitmapFont testFont;
			int xPos = GetRandomNum(0, bw - 1);
			int yPos = GetRandomNum(0, bh - 1);
			testFont.Draw(photo, waterMarkText, xPos, yPos, true);
		}
	}

	//add random "transparent marks" to change up some pixels even more
	for (int i = 0; i < 50; i++)
	{
		int pix = GetRandomNum(0, (bw * bh));
		photo->pixels[pix] = RGBA{ (unsigned char)GetRandomNum(0,255),(unsigned char)GetRandomNum(0,255),(unsigned char)GetRandomNum(0,255),255 };

		int xx = GetRandomNum(0, (bw - 10));
		int yy = GetRandomNum(0, (bh - 10));

		RGBA col = GetPixelsAvgColor(photo, xx, yy, 8, 8);
		StockBitmapFont testFont(col);
		testFont.Draw(photo, "X", xx, yy, true);
	}

	//do we want to add a border?
	if (GetRandomNum(0, 100) > 50)
		addBorder = false;

	if (addBorder)
	{
		int borderSize = GetRandomNum(10, 20);
		border = new PIXMAP(photo->w + borderSize, photo->h + borderSize);

		//do we want to add a border?
		if (GetRandomNum(0, 100) > 50)
			border->Fill(RGBA{ (unsigned char)GetRandomNum(0,255),(unsigned char)GetRandomNum(0,255),(unsigned char)GetRandomNum(0,255),255 });
		else
		{
			for (int i = 0; i < border->w; i++)
				for (int j = 0; j < border->h; j++)
					border->PutPixel(RGBA{ (unsigned char)GetRandomNum(0,255),(unsigned char)GetRandomNum(0,255),(unsigned char)GetRandomNum(0,255),255 }, i, j);
		}

		photo->Blit(border, borderSize / 2, borderSize / 2);
		finalImageRawPixels = (unsigned char *)border->pixels;
		bw = border->w;
		bh = border->h;

	}
	else
		finalImageRawPixels = (unsigned char *)photo->pixels;

	//4th param = comp, which is 1=Y, 2=YA, 3=RGB, 4=RGBA.
	stbi_write_png(destFile.c_str(), bw, bh, 4, finalImageRawPixels, bw * sizeof(unsigned int));
	ret += "\nsuccess! new file: " + destFile;


	photo->Destroy();
	if (addBorder)
		border->Destroy();
	finalImageRawPixels = nullptr;

	return 0;

	
}

