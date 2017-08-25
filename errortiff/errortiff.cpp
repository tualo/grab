
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <zbar.h>
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "args.hxx"
#include <list>
#include <vector>

int main(int argc, char* argv[])
{


    int exitCode = 0;

    args::ArgumentParser parser("Try to create a Image from an error bin file.", "");
    args::HelpFlag help(parser, "help", "Display this help menu", { "help"});
    args::Flag debug(parser, "debug", "Show debug messages", {'d', "debug"});

    args::ValueFlag<int> width(parser, "width", "width (2048)", { 'w',"width"});
    args::ValueFlag<std::string> infilename(parser, "infilename", "The filename", {'f',"infile"});
    args::ValueFlag<std::string> outfilename(parser, "outfilename", "The filename", {'o',"outfile"});


    try{
        parser.ParseCLI(argc, argv);
    }catch (args::Help){
        std::cout << parser;
        return 0;
    }catch (args::ParseError e){
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }


    int length;
    char * buffer;

if (infilename){
    int offset=16;
    std::ifstream is;
    is.open(args::get(infilename), std::ifstream::binary );
    // get length of file:
    is.seekg(0, is.end);
    length = (int)is.tellg() - offset;
    is.seekg(0, is.beg);
    // allocate memory:
    buffer = new char [length];
    is.read (buffer,offset);
    // read data as a block:
    is.read (buffer,length);
    is.close();

    int int_width=2048;

    if (width){
      int_width = args::get(width);
    }
    int int_height = (int) (  ( (length)/3 ) / int_width ) ;

    std::cout << "Length: " << (length)/3 << std::endl;
    std::cout << "Width: " << int_width << std::endl;
    std::cout << "Height: " << int_height << std::endl;

    cv::Mat mat = cv::Mat(int_height, int_width, CV_8UC3, cv::Scalar(0));
    std::memcpy( mat.ptr(), buffer, int_height*int_width*3);

    if (outfilename){
      cv::imwrite( args::get(outfilename), mat );
    }
    //currentImage = cv::Mat(imageHeight, camera.Width.GetValue(), CV_8UC1, cv::Scalar(0));
    //std::memcpy( currentImage.ptr(), currentImage.ptr()+grabbedImageSize, totalImageSize-grabbedImageSize);
}

    return exitCode;
}
