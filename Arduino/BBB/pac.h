// Palette table
const unsigned int pac_Palette[] PROGMEM = {
             0x0000,  //Color index  0 (  0-  0-  0)occurs 445 times
             0xFFE0   //Color index  1 (248-252-  0)occurs 323 times
             };

const unsigned int pac_Pac_1[] PROGMEM = {
             0x1010, //Width, height
             0x1500, //22 x color 0
             0x0401, //5 x color 1
             0x0800, //9 x color 0
             0x0801, //9 x color 1
             0x0500, //6 x color 0
             0x0A01, //11 x color 1
             0x0400, //5 x color 0
             0x0A01, //11 x color 1
             0x0300, //4 x color 0
             0x0C01, //13 x color 1
             0x0200, //3 x color 0
             0x0C01, //13 x color 1
             0x0200, //3 x color 0
             0x0C01, //13 x color 1
             0x0200, //3 x color 0
             0x0C01, //13 x color 1
             0x0200, //3 x color 0
             0x0C01, //13 x color 1
             0x0300, //4 x color 0
             0x0A01, //11 x color 1
             0x0400, //5 x color 0
             0x0A01, //11 x color 1
             0x0500, //6 x color 0
             0x0801, //9 x color 1
             0x0800, //9 x color 0
             0x0401, //5 x color 1
             0x2400  //37 x color 0
              };

const unsigned int pac_Pac_2[] PROGMEM = {
             0x1010, //Width, height
             0x1500, //22 x color 0
             0x0401, //5 x color 1
             0x0800, //9 x color 0
             0x0801, //9 x color 1
             0x0500, //6 x color 0
             0x0A01, //11 x color 1
             0x0400, //5 x color 0
             0x0A01, //11 x color 1
             0x0300, //4 x color 0
             0x0901, //10 x color 1
             0x0500, //6 x color 0
             0x0601, //7 x color 1
             0x0800, //9 x color 0
             0x0301, //4 x color 1
             0x0B00, //12 x color 0
             0x0601, //7 x color 1
             0x0800, //9 x color 0
             0x0901, //10 x color 1
             0x0600, //7 x color 0
             0x0A01, //11 x color 1
             0x0400, //5 x color 0
             0x0A01, //11 x color 1
             0x0500, //6 x color 0
             0x0801, //9 x color 1
             0x0800, //9 x color 0
             0x0401, //5 x color 1
             0x2400  //37 x color 0
              };

const unsigned int pac_Pac_3[] PROGMEM = {
             0x1010, //Width, height
             0x1500, //22 x color 0
             0x0401, //5 x color 1
             0x0800, //9 x color 0
             0x0601, //7 x color 1
             0x0700, //8 x color 0
             0x0601, //7 x color 1
             0x0800, //9 x color 0
             0x0501, //6 x color 1
             0x0800, //9 x color 0
             0x0501, //6 x color 1
             0x0900, //10 x color 0
             0x0401, //5 x color 1
             0x0A00, //11 x color 0
             0x0301, //4 x color 1
             0x0B00, //12 x color 0
             0x0401, //5 x color 1
             0x0A00, //11 x color 0
             0x0501, //6 x color 1
             0x0A00, //11 x color 0
             0x0501, //6 x color 1
             0x0900, //10 x color 0
             0x0601, //7 x color 1
             0x0900, //10 x color 0
             0x0601, //7 x color 1
             0x0A00, //11 x color 0
             0x0401, //5 x color 1
             0x2400  //37 x color 0
              };

const unsigned int pac_ImagePointers[] PROGMEM = {
         &pac_Pac_1,
         &pac_Pac_2,
         &pac_Pac_3,
             };

//#defines to simplify image referencing
#define PAC_PAC_1                           0
#define PAC_PAC_2                           1
#define PAC_PAC_3                           2

const unsigned int pac_ImageInfo[] PROGMEM = {
               3,         //Number of images in set
               2,        //Palette size
               1000,      //Default inter image delay
               30,        //Default number of images to cycle for set
               &pac_Palette, //Pointer to palette table
               &pac_ImagePointers, //Pointer to image pointer list
               };

