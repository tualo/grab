#include <pylon/PylonIncludes.h>
#include <stdio.h>
#include <cstdlib>
#include <fstream>
#include <pylon/gige/BaslerGigEInstantCamera.h>

#include <sys/time.h>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"


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


double contrast_measure( const cv::Mat&img )
{
    cv::Mat dx, dy;
    cv::Sobel( img, dx, CV_32F, 1, 0, 3 );
    cv::Sobel( img, dy, CV_32F, 0, 1, 3 );
    cv::magnitude( dx, dy, dx );
    return cv::sum(dx)[0];
}


void showImage(cv::Mat& src, int scale, int timeout){
  cv::Mat rotated=src.clone();
  int x=src.cols / scale;
  int y=src.rows / scale;

  cv::Mat res = cv::Mat(x, y, CV_32FC3);
  cv::resize(rotated, res, cv::Size(x, y), 0, 0, 3);
  cv::namedWindow("DEBUG", CV_WINDOW_AUTOSIZE );
  cv::imshow("DEBUG", res );
  cv::waitKey(timeout);
}

int main(int argc, char* argv[]) {
    int m=0;
    int i=0;
    int mainAVGSum = 0;
    int exitCode = 0;
    int grabExposure = 1000;
    int grabHeight = 128;
    bool grabAutoExposure = false;
    bool debug_window = false;
    bool barcode_light_correction=false;
    int debug_window_scale = 2;
    int debug_window_timeout = 10;




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

    if(const char* env_bc_light = std::getenv("BARCODE_LIGHT_CORRECTION")){
      if (atoi(env_bc_light)==1){
        barcode_light_correction = true;
      }
    }

    if(const char* env_dbg_wnd = std::getenv("DEBUG_WINDOW")){
      if (atoi(env_dbg_wnd)==1){
        debug_window = true;
      }
    }


    if(const char* env_dbg_wnd_tmr = std::getenv("DEBUG_WINDOW_TIMEOUT")){
      debug_window_timeout = atoi(env_dbg_wnd_tmr);
    }

    if(const char* env_dbg_wnd_scl = std::getenv("DEBUG_WINDOW_SCALE")){
      debug_window_scale = atoi(env_dbg_wnd_scl);
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
          camera.ExposureAuto.SetValue(ExposureAuto_Continuous);
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
/*
scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 1);
scanner.set_config(zbar::ZBAR_CODE128, zbar::ZBAR_CFG_ENABLE, 1);
scanner.set_config(zbar::ZBAR_I25, zbar::ZBAR_CFG_ADD_CHECK, 1);

*/
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

                cv::Mat cv_image(cv::Size(width, height), CV_8UC1, pImageBuffer);


                if (barcode_light_correction==true){
                  cv::Mat lab_image;
                  cv::cvtColor(cv_image, lab_image, CV_BGR2Lab);

                  // Extract the L channel
                  std::vector<cv::Mat> lab_planes(3);
                  cv::split(lab_image, lab_planes);  // now we have the L image in lab_planes[0]

                  // apply the CLAHE algorithm to the L channel
                  cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
                  clahe->setClipLimit(4);
                  cv::Mat dst;
                  clahe->apply(lab_planes[0], dst);

                  // Merge the the color planes back into an Lab image
                  dst.copyTo(lab_planes[0]);
                  cv::merge(lab_planes, lab_image);

                  // convert back to RGB
                  cv::Mat image_clahe;
                  cv::cvtColor(lab_image, image_clahe, CV_Lab2BGR);
                  cv::cvtColor(image_clahe, cv_image, CV_BGR2GRAY);

                }

                if (debug_window==true){
                  showImage(cv_image,debug_window_scale,debug_window_timeout);
                }


                mainAVGSum = 0;
                m = (int)ptrGrabResult->GetImageSize();
                for(i=0;i<m;i+=3){
                  mainAVGSum+= (uint32_t) pImageBuffer[i];
                }

		            double contrast = contrast_measure( cv_image );
                std::cout << "contrast: " << contrast << "" << std::endl;
                std::cout << "AVG: " <<  (mainAVGSum/m) << "" << std::endl;


                // wrap image data
                //Image image(width, height, "Y800", pImageBuffer, width * height);
                Image image(cv_image.cols, cv_image.rows, "Y800", (uchar *)cv_image.data, cv_image.cols * cv_image.rows);


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
