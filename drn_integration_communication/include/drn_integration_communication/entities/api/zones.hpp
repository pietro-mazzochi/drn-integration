#ifndef DRN_INTEGRATION_COMUNICATION_ZONE_H
#define DRN_INTEGRATION_COMUNICATION_ZONE_H

#include <stdio.h>
#include <numeric> 
#include <string>
#include <vector>

namespace drn_integration_communication
{
    class Zone
    {
        public:
            int id;
            std::string max_velocity;
            std::vector<std::vector<double>> area;
            int map_id;
            int type_zone_id;

            Zone(int id, const std::string& max_velocity, const std::vector<std::vector<double>>& area, int map_id, int type_zone_id);
    };        
}
#endif