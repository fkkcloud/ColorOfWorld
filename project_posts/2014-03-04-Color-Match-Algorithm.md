Continued from Second Post

From the collected 5 color palettes from 5 different street views, we can find best-matched classical drawing from the library that has best-matched color palette (top 5)

![Example Image](../project_images/prog4.jpg?raw=true "Example Image")

<code>
using namespace Gdiplus;
using namespace std;

typedef std::map<std::string, float> TStrClrMap;
typedef std::pair<std::string, float> TStrClrPair

void main()
{

  Bitmap* sourceImg = new("sourceImg.bmp");
  int x;
  int y;
  Color pixelColor;

  TStrClrMap tMap;
  tMap.insert(TStrClrPair("Red", 0));
  tMap.insert(TStrClrPair("Green", 0));
  tMap.insert(TStrClrPair("Blue", 0));
  tMap.insert(TStrClrPair("Cyan", 0));
  tMap.insert(TStrClrPair("Magenta", 0));
  tMap.insert(TStrClrPair("Yellow", 0));
  
  for ( x = 0; x < 500; x++ )
    {
      for ( y = 0; y < 500; y++ )
	{
	  sourceImg->GetPixel( x, y, &pixelColor );
	  float r = pixelColor.getRed();
	  float g = pixelColor.getGreen();
	  float b = pixelColor.getBlue();
	  float average = (r + g + b) / 3;
	  int cmp1, cmp2, cmp3;
	 
	  if ( r > average ) {
	    cmp1 = 1;
	  } else {
	    cmp1 = 0;
	  }
	  if ( g > average ) {
	    cmp2 = 1;
	  } else {
	    cmp2 = 0;
	  }
	  if ( b > average ) {
	    cmp3 = 1;
	  } else {
	    cmp3 = 0;
	  }

	  if ( cmp1 == 1 && cmp2 == 0 && cmp3 == 0 ) {
	    tMap["Red"] += 1;
	  } else if ( cmp1 == 0 && cmp2 == 1 && cmp3 == 0 ) {
	    tMap["Green"] += 1;
	  } else if ( cmp1 == 0 && cmp2 == 0 && cmp3 == 1 ) {
	    tMap["Blue"] += 1;
	  } else if ( cmp1 == 1 && cmp2 == 1 && cmp3 == 0 ) {
	    tMap["Yellow"] += 1;
	  } else if ( cmp1 == 1 && cmp2 == 0 && cmp3 == 1 ) {
	    tMap["Cyan"] += 1;
	  } else if ( cmp1 == 0 && cmp2 == 1 && cmp3 == 1 ) {
	    tMap["Magenta"] += 1;
	  }
 
	}
    }

</code>
