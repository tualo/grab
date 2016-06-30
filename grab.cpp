// grab.cpp

#include <pylon/PylonIncludes.h>
#include <stdio.h>
#include <cstdlib>
#include <fstream>
#include <pylon/gige/BaslerGigEInstantCamera.h>

#include <sys/time.h>



// needed for beep
#include <iostream>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/kd.h>
#include <unistd.h>


// Namespace for using pylon objects.
using namespace Pylon;
using namespace GenApi;
using namespace std;
typedef Pylon::CBaslerGigEInstantCamera Camera_t;
using namespace Basler_GigECameraParams;
struct timeval ts;
// Number of images to be grabbed.
static const uint32_t c_countOfImagesToGrab = 10000;
std::string prefix = "/tmp/";





// write tiff tags
void writetags(ofstream *pFile,int nx,int ny){
  int offset =0;
  int samples=3;
  // number of dictionary entries
  pFile->put(0x00);
  pFile->put(0x0e);


  // width
  pFile->put(0x01); // tag
  pFile->put(0x00); // tag

  pFile->put(0x00); // tagtype
  pFile->put(0x03); // tagtype

  pFile->put(0x00); // value count
  pFile->put(0x00); // value count
  pFile->put(0x00); // value count
  pFile->put(0x01); // value count

  pFile->put( (nx & 0xff00) / 256 );
  pFile->put( (nx & 0x00ff) );

  pFile->put(0x00); // space
  pFile->put(0x00); // space


  // height
  pFile->put(0x01); // tag
  pFile->put(0x01); // tag

  pFile->put(0x00); // tagtype
  pFile->put(0x03); // tagtype

  pFile->put(0x00); // value count
  pFile->put(0x00); // value count
  pFile->put(0x00); // value count
  pFile->put(0x01); // value count

  pFile->put( (ny & 0xff00) / 256 );
  pFile->put( (ny & 0x00ff) );

  pFile->put(0x00); // space
  pFile->put(0x00); // space

  // bits per sample
  pFile->put(0x01); // tag
  pFile->put(0x02); // tag

  pFile->put(0x00); // tagtype
  pFile->put(0x03); // tagtype

  pFile->put(0x00); // value count
  pFile->put(0x00); // value count
  pFile->put(0x00); // value count
  pFile->put((char) samples); // value count


  offset = nx * ny * samples + 182;
  pFile->put( (offset & 0xff000000) / 16777216) ;
  pFile->put( (offset & 0x00ff0000) / 65536 );
  pFile->put( (offset & 0x0000ff00) / 256 );
  pFile->put( (offset & 0x000000ff) );

  // Compression //010300030000000100010000
  pFile->put(0x01); // tag
  pFile->put(0x03); // tag

  pFile->put(0x00); // tagtype
  pFile->put(0x03); // tagtype

  pFile->put(0x00); // value count
  pFile->put(0x00); // value count
  pFile->put(0x00); // value count
  pFile->put(0x01); // value count

  pFile->put(0x00);
  pFile->put(0x01);

  pFile->put(0x00); // space
  pFile->put(0x00); // space

  // interpolation //010600030000000100020000
  pFile->put(0x01); // tag
  pFile->put(0x06); // tag

  pFile->put(0x00); // tagtype
  pFile->put(0x03); // tagtype

  pFile->put(0x00); // value count
  pFile->put(0x00); // value count
  pFile->put(0x00); // value count
  pFile->put(0x01); // value count

  pFile->put(0x00);
  pFile->put(0x02);

  pFile->put(0x00); // space
  pFile->put(0x00); // space

  // strip offset  //011100040000000100000008
  pFile->put(0x01); // tag
  pFile->put(0x11); // tag

  pFile->put(0x00); // tagtype
  pFile->put(0x04); // tagtype

  pFile->put(0x00); // value count
  pFile->put(0x00); // value count
  pFile->put(0x00); // value count
  pFile->put(0x01); // value count

  pFile->put(0x00);
  pFile->put(0x00);

  pFile->put(0x00);
  pFile->put(0x08);

  //orientation 011200030000000100010000
  pFile->put(0x01); // tag
  pFile->put(0x12); // tag

  pFile->put(0x00); // tagtype
  pFile->put(0x03); // tagtype

  pFile->put(0x00); // value count
  pFile->put(0x00); // value count
  pFile->put(0x00); // value count
  pFile->put(0x01); // value count

  pFile->put(0x00);
  pFile->put(0x01);

  pFile->put(0x00); // space
  pFile->put(0x00); // space

  // samples per pixel 011500030000000100030000
  pFile->put(0x01); // tag
  pFile->put(0x15); // tag

  pFile->put(0x00); // tagtype
  pFile->put(0x03); // tagtype

  pFile->put(0x00); // value count
  pFile->put(0x00); // value count
  pFile->put(0x00); // value count
  pFile->put(0x01); // value count

  pFile->put(0x00);
  pFile->put((char)samples);

  pFile->put(0x00); // space
  pFile->put(0x00); // space

  // rows per strip 0116000300000001
  pFile->put(0x01); // tag
  pFile->put(0x16); // tag

  pFile->put(0x00); // tagtype
  pFile->put(0x03); // tagtype

  pFile->put(0x00); // value count
  pFile->put(0x00); // value count
  pFile->put(0x00); // value count
  pFile->put(0x01); // value count

  pFile->put( (ny & 0xff00) / 256);
  pFile->put( (ny & 0x00ff) );

  pFile->put(0x00); // space
  pFile->put(0x00); // space

  // byte count  0117000400000001
  pFile->put(0x01); // tag
  pFile->put(0x17); // tag

  pFile->put(0x00); // tagtype
  pFile->put(0x04); // tagtype

  pFile->put(0x00); // value count
  pFile->put(0x00); // value count
  pFile->put(0x00); // value count
  pFile->put(0x01); // value count

  offset = nx * ny * samples;
  pFile->put((offset & 0xff000000) / 16777216 );
  pFile->put((offset & 0x00ff0000) / 65536 );
  pFile->put((offset & 0x0000ff00) / 256 );
  pFile->put((offset & 0x000000ff) );

  // minimum sample value 0118000300000003
  pFile->put(0x01); // tag
  pFile->put(0x18); // tag

  pFile->put(0x00); // tagtype
  pFile->put(0x03); // tagtype

  pFile->put(0x00); // value count
  pFile->put(0x00); // value count
  pFile->put(0x00); // value count
  pFile->put((char)samples); // value count

  offset = nx * ny * samples + 188;
  pFile->put((offset & 0xff000000) / 16777216 );
  pFile->put((offset & 0x00ff0000) / 65536 );
  pFile->put((offset & 0x0000ff00) / 256 );
  pFile->put((offset & 0x000000ff) );

  // maximum sample value 0119000300000003
  pFile->put(0x01); // tag
  pFile->put(0x19); // tag

  pFile->put(0x00); // tagtype
  pFile->put(0x03); // tagtype

  pFile->put(0x00); // value count
  pFile->put(0x00); // value count
  pFile->put(0x00); // value count
  pFile->put((char)samples); // value count

  offset = nx * ny * samples + 194;
  pFile->put((offset & 0xff000000) / 16777216 );
  pFile->put((offset & 0x00ff0000) / 65536 );
  pFile->put((offset & 0x0000ff00) / 256 );
  pFile->put((offset & 0x000000ff) );

  //planar configuration tag,  011c00030000000100010000
  pFile->put(0x01); // tag
  pFile->put(0x1c); // tag

  pFile->put(0x00); // tagtype
  pFile->put(0x03); // tagtype

  pFile->put(0x00); // value count
  pFile->put(0x00); // value count
  pFile->put(0x00); // value count
  pFile->put(0x01); // value count

  pFile->put(0x00); // value
  pFile->put(0x01); // value
  pFile->put(0x00); // value
  pFile->put(0x00); // value

  // sample format 0153000300000003
  pFile->put(0x01); // tag
  pFile->put(0x53); // tag

  pFile->put(0x00); // tagtype
  pFile->put(0x03); // tagtype

  pFile->put(0x00); // value count
  pFile->put(0x00); // value count
  pFile->put(0x00); // value count
  pFile->put(0x03); // value count

  offset = nx * ny * samples + 200;
  pFile->put((offset & 0xff000000) / 16777216 );
  pFile->put((offset & 0x00ff0000) / 65536 );
  pFile->put((offset & 0x0000ff00) / 256 );
  pFile->put((offset & 0x000000ff) );
  // dic end 00 00 00 00
  pFile->put(0x00);
  pFile->put(0x00);
  pFile->put(0x00);
  pFile->put(0x00);

  // bits color channel
  pFile->put(0x00);
  pFile->put(0x08);
  pFile->put(0x00);
  pFile->put(0x08);
  pFile->put(0x00);
  pFile->put(0x08);
  // minimum
  pFile->put(0x00);
  pFile->put(0x00);
  pFile->put(0x00);
  pFile->put(0x00);
  pFile->put(0x00);
  pFile->put(0x00);
  // maximum
  pFile->put(0x00);
  pFile->put(0xff);
  pFile->put(0x00);
  pFile->put(0xff);
  pFile->put(0x00);
  pFile->put(0xff);
  //samples per pixel
  pFile->put(0x00);
  pFile->put(0x01);
  pFile->put(0x00);
  pFile->put(0x01);
  pFile->put(0x00);
  pFile->put(0x01);
}

#include <pthread.h>

#define NUM_THREADS 100

struct thread_info {
  int index;
  std::string filename;
};
//typedef struct thread_info thread_info;
struct thread_info threadInfo[NUM_THREADS];// = {0,0,0,0};// = malloc(sizeof(struct thread_block_info));

void* processImage(void *data ){
  struct thread_info *tib;
  tib = (struct thread_info *)data;


  //std::cout << " inside thread " << tib->filename << std::endl;
  //std::string sfilename( tib->filename );
  std::string scmd = "nohup ocrs "+tib->filename+" &";
  const char* ccmd = scmd.c_str();
  int result = system(ccmd);


  pthread_exit((void*)42);
	return 0;
}


// write tiff header
// you have to run this function again at the end of the image
// because the height of the image is kown only at the end
void writeheader(ofstream *pFile, int nx,int ny){
  int offset =0;
  int samples=3;
  pFile->seekp(0,ios_base::beg);
  // tiff header 4d4d002a
  pFile->put(0x4d);
  pFile->put(0x4d);
  pFile->put(0x00);
  pFile->put(0x2a);

  offset = nx * ny * samples + 8;
  pFile->put( (offset & 0xff000000) / 16777216) ;
  pFile->put( (offset & 0x00ff0000) / 65536 );
  pFile->put( (offset & 0x0000ff00) / 256 );
  pFile->put( (offset & 0x000000ff) );

}
/*
Using device raL2048-48gm
Camera Device Information
=========================
Vendor           : Basler
Model            : raL2048-48gm
Firmware version : 105993-14;U;raL2048_48gm;V1.1-2;0

Camera Device Settings
======================
Old PixelFormat  : Mono8
New PixelFormat  : Mono8
startAVG: 18
stopAVG: 8
*/
int main(int argc, char* argv[])
{




    // The exit code of the sample application.
    int exitCode = 0;
    char filename[128];
    char result_filename[128];


    unsigned int offset,nx,ny;
    int adjustAVG = 1;

    int mainAVG = 0;
    int mainAVGSum = 0;
    int currentAVG = 0;

    int startAVG = 0;
    int stopAVG = 0;

    int i=0;
    int m=0;
    bool inimage = false;
    bool usetiff = true;
    //ofstream pFile;
    ofstream* pFile;// = new fstream();

    int imageCounter = 0;

    int grabExposure = 1000;
    int grabHeight = 32;
    int currentHeight = 0;


    if(const char* env_start = std::getenv("GRAB_START_AVG")){
      startAVG = atoi(env_start);
      adjustAVG=0;
    }
    if(const char* env_stop = std::getenv("GRAB_STOP_AVG")){
      stopAVG = atoi(env_stop);
    }

    if(const char* env_path = std::getenv("GRAB_PATH")){
      prefix = env_path;
    }
    if(const char* env_exp = std::getenv("GRAB_EXPOSURE")){
      grabExposure = atoi(env_exp);
      cout << "setting exposure to " << grabExposure;
    }
    if(const char* env_hgt = std::getenv("GRAB_HEIGHT")){
      grabHeight = atoi(env_hgt);
    }

    // Automagically call PylonInitialize and PylonTerminate to ensure the pylon runtime system
    // is initialized during the lifetime of this object.
    Pylon::PylonAutoInitTerm autoInitTerm;

    try
    {
        // Create an instant camera object with the camera device found first.
        //CInstantCamera camera( CTlFactory::GetInstance().CreateFirstDevice());

        // Only look for cameras supported by Camera_t
        CDeviceInfo info;
        info.SetDeviceClass( Camera_t::DeviceClass());

        // Create an instant camera object with the first found camera device matching the specified device class.
        Camera_t camera( CTlFactory::GetInstance().CreateFirstDevice( info));


        // Print the model name of the camera.
        cout << "Using device " << camera.GetDeviceInfo().GetModelName() << endl;

        INodeMap& nodemap = camera.GetNodeMap();

        // Open the camera for accessing the parameters.
        camera.Open();

        // Get camera device information.
        cout << "Camera Device Information" << endl
             << "=========================" << endl;
        cout << "Vendor           : "
             << CStringPtr( nodemap.GetNode( "DeviceVendorName") )->GetValue() << endl;
        cout << "Model            : "
             << CStringPtr( nodemap.GetNode( "DeviceModelName") )->GetValue() << endl;
        cout << "Firmware version : "
             << CStringPtr( nodemap.GetNode( "DeviceFirmwareVersion") )->GetValue() << endl << endl;

        // Camera settings.
        cout << "Camera Device Settings" << endl
             << "======================" << endl;


        camera.ExposureAuto.SetValue(ExposureAuto_Off);
        camera.ExposureTimeRaw.SetValue(grabExposure);
        //camera.ExposureTime.GetMin()

        CIntegerPtr width( nodemap.GetNode( "Width"));
        CIntegerPtr height( nodemap.GetNode( "Height"));
        height->SetValue(grabHeight);

        //CIntegerPtr exposure( nodemap.GetNode( "Exposure"));
        //exposure->SetValue(12000);

        // Access the PixelFormat enumeration type node.
        CEnumerationPtr pixelFormat( nodemap.GetNode( "PixelFormat"));
        // Remember the current pixel format.
        String_t oldPixelFormat = pixelFormat->ToString();
        cout << "Old PixelFormat  : " << oldPixelFormat << endl;

        // Set the pixel format to Mono8 if available.
        if ( IsAvailable( pixelFormat->GetEntryByName( "Mono8")))
        {
            pixelFormat->FromString( "Mono8");
            cout << "New PixelFormat  : " << pixelFormat->ToString() << endl;
        }


        // The parameter MaxNumBuffer can be used to control the count of buffers
        // allocated for grabbing. The default value of this parameter is 10.
        camera.MaxNumBuffer = 10;

        // Start the grabbing of c_countOfImagesToGrab images.
        // The camera device is parameterized with a default configuration which
        // sets up free-running continuous acquisition.
        camera.StartGrabbing();// c_countOfImagesToGrab);

        // This smart pointer will receive the grab result data.
        CGrabResultPtr ptrGrabResult;

        int cthread=0;
        // Camera.StopGrabbing() is called automatically by the RetrieveResult() method
        // when c_countOfImagesToGrab images have been retrieved.
        while ( camera.IsGrabbing())
        {
            // Wait for an image and then retrieve it. A timeout of 5000 ms is used.
            camera.RetrieveResult( 5000, ptrGrabResult, TimeoutHandling_ThrowException);

            // Image grabbed successfully?
            if (ptrGrabResult->GrabSucceeded())
            {
                // Access the image data.
                //cout << "SizeX: " << ptrGrabResult->GetWidth() << endl;
                //cout << "SizeY: " << ptrGrabResult->GetHeight() << endl;
                const uint8_t *pImageBuffer = (uint8_t *) ptrGrabResult->GetBuffer();
                //cout << "Gray value of first pixel: " << (uint32_t) pImageBuffer[0] << endl << endl;
                mainAVGSum = 0;
                m = (int)ptrGrabResult->GetImageSize();
                char dst[m*3];
                for(i=0;i<m;i++){
                  mainAVGSum+= (uint32_t) pImageBuffer[i];

                  dst[i*3]=pImageBuffer[i];
                  dst[i*3+1]=pImageBuffer[i];
                  dst[i*3+2]=pImageBuffer[i];

                }
                currentAVG = (mainAVGSum/m);
                if ( adjustAVG == 1){
                  mainAVG = currentAVG;
                  startAVG =  mainAVG + 10;
                  stopAVG =  mainAVG;// - 10;
                  cout << "startAVG: " << startAVG << endl;
                  cout << "stopAVG: " << stopAVG << endl;
                  adjustAVG = 0;
                }else if (inimage){
                  if (currentAVG<=stopAVG){
                    if(usetiff){
                       ny = currentHeight;
                       nx = ptrGrabResult->GetWidth();
                       writetags(pFile,nx,ny);
                       writeheader(pFile,nx,ny);
                    }
                    pFile->close();


                    rename(filename,result_filename);

                    cout << endl;
                    inimage = false;

                  }else{
                    currentHeight += ptrGrabResult->GetHeight();
                    if (currentHeight<10000){
                      pFile->write(dst,m*3);
                    }
                  }

                }else if (!inimage){
                  if (currentAVG>=startAVG){
                    inimage=true;
                    gettimeofday(&ts,NULL);
                    // getting the customer number
                    std::string customer;
                    std::string line;
                    ifstream myfile ("/opt/grab/customer.txt");
                    if (myfile.is_open())
                    {
                      while ( getline (myfile,line) )
                      {
                        customer = line;
                      }
                      myfile.close();
                    }

                    std::string format = prefix+std::string(customer+"N%012d.%06d.bin");
                    std::string result_format = prefix+std::string(customer+"N%012d.%06d.tiff");
                    sprintf(filename, format.c_str() , ts.tv_sec, ts.tv_usec);
                    sprintf(result_filename, result_format.c_str() , ts.tv_sec, ts.tv_usec);
                    cout << result_filename;


                    pFile = new ofstream(filename, ios::out | ios::binary);
                    currentHeight = 0;
                    if (usetiff){
                      writeheader(pFile,nx,ny);
                    }
                    currentHeight += ptrGrabResult->GetHeight();
                    pFile->write(dst,m*3);

                  }
                }
                //cout << "data length: " << m << endl;
                //cout << "data avg: " << (mainAVGSum/m) << endl;


            }
            else
            {
                cout << "Error: " << ptrGrabResult->GetErrorCode() << " " << ptrGrabResult->GetErrorDescription() << endl;
            }
        }
    }
    catch (GenICam::GenericException &e)
    {
        // Error handling.
        cerr << "An exception occurred." << endl
        << e.GetDescription() << endl
        << e.what() << endl;
        exitCode = 1;
    }

    // Comment the following two lines to disable waiting on exit.
    //cerr << endl << "Press Enter to exit." << endl;
    //while( cin.get() != '\n');

    if (inimage){
      pFile->close();
    }
    return exitCode;
}
