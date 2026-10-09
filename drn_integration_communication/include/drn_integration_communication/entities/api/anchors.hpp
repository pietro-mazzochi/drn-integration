#ifndef DRN_INTEGRATION_COMUNICATION_ANCHORS_H
#define DRN_INTEGRATION_COMUNICATION_ANCHORS_H

#include <stdio.h>
#include <numeric> 
#include <string>

namespace drn_integration_communication
{
    class Anchor
    {
        public:
            int id;
            std::string name;
            std::string pos_x;
            std::string pos_y;
            std::string angle;            

            Anchor(int id, const std::string& name, const std::string& pos_x, const std::string& pos_y, const std::string& angle);
    };        
}
#endif