#ifndef DRN_INTEGRATION_BRINGUP_LIBTEST_H
#define DRN_INTEGRATION_BRINGUP_LIBTEST_H

#include <stdio.h>
#include <vector>
#include <functional> 
#include <numeric> 
#include <iostream>
#include <string>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>

namespace drn_integration_bringup
{
    class LibTest
    {
        public:
            std::string libTest();
    };        
}
#endif