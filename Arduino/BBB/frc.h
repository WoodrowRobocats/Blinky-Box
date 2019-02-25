// Palette table
const unsigned int frc_Palette[] PROGMEM = {
             0x0000,  //Color index  0 (  0-  0-  0)occurs 182 times
             0x2902,  //Color index  1 ( 40- 32- 16)occurs 25 times
             0x4A85,  //Color index  2 ( 72- 80- 40)occurs 33 times
             0x5548,  //Color index  3 ( 80-168- 64)occurs 34 times
             0xC634,  //Color index  4 (192-196-160)occurs 5 times
             0x93A9,  //Color index  5 (144-116- 72)occurs 12 times
             0xCD0D,  //Color index  6 (200-160-104)occurs 11 times
             0xFEB2,  //Color index  7 (248-212-144)occurs 40 times
             0x61C4,  //Color index  8 ( 96- 56- 32)occurs 32 times
             0xF182,  //Color index  9 (240- 48- 16)occurs 30 times
             0x30A1,  //Color index 10 ( 48- 20-  8)occurs 29 times
             0xAA24,  //Color index 11 (168- 68- 32)occurs 21 times
             0xF5B7,  //Color index 12 (240-180-184)occurs 5 times
             0xC4CD,  //Color index 13 (192-152-104)occurs 10 times
             0xFED2   //Color index 14 (248-216-144)occurs 43 times
             };

const unsigned int frc_MarioGreen[] PROGMEM = {
             0x1010, //Width, height
             0x0600, //7 x color 0
             0x0001, //1 x color 1
             0x0102, //2 x color 2
             0x0B00, //12 x color 0
             0x0203, //3 x color 3
             0x0004, //1 x color 4
             0x0003, //1 x color 3
             0x0900, //10 x color 0
             0x0103, //2 x color 3
             0x0002, //1 x color 2
             0x0004, //1 x color 4
             0x0003, //1 x color 3
             0x0004, //1 x color 4
             0x0001, //1 x color 1
             0x0700, //8 x color 0
             0x0303, //4 x color 3
             0x0104, //2 x color 4
             0x0005, //1 x color 5
             0x0002, //1 x color 2
             0x0000, //1 x color 0
             0x0001, //1 x color 1
             0x0400, //5 x color 0
             0x0002, //1 x color 2
             0x0303, //4 x color 3
             0x0005, //1 x color 5
             0x0002, //1 x color 2
             0x0203, //3 x color 3
             0x0102, //2 x color 2
             0x0300, //4 x color 0
             0x0203, //3 x color 3
             0x0002, //1 x color 2
             0x0001, //1 x color 1
             0x0203, //3 x color 3
             0x0002, //1 x color 2
             0x0500, //6 x color 0
             0x0001, //1 x color 1
             0x0103, //2 x color 3
             0x0102, //2 x color 2
             0x0103, //2 x color 3
             0x0002, //1 x color 2
             0x0106, //2 x color 6
             0x0002, //1 x color 2
             0x0400, //5 x color 0
             0x0103, //2 x color 3
             0x0402, //5 x color 2
             0x0006, //1 x color 6
             0x0007, //1 x color 7
             0x0002, //1 x color 2
             0x0000, //1 x color 0
             0x0001, //1 x color 1
             0x0200, //3 x color 0
             0x0203, //3 x color 3
             0x0000, //1 x color 0
             0x0005, //1 x color 5
             0x0107, //2 x color 7
             0x0005, //1 x color 5
             0x0001, //1 x color 1
             0x0007, //1 x color 7
             0x0005, //1 x color 5
             0x0207, //3 x color 7
             0x0000, //1 x color 0
             0x0002, //1 x color 2
             0x0003, //1 x color 3
             0x0002, //1 x color 2
             0x0001, //1 x color 1
             0x0102, //2 x color 2
             0x0207, //3 x color 7
             0x0002, //1 x color 2
             0x0407, //5 x color 7
             0x0002, //1 x color 2
             0x0001, //1 x color 1
             0x0002, //1 x color 2
             0x0106, //2 x color 6
             0x0001, //1 x color 1
             0x0102, //2 x color 2
             0x0007, //1 x color 7
             0x0005, //1 x color 5
             0x0507, //6 x color 7
             0x0002, //1 x color 2
             0x0000, //1 x color 0
             0x0107, //2 x color 7
             0x0106, //2 x color 6
             0x0000, //1 x color 0
             0x0006, //1 x color 6
             0x0005, //1 x color 5
             0x0100, //2 x color 0
             0x0002, //1 x color 2
             0x0006, //1 x color 6
             0x0107, //2 x color 7
             0x0005, //1 x color 5
             0x0100, //2 x color 0
             0x0107, //2 x color 7
             0x0005, //1 x color 5
             0x0207, //3 x color 7
             0x0006, //1 x color 6
             0x0501, //6 x color 1
             0x0200, //3 x color 0
             0x0002, //1 x color 2
             0x0507, //6 x color 7
             0x0006, //1 x color 6
             0x0000, //1 x color 0
             0x0201, //3 x color 1
             0x0500, //6 x color 0
             0x0001, //1 x color 1
             0x0000, //1 x color 0
             0x0005, //1 x color 5
             0x0207, //3 x color 7
             0x0101, //2 x color 1
             0x0002, //1 x color 2
             0x0900, //10 x color 0
             0x0001, //1 x color 1
             0x0105, //2 x color 5
             0x0101, //2 x color 1
             0x0400  //5 x color 0
              };

const unsigned int frc_MarioRed[] PROGMEM = {
             0x1010, //Width, height
             0x0600, //7 x color 0
             0x0008, //1 x color 8
             0x0109, //2 x color 9
             0x0008, //1 x color 8
             0x0900, //10 x color 0
             0x000A, //1 x color 10
             0x0109, //2 x color 9
             0x000B, //1 x color 11
             0x000C, //1 x color 12
             0x000D, //1 x color 13
             0x000A, //1 x color 10
             0x0700, //8 x color 0
             0x000A, //1 x color 10
             0x0109, //2 x color 9
             0x000B, //1 x color 11
             0x000C, //1 x color 12
             0x0009, //1 x color 9
             0x000C, //1 x color 12
             0x0008, //1 x color 8
             0x0700, //8 x color 0
             0x0209, //3 x color 9
             0x000B, //1 x color 11
             0x010C, //2 x color 12
             0x000B, //1 x color 11
             0x0308, //4 x color 8
             0x0300, //4 x color 0
             0x0008, //1 x color 8
             0x0309, //4 x color 9
             0x0008, //1 x color 8
             0x000B, //1 x color 11
             0x0209, //3 x color 9
             0x0008, //1 x color 8
             0x010A, //2 x color 10
             0x0200, //3 x color 0
             0x0209, //3 x color 9
             0x000B, //1 x color 11
             0x0008, //1 x color 8
             0x0109, //2 x color 9
             0x010B, //2 x color 11
             0x0500, //6 x color 0
             0x000A, //1 x color 10
             0x0109, //2 x color 9
             0x010B, //2 x color 11
             0x0009, //1 x color 9
             0x000B, //1 x color 11
             0x0008, //1 x color 8
             0x000E, //1 x color 14
             0x000D, //1 x color 13
             0x0008, //1 x color 8
             0x0400, //5 x color 0
             0x000B, //1 x color 11
             0x0009, //1 x color 9
             0x000B, //1 x color 11
             0x0308, //4 x color 8
             0x000B, //1 x color 11
             0x000E, //1 x color 14
             0x000B, //1 x color 11
             0x0000, //1 x color 0
             0x0008, //1 x color 8
             0x0200, //3 x color 0
             0x0209, //3 x color 9
             0x000A, //1 x color 10
             0x000B, //1 x color 11
             0x010E, //2 x color 14
             0x000D, //1 x color 13
             0x0000, //1 x color 0
             0x000E, //1 x color 14
             0x000D, //1 x color 13
             0x020E, //3 x color 14
             0x000A, //1 x color 10
             0x0008, //1 x color 8
             0x0009, //1 x color 9
             0x000B, //1 x color 11
             0x000A, //1 x color 10
             0x0108, //2 x color 8
             0x020E, //3 x color 14
             0x0008, //1 x color 8
             0x040E, //5 x color 14
             0x000B, //1 x color 11
             0x000A, //1 x color 10
             0x0008, //1 x color 8
             0x010E, //2 x color 14
             0x000A, //1 x color 10
             0x0008, //1 x color 8
             0x000A, //1 x color 10
             0x000E, //1 x color 14
             0x0008, //1 x color 8
             0x050E, //6 x color 14
             0x0008, //1 x color 8
             0x0000, //1 x color 0
             0x000D, //1 x color 13
             0x000E, //1 x color 14
             0x000B, //1 x color 11
             0x000E, //1 x color 14
             0x000A, //1 x color 10
             0x000B, //1 x color 11
             0x000D, //1 x color 13
             0x0000, //1 x color 0
             0x010A, //2 x color 10
             0x000B, //1 x color 11
             0x010E, //2 x color 14
             0x000D, //1 x color 13
             0x0100, //2 x color 0
             0x000D, //1 x color 13
             0x000E, //1 x color 14
             0x010D, //2 x color 13
             0x020E, //3 x color 14
             0x050A, //6 x color 10
             0x0200, //3 x color 0
             0x000A, //1 x color 10
             0x060E, //7 x color 14
             0x0000, //1 x color 0
             0x030A, //4 x color 10
             0x0400, //5 x color 0
             0x000A, //1 x color 10
             0x0000, //1 x color 0
             0x0008, //1 x color 8
             0x020E, //3 x color 14
             0x0208, //3 x color 8
             0x0A00, //11 x color 0
             0x0108, //2 x color 8
             0x010A, //2 x color 10
             0x0400  //5 x color 0
              };

const unsigned int frc_ImagePointers[] PROGMEM = {
         &frc_MarioGreen,
         &frc_MarioRed,
             };

//#defines to simplify image referencing
#define FRC_MARIOGREEN                      0
#define FRC_MARIORED                        1

const unsigned int frc_ImageInfo[] PROGMEM = {
               2,         //Number of images in set
               15,        //Palette size
               1000,      //Default inter image delay
               30,        //Default number of images to cycle for set
               &frc_Palette, //Pointer to palette table
               &frc_ImagePointers, //Pointer to image pointer list
               };

