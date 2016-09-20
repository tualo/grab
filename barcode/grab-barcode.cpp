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

#include <zbar.h>

using namespace Pylon;
using namespace GenApi;
using namespace std;
using namespace zbar;

typedef Pylon::CBaslerGigEInstantCamera Camera_t;
using namespace Basler_GigECameraParams;
struct timeval ts;
// Number of images to be grabbed.
static const uint32_t c_countOfImagesToGrab = 10000;


int main(int argc, char* argv[]) {

    int exitCode = 0;
    int grabExposure = 1000;
    int grabHeight = 128;
    bool grabAutoExposure = false;



    if(const char* env_auto_exp = std::getenv("GRAB_AUTO_EXPOSURE")){
      if (atoi(env_auto_exp)==1){
        grabAutoExposure=true;
        cout << "setting exposure to auto " << endl;
      }
    }

    if(const char* env_exp = std::getenv("GRAB_EXPOSURE")){
      grabExposure = atoi(env_exp);
      cout << "setting exposure to " << grabExposure << endl;
    }

    if(const char* env_hgt = std::getenv("GRAB_HEIGHT")){
      grabHeight = atoi(env_hgt);
      cout << "setting grab height to " << grabHeight << endl;
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

        if (grabAutoExposure==true){
          camera.ExposureAuto.SetValue(ExposureAuto_On);
        }else{
          camera.ExposureAuto.SetValue(ExposureAuto_Off);
          camera.ExposureTimeRaw.SetValue(grabExposure);
        }
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

        ImageScanner scanner;

        // configure the reader
        scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);

        // Camera.StopGrabbing() is called automatically by the RetrieveResult() method
        // when c_countOfImagesToGrab images have been retrieved.
        while ( camera.IsGrabbing())
        {
            // Wait for an image and then retrieve it. A timeout of 5000 ms is used.
            camera.RetrieveResult( 5000, ptrGrabResult, TimeoutHandling_ThrowException);

            // Image grabbed successfully?
            if (ptrGrabResult->GrabSucceeded())
            {

                unsigned char *pImageBuffer = (unsigned char *) ptrGrabResult->GetBuffer();
                int width = ptrGrabResult->GetWidth();
                int height = ptrGrabResult->GetHeight();
                // wrap image data
                Image image(width, height, "Y800", pImageBuffer, width * height);

                // scan the image for barcodes
                int n = scanner.scan(image);

                // extract results
                for(Image::SymbolIterator symbol = image.symbol_begin();
                    symbol != image.symbol_end();
                    ++symbol) {
                    // do something useful with results
                    cout << "decoded " << symbol->get_type_name()
                         << " symbol \"" << symbol->get_data() << '"' << endl;
                }

                // clean up
                image.set_data(NULL, 0);

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

    return exitCode;
}
