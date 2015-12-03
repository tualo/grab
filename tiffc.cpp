#include <stdio.h>
#include <cstdlib>
#include <cstring>

#include <fstream>

using namespace std;

int main(int argc, char* argv[]){
  int offset=0;
  int nx=100;
  int ny=200;
  int samples=3;
  int i=0;
  int j=0;
  int s=0;




  ofstream pFile ("data_mono.tiff", ios::out | ios::binary);
  // tiff header 4d4d002a
  pFile.put(0x4d);
  pFile.put(0x4d);
  pFile.put(0x00);
  pFile.put(0x2a);

  offset = nx * ny * samples + 8;
  pFile.put( (offset & 0xff000000) / 16777216) ;
  pFile.put( (offset & 0x00ff0000) / 65536 );
  pFile.put( (offset & 0x0000ff00) / 256 );
  pFile.put( (offset & 0x000000ff) );

  for (j=0;j<ny;j++) {
    for (i=0;i<nx;i++) {
      for (s=0;s<samples;s++){
        if (i==j){
          pFile.put(0xff);
        }else{
          pFile.put(0x00);
        }
      }
    }
  }
  // number of dictionary entries
  pFile.put(0x00);
  pFile.put(0x0e);


  // width
  pFile.put(0x01); // tag
  pFile.put(0x00); // tag

  pFile.put(0x00); // tagtype
  pFile.put(0x03); // tagtype

  pFile.put(0x00); // value count
  pFile.put(0x00); // value count
  pFile.put(0x00); // value count
  pFile.put(0x01); // value count

  pFile.put( (nx & 0xff00) / 256 );
  pFile.put( (nx & 0x00ff) );

  pFile.put(0x00); // space
  pFile.put(0x00); // space


  // height
  pFile.put(0x01); // tag
  pFile.put(0x01); // tag

  pFile.put(0x00); // tagtype
  pFile.put(0x03); // tagtype

  pFile.put(0x00); // value count
  pFile.put(0x00); // value count
  pFile.put(0x00); // value count
  pFile.put(0x01); // value count

  pFile.put( (ny & 0xff00) / 256 );
  pFile.put( (ny & 0x00ff) );

  pFile.put(0x00); // space
  pFile.put(0x00); // space

  // bits per sample
  pFile.put(0x01); // tag
  pFile.put(0x02); // tag

  pFile.put(0x00); // tagtype
  pFile.put(0x03); // tagtype

  pFile.put(0x00); // value count
  pFile.put(0x00); // value count
  pFile.put(0x00); // value count
  pFile.put((char) samples); // value count


  offset = nx * ny * samples + 182;
  pFile.put( (offset & 0xff000000) / 16777216) ;
  pFile.put( (offset & 0x00ff0000) / 65536 );
  pFile.put( (offset & 0x0000ff00) / 256 );
  pFile.put( (offset & 0x000000ff) );

  // Compression //010300030000000100010000
  pFile.put(0x01); // tag
  pFile.put(0x03); // tag

  pFile.put(0x00); // tagtype
  pFile.put(0x03); // tagtype

  pFile.put(0x00); // value count
  pFile.put(0x00); // value count
  pFile.put(0x00); // value count
  pFile.put(0x01); // value count

  pFile.put(0x00);
  pFile.put(0x01);

  pFile.put(0x00); // space
  pFile.put(0x00); // space

  // interpolation //010600030000000100020000
  pFile.put(0x01); // tag
  pFile.put(0x06); // tag

  pFile.put(0x00); // tagtype
  pFile.put(0x03); // tagtype

  pFile.put(0x00); // value count
  pFile.put(0x00); // value count
  pFile.put(0x00); // value count
  pFile.put(0x01); // value count

  pFile.put(0x00);
  pFile.put(0x02);

  pFile.put(0x00); // space
  pFile.put(0x00); // space

  // strip offset  //011100040000000100000008
  pFile.put(0x01); // tag
  pFile.put(0x11); // tag

  pFile.put(0x00); // tagtype
  pFile.put(0x04); // tagtype

  pFile.put(0x00); // value count
  pFile.put(0x00); // value count
  pFile.put(0x00); // value count
  pFile.put(0x01); // value count

  pFile.put(0x00);
  pFile.put(0x00);

  pFile.put(0x00);
  pFile.put(0x08);

  //orientation 011200030000000100010000
  pFile.put(0x01); // tag
  pFile.put(0x12); // tag

  pFile.put(0x00); // tagtype
  pFile.put(0x03); // tagtype

  pFile.put(0x00); // value count
  pFile.put(0x00); // value count
  pFile.put(0x00); // value count
  pFile.put(0x01); // value count

  pFile.put(0x00);
  pFile.put(0x01);

  pFile.put(0x00); // space
  pFile.put(0x00); // space

  // samples per pixel 011500030000000100030000
  pFile.put(0x01); // tag
  pFile.put(0x15); // tag

  pFile.put(0x00); // tagtype
  pFile.put(0x03); // tagtype

  pFile.put(0x00); // value count
  pFile.put(0x00); // value count
  pFile.put(0x00); // value count
  pFile.put(0x01); // value count

  pFile.put(0x00);
  pFile.put((char)samples);

  pFile.put(0x00); // space
  pFile.put(0x00); // space

  // rows per strip 0116000300000001
  pFile.put(0x01); // tag
  pFile.put(0x16); // tag

  pFile.put(0x00); // tagtype
  pFile.put(0x03); // tagtype

  pFile.put(0x00); // value count
  pFile.put(0x00); // value count
  pFile.put(0x00); // value count
  pFile.put(0x01); // value count

  pFile.put( (ny & 0xff00) / 256);
  pFile.put( (ny & 0x00ff) );

  pFile.put(0x00); // space
  pFile.put(0x00); // space

  // byte count  0117000400000001
  pFile.put(0x01); // tag
  pFile.put(0x17); // tag

  pFile.put(0x00); // tagtype
  pFile.put(0x04); // tagtype

  pFile.put(0x00); // value count
  pFile.put(0x00); // value count
  pFile.put(0x00); // value count
  pFile.put(0x01); // value count

  offset = nx * ny * samples;
  pFile.put((offset & 0xff000000) / 16777216 );
  pFile.put((offset & 0x00ff0000) / 65536 );
  pFile.put((offset & 0x0000ff00) / 256 );
  pFile.put((offset & 0x000000ff) );

  // minimum sample value 0118000300000003
  pFile.put(0x01); // tag
  pFile.put(0x18); // tag

  pFile.put(0x00); // tagtype
  pFile.put(0x03); // tagtype

  pFile.put(0x00); // value count
  pFile.put(0x00); // value count
  pFile.put(0x00); // value count
  pFile.put((char)samples); // value count

  offset = nx * ny * samples + 188;
  pFile.put((offset & 0xff000000) / 16777216 );
  pFile.put((offset & 0x00ff0000) / 65536 );
  pFile.put((offset & 0x0000ff00) / 256 );
  pFile.put((offset & 0x000000ff) );

  // maximum sample value 0119000300000003
  pFile.put(0x01); // tag
  pFile.put(0x19); // tag

  pFile.put(0x00); // tagtype
  pFile.put(0x03); // tagtype

  pFile.put(0x00); // value count
  pFile.put(0x00); // value count
  pFile.put(0x00); // value count
  pFile.put((char)samples); // value count

  offset = nx * ny * samples + 194;
  pFile.put((offset & 0xff000000) / 16777216 );
  pFile.put((offset & 0x00ff0000) / 65536 );
  pFile.put((offset & 0x0000ff00) / 256 );
  pFile.put((offset & 0x000000ff) );

  //planar configuration tag,  011c00030000000100010000
  pFile.put(0x01); // tag
  pFile.put(0x1c); // tag

  pFile.put(0x00); // tagtype
  pFile.put(0x03); // tagtype

  pFile.put(0x00); // value count
  pFile.put(0x00); // value count
  pFile.put(0x00); // value count
  pFile.put(0x01); // value count

  pFile.put(0x00); // value
  pFile.put(0x01); // value
  pFile.put(0x00); // value
  pFile.put(0x00); // value

  // sample format 0153000300000003
  pFile.put(0x01); // tag
  pFile.put(0x53); // tag

  pFile.put(0x00); // tagtype
  pFile.put(0x03); // tagtype

  pFile.put(0x00); // value count
  pFile.put(0x00); // value count
  pFile.put(0x00); // value count
  pFile.put(0x03); // value count

  offset = nx * ny * samples + 200;
  pFile.put((offset & 0xff000000) / 16777216 );
  pFile.put((offset & 0x00ff0000) / 65536 );
  pFile.put((offset & 0x0000ff00) / 256 );
  pFile.put((offset & 0x000000ff) );
  // dic end 00 00 00 00
  pFile.put(0x00);
  pFile.put(0x00);
  pFile.put(0x00);
  pFile.put(0x00);

  // bits color channel
  pFile.put(0x00);
  pFile.put(0x08);
  pFile.put(0x00);
  pFile.put(0x08);
  pFile.put(0x00);
  pFile.put(0x08);
  // minimum
  pFile.put(0x00);
  pFile.put(0x00);
  pFile.put(0x00);
  pFile.put(0x00);
  pFile.put(0x00);
  pFile.put(0x00);
  // maximum
  pFile.put(0x00);
  pFile.put(0xff);
  pFile.put(0x00);
  pFile.put(0xff);
  pFile.put(0x00);
  pFile.put(0xff);
  //samples per pixel
  pFile.put(0x00);
  pFile.put(0x01);
  pFile.put(0x00);
  pFile.put(0x01);
  pFile.put(0x00);
  pFile.put(0x01);

  pFile.close();





}
