/*
 * Acid Cam v2 - OpenCV Edition
 * written by Jared Bruni ( http://lostsidedead.com / https://github.com/lostjared )
 
 
 GitHub: http://github.com/lostjared
 Website: http://lostsidedead.com
 YouTube: http://youtube.com/LostSideDead
 Instagram: http://instagram.com/jaredbruni
 Twitter: http://twitter.com/jaredbruni
 Facebook: http://facebook.com/LostSideDead0x
 
 You can use this program free of charge and redistrubute as long
 as you do not charge anything for this program. This program is 100%
 Free.
 
 BSD 2-Clause License
 
 Copyright (c) 2017, Jared Bruni
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 
 * Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.
 
 * Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
 */
#include"acidcam-cli.hpp"
#include<unistd.h>
/* required to be declared in source file */

/*
 
 Command Line Arguments
 -l List Filters

*/

cv::Mat blend_image;
bool blend_set = false;

void custom_filter(cv::Mat &frame) {}
void ac::plugin(cv::Mat &frame) {}



void listFilters() {
    std::cout << "List of Filters by Index\n";
    for(unsigned int i = 0; i < ac::draw_max-3; ++i) {
        std::cout << std::setw(4) << std::left << i << std::setw(50) << std::left << ac::draw_strings[i] << "\n";
    }
}


/* main function */
int main(int argc, char **argv) {
    ac::fill_filter_map();
    if(argc > 1) {
        int opt = 0;
        while((opt = getopt(argc, argv, "l")) != -1) {
            switch(opt) {
                case 'l':
                    listFilters();
                    break;
            }
        }
    }
	return 0;
}
