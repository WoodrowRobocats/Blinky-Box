#include <bitswap.h>
#include <chipsets.h>
#include <color.h>
#include <colorpalettes.h>
#include <colorutils.h>
#include <controller.h>
#include <cpp_compat.h>
#include <dmx.h>
#include <FastLED.h>
#include <fastled_config.h>
#include <fastled_delay.h>
#include <fastled_progmem.h>
#include <fastpin.h>
#include <fastspi.h>
#include <fastspi_bitbang.h>
#include <fastspi_dma.h>
#include <fastspi_nop.h>
#include <fastspi_ref.h>
#include <fastspi_types.h>
#include <hsv2rgb.h>
#include <led_sysdefs.h>
#include <lib8tion.h>
#include <noise.h>
#include <pixelset.h>
#include <pixeltypes.h>
#include <platforms.h>
#include <power_mgt.h>

//Include the image set data
#include "FRC.h"
#include "pac.h"
#include "pacman.h"
#include "banners.h"

//Debug messages takes about 2K flash and 300B RAM
//#define DEBUG_MESSAGES

#define NUM_LEDS 256
#define DATA_PIN 6

#define DISPLAY_WIDTH  16
#define DISPLAY_HEIGHT 16

#define YES_FLIP_X         true
#define NO_FLIP_X          false
#define YES_FLIP_Y         true
#define NO_FLIP_Y          false
#define YES_FLIP_ALTERNATE true
#define NO_FLIP_ALTERNATE  false
#define YES_SWAP_XY        true
#define NO_SWAP_XY         false

CRGB leds[NUM_LEDS];

byte ImageSets = 4;
const unsigned int ImageSetList[] = {
  &frc_ImageInfo,
  &pac_ImageInfo,
  &pacman_ImageInfo,
  &banners_ImageInfo
};

#ifdef DEBUG_MESSAGES
char Buffer[128];
#endif

unsigned int ImageSetPointer;           //Points to current_ImageInfo structure
unsigned int ImageSetCount;             //Number of images in current set
unsigned int ImageSetPaletteSize;       //Size of current image set palette
unsigned int ImageCycleCount;           //Default number of images to cycle for the current set
unsigned int ImageSetDelay;             //Delay between each image in the current set

unsigned int SetPalettePointer;         //Pointer to current set palette
unsigned int SetImageTablePointer;      //Pointer to current image pointer table

byte ImageSetIndex = 0;                 //Used to cycle through multiple image sets
unsigned int CurrentDataPointer;        //Pointer to current image data entry
byte CurrentDecompressionCount;         //Counter to track how many pixels of the current color have been extracted

byte CurrentImageWidth;
byte CurrentImageHeight;

CRGB CurrentPixelColor;

#define PIXEL_FROM_PALETTE                  0
#define PIXEL_FROM_INDEX_GLOBAL             1
#define PIXEL_FROM_INDEX_RANDOM_FOREGROUND  2
#define PIXEL_FROM_INDEX_RANDOM_BACKGROUND  3
#define PIXEL_FROM_INDEX_CYCLE_FOREGROUND   4
#define PIXEL_FROM_INDEX_CYCLE_BACKGROUND   5

byte PixelColorType = PIXEL_FROM_PALETTE;

CRGB GlobalForegroundColor    =  CRGB(255, 255, 255);
CRGB GlobalBackgroundColor    =  CRGB(255,   0,   0);

void DebugMessage(char* Message)
{
  Serial.print(Message);  
}

void setup() 
{
  // initialize serial communications at 9600 bps:
  Serial.begin(115200);

  //Initialize the LED library
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
}

//This is a fairly advanced method, using function pointers, but is VERY useful here !!!
void (* GetPixelColor[6])(byte PixelIndex) = { GetPixelColorFromPalette, GetPixelColorFromGlobal, GetPixelColorFromIndexRandomForeground, GetPixelColorFromIndexRandomBackground, GetPixelColorFromIndexCycleForeground, GetPixelColorFromIndexCycleBackground};

void GetPixelColorFromPalette(byte PixelIndex)
{
  unsigned int PaletteColor;
  
  PaletteColor = pgm_read_word(SetPalettePointer + (2 * PixelIndex));
  CurrentPixelColor.red   = (PaletteColor >> 11) << 3;
  CurrentPixelColor.green = ((PaletteColor >> 5) & 0x3f) << 2;
  CurrentPixelColor.blue  = (PaletteColor & 0x1f) << 3;
}

void GetPixelColorFromGlobal(byte PixelIndex)
{
  if (PixelIndex == 0)
    CurrentPixelColor  = GlobalBackgroundColor;
  else
    CurrentPixelColor  = GlobalForegroundColor;
}

void GetPixelColorFromIndexRandomForeground(byte PixelIndex)
{
  if (PixelIndex == 0)
    CurrentPixelColor  = GlobalBackgroundColor;
  else
    CurrentPixelColor   = CRGB(random(256), random(256), random(256));
}

void GetPixelColorFromIndexRandomBackground(byte PixelIndex)
{
  if (PixelIndex == 0)
    CurrentPixelColor   = CRGB(random(256), random(256), random(256));
  else
    CurrentPixelColor  = GlobalForegroundColor;
}

void GetPixelColorFromIndexCycleForeground(byte PixelIndex)
{
  if (PixelIndex == 0)
    CurrentPixelColor  = GlobalBackgroundColor;
  else
    CurrentPixelColor   = CRGB(random(256), random(256), random(256));

  CycleColor(GlobalForegroundColor);

  #ifdef DEBUG_MESSAGES
  DebugMessage("Cycle foreground ");
  #endif
}

void GetPixelColorFromIndexCycleBackground(byte PixelIndex)
{
  if (PixelIndex == 0)
    CurrentPixelColor   = CRGB(random(256), random(256), random(256));
  else
    CurrentPixelColor  = GlobalForegroundColor;

  CycleColor(GlobalBackgroundColor);

  #ifdef DEBUG_MESSAGES
  DebugMessage("Cycle background ");
  #endif
}

unsigned long GetNextPixel()
{
  static byte Count;
  byte PaletteIndex;

  //Count how many pixels we have returned for the current entry
  //If CurrentDecompressionCount == 0 then start of compression set, so retrieve count and color
  if (CurrentDecompressionCount == 0)
  {
    PaletteIndex = pgm_read_byte(CurrentDataPointer++);
    Count        = pgm_read_byte(CurrentDataPointer++);
  }
  //Determine how to use the pixel information and set the global current color values
  GetPixelColor[PixelColorType](PaletteIndex);
  //Have we done enough of this color? Note, count is really zero indexed, i.e. 0 gives 1 pixel, 1 gives 2 pixels etc... since no need to send 0 pixels
  if (CurrentDecompressionCount == Count)
    CurrentDecompressionCount = 0;
  else
    CurrentDecompressionCount++;

  /*
  #ifdef DEBUG_MESSAGES
  sprintf(Buffer, "R = %3d, G = %3d, B = %3d\n", CurrentPixelColor.red, CurrentPixelColor.green, CurrentPixelColor.blue);
  DebugMessage(Buffer);
  #endif
  */
}

byte GetImageWidth(unsigned int ImageSetPointer, byte ImageIndex)
{
  unsigned int TablePointer;
  unsigned int ImagePointer;
  byte ImageWidth;
  
  TablePointer = pgm_read_word(ImageSetPointer + 10);
  ImagePointer = pgm_read_word(TablePointer + (2*ImageIndex));
  ImageWidth = pgm_read_byte(ImagePointer + 1);
  return ImageWidth;
}

void RetrieveSetInfo(unsigned int ImageSetPointer)
{
  //Setup image index pointers etc...
  ImageSetCount        = pgm_read_word(ImageSetPointer);
  ImageSetPaletteSize  = pgm_read_word(ImageSetPointer + 2);
  ImageSetDelay        = pgm_read_word(ImageSetPointer + 4);
  ImageCycleCount      = pgm_read_word(ImageSetPointer + 6);
  SetPalettePointer    = pgm_read_word(ImageSetPointer + 8);
  SetImageTablePointer = pgm_read_word(ImageSetPointer + 10);

  #ifdef DEBUG_MESSAGES
  sprintf(Buffer, "Image set pointer =  %d \n", ImageSetPointer);
  DebugMessage(Buffer);
  sprintf(Buffer, "Image set count =  %d images\n", ImageSetCount);
  DebugMessage(Buffer);
  sprintf(Buffer, "Image palette set has %d entries\n",ImageSetPaletteSize);
  DebugMessage(Buffer);
  sprintf(Buffer, "Image delay = %d ms\n", ImageSetDelay);
  DebugMessage(Buffer);
  sprintf(Buffer, "Image set cycles = %d images\n", ImageSetCount);
  DebugMessage(Buffer);
  #endif
}

CRGB CycleColor(CRGB Color)
{
  Color.red = Color.red + 7;
  Color.green = Color.green + 11;
  Color.blue = Color.blue + 15;
}

void SkipPixels(byte PixelsToSkip)
{
  byte SkipCount;
  
  //The following can be made faster, but not really necessary
  for (SkipCount = 0; SkipCount < PixelsToSkip; SkipCount++)
    GetNextPixel();
}

void SeekPixel(unsigned int ImagePointer, byte XOffset, byte YOffset)
{
  byte PixelsToSkip;

  //Retrieve the source image dimensions
  CurrentImageHeight = pgm_read_byte(ImagePointer);
  CurrentImageWidth = pgm_read_byte(ImagePointer + 1);

  //Get ready to start scanning through the source image
  CurrentDataPointer = ImagePointer + 2;  //Point to the start of the current image data
  CurrentDecompressionCount = 0;          //Make sure compression alignment is reset
  PixelsToSkip = (CurrentImageWidth * YOffset) + XOffset;
  SkipPixels(PixelsToSkip);
  //At this point in time the current colors should be the first desired pixel and current data pointer and compression count should be ready for subsequent pixels
}

void RenderImage(byte ImageIndex, byte XOffset, byte YOffset, bool FlipX, bool FlipY, bool SwapXY, bool FlipAlternateRows)
{
  byte x;
  byte y;
  byte StoreAtX;
  byte StoreAtY;
  byte Temp;
  byte RowFlip = 0;
  byte PixelsToSkip;
  unsigned int Index;
  unsigned int ImagePointer;

  //Source image can be larger than the display. 
  //This allows panning through a larger image to give a scrolling effect.
  //Using the flip allows mirroring the image to scroll in the opposite direction
  //e.g. Pacman & ghosts going in both directions

  ImagePointer = pgm_read_word(SetImageTablePointer + (2*ImageIndex));

  //Scan through the compressed image to find the first pixel in the source image
  SeekPixel(ImagePointer, XOffset, YOffset);

  for (y = 0; y < DISPLAY_HEIGHT; y ++)
  {
    for(x = 0; x < DISPLAY_WIDTH; x++)
    {
      GetNextPixel();
      //After the call above we should have CurrentPixelColor (red, green & blue)
      //These colors now need to be stored in the LED strip array as we retrieve them

      //Allow flipping the image in x and/or y
      if (FlipX)
        StoreAtX = DISPLAY_WIDTH - x - 1;
      else
        StoreAtX = x;
        
      if (FlipY)
        StoreAtY = DISPLAY_HEIGHT - y - 1;
      else
        StoreAtY = y;

      //Allow swapping x & y (allows simple rotation in conjunction with flip x/y)
      if (SwapXY)
      {
        Temp = StoreAtX;
        StoreAtX = StoreAtY;
        StoreAtY = Temp;
      }

      //If the LED strip is snaked back and forth then every other row needs to fill from the opposite side
      if (RowFlip)
        StoreAtX = DISPLAY_WIDTH - StoreAtX - 1;

      //Calculate which LED this really is now
      Index = (StoreAtY * DISPLAY_WIDTH) + StoreAtX;

      //And store the color
      leds[Index] = CurrentPixelColor;
      
    }
    //If flipping rows then do it
    if (FlipAlternateRows)
      RowFlip = (RowFlip + 1) & 1;
    //If necessary skip pixels to align at the start column of the next row in the source image
    PixelsToSkip = CurrentImageWidth - DISPLAY_WIDTH;
    SkipPixels(PixelsToSkip);
  }
  //Now update the LEDs
  FastLED.show();
  //Is this ncessary?
  digitalWrite(LED_BUILTIN, LOW);
}

void CycleImageSet(unsigned int ImageSetPointer, unsigned int Cycles, byte XOffset, byte YOffset, bool FlipX, bool FlipY, bool SwapXY, bool FlipAlternateRows)
{
  unsigned int CurrentCycle;
  unsigned int CycleCount;
  byte CurrentImageIndex = 0;
  
  RetrieveSetInfo(ImageSetPointer);

  //Set cycles coming in to 0 to use default from tables
  if (Cycles == 0)
    Cycles = ImageCycleCount;

  for (CycleCount = 0; CycleCount < Cycles; CycleCount++)
  {
    #ifdef DEBUG_MESSAGES
    sprintf(Buffer, "Rendering image %d\n",CurrentImageIndex);
    DebugMessage(Buffer);
    #endif
    
    RenderImage(CurrentImageIndex, XOffset, YOffset, FlipX, FlipY, SwapXY, FlipAlternateRows);

    delay(ImageSetDelay);
    
    CurrentImageIndex++;
    if (CurrentImageIndex == ImageSetCount)
      CurrentImageIndex = 0;
  }
}

void ShowImage(unsigned int SetInfo, byte ImageIndex, byte XOffset, byte YOffset, bool FlipX, bool FlipY, bool SwapXY, bool FlipAlternateRows)
{
  RetrieveSetInfo(SetInfo);
  RenderImage(ImageIndex, XOffset, YOffset, FlipX, FlipY, SwapXY, FlipAlternateRows);
}

void Demo1()
{
  //This demo simply retrieves an image from a set, in this case Mario Green from the frc set
  //Source image is 16x16 so no offsets necessary (nor allowed at the moment)
  ShowImage(frc_ImageInfo, FRC_MARIOGREEN, 0, 0, NO_FLIP_X, NO_FLIP_Y, NO_SWAP_XY, YES_FLIP_ALTERNATE);
  delay(3000);
}

void Demo2()
{
  //This demo retrieves an image from a set and effectively rotates it
  //Source image is 16x16 so no offsets necessary, so set to 0, 0 (nor allowed at the moment)
  byte loop;
  
  for (loop = 0; loop < 10; loop++)
  {
    //Base orientation
    ShowImage(frc_ImageInfo, FRC_MARIORED, 0, 0, NO_FLIP_X, NO_FLIP_Y, NO_SWAP_XY, YES_FLIP_ALTERNATE);
    delay(500);
    //Anti-clockwise 90 degrees
    ShowImage(frc_ImageInfo, FRC_MARIORED, 0, 0, NO_FLIP_X, YES_FLIP_Y, YES_SWAP_XY, YES_FLIP_ALTERNATE);
    delay(500);
    //180 degrees
    ShowImage(frc_ImageInfo, FRC_MARIORED, 0, 0, YES_FLIP_X, YES_FLIP_Y, NO_SWAP_XY, YES_FLIP_ALTERNATE);
    delay(500);
    //Clockwise 90 degrees
    ShowImage(frc_ImageInfo, FRC_MARIORED, 0, 0, YES_FLIP_X, NO_FLIP_Y, YES_SWAP_XY, YES_FLIP_ALTERNATE);
    delay(500);
  }
}

void Demo3()
{
  //This demo simply cycles through the Pacman graphic 50 times. Change "50" to "0" to use default count from the tables
  //Note, the pac man image pointers array and image set count have been manually monified to repeat the middle graphic
  //The delay has also been changed from the default to make pacman bite faster
  //Source image is 16x16 so no offsets necessary (nor allowed at the moment)
  
  CycleImageSet(pac_ImageInfo, 50, 0, 0, NO_FLIP_X, NO_FLIP_Y, NO_SWAP_XY, YES_FLIP_ALTERNATE);
}

void Demo4Part1()
{
  //Render Pacman scrolling from left to right, opening and closing his mouth
  int Position = 0;
  byte ImageIndex;
  byte IncrementCounter = 0;
  const byte Sequence[] = {PACMAN_PAC_1_BORDER, PACMAN_PAC_2_BORDER, PACMAN_PAC_3_BORDER, PACMAN_PAC_2_BORDER};//Indexes in pacman_ImagePointers
  byte CurrentWidth;
  byte TotalWidth;

  //Get the palette info and image pointers
  RetrieveSetInfo(pacman_ImageInfo);

  //Get the width of the pacman images in this sequence. All the same so any image will do
  TotalWidth = GetImageWidth(pacman_ImageInfo, Sequence[0]);

  //Render Pacman scrolling across the screen, switching between effectively 4 different images to make it look like he is eating
  while(Position < (TotalWidth - DISPLAY_WIDTH))
  {
    #ifdef DEBUG_MESSAGES
    sprintf(Buffer, "Pac position  = %d\n", Position);
    DebugMessage(Buffer);
    #endif
    //Get the width of the images in this sequence
    CurrentWidth = GetImageWidth(pacman_ImageInfo, Sequence[ImageIndex]);
    //Start from the right most section of the image and move the displayed window right
    RenderImage(Sequence[ImageIndex], (CurrentWidth - DISPLAY_WIDTH - Position), 0, NO_FLIP_X, NO_FLIP_Y, NO_SWAP_XY, YES_FLIP_ALTERNATE);
    ImageIndex = (ImageIndex + 1) & 0x03;//Count 0, 1, 2, 3, 0, 1, 2, 3
    //Move across the display every 3 pictures
    IncrementCounter++;
    if (IncrementCounter == 3)
    {
      IncrementCounter = 0;
      Position++;
    }
    delay(100);
  }
}

void Demo4Part2()
{
  //Render ghosts scrolling from left to right, swishing 'feet'
  int Position = 0;
  byte ImageIndex;
  byte IncrementCounter = 0;
  const byte Sequence[] = {PACMAN_GHOSTS1, PACMAN_GHOSTS2};
  byte CurrentWidth;
  byte TotalWidth;

  //Get the palette info and image pointers
  RetrieveSetInfo(pacman_ImageInfo);

  //Get the width of the ghosts images in this sequence. All the same so any image will do
  TotalWidth = GetImageWidth(pacman_ImageInfo, Sequence[0]);

  //Render ghosts scrolling across the screen, switching between effectively 2 different images to make it look like bottom of image is 'swishing'
  while(Position < (TotalWidth - DISPLAY_WIDTH))
  {
    #ifdef DEBUG_MESSAGES
    sprintf(Buffer, "Ghosts chase position  = %d\n", Position);
    DebugMessage(Buffer);
    #endif
    //Get the width of the images in this sequence
    CurrentWidth = GetImageWidth(pacman_ImageInfo, Sequence[ImageIndex]);
    //Start from the right most section of the image and move the displayed window left
    RenderImage(Sequence[ImageIndex], (CurrentWidth - DISPLAY_WIDTH - Position), 0, NO_FLIP_X, NO_FLIP_Y, NO_SWAP_XY, YES_FLIP_ALTERNATE);
    ImageIndex = (ImageIndex + 1) & 0x01;//Count 0, 1, 0, 1 etc...
    //Move across the display every 3 pictures
    IncrementCounter++;
    if (IncrementCounter == 3)
    {
      IncrementCounter = 0;
      Position++;
    }
    delay(100);
  }
}

void Demo4Part3()
{
  //Render 'scared' ghosts scrolling from right to left, swishing 'feet'
  int Position = 0;
  byte ImageIndex;
  byte IncrementCounter = 0;
  const byte Sequence[] = {PACMAN_GHOSTS3, PACMAN_GHOSTS4};
  byte CurrentWidth;
  byte TotalWidth;

  //Get the palette info and image pointers
  RetrieveSetInfo(pacman_ImageInfo);

  //Get the width of the ghosts images in this sequence. All the same so any image will do
  TotalWidth = GetImageWidth(pacman_ImageInfo, Sequence[0]);

  //Render ghosts scrolling across the screen, switching between effectively 2 different images to make it look like bottom of image is 'swishing'
  while(Position < (TotalWidth - DISPLAY_WIDTH))
  {
    #ifdef DEBUG_MESSAGES
    sprintf(Buffer, "Ghosts retreat position  = %d\n", Position);
    DebugMessage(Buffer);
    #endif
    //Get the width of the images in this sequence
    CurrentWidth = GetImageWidth(pacman_ImageInfo, Sequence[ImageIndex]);
    //Start from the left most section of the image and move the displayed window right
    RenderImage(Sequence[ImageIndex], Position, 0, NO_FLIP_X, NO_FLIP_Y, NO_SWAP_XY, YES_FLIP_ALTERNATE);
    ImageIndex = (ImageIndex + 1) & 0x01;//Count 0, 1, 0, 1 etc...
    //Move across the display every 3 pictures
    IncrementCounter++;
    if (IncrementCounter == 3)
    {
      IncrementCounter = 0;
      Position++;
    }
    delay(100);
  }
}

void Demo4Part4()
{
  //Render Pacman scrolling from right to left, opening and closing his mouth
  //Note, this time we are using the flip X to allow us to use the same image as going the other way
  int Position = 0;
  byte ImageIndex;
  byte IncrementCounter = 0;
  const byte Sequence[] = {PACMAN_PAC_1_BORDER, PACMAN_PAC_2_BORDER, PACMAN_PAC_3_BORDER, PACMAN_PAC_2_BORDER};//Indexes in pacman_ImagePointers
  byte CurrentWidth;
  byte TotalWidth;

  //Get the palette info and image pointers
  RetrieveSetInfo(pacman_ImageInfo);

  //Get the width of the pacman images in this sequence. All the same so any image will do
  TotalWidth = GetImageWidth(pacman_ImageInfo, Sequence[0]);

  //Render Pacman scrolling across the screen, switching between effectively 4 different images to make it look like he is eating
  while(Position < (TotalWidth - DISPLAY_WIDTH))
  {
    #ifdef DEBUG_MESSAGES
    sprintf(Buffer, "Pac chase position  = %d\n", Position);
    DebugMessage(Buffer);
    #endif
    //Get the width of the images in this sequence
    CurrentWidth = GetImageWidth(pacman_ImageInfo, Sequence[ImageIndex]);
    //Start from the right most section of the image and move the displayed window right
    RenderImage(Sequence[ImageIndex], (CurrentWidth - DISPLAY_WIDTH - Position), 0, YES_FLIP_X, NO_FLIP_Y, NO_SWAP_XY, YES_FLIP_ALTERNATE);
    ImageIndex = (ImageIndex + 1) & 0x03;//Count 0, 1, 2, 3, 0, 1, 2, 3
    //Move across the display every 3 pictures
    IncrementCounter++;
    if (IncrementCounter == 3)
    {
      IncrementCounter = 0;
      Position++;
    }
    delay(100);
  }
}

void Demo4()
{
  //Scroll through images to render scrolling whilst cycling through the images within the set to provide animations
  //Note, we could really make Part1 & Part4 the same function with monor changes since almost identical, but I am lazy and don't want to make things look more complex than they are
  //Note, we could really make Part2 & Part3 the same function with monor changes since almost identical, but I am lazy and don't want to make things look more complex than they are
  //Pacman scrolls across
  Demo4Part1();
  //The ghosts chase
  Demo4Part2();
  //Now ghosts are being chased
  Demo4Part3();
  //Pacman doing the chasing now
  Demo4Part4();
}

void ScrollImage(unsigned int SetInfo, byte ImageIndex, byte XOffset, byte YOffset, bool FlipX, bool FlipY, bool SwapXY, bool FlipAlternateRows, unsigned int Delay)
{
  byte TotalWidth;
  byte Position;
  
  //Get the palette info and image pointers
  RetrieveSetInfo(SetInfo);

  //Get the width of the pacman images in this sequence. All the same so any image will do
  TotalWidth = GetImageWidth(SetInfo, ImageIndex);

  //Render Pacman scrolling across the screen, switching between effectively 4 different images to make it look like he is eating
  while(Position < (TotalWidth - DISPLAY_WIDTH))
  {
    #ifdef DEBUG_MESSAGES
    sprintf(Buffer, "Smooth message position  = %d\n", Position);
    DebugMessage(Buffer);
    #endif
    //Start from the left most section of the image and move the displayed window right
    RenderImage(ImageIndex, Position + XOffset, YOffset, FlipX, FlipY, SwapXY, FlipAlternateRows);
    Position++;
    delay(Delay);
  } 
}

void Demo5()
{
  //Simply scroll the smoothed Woodrow Wilson banner. Text should look nicer from a distance
  ScrollImage(banners_ImageInfo, BANNERS_WW_REDBLACKSMOOTH, 0, 0, NO_FLIP_X, NO_FLIP_Y, NO_SWAP_XY, YES_FLIP_ALTERNATE, 50);
}

void Demo6()
{
  //Use the mono banner image as a source for random colors for the foreground
  PixelColorType = PIXEL_FROM_INDEX_RANDOM_FOREGROUND;
  ScrollImage(banners_ImageInfo, BANNERS_WW_BLACKWHITEMONO, 0, 0, NO_FLIP_X, NO_FLIP_Y, NO_SWAP_XY, YES_FLIP_ALTERNATE, 50);
}

void Demo7()
{
  //Use the mono banner image as a source for random colors for the background
  PixelColorType = PIXEL_FROM_INDEX_RANDOM_BACKGROUND;
  ScrollImage(banners_ImageInfo, BANNERS_WW_BLACKWHITEMONO, 0, 0, NO_FLIP_X, NO_FLIP_Y, NO_SWAP_XY, YES_FLIP_ALTERNATE, 50);
}

void Demo8()
{
  //Use the mono banner image as a source for cycling colors for the foreground
  PixelColorType = PIXEL_FROM_INDEX_CYCLE_FOREGROUND;
  ScrollImage(banners_ImageInfo, BANNERS_WW_BLACKWHITEMONO, 0, 0, NO_FLIP_X, NO_FLIP_Y, NO_SWAP_XY, YES_FLIP_ALTERNATE, 50);
}

void Demo9()
{
  //Use the mono banner image as a source for cycling colors for the background
  PixelColorType = PIXEL_FROM_INDEX_CYCLE_BACKGROUND;
  ScrollImage(banners_ImageInfo, BANNERS_WW_BLACKWHITEMONO, 0, 0, NO_FLIP_X, NO_FLIP_Y, NO_SWAP_XY, YES_FLIP_ALTERNATE, 50);
}

void loop() 
{
  Demo1();
  Demo2();
  Demo3();
  Demo4();
  Demo5();
  Demo6();
  Demo7();
  Demo8();
  Demo9();
}
