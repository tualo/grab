// grab.cpp
/*
   This sample illustrates how to grab and process images using the CInstantCamera class.
   The images are grabbed and processed asynchronously, i.e.,
   while the application is processing a buffer, the acquisition of the next buffer is done
   in parallel.

   The CInstantCamera class uses a pool of buffers to retrieve image data
   from the camera device. Once a buffer is filled and ready,
   the buffer can be retrieved from the camera object for processing. The buffer
   and additional image data are collected in a grab result. The grab result is
   held by a smart pointer after retrieval. The buffer is automatically reused
   when explicitly released or when the smart pointer object is destroyed.
*/

// Include files to use the PYLON API.
#include <pylon/PylonIncludes.h>
#ifdef PYLON_WIN_BUILD
#    include <pylon/PylonGUI.h>
#endif

#include <stdio.h>
#include <cstdlib>
// Namespace for using pylon objects.
using namespace Pylon;
using namespace GenApi;

// Namespace for using cout.
using namespace std;

// Number of images to be grabbed.
static const uint32_t c_countOfImagesToGrab = 10000;

const char Filename[] = "NodeMap.pfs";

#include <pylon/gige/BaslerGigEInstantCamera.h>
typedef Pylon::CBaslerGigEInstantCamera Camera_t;
using namespace Basler_GigECameraParams;



std::string prefix = "/tmp/";


void BM_WriteHexString(FILE *fptr,char *s)
{
   unsigned int i,c;
   char hex[3];

   for (i=0;i<strlen(s);i+=2) {
      hex[0] = s[i];
      hex[1] = s[i+1];
      hex[2] = '\0';
      sscanf(hex,"%X",&c);
      putc(c,fptr);
   }
}


int main(int argc, char* argv[])
{
    // The exit code of the sample application.
    int exitCode = 0;
    char filename[128];

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
    FILE* pFile;

    int imageCounter = 0;

    int grabExposure = 1000;
    int grabHeight = 32;
    int currentHeight = 0;

    if(const char* env_path = std::getenv("GRAB_PATH")){
      prefix = env_path;
    }
    if(const char* env_exp = std::getenv("GRAB_EXPOSURE")){
      grabExposure = atoi(env_exp);
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
        camera.StartGrabbing( c_countOfImagesToGrab);

        // This smart pointer will receive the grab result data.
        CGrabResultPtr ptrGrabResult;


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
                for(i=0;i<m;i++){
                  mainAVGSum+= (uint32_t) pImageBuffer[i];
                }

                currentAVG = (mainAVGSum/m);

                //cout << "AVG: " << currentAVG << endl;

                if ( adjustAVG == 1){
                  mainAVG = currentAVG;
                  startAVG =  mainAVG + mainAVG*0.1;
                  stopAVG =  mainAVG - mainAVG*0.1;
                  cout << "startAVG: " << startAVG << endl;
                  cout << "stopAVG: " << stopAVG << endl;

                  adjustAVG = 0;
                }else if (inimage){
                  if (currentAVG<=stopAVG){
                    if(usetiff){
                       ny = currentHeight;
                       nx = ptrGrabResult->GetWidth();

                       /* Write the footer */
                       WriteHexString(pFile,"000e");  /* The number of directory entries (14) */

                       /* Width tag, short int */
                       WriteHexString(pFile,"0100000300000001");
                       fputc((nx & 0xff00) / 256,pFile);    /* Image width */
                       fputc((nx & 0x00ff),pFile);
                       WriteHexString(pFile,"0000");

                       /* Height tag, short int */
                       WriteHexString(pFile,"0101000300000001");
                       fputc((ny & 0xff00) / 256,pFile);    /* Image height */
                       fputc((ny & 0x00ff),pFile);
                       WriteHexString(pFile,"0000");

                       /* Bits per sample tag, short int */
                       WriteHexString(pFile,"0102000300000003");
                       offset = nx * ny * 1 + 182;
                       putc((offset & 0xff000000) / 16777216,pFile);
                       putc((offset & 0x00ff0000) / 65536,pFile);
                       putc((offset & 0x0000ff00) / 256,pFile);
                       putc((offset & 0x000000ff),pFile);

                       /* Compression flag, short int */
                       WriteHexString(pFile,"010300030000000100010000");

                       /* Photometric interpolation tag, short int */
                       WriteHexString(pFile,"010600030000000100020000");

                       /* Strip offset tag, long int */
                       WriteHexString(pFile,"011100040000000100000008");

                       /* Orientation flag, short int */
                       WriteHexString(pFile,"011200030000000100010000");

                       /* Sample per pixel tag, short int */
                       WriteHexString(pFile,"011500030000000100010000");

                       /* Rows per strip tag, short int */
                       WriteHexString(pFile,"0116000300000001");
                       fputc((ny & 0xff00) / 256,pFile);
                       fputc((ny & 0x00ff),pFile);
                       WriteHexString(pFile,"0000");

                       /* Strip byte count flag, long int */
                       WriteHexString(pFile,"0117000400000001");
                       offset = nx * ny * 1;
                       putc((offset & 0xff000000) / 16777216,pFile);
                       putc((offset & 0x00ff0000) / 65536,pFile);
                       putc((offset & 0x0000ff00) / 256,pFile);
                       putc((offset & 0x000000ff),pFile);

                       /* Minimum sample value flag, short int */
                       WriteHexString(pFile,"0118000300000001");
                       offset = nx * ny * 1 + 188;
                       putc((offset & 0xff000000) / 16777216,pFile);
                       putc((offset & 0x00ff0000) / 65536,pFile);
                       putc((offset & 0x0000ff00) / 256,pFile);
                       putc((offset & 0x000000ff),pFile);

                       /* Maximum sample value tag, short int */
                       WriteHexString(pFile,"0119000300000001");
                       offset = nx * ny * 1 + 194;
                       putc((offset & 0xff000000) / 16777216,pFile);
                       putc((offset & 0x00ff0000) / 65536,pFile);
                       putc((offset & 0x0000ff00) / 256,pFile);
                       putc((offset & 0x000000ff),pFile);

                       /* Planar configuration tag, short int */
                       WriteHexString(pFile,"011c00030000000100010000");

                       /* Sample format tag, short int */
                       WriteHexString(pFile,"0153000300000003");
                       offset = nx * ny * 1 + 200;
                       putc((offset & 0xff000000) / 16777216,pFile);
                       putc((offset & 0x00ff0000) / 65536,pFile);
                       putc((offset & 0x0000ff00) / 256,pFile);
                       putc((offset & 0x000000ff),pFile);

                       /* End of the directory entry */
                       WriteHexString(pFile,"00000000");

                       /* Bits for each colour channel */
                       WriteHexString(pFile,"000800080008");

                       /* Minimum value for each component */
                       WriteHexString(pFile,"000000000000");

                       /* Maximum value per channel */
                       WriteHexString(pFile,"00ff");//00ff00ff");

                       /* Samples per pixel for each channel */
                       WriteHexString(pFile,"0001");//00010001");


                       // correcting the header
                       cout << "todo, orrecting the header" << endl;
                       /* Big endian & TIFF identifier */
                       offset = nx * ny * 1 + 8;
                       fseek ( pFile , 8 , SEEK_SET );
                       putc((offset & 0xff000000) / 16777216,pFile);
                       putc((offset & 0x00ff0000) / 65536,pFile);
                       putc((offset & 0x0000ff00) / 256,pFile);
                       putc((offset & 0x000000ff),pFile);
                    }

                    fclose(pFile);

                    cout << "stop image " << endl;
                    inimage = false;

                  }else{
                    // ok write the image
                    currentHeight += ptrGrabResult->GetHeight();
                    fwrite(pImageBuffer, 1, m, pFile);
                  }

                }else if (!inimage){
                  if (currentAVG>=startAVG){
                    cout << "start image " << imageCounter << endl;
                    inimage=true;



                    std::string format = prefix+std::string("%08d.tiff");
                    sprintf(filename, format.c_str() , imageCounter++);

                    pFile = fopen(filename, "wb");
                    currentHeight = 0;
                    if (usetiff){
                      /* Write the header */
                       WriteHexString(pFile,"4d4d002a");
                       /* Big endian & TIFF identifier */
                       offset = nx * ny * 1 + 8;
                       putc((offset & 0xff000000) / 16777216,pFile);
                       putc((offset & 0x00ff0000) / 65536,pFile);
                       putc((offset & 0x0000ff00) / 256,pFile);
                       putc((offset & 0x000000ff),pFile);
                    }

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
    cerr << endl << "Press Enter to exit." << endl;
    while( cin.get() != '\n');

    if (inimage){
      fclose(pFile);
    }
    return exitCode;
}
