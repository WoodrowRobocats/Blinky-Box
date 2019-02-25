#!/usr/bin/perl -w
#use strict;

$| = 1;

############################################################################################################
############################################################################################################
##                                                                                                        ##
## Visit http://www.ezimba.com/index-ln.html and select 'reduce color' to generate a reduced color image  ##
##                                                                                                        ##
############################################################################################################
############################################################################################################

#Resultant output data...
#uint16_t : Width
#uint16_t  : Height
#uint8_t  : Type
#uint8_t  : Palette entries
#uint16_t  : ImageWords
#uint16_t : Color[0]
#uint16_t : Color[1]
#...
#uint16_t : Color[n]n
#uint16_t : ImageData[0]
#uint16_t : ImageData[1]
#...
#uint16_t : ImageData[n]

#Read a 24 bit BMP and RLE compress to paletized image
#16 bit entries in data table
#Type 0 = uncompressed : 0  bits RLE, 8 bit color
#Type 1 = 2 color      : 15 bits RLE, 1 bit color
#Type 2 = 4 color      : 14 bits RLE, 2 bit color
#Type 3 = 8 color      : 13 bits RLE, 3 bit color
#Type 4 = 16 color     : 12 bits RLE, 4 bit color
#Type 5 = 32 color     : 11 bits RLE, 5 bit color
#Type 6 = 64 color     : 10 bits RLE, 6 bit color
#Type 7 = 128 color    : 9  bits RLE, 7 bit color
#Type 8 = 256 color    : 8  bits RLE, 8 bit color

#If more pixels than RLE length can accomodate then just max first entry and repeat
#NB RLE length count +1, i.e. length 0 really has 1 pixel


#These should really be command lines...
my $BaseName = "notset";
my $LeftClip = 0;
my $RightClip = 0;
my $TopClip = 0;
my $BottomClip = 0;
my $Compress = "notset";#"Y" or "N"
my $Filename = "notset.bmp";
my $OutFilename = "notset.c";
my $Width;
my $Height;
my $Depth;
my $X;
my $Y;
my $Data;
my $PixelColor;
my $XPos;
my $YPos;
my $FilePos;
my $ColorCount;
my $UniqueColors;
my $PixelCount;
my $ImageBuffer;
my $ImageBufferIndex = 0;
my $Palette;
my $PaletteCount = 0;
my $Type;
my $MaxRLE;
my $Count;
my $LastColor;
my $OutputCount = 0;
my $FinalSize;
my $Ratio;
my $OutputBuffer = "";

sub GetArg
{
  $Find = $_[0];  
  $Default = $_[1];
  $Index = 0;
  while(exists $ARGV[$Index])
  {
    if ($ARGV[$Index] =~ /$Find/i)
    {
      return $ARGV[$Index + 1];
    }
    $Index += 2;
  }
  return $Default;
}

sub DataPack
{
    my $Count = $_[0];
    my $Color = $_[1];
    my $Type  = $_[2];
    return (($Count - 1) << $Type) | ($Color);
}

sub RGB_2_16
{
    my $R = ($_[0] >> 19) & 0x1f;# 5 upper bits
    my $G = ($_[0] >> 10) & 0x3f;# 6 upper bits
    my $B = ($_[0] >> 3)  & 0x1f;# 5 upper bits
    
    return ($R << 11) + ($G << 5) + ($B << 0)
}

sub ReadTripple
{
  my $T1;
  my $T2;
  my $T3;
  read (INHANDLE, $T1, 1);
  read (INHANDLE, $T2, 1);
  read (INHANDLE, $T3, 1);
  return ord($T1) + (ord($T2) * 256) + (ord($T3) * 256 * 256);
}

sub ReadDWord
{
  my $T1;
  my $T2;
  my $T3;
  my $T4;
  read (INHANDLE, $T1, 1);
  read (INHANDLE, $T2, 1);
  read (INHANDLE, $T3, 1);
  read (INHANDLE, $T4, 1);
  return ord($T1) + (ord($T2) * 256) + (ord($T3) * 256 * 256) + (ord($T4) * 256 * 256 * 256);
}

sub ReadWord
{
  my $T1;
  my $T2;
  read (INHANDLE, $T1, 1);
  read (INHANDLE, $T2, 1);
  return ord($T1) + (ord($T2) * 256);
}

sub ReadByte
{
  my $T1;
  read (INHANDLE, $T1, 1);
  return ord($T1);
}

sub LoadImage
{
    my $BMPType;
    my $BMPSize;
    my $Res1;
    my $Res2;
    my $Offset;
    my $BufferIndex = 0;

    my $SkipSummary = $_[0];

    my $ColorTripplet;
    open(INHANDLE, "$Filename")|| die "Could not open '$Filename'";
    binmode INHANDLE;

    $BMPType = ReadWord();
    $BMPSize = ReadDWord();
    $Res1 = ReadWord();;
    $Res2 = ReadWord();;
    $Offset = ReadDWord();;
    
    seek (INHANDLE, 0x12, 0);
    $Width = ReadDWord();
    seek (INHANDLE, 0x16, 0);
    $Height = ReadDWord();
    seek (INHANDLE, 0x1C, 0);
    $Depth = ReadWord();
    
    if ($Depth != 24)
    {
        print "Error, color depth not set to 24 bits per color\n";
        exit;
    }
    
    if ($SkipSummary ne "Y")
    {
        print "Source Image Width  = $Width\n";
        print "Source Image Height = $Height\n";
    }
    
    if (($Width & 3) != 0)
    {
      print "Image width must be multiple of 4";
      exit(-1);
    }

    seek (INHANDLE, 0x36, 0);
    #Scan the image and build a palette list & paletized image buffer
    for ($Y = 0; $Y < $Height; $Y++)
    {
        for ($X = $LeftClip; $X < $Width - $RightClip; $X++)
        {
            $FilePos = ($X + (($Height - 1 - $Y) * $Width));
            seek (INHANDLE, $Offset + 3 * $FilePos, 0);
            $ColorTripplet = ReadTripple();
#            $ColorHex = sprintf("0x%06X", $ColorTripplet);
            $PixelColor = RGB_2_16($ColorTripplet);
#            $PixelHex = sprintf("0x%04X", $PixelColor);
            if (!exists $ColorCount{$PixelColor})#Color not yet declared so add as new entry
            {
                $PaletteRed[$PaletteCount]   = ($ColorTripplet >> 19) << 3;
                $PaletteGreen[$PaletteCount] = (($ColorTripplet >> 10) & 0x3f) << 2;
                $PaletteBlue[$PaletteCount]  = (($ColorTripplet >> 3) & 0x1f) << 3;
                
                $Palette[$PaletteCount] = $PixelColor;
                $ColorToPalette{$PixelColor} = $PaletteCount;
                $PaletteCount++;
            }
            $ColorCount{$PixelColor}++;
            $ImageBuffer[$BufferIndex] = $ColorToPalette{$PixelColor};
            $BufferIndex++;
        }
    }
    #Correct limits for any cliping
    $Width = $Width - $LeftClip - $RightClip;
    $Height = $Height - $TopClip - $BottomClip;
    
    $UniqueColors = keys %ColorCount;
    $PixelCount = $Width *$Height;
    if ($Compress eq "N")
    {
        $Type = 0;
    }
    elsif    ($UniqueColors <= 2)
    {
        $Type = 1;
    }
    elsif ($UniqueColors <= 4)
    {
        $Type = 2;
    }
    elsif ($UniqueColors <= 8)
    {
        $Type = 3;
    }
    elsif ($UniqueColors <= 16)
    {
        $Type = 4;
    }
    elsif ($UniqueColors <= 32)
    {
        $Type = 5;
    }
    elsif ($UniqueColors <= 64)
    {
        $Type = 6;
    }
    elsif ($UniqueColors <= 128)
    {
        $Type = 7;
    }
    elsif ($UniqueColors <= 256)
    {
        $Type = 8;
    }
    else
    {
        print "Too many colors. Not currently supported\n";
        print "Unique colors = $UniqueColors\n";
        exit(-1);
    }
    close INHANDLE;

    if ($SkipSummary ne "Y")
    {
        print "Output Image Width  = $Width\n";
        print "Output Image Height = $Height\n";
    }
}

sub BuildHeader()
{
    $OutputBuffer .= sprintf("#include <stdint.h>\n\n");
    $OutputBuffer .= sprintf("const uint16_t $BaseName" . "[] = {\n");
    $OutputBuffer .= sprintf("             0x%04X, //Width = %d\n", $Width, $Width);
    $OutputBuffer .= sprintf("             0x%04X, //Height = %d\n", $Height, $Height);
    $OutputBuffer .= sprintf("             0x%04X, //Type = %d, Colors = %d\n", ($Type * 256) + ($UniqueColors & 0xff), $Type, $UniqueColors);
    #Need to come back later and fill in the resultant size
    $OutputBuffer .= sprintf("##OUTPUTSIZE##\n");
}

sub BuildHeaderArduino
{
    $BaseName = $_[0];
    $OutputBuffer .= sprintf("const unsigned int $BaseName" . "[] PROGMEM = {\n");
    $OutputBuffer .= sprintf("             0x%02X%02X, //Width, height\n", $Width, $Height);
}

sub AppendPalette
{
    $SkipLastComma = $_[0];
    
    for ($PaletteCount = 0; $PaletteCount < $UniqueColors; $PaletteCount++)
    {
        if (($SkipLastComma eq "Y") && ($PaletteCount == $UniqueColors - 1))
        {
            $OutputBuffer .= sprintf("             0x%04X   //Color index %2d (%3d-%3d-%3d)occurs %d times\n", $Palette[$PaletteCount], $PaletteCount, $PaletteRed[$PaletteCount], $PaletteGreen[$PaletteCount], $PaletteBlue[$PaletteCount], $ColorCount{$Palette[$PaletteCount]});
        }
        else
        {
            $OutputBuffer .= sprintf("             0x%04X,  //Color index %2d (%3d-%3d-%3d)occurs %d times\n", $Palette[$PaletteCount], $PaletteCount, $PaletteRed[$PaletteCount], $PaletteGreen[$PaletteCount], $PaletteBlue[$PaletteCount], $ColorCount{$Palette[$PaletteCount]});
        }
    }
    if ($SkipLastComma eq "Y")
    {
        $OutputBuffer .= sprintf("             };\n");
    }
}

sub GeneratePaletteData
{
    $PaletteName = $_[0];
    
    $OutputBuffer .= sprintf("// Palette table\n");
    $OutputBuffer .= sprintf("const unsigned int $PaletteName". "[] PROGMEM = {\n");
    AppendPalette("Y");
    $OutputBuffer .= sprintf("\n");
}

sub AppendRLEImage()
{
    $MaxRLE = 2 ** (16 - $Type);
    
    $Count = 1;
    $LastColor = $ImageBuffer[0];
    #Now scan the indexed buffer and do RLE
    for ($ImageBufferIndex = 1; $ImageBufferIndex < $PixelCount; $ImageBufferIndex++)#Count from 1 since 0 already counted above (LastColor)
    {
      if (($LastColor != $ImageBuffer[$ImageBufferIndex]) or ($Count == $MaxRLE))
      {
        #Pixel color different so send count so far and update current
    #    print"$LastColor : $Count\n";
        #printf("             0x%04X, //%d x color %d\n", DataPack($Count, $LastColor, $Type), $Count, $LastColor);
        $OutputBuffer .= sprintf("             0x%04X, //%d x color %d\n", DataPack($Count, $LastColor, $Type), $Count, $LastColor);
        $Count = 1;
        $LastColor = $ImageBuffer[$ImageBufferIndex];
        $OutputCount ++;
      }
      else
      {
        #Pixel color the same so increment count
        $Count++;   
      }
    }
    #Send remaining data
    #print"$LastColor : $Count\n";
    #printf("             0x%04X  //%d x color %d\n", DataPack($Count, $LastColor, $Type), $Count, $LastColor);
    #printf("              };\n");
    $OutputBuffer .= sprintf("             0x%04X  //%d x color %d\n", DataPack($Count, $LastColor, $Type), $Count, $LastColor);
    $OutputBuffer .= sprintf("              };\n");
    $OutputCount ++;
}

sub DisplaySummary()
{
    print "Pixel count = $PixelCount\n";
    print "Unique colors = $UniqueColors\n";
    $FinalSize = 3 + $UniqueColors + $OutputCount;
    print "Final image data size = $FinalSize data words\n";
    $Ratio = 100 * $FinalSize / $PixelCount;
    print "Compression ratio = $Ratio\%\n";
}

sub GenerateOutput()
{
    print "Creating output file $OutFilename\n";
    #Update the image data size
    $OutputSizeData = sprintf("             0x%04X, //Image words = %d", $OutputCount, $OutputCount);
    $OutputBuffer =~ s/##OUTPUTSIZE##/$OutputSizeData/;
    
    $OutputBuffer .= "\n";
    
    $OutFilename = $Filename;
    $OutFilename =~ s/\.bmp/\.c/;
    open(OUTHANDLE, ">", "$OutFilename")|| die "Could not open '$OutFilename'";
    printf(OUTHANDLE $OutputBuffer);
    close OUTHANDLE;
}

sub AppendRawImage()
{
    $ImageBufferIndex = 0;
    for ($Y = 0; $Y < $Height; $Y++)
    {
        for ($X = 0; $X < $Width; $X+=2)
        {
            if (($X == ($Width - 2)) and ($Y == ($Height - 1)))#Last entry so do not include ","
            {
                $OutputBuffer .= sprintf("             0x%02X%02X  //(%03d:%03d) index %d | index %d\n", $ImageBuffer[$ImageBufferIndex], $ImageBuffer[$ImageBufferIndex + 1], $X, $Y, $ImageBuffer[$ImageBufferIndex], $ImageBuffer[$ImageBufferIndex + 1]);
            }
            else
            {
                $OutputBuffer .= sprintf("             0x%02X%02X, //(%03d:%03d) index %d | index %d\n", $ImageBuffer[$ImageBufferIndex], $ImageBuffer[$ImageBufferIndex + 1], $X, $Y, $ImageBuffer[$ImageBufferIndex], $ImageBuffer[$ImageBufferIndex + 1]);
            }
            $ImageBufferIndex+=2;
            $OutputCount +=1;#Reducing from 16 bit (actually 24) down to 8 bit indexed so only 1 word per 2 pixels
        }
    }
    $OutputBuffer .= sprintf("             };\n\n");
}

sub AppendRawImageArduino()
{
    $ImageBufferIndex = 0;
    for ($Y = 0; $Y < $Height; $Y++)
    {
        for ($X = 0; $X < $Width; $X++)
        {
            if (($X == ($Width - 1)) and ($Y == ($Height - 1)))#Last entry so do not include ","
            {
                $OutputBuffer .= sprintf("%3d  //(%03d:%03d) index %d | index %d\n", $ImageBuffer[$ImageBufferIndex], $X, $Y, $ImageBuffer[$ImageBufferIndex], $ImageBuffer[$ImageBufferIndex + 1]);
            }
            else
            {
                $OutputBuffer .= sprintf("%3d, //(%03d:%03d) index %d | index %d\n", $ImageBuffer[$ImageBufferIndex], $X, $Y, $ImageBuffer[$ImageBufferIndex], $ImageBuffer[$ImageBufferIndex + 1]);
            }
            $ImageBufferIndex+=1;
            $OutputCount +=1;#Reducing from 16 bit (actually 24) down to 8 bit indexed so only 1 word per 2 pixels
        }
    }
    $OutputBuffer .= sprintf("             };\n\n");
}

sub SendToFile
{
    $Bytes = $_[0];
    $Data = $_[1];
    for (my $i=0; $i < $Bytes; $i++)
    {
        $OutputChar = ($Data >> ($i * 8)) & 0xff;
        print OUT chr($OutputChar);
    }
}

sub SendBMPHeader
{
    my $Width = $_[0];
    my $Height = $_[1];
    my $Filename = $_[2];

    my $BMPHeaderID = 0x4d42;
    my $BMPFileSize = 230454;
    my $BMPUnused = 0;
    my $BMPDataOffset = 0x36;
    my $BMPHeaderSize = 0x28;
    my $BMPWidth = $Width;
    my $BMPHeight = $Height;
    my $BMPPlanes = 1;
    my $BMPBPP = 24;
    my $BMPCompression = 0;
    my $BMPImageSize = 0x38400;
    my $BMPHResolution = 0;
    my $BMPVResolution = 0;
    my $BMPPalette = 0;
    my $BMPImportant = 0;
    
    open(OUT, ">:raw", $Filename);
    SendToFile(2, $BMPHeaderID);
    SendToFile(4, $BMPFileSize);
    SendToFile(2, $BMPUnused);
    SendToFile(2, $BMPUnused);
    SendToFile(4, $BMPDataOffset);
    SendToFile(4, $BMPHeaderSize);
    SendToFile(4, $BMPWidth);
    SendToFile(4, $BMPHeight);
    SendToFile(2, $BMPPlanes);
    SendToFile(2, $BMPBPP);
    SendToFile(4, $BMPCompression);
    SendToFile(4, $BMPImageSize);
    SendToFile(4, $BMPHResolution);
    SendToFile(4, $BMPVResolution);
    SendToFile(4, $BMPPalette);
    SendToFile(4, $BMPImportant);
}

sub GenTestBMP
{
    SendBMPHeader(320, 240, "test.bmp");
    $Data = 1;
    for ($y = 0; $y < 240; $y++)
    {
        for ($x = 0; $x < 320; $x++)
        {
            SendToFile(1, $Data);
            $Data++;
            SendToFile(1, $Data);
            $Data++;
            SendToFile(1, $Data);
            $Data++;
#            SendToFile(1, $x + 1);
#            SendToFile(1, $y + 1);
#            SendToFile(1, ($x + $y + 1));
        }
        
    }
    close(OUT)
}

sub BMP2RLE()
{
    LoadImage();
    BuildHeader();
    AppendPalette();
    
    if ($Compress eq "Y")
    {
        AppendRLEImage();
    }
    else
    {
        AppendRawImage();
    }
    
    DisplaySummary();
    
    GenerateOutput();    
}

sub BMP2Arduino
{
    LoadImage();
    BuildHeaderArduino();
    
    if ($Compress eq "Y")
    {
        AppendRLEImage();
    }
    else
    {
        AppendRawImageArduino();
    }

    AppendPalette();
    
    DisplaySummary();
    
    GenerateOutput();    
}

sub BMPScanBMP
{
    #Scan the bmp and add the colors to the palette
    $BaseName = $_[0];
    $DIRName = $_[1];
    printf("Scanning file $DIRName/$BaseName\n");
    $Filename = $DIRName . "/" . $BaseName . ".bmp";
    LoadImage("Y");
}
sub BMPSet2Arduino
{
    #Scan the directory and build a list of images
    $DIRName = $_[0];
    opendir my $dir, $DIRName or die "Cannot open directory: $DIRName\n";
    my @files = readdir $dir;
    closedir $dir;

    #Remove . and ..    
    shift @files;
    shift @files;
    
    #Scan each image to build palette list
    foreach $Filename(@files)
    {
        #Check if really a bitmap file
        if ($Filename =~ s/\.bmp$//ig)
        {
            BMPScanBMP($Filename, $DIRName);
            push @BitmapFileList, $Filename;
        }
    }
    print "Unique colors = $UniqueColors\n";
    
    #Now start processing the images
    #First, send the palette that was built
    GeneratePaletteData($DIRName . "_Palette");
    
    #Next scan each image again to recover the pixel data for each image
    foreach $Filename(@BitmapFileList)
    {
        #Scan the image again to pull into the buffer
        $Filename =~ s/\.bmp//ig;
        BMPScanBMP($Filename, $DIRName);
        #Now send the image data in a simplified count/index format
        $Type = 8;
        $ImageName = $DIRName . "_" . $Filename;
        BuildHeaderArduino($ImageName);
        AppendRLEImage();
        $OutputBuffer .= sprintf("\n");
        push @ImageNameList, $ImageName;
    }
    $ImageSetCount = @ImageNameList;
    
    #Now build the image array lists
    $OutputBuffer .= sprintf("const unsigned int $DIRName" . "_ImagePointers[] PROGMEM = {\n");
    for ($Index = 0; $Index < $ImageSetCount; $Index++)
    {
        $ImageName = $ImageNameList[$Index];
        if (@ImageNameList > 0)
        {
            $OutputBuffer .= sprintf("         &$ImageName,\n");
        }
        else
        {
            $OutputBuffer .= sprintf("         &$ImageName\n");
        }
    }
    $OutputBuffer .= sprintf("             };\n\n");

    #Declare some #defines to help image indexing
    $OutputBuffer .= sprintf("//#defines to simplify image referencing\n");
    for ($Index = 0; $Index < $ImageSetCount; $Index++)
    {
        $ImageName = uc $ImageNameList[$Index];
        $OutputBuffer .= sprintf("#define %-35s %d\n", $ImageName, $Index);
    }
    $OutputBuffer .= sprintf("\n");
    
    #Finally build an image info table
    $OutputBuffer .= sprintf("const unsigned int $DIRName" . "_ImageInfo[] PROGMEM = {\n");
    $OutputBuffer .= sprintf("               $ImageSetCount,         //Number of images in set\n");
    $OutputBuffer .= sprintf("               $UniqueColors,        //Palette size\n");
    $OutputBuffer .= sprintf("               1000,      //Default inter image delay\n");
    $OutputBuffer .= sprintf("               30,        //Default number of images to cycle for set\n");
    $OutputBuffer .= sprintf("               &$DIRName" . "_Palette, //Pointer to palette table\n");
    $OutputBuffer .= sprintf("               &$DIRName" . "_ImagePointers, //Pointer to image pointer list\n");
    $OutputBuffer .= sprintf("               };\n\n");
    
    #Finally send the buffer to the output file
    $OutFilename = "$DIRName.h";
    open(OUTHANDLE, ">", "$OutFilename")|| die "Could not open '$OutFilename'";
    printf(OUTHANDLE $OutputBuffer);
    close OUTHANDLE;
    print "Finished. Results in file '$OutFilename'\n";
}

###############################################################
###############################################################
##  Main script start                                        ##
###############################################################
###############################################################
$BaseName = "MarioRed-8";
$Compress = "Y";#"Y" or "N"
$Filename = $BaseName . ".bmp";
$OutFilename = $BaseName . ".c";

#GenTestBMP();

#BMP2RLE();

$DIRToScan = GetArg("-dir", "banners");

BMPSet2Arduino($DIRToScan);


