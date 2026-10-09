#ifndef DRN_INTEGRATION_COMUNICATION_MAPS_H
#define DRN_INTEGRATION_COMUNICATION_MAPS_H

#include <stdio.h>
#include <numeric> 
#include <string>

namespace drn_integration_communication
{
    class Map
    {
        public:
            int id;
            std::string name;
            std::string mode;
            std::string resolution;
            std::string origin_x;
            std::string origin_y;
            std::string origin_z;
            std::string negate;
            std::string occupied_thresh;
            std::string free_thresh;   

            Map(int id, const std::string& name, const std::string& mode, const std::string& resolution, const std::string& origin_x, const std::string& origin_y, const std::string& origin_z, const std::string& negate, const std::string& occupied_thresh, const std::string& free_thresh);
    };        
}
#endif