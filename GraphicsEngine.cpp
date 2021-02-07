




// This is GraphicsEngine.CPP
//
// This has the implementation of the methods
//
///////////////////////////////////////////////////////////////////////////////////////////////

// INCLUDES  ////////////////////////////////////////////////////////////////////
#include "Engine.h"


// this class variable is used to hold the only instance of the GraphicsEngine class
static GraphicsEngine* instance = 0;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:  getInstance
//
// Arguments:
//            HWND:  a handle to the window that we intend to use direct input on
//
//            int:  the number of bits per pixel that the display will use.
//                  this value can be either 16, 24 or 32 bits.
//
// Return type:
//             GraphicsEngine* : a pointer to the GraphicsEngine object having the
//                               properties specified in the arguments
//
//  This is a class vaiable and is used to implement the singleton property.
//  If before calling this function (instance = 0) then an instance is returned
//  that has the screen width and screen height set to the width and height of
//  the system the program will run on.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GraphicsEngine* GraphicsEngine::getInstance(HWND hwnd, int bitsPerPixel)
{
     if(instance == 0)  // if no instance Exists
     {
         instance = new GraphicsEngine(hwnd, bitsPerPixel);  // create an new instance
     }

     return instance;  // return current instance
}




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Name:  getInstance
//
// Arguments:
//            HWND:  a handle to the window that we intend to use direct input on
//
//            int bitsPerPixel:  the number of bits per pixel that the display will use.
//                  this value can be either 16, 24 or 32 bits.
//
//            int screenWidth:  the width of the screen in pixels.
//
//            int screenheight: the height of the screen in pixels.
//
// Return type:
//             GraphicsEngine* : a pointer to the GraphicsEngine object having the
//                               properties specified in the arguments
//
//  This is a class vaiable and is used to implement the singleton property.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GraphicsEngine* GraphicsEngine::getInstance(HWND hwnd, int bitsPerPixel, int screenWidth, int screenHeight)
{
     if(instance == 0)  // if no instance Exists
     {
         instance = new GraphicsEngine(hwnd, bitsPerPixel, screenWidth, screenHeight);  // create an new instance
     }

     return instance;  // return current instance
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// this is the default constructor
//
// Arguments:  HWND:  a handle to the window that we intend to use direct input on.
//
//             int  : The number of bits per pixel that we wish the application to run with.
//                    the value must be either 16, 24 or 32.
//
//  This constructor is private so that a singleton class can be inplemented
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GraphicsEngine::GraphicsEngine(HWND hwnd, int bitsPerPixel)
{
      // initializing primitive variables /////////////////////////////////////////
      this->screenWidth = GetSystemMetrics(SM_CXSCREEN);
      this->screenHeight = GetSystemMetrics(SM_CYSCREEN);

      this->hwnd = hwnd;

      if(bitsPerPixel != 16 && bitsPerPixel != 24 && bitsPerPixel != 32)
      {
           MessageBox(hwnd, "The bpp was not set to 16, 24 or 32 bits. Hence the default of 16 will be set", "GraphicsEngine::GraphicsEngine(HWND hwnd, int bitsPerPixel)", MB_OK);

           this->bitsPerPixel = 16;
      }
      else
          this->bitsPerPixel = bitsPerPixel;



      this->clips = 0;
      /////////////////////////////////////////////////////////////////////////////




      // initializing direct draw //////////////////////////////////////////////////
      if(DirectDrawCreateEx(NULL, (void **)&lpdd, IID_IDirectDraw7, NULL)!= DD_OK)
         MessageBox(hwnd, "Failed to initialize direct draw", "GraphicsEngine::GraphicsEngine(HWND hwnd, int bitsPerPixel) ", MB_OK);



           // set corporation level to windowed mode normal
      if(lpdd->SetCooperativeLevel(hwnd,DDSCL_ALLOWMODEX | DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT) != DD_OK)
         MessageBox(hwnd, "Failed to set coorporative level", " GraphicsEngine::GraphicsEngine(HWND hwnd, int bitsPerPixel)", MB_OK);

      // set the display mode
      if(lpdd->SetDisplayMode(screenWidth,screenHeight,bitsPerPixel,0,0) != DD_OK)
         MessageBox(hwnd, "Failed to set display mode", " GraphicsEngine::GraphicsEngine(HWND hwnd, int bitsPerPixel)", MB_OK);
      ///////////////////////////////////////////////////////////////////////////////




     // creating the primary surface with a backbuffer //////////////////////////////////////////////////////////////////////
     memset(&ddsd,0,sizeof(ddsd));
     ddsd.dwSize = sizeof(ddsd);                          // manually set the size


     ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT | DDSD_CKSRCBLT;     // this surface has a backbuffer

     ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;   // a flippable complex surface

     ddsd.dwBackBufferCount = 1;      // set the backbuffer count to 1



     //creating the surface
     if(lpdd->CreateSurface(&ddsd,&primary,NULL) != DD_OK)
        MessageBox(hwnd, "Failed to create the surface", " GraphicsEngine::GraphicsEngine(HWND hwnd, int bitsPerPixel)", MB_OK);


     // Query for the backbuffer or secondary surface
     ddscaps.dwCaps = DDSCAPS_BACKBUFFER;

     //get the surface
     if(primary->GetAttachedSurface(&ddscaps,&secondary) != DD_OK)
         MessageBox(hwnd, "Failed to get secondary surface", "GraphicsEngine::GraphicsEngine(HWND hwnd, int bitsPerPixel) ", MB_OK);

     workBench = createSurface(workBench, screenWidth, screenHeight);



     if(lpdd->CreateClipper(0, &clipper, NULL) != DD_OK)
        MessageBox(hwnd, "Failed to create clipper", "GraphicsEngine::GraphicsEngine(HWND hwnd, int bitsPerPixel) ", MB_OK);

     // attach the clpipper to the primary surface
     if(clipper->SetHWnd(0, this->hwnd) != DD_OK)
        MessageBox(hwnd, "Failed to set hwnd", "GraphicsEngine::GraphicsEngine(HWND hwnd, int bitsPerPixel) ", MB_OK);


     if(workBench->SetClipper(clipper) != DD_OK)
         MessageBox(hwnd, "Failed to set clipper to workbench", "GraphicsEngine::GraphicsEngine(HWND hwnd, int bitsPerPixel) ", MB_OK);

     if(secondary->SetClipper(clipper) != DD_OK)
         MessageBox(hwnd, "Failed to set clipper to secondary", "GraphicsEngine::GraphicsEngine(HWND hwnd, int bitsPerPixel) ", MB_OK);

     // setting the color key
     key.dwColorSpaceLowValue = RGB16BIT565(0,0,0);
     key.dwColorSpaceLowValue = RGB16BIT565(0,0,0);



     if(workBench->SetColorKey(DDCKEY_SRCBLT, &key) != DD_OK)
        MessageBox(hwnd, "Failed to set colur key to workbench surface", "GraphicsEngine::GraphicsEngine(HWND hwnd, int bitsPerPixel) ", MB_OK);


}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// this is the default constructor
//
// Arguments:  HWND:  a handle to the window that we intend to use direct input on.
//
//             int bitsPerPixel  : The number of bits per pixel that we wish the application to run with.
//                    the value must be either 16, 24 or 32.
//
//             int screenWidth:  the width of the screen in pixels.
//
//             int screenheight: the height of the screen in pixels.
//
//  This constructor is private so that a singleton class can be inplemented
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GraphicsEngine::GraphicsEngine(HWND hwnd, int bitsPerPixel, int screenWidth, int screenHeight)
{
      // initializing primitive variables /////////////////////////////////////////
      this->screenWidth = screenWidth;
      this->screenHeight = screenHeight;

      this->hwnd = hwnd;

      if(bitsPerPixel != 16 && bitsPerPixel != 24 && bitsPerPixel != 32)
      {
           MessageBox(hwnd, "The bpp was not set to 16, 24 or 32 bits. Hence the default of 16 will be set", " GraphicsEngine::GraphicsEngine(HWND hwnd, int bitsPerPixel, int screenWidth, int screenHeight)", MB_OK);

           this->bitsPerPixel = 16;
      }
      else
          this->bitsPerPixel = bitsPerPixel;



      this->clips = 0;
      /////////////////////////////////////////////////////////////////////////////




      // initializing direct draw //////////////////////////////////////////////////
      if(DirectDrawCreateEx(NULL, (void **)&lpdd, IID_IDirectDraw7, NULL)!= DD_OK)
         MessageBox(hwnd, "Failed to initialize direct draw", " GraphicsEngine::GraphicsEngine(HWND hwnd, int bitsPerPixel, int screenWidth, int screenHeight)", MB_OK);



           // set corporation level to windowed mode normal
      if(lpdd->SetCooperativeLevel(hwnd,DDSCL_ALLOWMODEX | DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT) != DD_OK)
         MessageBox(hwnd, "Failed to set coorporative level", " GraphicsEngine::GraphicsEngine(HWND hwnd, int bitsPerPixel, int screenWidth, int screenHeight)", MB_OK);

      // set the display mode
      if(lpdd->SetDisplayMode(screenWidth,screenHeight,bitsPerPixel,0,0) != DD_OK)
         MessageBox(hwnd, "Failed to set display mode", "GraphicsEngine::GraphicsEngine(HWND hwnd, int bitsPerPixel, int screenWidth, int screenHeight)", MB_OK);
      ///////////////////////////////////////////////////////////////////////////////



      
     // creating the primary surface with a backbuffer //////////////////////////////////////////////////////////////////////
     memset(&ddsd,0,sizeof(ddsd));
     ddsd.dwSize = sizeof(ddsd);                          // manually set the size


     ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT | DDSD_CKSRCBLT;     // this surface has a backbuffer

     ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;   // a flippable complex surface

     ddsd.dwBackBufferCount = 1;      // set the backbuffer count to 1



     //creating the surface
     if(lpdd->CreateSurface(&ddsd,&primary,NULL) != DD_OK)
        MessageBox(hwnd, "Failed to create the surface", "GraphicsEngine::GraphicsEngine(HWND hwnd, int bitsPerPixel, int screenWidth, int screenHeight)", MB_OK);


     // Query for the backbuffer or secondary surface
     ddscaps.dwCaps = DDSCAPS_BACKBUFFER;

     //get the surface
     if(primary->GetAttachedSurface(&ddscaps,&secondary) != DD_OK)
        MessageBox(hwnd, "Failed to get secondary surface", "GraphicsEngine::GraphicsEngine(HWND hwnd, int bitsPerPixel, int screenWidth, int screenHeight)", MB_OK);





     workBench = createSurface(workBench, screenWidth, screenHeight);


     if(lpdd->CreateClipper(0, &clipper, NULL) != DD_OK)
        MessageBox(hwnd, "Failed to create clipper", "GraphicsEngine::GraphicsEngine(HWND hwnd, int bitsPerPixel) ", MB_OK);

     // attach the clpipper to the primary surface
     if(clipper->SetHWnd(0, this->hwnd) != DD_OK)
        MessageBox(hwnd, "Failed to set hwnd", "GraphicsEngine::GraphicsEngine(HWND hwnd, int bitsPerPixel) ", MB_OK);


     if(workBench->SetClipper(clipper) != DD_OK)
         MessageBox(hwnd, "Failed to set clipper to workbench", "GraphicsEngine::GraphicsEngine(HWND hwnd, int bitsPerPixel) ", MB_OK);

     if(secondary->SetClipper(clipper) != DD_OK)
         MessageBox(hwnd, "Failed to set clipper to secondary", "GraphicsEngine::GraphicsEngine(HWND hwnd, int bitsPerPixel) ", MB_OK);


         


     if(workBench->SetColorKey(DDCKEY_SRCBLT, &key) != DD_OK)
        MessageBox(hwnd, "Failed to set colur key to workbench surface", "GraphicsEngine::GraphicsEngine(HWND hwnd, int bitsPerPixel) ", MB_OK);



}






////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// this is the destructor
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GraphicsEngine::~GraphicsEngine()
{
    if(secondary)
       secondary->Release();

    if(primary)
        primary->Release();

    if(lpdd != NULL)
        lpdd->Release();


    if(workBench != NULL)
       workBench->Release();

        // delete this first
    if(instance != 0)
        delete instance;

}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// name: randomdraw
//
// Arguments: none.
//
// Return Type: none.
//
// This method was intended to test the primary surface to ensure that  we can draw to it.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GraphicsEngine::randomdraw()
{

    // this will be aliased to the video memory
    unsigned short* video_buffer = NULL;

    // this will hold the number of 16 bit words per video line
    int words_per_line = 0;
                             





    // set up the surface description to lock the surface
    memset(&ddsd,0,sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);

    // lock the primary surface

    primary->Lock(NULL,&ddsd,
                  DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT,NULL);

    // get video pointer, we could have just as easily used

    video_buffer = (unsigned short*)ddsd.lpSurface;

    // compute words per line
    words_per_line = (ddsd.lPitch >> 1);




    // draw 100 pixels
    for (int index=0; index<100; index++)
    {
        // get random x,y
        int x = rand()%screenWidth;
        int y = rand()%screenHeight;

        // get random color
        unsigned char red   = rand()%256;
        unsigned char green = rand()%256;
        unsigned char blue  = rand()%256;

        // plot the pixel with a random color
    
        video_buffer[x + (y*words_per_line)] = RGB24BIT(0,red,green,blue);

    }

    // unlock the surface, very important!
    primary->Unlock(NULL);

    return;

}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Name: drawPixelPrimary
//
// Arguments:
//           int x: x coordinate to draw to.
//
//           int y: y coordinate to draw to.
//
//           int red: the value of the red bit.
//
//           int green: the value of the green bit.
//
//           int blue: the value of the blue bit.
//
// Return type: none.
//
//   This method is used to draw a pixel to the primary surface at coordinates (x,y)
//   with colour speciifed by the values of the red, green and blue parameters.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GraphicsEngine::drawPixelPrimary(int x, int y, int red, int green, int blue)
{

    if( red > 256 || green > 256 || blue > 256 || red < 0 || green < 0 || blue < 0)
    {
		MessageBox(hwnd, "The red, green and blue values must >= 0 and <= 255", "void GraphicsEngine::drawPixelPrimary(int x, int y, int red, int green, int blue) ", MB_OK);

		return;
    }

     //set up surface descriptor to lock the surface
     memset(&ddsd,0,sizeof(ddsd));
     ddsd.dwSize = sizeof(ddsd);

     // lock the primary surface
     primary->Lock(NULL,&ddsd,DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);

     //get video pointer
     unsigned short* video = (unsigned short*)ddsd.lpSurface;

     // use video poiter to writ to the surface
     if(bitsPerPixel == 16)
        video[x + (y*ddsd.lPitch >> 1)] = RGB16BIT565(red,green,blue);
     else
        video[x + (y*ddsd.lPitch >> 1)] = RGB24BIT(0,red,green,blue);

     //unlock the surface
     primary->Unlock(NULL);
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Name: drawPixelSecondary
//
// Arguments:
//           int x: x coordinate to draw to.
//
//           int y: y coordinate to draw to.
//
//           int red: the value of the red bit.
//
//           int green: the value of the green bit.
//
//           int blue: the value of the blue bit.
//
// Return type: none.
//
//   This method is used to draw a pixel to the seondary surface at coordinates (x,y)
//   with colour speciifed by the values of the red, green and blue parameters.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GraphicsEngine::drawPixelSecondary(int x, int y, int red, int green, int blue)
{

    if( red > 256 || green > 256 || blue > 256 || red < 0 || green < 0 || blue < 0)
    {
		MessageBox(hwnd, "The red, green and blue values must >= 0 and <= 255", " void GraphicsEngine::drawPixelSecondary(int x, int y, int red, int green, int blue)", MB_OK);

		return;
    }

     //set up surface descriptor to lock the surface
     memset(&ddsd,0,sizeof(ddsd));
     ddsd.dwSize = sizeof(ddsd);

     // lock the primary surface
     secondary->Lock(NULL,&ddsd,DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);

     //get video pointer
     unsigned short* video = (unsigned short*)ddsd.lpSurface;

     // use video poiter to writ to the surface
     if(bitsPerPixel == 16)
        video[x + (y*ddsd.lPitch >> 1)] = RGB16BIT565(red,green,blue);
     else
        video[x + (y*ddsd.lPitch >> 1)] = RGB24BIT(0,red,green,blue);

     //unlock the surface
     secondary->Unlock(NULL);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Name: drawPixelWorkbench
//
// Arguments:
//           int x: x coordinate to draw to.
//
//           int y: y coordinate to draw to.
//
//           int red: the value of the red bit.
//
//           int green: the value of the green bit.
//
//           int blue: the value of the blue bit.
//
// Return type: none.
//
//   This method is used to draw a pixel to the workbench surface at coordinates (x,y)
//   with colour speciifed by the values of the red, green and blue parameters.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GraphicsEngine::drawPixelWorkbench(int x, int y, int red, int green, int blue)
{

    if( red > 256 || green > 256 || blue > 256 || red < 0 || green < 0 || blue < 0)
    {
		MessageBox(hwnd, "The red, green and blue values must >= 0 and <= 255", " void GraphicsEngine::drawPixelSecondary(int x, int y, int red, int green, int blue)", MB_OK);

		return;
    }

     //set up surface descriptor to lock the surface
     memset(&ddsd,0,sizeof(ddsd));
     ddsd.dwSize = sizeof(ddsd);

     // lock the primary surface
     workBench->Lock(NULL,&ddsd,DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);

     //get video pointer
     unsigned short* video = (unsigned short*)ddsd.lpSurface;

     // use video poiter to writ to the surface
     if(bitsPerPixel == 16)
        video[x + (y*ddsd.lPitch >> 1)] = RGB16BIT565(red,green,blue);
     else
        video[x + (y*ddsd.lPitch >> 1)] = RGB24BIT(0,red,green,blue);

     //unlock the surface
     workBench->Unlock(NULL);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Name: drawPixelSurface
//
// Arguments:
//           int x: x coordinate to draw to.
//
//           int y: y coordinate to draw to.
//
//           int red: the value of the red bit.
//
//           int green: the value of the green bit.
//
//           int blue: the value of the blue bit.
//
// Return type: none.
//
//   This method is used to draw a pixel to an offscreen surface at coordinates (x,y)
//   with colour speciifed by the values of the red, green and blue parameters.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GraphicsEngine::drawPixelSurface(LPDIRECTDRAWSURFACE7 surface, int x, int y, int red, int green, int blue)
{

    if( red > 256 || green > 256 || blue > 256 || red < 0 || green < 0 || blue < 0)
    {
		MessageBox(hwnd, "The red, green and blue values must >= 0 and <= 255", " void GraphicsEngine::drawPixelSurface(LPDIRECTDRAWSURFACE7 surface, int x, int y, int red, int green, int blue)", MB_OK);

		return;
    }

     //set up surface descriptor to lock the surface
     memset(&ddsd,0,sizeof(ddsd));
     ddsd.dwSize = sizeof(ddsd);

     // lock the primary surface
     surface->Lock(NULL,&ddsd,DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);

     //get video pointer
     unsigned short* video = (unsigned short*)ddsd.lpSurface;

     // use video poiter to writ to the surface
     video[x + (y*ddsd.lPitch >> 1)] = RGB16BIT565(red,green,blue);

     //unlock the surface
     surface->Unlock(NULL);
}

	

        
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Name: createSurface
//
// Arguments:
//           LPDIRECTDRAWSURFACE7 surface:  A pointer to a direct draw7 surface.
//
//           int width: the width of the surface speciifed in the first argument.
//
//           int height: The height of the surface specified in the first argument.
//
// Return type: A valid direct draw surface having the given dimensions.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LPDIRECTDRAWSURFACE7 GraphicsEngine::createSurface(LPDIRECTDRAWSURFACE7 surface, int width, int height)
{
     // setting the size parameters
     memset(&ddsd,0,sizeof(ddsd));
     ddsd.dwSize = sizeof(ddsd);

     // setting the flags
     ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_CKSRCBLT;

     // set dimentsions of the surface
     ddsd.dwWidth = width;
     ddsd.dwHeight =  height;



     // set the memory. vram is default
     ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;

     // now create the surface and check for error
     if(lpdd->CreateSurface(&ddsd,&surface,NULL) != DD_OK)
     {
         MessageBox(hwnd, "the offscreen surface has not been created"," LPDIRECTDRAWSURFACE7 GraphicsEngine::createSurface(LPDIRECTDRAWSURFACE7 surface, int width, int height)", MB_OK);

         return NULL;
     }

     // set color key to color to RGB(0,0,0)
     DDCOLORKEY colorkey; // used to set color key
     colorkey.dwColorSpaceLowValue  = RGB16BIT565(0,0,0);
     colorkey.dwColorSpaceHighValue = RGB16BIT565(0,0,0);

     // now set the color key for source blitting
     if(surface->SetColorKey(DDCKEY_SRCBLT, &colorkey) != DD_OK)
        MessageBox(hwnd, "Could not set colour key", " LPDIRECTDRAWSURFACE7 GraphicsEngine::createSurface(LPDIRECTDRAWSURFACE7 surface, int width, int height)", MB_OK);

     return surface;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Name: bitmapToSurface
//
// Arguments:
//           LPDIRECTDRAWSURFACE7 surface: A pointer to the surface that we wish to copy the bitmap onto.
//
//           HBITMAP bitmap: The windows bitmap in memory that we want to copy.
//
//           int width: The width of the bitmap
//
//           int height: The height of the bitmap
//
// Return type:
//             HRESULT: this is flag returned. I can be tested to see if an erroe has occured in claaing this method
//
// This method is used to copy a bitmap that was loaded into memory onto the direct draw surface passes as an argument.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HRESULT GraphicsEngine::bitmapToSurface(LPDIRECTDRAWSURFACE7 surface, HBITMAP bitmap, int width, int height)
{
     HDC hdcImage;
     HDC hdc;
     HRESULT hr;
     HBITMAP hbmOld;

     // select bitmap into a memory Dc so we can use it

     hdcImage = CreateCompatibleDC(NULL);
     hbmOld = (HBITMAP)SelectObject(hdcImage, bitmap);

     if((hr = surface->GetDC(&hdc)) == DD_OK)
     {
          BitBlt(hdc,0,0,width,height,hdcImage,0,0,SRCCOPY);

          surface->ReleaseDC(hdc);
     }

     SelectObject(hdcImage, hbmOld);
     DeleteDC(hdcImage);

     return hr;
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Name: loadBitmap.
//
// Arguments: LPCSTR bitmapName: the name of the .bmp file that we intend to load into memory eg "fish.bmp".
//
// Return type: A direct draw surface that has the bitmap copied onto it.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////   
LPDIRECTDRAWSURFACE7 GraphicsEngine::loadBitmap(LPCSTR bitmapName)
{
     HBITMAP hBitmap;
     BITMAP bitmap;
     LPDIRECTDRAWSURFACE7 surface;

     hBitmap = (HBITMAP)LoadImage(NULL,bitmapName,IMAGE_BITMAP,0,0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);

     if(hBitmap == NULL)
        return NULL;

     GetObject(hBitmap, sizeof(bitmap), &bitmap);     // get size of bitmap

     // create a direct draw surface for this bitmap immediately
     surface = createSurface(surface, bitmap.bmWidth, bitmap.bmHeight);

     if(surface)
     {
          bitmapToSurface(surface, hBitmap, bitmap.bmWidth, bitmap.bmHeight);
     }

     DeleteObject(hBitmap);

     return surface;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Name: toPrimarySurface.
//
// Arguments: LPDIRECTDRAWSURFACE7: The direct draw surface that we wish to copy onto to primary surface
//
// Return Type: void
//
// This method is used to copy the contents of the surface specifed in the argument onto the primary surface or display at
// coordinates (0,0) on the primary surface.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GraphicsEngine::toPrimarySurface(LPDIRECTDRAWSURFACE7 surface)
{
    // clear out the structure and set the size field
    memset(&blitfx,0,sizeof(DDBLTFX));

    blitfx.dwSize = sizeof(DDBLTFX);

    //ddsd
    //set up surface descriptor to lock the surface
    memset(&ddsd,0,sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);

    // lock the surface so we can get the height and the width of the surface
    if(surface->Lock(NULL,&ddsd,DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT  , NULL) != DD_OK)
       MessageBox(hwnd, "could not lock offscreen", "void GraphicsEngine::toPrimarySurface(LPDIRECTDRAWSURFACE7 surface) ", MB_OK);

    int width = ddsd.dwWidth;
    int height = ddsd.dwHeight;  //unsigned long

    surface->Unlock(NULL);  // unlock surface

    // define the parameters of the destination rectangle
    RECT dest;

    dest.top = 0;
    dest.left = 0;
    dest.bottom = height;
    dest.right = width;



    //ready to blit to surface
    if(primary->Blt(&dest, surface, NULL,DDBLT_WAIT | DDBLT_KEYSRC, &blitfx) != DD_OK)
        MessageBox(hwnd, "could not blit", " void GraphicsEngine::toPrimarySurface(LPDIRECTDRAWSURFACE7 surface)", MB_OK);
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Name: toPrimarySurface.
//
// Arguments: LPDIRECTDRAWSURFACE7: The direct draw surface that we wish to copy onto to primary surface
//
//            int x: The x coordinate of the screen to blit to.
//
//            int y: The y coordinate of the screen to blit to.
//
// Return Type: void
//
// This method is used to copy the contents of the surface specifed in the argument onto the primary surface or display at
// coordinates (x,y) on the primary surface.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GraphicsEngine::toPrimarySurface(LPDIRECTDRAWSURFACE7 surface, int x, int y)
{

    // clear out the structure and set the size field
    memset(&blitfx,0,sizeof(DDBLTFX));

    blitfx.dwSize = sizeof(DDBLTFX);

    //ddsd
    //set up surface descriptor to lock the surface
    memset(&ddsd,0,sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);

    // lock the surface so we can get the height and the width of the surface
    if(surface->Lock(NULL,&ddsd,DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL) != DD_OK)
       MessageBox(hwnd, "could not lock offscreen", " void GraphicsEngine::toPrimarySurface(LPDIRECTDRAWSURFACE7 surface, int x, int y)", MB_OK);

    int width = ddsd.dwWidth;
    int height = ddsd.dwHeight;  //unsigned long

    surface->Unlock(NULL);  // unlock surface

    // define the parameters of the destination rectangle
    RECT dest;

    dest.top = x;
    dest.left = y;
    dest.bottom = height + x;
    dest.right = width + y;


    //ready to blit to surface
    if(primary->Blt(&dest, surface, NULL,DDBLT_WAIT | DDBLT_KEYSRC , &blitfx) != DD_OK)
        MessageBox(hwnd, "could not blit", " void GraphicsEngine::toPrimarySurface(LPDIRECTDRAWSURFACE7 surface, int x, int y)", MB_OK);

}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Name: toPrimarySurface.
//
// Arguments: LPDIRECTDRAWSURFACE7: The direct draw surface that we wish to copy onto to secondary surface
//
// Return Type: void
//
// This method is used to copy the contents of the surface specifed in the argument onto the secondary surface at
// coordinates (0,0).
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GraphicsEngine::toSecondarySurface(LPDIRECTDRAWSURFACE7 surface)
{

    // clear out the structure and set the size field
    memset(&blitfx,0,sizeof(DDBLTFX));

    blitfx.dwSize = sizeof(DDBLTFX);

    //ddsd
    //set up surface descriptor to lock the surface
    memset(&ddsd,0,sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);

    // lock the surface so we can get the height and the width of the surface
    if(surface->Lock(NULL,&ddsd,DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL) != DD_OK)
       MessageBox(hwnd, "could not lock offscreen", " void GraphicsEngine::toSecondarySurface(LPDIRECTDRAWSURFACE7 surface)", MB_OK);

    int width = ddsd.dwWidth;
    int height = ddsd.dwHeight;  //unsigned long

    surface->Unlock(NULL);  // unlock surface

    // define the parameters of the destination rectangle
    RECT dest;

    dest.top = 0;
    dest.left = 0;
    dest.bottom = height;
    dest.right = width;

    //ready to blit to surface
    if(secondary->Blt(&dest, surface, NULL, DDBLT_KEYSRC | DDBLT_WAIT, &blitfx) != DD_OK)
        MessageBox(hwnd, "could not blit to secondary", " void GraphicsEngine::toSecondarySurface(LPDIRECTDRAWSURFACE7 surface)", MB_OK);

}




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Name: toPrimarySurface.
//
// Arguments: LPDIRECTDRAWSURFACE7: The direct draw surface that we wish to copy onto to secondary surface
//
//            int x: The x coordinate of the surface to blit to.
//
//            int y: The y coordinate of the surface to blit to.
//
// Return Type: void
//
// This method is used to copy the contents of the surface specifed in the argument onto the secondary surface at
// coordinates (x,y) on the secondary surface.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GraphicsEngine::toSecondarySurface(LPDIRECTDRAWSURFACE7 surface, int x, int y)
{
    // clear out the structure and set the size field
    memset(&blitfx,0,sizeof(DDBLTFX));

    blitfx.dwSize = sizeof(DDBLTFX);

    //ddsd
    //set up surface descriptor to lock the surface
    memset(&ddsd,0,sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);

    // lock the surface so we can get the height and the width of the surface
    if(surface->Lock(NULL,&ddsd,DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL) != DD_OK)
       MessageBox(hwnd, "could not lock offscreen", " void GraphicsEngine::toSecondarySurface(LPDIRECTDRAWSURFACE7 surface, int x, int y)", MB_OK);

    int width = ddsd.dwWidth;
    int height = ddsd.dwHeight;  //unsigned long

    surface->Unlock(NULL);  // unlock surface

    // define the parameters of the destination rectangle
    RECT dest;

    dest.top = x;
    dest.left = y;
    dest.bottom = height + x;
    dest.right = width + y;


    //ready to blit to surface
    if(secondary->Blt(&dest, surface, NULL,DDBLT_WAIT | DDBLT_KEYSRC, &blitfx) != DD_OK)
        MessageBox(hwnd, "could not blit to secondary", " void GraphicsEngine::toSecondarySurface(LPDIRECTDRAWSURFACE7 surface, int x, int y)", MB_OK);


}






void GraphicsEngine::toWorkbench(LPDIRECTDRAWSURFACE7 surface)
{

    // clear out the structure and set the size field
    memset(&blitfx,0,sizeof(DDBLTFX));

    blitfx.dwSize = sizeof(DDBLTFX);

    //ddsd
    //set up surface descriptor to lock the surface
    memset(&ddsd,0,sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);

    // lock the surface so we can get the height and the width of the surface
    if(surface->Lock(NULL,&ddsd,DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL) != DD_OK)
       MessageBox(hwnd, "could not lock offscreen", " void GraphicsEngine::toWorkbench(LPDIRECTDRAWSURFACE7 surface)", MB_OK);

    int width = ddsd.dwWidth;
    int height = ddsd.dwHeight;  //unsigned long

    surface->Unlock(NULL);  // unlock surface

    // define the parameters of the destination rectangle
    RECT dest;

    dest.top = 0;
    dest.left = 0;
    dest.bottom = height;
    dest.right = width;

    //ready to blit to surface
    if(workBench->Blt(&dest, surface, NULL,DDBLT_WAIT | DDBLT_KEYSRC, &blitfx) != DD_OK)
        MessageBox(hwnd, "could not blit ", " void GraphicsEngine::toWorkbench(LPDIRECTDRAWSURFACE7 surface) ", MB_OK);

}


void GraphicsEngine::toWorkbench(LPDIRECTDRAWSURFACE7 surface, int x, int y)
{
    // clear out the structure and set the size field
    memset(&blitfx,0,sizeof(DDBLTFX));

    blitfx.dwSize = sizeof(DDBLTFX);

    //ddsd
    //set up surface descriptor to lock the surface
    memset(&ddsd,0,sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);

    // lock the surface so we can get the height and the width of the surface
    if(surface->Lock(NULL,&ddsd,DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL) != DD_OK)
       MessageBox(hwnd, "could not lock offscreen", " void GraphicsEngine::toWorkbench(LPDIRECTDRAWSURFACE7 surface, int x, int y) ", MB_OK);

    int width = ddsd.dwWidth;
    int height = ddsd.dwHeight;  //unsigned long

    surface->Unlock(NULL);  // unlock surface

    // define the parameters of the destination rectangle
    RECT dest;

    dest.top = y;
    dest.left = x;
    dest.bottom = height + y;
    dest.right = width + x;


    //ready to blit to surface
    if(workBench->Blt(&dest, surface, NULL,DDBLT_WAIT | DDBLT_KEYSRC, &blitfx) != DD_OK)
        MessageBox(hwnd, "could not blit to workbench", " void GraphicsEngine::toWorkbench(LPDIRECTDRAWSURFACE7 surface, int x, int y) ", MB_OK);


}










/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Name: blitToSurface
//
// Arguments: LPDIRECTDRAWSURFACE7 source: A pointer to the surface we want to copy from.
//
//            LPDIRECTDRAWSURFACE7 destination: A pointer to the surface we want to copy to.
//
//            RECT sourceRect: The rectangular area of the source we want to copy from.
//
//            RECT destRect: The rectangluar area of the destination we wish to copy to.
//
// Return type: void
//
// This methos is used to copy a specified part to the source surface onto a specified part of the destination surface.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GraphicsEngine::blitToSurface(LPDIRECTDRAWSURFACE7 source, LPDIRECTDRAWSURFACE7 destination, RECT sourceRect, RECT destRect)
{

   if(destination->Blt(&destRect, source, &sourceRect, DDBLT_KEYSRC | DDBLT_WAIT, NULL) != DD_OK)
   {
      MessageBox(hwnd, "could not blit, blitToSurface, ", " void GraphicsEngine::blitToSurface(LPDIRECTDRAWSURFACE7 source, LPDIRECTDRAWSURFACE7 destination, RECT sourceRect, RECT destRect)", MB_OK);

      return;
   }   

}


void GraphicsEngine::toDestinationSurface(LPDIRECTDRAWSURFACE7 source, LPDIRECTDRAWSURFACE7 destination)
{


    // lock the surface so we can get the height and the width of the surface
    if(source->Lock(NULL,&ddsd,DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL) != DD_OK)
       MessageBox(hwnd, "could not lock offscreen", " void GraphicsEngine::toDestinationSurface(LPDIRECTDRAWSURFACE7 source, LPDIRECTDRAWSURFACE7 destination)", MB_OK);

    int width = ddsd.dwWidth;
    int height = ddsd.dwHeight;  //unsigned long

    source->Unlock(NULL);  // unlock surface

    // define the parameters of the destination rectangle
    RECT dest;

    dest.top = 0;
    dest.left = 0;
    dest.bottom = height + dest.top;
    dest.right = width + dest.left;


    //ready to blit to surface
    if(destination->Blt(&dest, source, NULL,DDBLT_WAIT | DDBLT_KEYSRC, &blitfx) != DD_OK)
        MessageBox(hwnd, "could not blit to secondary", " void GraphicsEngine::toDestinationSurface(LPDIRECTDRAWSURFACE7 source, LPDIRECTDRAWSURFACE7 destination)", MB_OK);

}        



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Name: fillPrimary
//
// Arguments: int red: The value of the red component of the colour to fill with.
//
//            int green: The value of the green component of the colour to fill with.
//
//            int blue: The value of the red component of the colour to fill with.
//
// Return type: void.
//
// this method is used to fill the screen with the specified colour.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GraphicsEngine::fillPrimary(int red, int green, int blue)
{

        if(red < 0 || red > 256 || green < 0 || green > 256 || blue < 0 || blue > 256)
        {
            MessageBox(hwnd, "red, green and blue must be >= 0 and <= 255", " void GraphicsEngine::fillPrimary(int red, int green, int blue)", MB_OK);

            return;
        }

	// clear out blit structure
	memset(&blitfx,0,sizeof(DDBLTFX));

	blitfx.dwSize = sizeof(DDBLTFX);

	// set the dwfillcolor to the desired color
        if(bitsPerPixel == 16)
	   blitfx.dwFillColor = RGB16BIT565(red,green,blue);   // for 16- bit
        else
           blitfx.dwFillColor = RGB24BIT(0,red,green,blue);    // for 24 and 32 bit

	// now fill
	if(primary->Blt(NULL,NULL,NULL, DDBLT_COLORFILL | DDBLT_WAIT, &blitfx) != MB_OK)
		MessageBox(hwnd, "could not fill primary surface", " void GraphicsEngine::fillPrimary(int red, int green, int blue)", MB_OK);
}





/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Name: fillPrimary
//
// Arguments: int red: The value of the red component of the colour to fill with.
//
//            int green: The value of the green component of the colour to fill with.
//
//            int blue: The value of the red component of the colour to fill with.
//
//            RECT area: The area of the screen to fill.
//
// Return type: void.
//
// this method is used to fill the screen area with the specified colour.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GraphicsEngine::fillPrimary(int red, int green, int blue, RECT area)
{

	if(red < 0 || red > 256 || green < 0 || green > 256 || blue < 0 || blue > 256)
        {
            MessageBox(hwnd, "red, green and blue must be >= 0 and <= 255", " void GraphicsEngine::fillPrimary(int red, int green, int blue, RECT area)", MB_OK);

            return;
        }
    
	// clear out blit structure
	memset(&blitfx,0,sizeof(DDBLTFX));

	blitfx.dwSize = sizeof(DDBLTFX);

	// set the dwfillcolor to the desired color
        if(bitsPerPixel == 16)
	   blitfx.dwFillColor = RGB16BIT565(red,green,blue);
        else
           blitfx.dwFillColor = RGB24BIT(0,red,green,blue);    // for 24 and 32 bit

	// now fill area
	if(primary->Blt(&area,NULL,NULL, DDBLT_COLORFILL | DDBLT_WAIT, &blitfx) != MB_OK)
		MessageBox(hwnd, "could not fill primary surface", " void GraphicsEngine::fillPrimary(int red, int green, int blue, RECT area)", MB_OK);

}




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Name: fillSecondary
//
// Arguments: int red: The value of the red component of the colour to fill with.
//
//            int green: The value of the green component of the colour to fill with.
//
//            int blue: The value of the red component of the colour to fill with.
//
// Return type: void.
//
// this method is used to fill the secondary surface or backbuffer with the specified colour.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GraphicsEngine::fillSecondary(int red, int green, int blue)
{

	if(red < 0 || red > 256 || green < 0 || green > 256 || blue < 0 || blue > 256)
        {
           MessageBox(hwnd, "red, green and blue must be >= 0 and <= 255", " void GraphicsEngine::fillSecondary(int red, int green, int blue)", MB_OK);

           return;
        }

	// clear out blit structure
	memset(&blitfx,0,sizeof(DDBLTFX));

	blitfx.dwSize = sizeof(DDBLTFX);

	// set the dwfillcolor to the desired color
        if(bitsPerPixel == 16)
	   blitfx.dwFillColor = RGB16BIT565(red,green,blue);
        else
           blitfx.dwFillColor = RGB24BIT(0,red,green,blue);    // for 24 and 32 bit

        // now fill
	if(secondary->Blt(NULL,NULL,NULL, DDBLT_COLORFILL | DDBLT_WAIT, &blitfx) != MB_OK)
		MessageBox(hwnd, "could not fill secondary surface", " void GraphicsEngine::fillSecondary(int red, int green, int blue)", MB_OK);
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Name: fillSecondary
//
// Arguments: int red: The value of the red component of the colour to fill with.
//
//            int green: The value of the green component of the colour to fill with.
//
//            int blue: The value of the red component of the colour to fill with.
//
//            RECT area: The area of the screen to fill.
//
// Return type: void.
//
// this method is used to fill the secondary surface or backbuffer area with the specified colour.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GraphicsEngine::fillSecondary(int red,int green, int blue, RECT area)
{

        if(red < 0 || red > 256 || green < 0 || green > 256 || blue < 0 || blue > 256)
        {
            MessageBox(hwnd, "red, green and blue must be >= 0 and <= 255", " void GraphicsEngine::fillSecondary(int red,int green, int blue, RECT area)", MB_OK);

            return;
        }

	// clear out blit structure
	memset(&blitfx,0,sizeof(DDBLTFX));

	blitfx.dwSize = sizeof(DDBLTFX);

	// set the dwfillcolor to the desired color
        if(bitsPerPixel == 16)
	   blitfx.dwFillColor = RGB16BIT565(red,green,blue);
        else
           blitfx.dwFillColor = RGB24BIT(0,red,green,blue);    // for 24 and 32 bit


        // now fill area
	if(secondary->Blt(&area,NULL,NULL, DDBLT_COLORFILL | DDBLT_WAIT, &blitfx) != MB_OK)
		MessageBox(hwnd, "could not fill primary surface", " void GraphicsEngine::fillSecondary(int red,int green, int blue, RECT area)", MB_OK);

}






 void GraphicsEngine::fillWorkbench(int red, int green, int blue)
{

        if(red < 0 || red > 256 || green < 0 || green > 256 || blue < 0 || blue > 256)
        {
            MessageBox(hwnd, "red, green and blue must be >= 0 and <= 255", " void GraphicsEngine::fillWorkbench(int red, int green, int blue)", MB_OK);

            return;
        }

	// clear out blit structure
	memset(&blitfx,0,sizeof(DDBLTFX));

	blitfx.dwSize = sizeof(DDBLTFX);

	// set the dwfillcolor to the desired color
        if(bitsPerPixel == 16)
	   blitfx.dwFillColor = RGB16BIT565(red,green,blue);   // for 16- bit
        else
           blitfx.dwFillColor = RGB24BIT(0,red,green,blue);    // for 24 and 32 bit

	// now fill
	if(workBench->Blt(NULL,NULL,NULL, DDBLT_COLORFILL | DDBLT_WAIT, &blitfx) != MB_OK)
		MessageBox(hwnd, "could not fill primary surface", " void GraphicsEngine::fillWorkbench(int red, int green, int blue)", MB_OK);
}




void GraphicsEngine::fillWorkbench(int red,int green, int blue, RECT area)
{

        if(red < 0 || red > 256 || green < 0 || green > 256 || blue < 0 || blue > 256)
        {
            MessageBox(hwnd, "red, green and blue must be >= 0 and <= 255", " void GraphicsEngine::fillWorkbench(int red,int green, int blue, RECT area)", MB_OK);

            return;
        }

	// clear out blit structure
	memset(&blitfx,0,sizeof(DDBLTFX));

	blitfx.dwSize = sizeof(DDBLTFX);

	// set the dwfillcolor to the desired color
        if(bitsPerPixel == 16)
	   blitfx.dwFillColor = RGB16BIT565(red,green,blue);
        else
           blitfx.dwFillColor = RGB24BIT(0,red,green,blue);    // for 24 and 32 bit


        // now fill area
	if(workBench->Blt(&area,NULL,NULL, DDBLT_COLORFILL | DDBLT_WAIT, &blitfx) != MB_OK)
		MessageBox(hwnd, "could not fill primary surface", " void GraphicsEngine::fillWorkbench(int red,int green, int blue, RECT area) ", MB_OK);

}




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Name: fillSurface
//
// Arguments: LPDIRECTDRAWSURFACE7 surface: A pointer to the surface that we wish to fill.
//
//            int red: The value of the red component of the colour to fill with.
//
//            int green: The value of the green component of the colour to fill with.
//
//            int blue: The value of the red component of the colour to fill with.
//
// Return type: void.
//
// This method is used to fill the surface given as a parameter with the specified colour.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GraphicsEngine::fillSurface(LPDIRECTDRAWSURFACE7 surface, int red, int green, int blue)
{
    if(red < 0 || red > 256 || green < 0 || green > 256 || blue < 0 || blue > 256)
    {
		MessageBox(hwnd, "red, green and blue must be >= 0 and <= 255", "void GraphicsEngine::fillSurface(LPDIRECTDRAWSURFACE7 surface, int red, int green, int blue) ", MB_OK);

		return;
    }

	// clear out blit structure
	memset(&blitfx,0,sizeof(DDBLTFX));

	blitfx.dwSize = sizeof(DDBLTFX);

	// set the dwfillcolor to the desired color
        if(bitsPerPixel == 16)
	   blitfx.dwFillColor = RGB16BIT565(red,green,blue);
        else
           blitfx.dwFillColor = RGB24BIT(0,red,green,blue);    // for 24 and 32 bit


        if(surface->Blt(NULL,NULL,NULL, DDBLT_COLORFILL | DDBLT_WAIT, &blitfx) != MB_OK)
		MessageBox(hwnd, "could not fill secondary surface", " void GraphicsEngine::fillSurface(LPDIRECTDRAWSURFACE7 surface, int red, int green, int blue)", MB_OK);
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Name: fillSurface
//
// Arguments: LPDIRECTDRAWSURFACE7 surface: A pointer to the surface that we wish to fill.
//
//            int red: The value of the red component of the colour to fill with.
//
//            int green: The value of the green component of the colour to fill with.
//
//            int blue: The value of the red component of the colour to fill with.
//
//            RECT area: The area of the screen to fill.
//
// Return type: void.
//
// This method is used to fill the area on the surface given as a parameter with the specified colour.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GraphicsEngine::fillSurface(LPDIRECTDRAWSURFACE7 surface, int red, int green, int blue, RECT area)
{

        if(red < 0 || red > 256 || green < 0 || green > 256 || blue < 0 || blue > 256)
        {
           MessageBox(hwnd, "red, green and blue must be >= 0 and <= 255", " void GraphicsEngine::fillSurface(LPDIRECTDRAWSURFACE7 surface, int red, int green, int blue, RECT area)", MB_OK);

           return;
        }

	// clear out blit structure
	memset(&blitfx,0,sizeof(DDBLTFX));

	blitfx.dwSize = sizeof(DDBLTFX);

	// set the dwfillcolor to the desired color
        if(bitsPerPixel == 16)
	   blitfx.dwFillColor = RGB16BIT565(red,green,blue);
        else
           blitfx.dwFillColor = RGB24BIT(0,red,green,blue);    // for 24 and 32 bit

        //now fill
	if(secondary->Blt(&area,NULL,NULL, DDBLT_COLORFILL | DDBLT_WAIT, &blitfx) != MB_OK)
            MessageBox(hwnd, "could not fill primary surface", " void GraphicsEngine::fillSurface(LPDIRECTDRAWSURFACE7 surface, int red, int green, int blue, RECT area)", MB_OK);

}




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Name: flipSurface.
//
// Arguments: none.
//
// Return Type: void.
//
// This method is used to flip the primary and secondary surfaces.
// When this method is called, th contents of the secondary surface are immediately copied to the primary surface.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GraphicsEngine::flipSurfaces()
{
	while(primary->Flip(NULL, DDFLIP_WAIT) != DD_OK);
}











/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Name: createRectangle.
//
// Arguments: int top: The position of the top of the rectangle in pixels.
//
//            int left: The position of the left of the rectangle in pixels.
//
//            int bottom: The position of the bottom of the rectangle in pixels.
//
//            int right: The postion of the right of the rectangle in pixels.
//
// return Type: A RECT. This is a struct that contains the demensions of a rectangle.
//
// This method returns a rectangle having the dimensions specified in the parameters.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
RECT GraphicsEngine::createRectangle(int top, int left, int bottom, int right)
{
     RECT rectangle;

     rectangle.top = top;
     rectangle.left = left;
     rectangle.bottom = bottom;
     rectangle.right = right;

     return rectangle;
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Name: drawTextPrimary.
//
// Arguments: char* text: This is a character string of the text we wish to display on the primary surface.
//
//            int x: The x position to display the text.
//
//            int y: the y position to display the text.
//
//            int red: The red component of the colour of the text.
//
//            int green: The green component of the colour of the text.
//
//            int blue: The blue component of the colour of the text.
//
// Return type: void.
//
// This method is used to display character strings on the display.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GraphicsEngine::drawTextPrimary(char *text, int x, int y, int red, int green, int blue)
{
     // this function draws the sent text on the sent surface


     HDC xdc; // the working dc

     // get the dc from surface
     if(primary->GetDC(&xdc)!=DD_OK)
     {
        MessageBox(hwnd, "Could not get the DC for drawing text", " void GraphicsEngine::drawTextPrimary(char *text, int x, int y, int red, int green, int blue)", MB_OK);

        return;
     }

     // set the colors for the text up
     SetTextColor(xdc,RGB16BIT565(red,green,blue));

     // set background mode to transparent so black isn't copied
     SetBkMode(xdc, TRANSPARENT);

     // draw the text
     TextOut(xdc,x,y,text,strlen(text));

     // release the dc
     primary->ReleaseDC(xdc);

}



 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Name: drawTextSeondary.
//
// Arguments: char* text: This is a character string of the text we wish to draw on the secondary surface.
//
//            int x: The x position to display the text.
//
//            int y: the y position to display the text.
//
//            int red: The red component of the colour of the text.
//
//            int green: The green component of the colour of the text.
//
//            int blue: The blue component of the colour of the text.
//
// Return type: void.
//
// This method is used to draw character strings on the secondary surface.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GraphicsEngine::drawTextSecondary(char* text, int x, int y, int red, int green, int blue)
{
          // this function draws the sent text on the sent surface
   

     HDC xdc; // the working dc

     // get the dc from surface
     if(secondary->GetDC(&xdc)!=DD_OK)
     {
        MessageBox(hwnd, "Could not get the DC for drawing text", " void GraphicsEngine::drawTextSecondary(char* text, int x, int y, int red, int green, int blue)", MB_OK);

        return;
     }

     // set the colors for the text up
     SetTextColor(xdc,RGB16BIT565(red,green,blue));

     // set background mode to transparent so black isn't copied
     SetBkMode(xdc, TRANSPARENT);

     // draw the text a
     TextOut(xdc,x,y,text,strlen(text));

     // release the dc
     secondary->ReleaseDC(xdc);
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// The next Three methods are used for timming.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
DWORD GraphicsEngine::getClock(void)
{
     // this function returns the current tick count

    // return time
    return(GetTickCount());

}
DWORD GraphicsEngine::startClock(void)
{
    // this function starts the clock, that is, saves the current
    // count, use in conjunction with waitClock()

    return(startClockCount = getClock());

}
DWORD GraphicsEngine::waitClock(DWORD count)
{
    // this function is used to wait for a specific number of clicks
    // since the call to startClock

    while((getClock() - startClockCount) < count);
    return(getClock());

}




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Accessor functions.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LPDIRECTDRAWSURFACE7 GraphicsEngine::getPrimarySurface()
{
    return this->primary;
}

LPDIRECTDRAWSURFACE7 GraphicsEngine::getSecondarySurface()
{
    return this->secondary;
}

LPDIRECTDRAWSURFACE7 GraphicsEngine::getWorkbench()
{
    return this->workBench;
}

int GraphicsEngine::getScreenWidth()
{
    return this->screenWidth;
}

int GraphicsEngine::getScreenHeight()
{
    return this->screenHeight;
}

int GraphicsEngine::getBitsPerPixel()
{
    return this->bitsPerPixel;
}

HWND GraphicsEngine::getWindowHandle()
{
    return this->hwnd;
}





















