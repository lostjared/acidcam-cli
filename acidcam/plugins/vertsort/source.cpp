/* Simple Filter - Plugin */

#include<opencv2/opencv.hpp>
#include<iostream>
#include<vector>
#include<functional>
#include<algorithm>

extern "C" void filter(cv::Mat &frame) {
    int w = frame.cols;// frame width
    int h = frame.rows;// frame height
    static std::vector<unsigned int> v;// static vector of unsigned int
    v.reserve(w);// reserve w bytes
    for(int i = 0; i < w; ++i) { // top to bottom
        for(int z = 0; z < h; ++z) { // left to right
            //unsigned int value = frame.at<unsigned int>(z, i);
            // grab pixel reference
            cv::Vec3b &value = frame.at<cv::Vec3b>(z, i);
            unsigned int vv = 0;
            // unsigned char * of vv
            unsigned char *cv = (unsigned char*)&vv;
            // set RGB values
            cv[0] = value[0];
            cv[1] = value[1];
            cv[2] = value[2];
            cv[3] = 0;
            // push back into vector v
            v.push_back(vv);
        }
        // sort vector v
        std::sort(v.begin(), v.end(), std::greater<int>());
        for(int q = 0; q < h; ++q) {// left to right
            // unsigned char pointer of vector v at index i
            unsigned char *value = (unsigned char*)&v[q];
            // get pixel reference
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(q, i);
            // add to pixel without scaling
            pixel[0] = value[0];
            pixel[1] = value[1];
            pixel[2] = value[2];
        }
        v.erase(v.begin(), v.end());
    }
}
