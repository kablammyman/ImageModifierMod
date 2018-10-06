#include "SDL.h"
#undef main
//#include "SDL_ttf.h"
#include "SDL_syswm.h"
#include <SDL_image.h>

#include <stdio.h>
#include <string>

#include "stb_image.h"
#include "stb_image_write.h"

#include "BitmapFont.h"

#include <string>
#include <time.h>

using namespace std;


//---------------------------------------------------------------------------------------------------
int main(int argc, const char * argv[])
{
	
#if _WIN32
	
#else
	slash '/';
#endif

	srand(time(0));


	parseArgs(argc, argv, inputFile, outputDir, waterMarkText);

	if (inputFile.empty())
	{
		printf("specify a .png or .jpg image to modify. Use the param -i or --input\n");
		printf("you can optionally specify the fully qualified output path or path+filename using -o or --output\n");
		return -1;
	}

	std::size_t found = inputFile.find_last_of(".");
	fileExt = inputFile.substr(found + 1);
	
	
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

	if(outputDir == inputDir)
		destFile = outputDir + "modified_" + outputFileNameOnly + ".png";
	else
		destFile = outputDir + outputFileNameOnly + ".png";

	string inputFileExt = inputFile.substr(inputFile.find_last_of(".") + 1);//remove file ext 
	
	int bw;
	int bh;
	int bpp;
	unsigned char* imgData = stbi_load(inputFile.c_str(), &bw, &bh, &bpp, 4);
	if (!imgData)
	{
		printf("couldnt load image: %s\n", inputFile.c_str());
		return -1;
	}

	photo = new PIXMAP(imgData, bw, bh);
	//once the data ahs been copied into our class, get rid of raw data
	stbi_image_free(imgData);

	if (!photo)//sometimes the bmp fails to load, until i figure out why, ill just do this
	{
		photo = nullptr;
		printf("couldnt convert image into PIXMAP: %s\n", inputFile.c_str());
		return -1;
	}

	
	bw *= GetRandomFloat(0.5f, 1.5f);
	bh *= GetRandomFloat(0.5f, 1.5f);
	
	photo->Scale(bw, bh);

	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window * window;
	SDL_Renderer * renderer;
	SDL_Texture * texture;
	if (showImageWindow)
	{
		window = SDL_CreateWindow("SDL2 Pixel Drawing", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, bw, bh, 0);
		renderer = SDL_CreateRenderer(window, -1, 0);
		texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, bw, bh);
	}

	//do we want to add a color tint?
	if (GetRandomNum(0, 100) > 50)
		addTint = false;
	if (addTint)
	{
		int amt = GetRandomNum(10,50);
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
		photo->pixels[pix] = RGBA{ (unsigned char)GetRandomNum(0,255),(unsigned char)GetRandomNum(0,255),(unsigned char)GetRandomNum(0,255),255};

		int xx = GetRandomNum(0, (bw - 10)); 
		int yy = GetRandomNum(0, (bh - 10));

		RGBA col = GetPixelsAvgColor(photo, xx, yy, 8, 8);
		StockBitmapFont testFont(col);
		testFont.Draw(photo, "X", xx, yy, true);
	}

	//do we want to add a border?
	if(GetRandomNum(0, 100) > 50)
		addBorder = false;

	if (addBorder)
	{
		int borderSize = GetRandomNum(10, 20);
		PIXMAP *border = new PIXMAP(photo->w + borderSize, photo->h + borderSize);

		//do we want to add a border?
		if (GetRandomNum(0, 100) > 50)
			border->Fill(RGBA{ (unsigned char)GetRandomNum(0,255),(unsigned char)GetRandomNum(0,255),(unsigned char)GetRandomNum(0,255),255 });
		else
		{
			for (int i = 0; i < border->w; i++)
				for (int j = 0; j < border->h; j++)
					border->PutPixel(RGBA{ (unsigned char)GetRandomNum(0,255),(unsigned char)GetRandomNum(0,255),(unsigned char)GetRandomNum(0,255),255 }, i, j);
		}

		photo->Blit(border, borderSize/2, borderSize/2);
		finalImageRawPixels = (unsigned char *)border->pixels;
		bw = border->w;
		bh = border->h;
	}
	else
		finalImageRawPixels = (unsigned char *)photo->pixels;
	
	//4th param = comp, which is 1=Y, 2=YA, 3=RGB, 4=RGBA.
	stbi_write_png(destFile.c_str(), bw, bh, 4, finalImageRawPixels, bw * sizeof(Uint32));
	
	if (showImageWindow)
	{
		bool quit = false;
		SDL_Event event;
		bool leftMouseButtonDown = false;

		while (!quit)
		{
			SDL_UpdateTexture(texture, NULL, finalImageRawPixels, bw * sizeof(Uint32));
			SDL_WaitEvent(&event);

			switch (event.type)
			{
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE)
					quit = true;
				break;

			case SDL_MOUSEBUTTONUP:
				if (event.button.button == SDL_BUTTON_LEFT)
					leftMouseButtonDown = false;
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT)
					leftMouseButtonDown = true;
			case SDL_MOUSEMOTION:
				if (leftMouseButtonDown)
				{
					int mouseX = event.motion.x;
					int mouseY = event.motion.y;
					//rawPixels[mouseY * 640 + mouseX] = 0;
				}
				break;
			case SDL_QUIT:
				quit = true;
				break;
			}

			SDL_RenderClear(renderer);
			SDL_RenderCopy(renderer, texture, NULL, NULL);
			SDL_RenderPresent(renderer);
		}

		SDL_DestroyTexture(texture);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
	}

	
	SDL_Quit();

	return 0;
}
