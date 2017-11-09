Below is an example of a simple acidcam-cli plugin
its basically just an exported function with one argument.
You can use whatever OpenCV functions you want just modify the Matrix passed 
in and it will be concatinated with other filters then written to the video file.


/* Simple Filter - Plugin */

#include<opencv2/opencv.hpp>
#include<iostream>

extern "C" void filter(cv::Mat &frame) {
        unsigned int w = frame.cols;// frame width
        unsigned int h = frame.rows;// frame height
        static float index = 1.0f;
        for(unsigned int x = 0; x < w; ++x) {
                for(unsigned int y = 0; y < h; ++y) {
                        cv::Vec3b &pixel = frame.at<cv::Vec3b>(y, x);
                        for(unsigned int q = 0; q < 3; ++q)
                                pixel[q] = static_cast<unsigned char>(pixel[q]*index);
                }
        }
        index += 0.01f;
}



