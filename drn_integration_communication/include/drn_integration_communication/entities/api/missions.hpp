#ifndef DRN_INTEGRATION_COMUNICATION_MISSIONS_H
#define DRN_INTEGRATION_COMUNICATION_MISSIONS_H

#include <stdio.h>
#include <numeric> 
#include <string>

namespace drn_integration_communication
{
    class Mission
    {
        public:
            int id;
            std::string title;
            std::string type;
            int end_id;
            std::string mission_direction;

            Mission(
                int id, 
                const std::string& title, 
                const std::string& type, 
                int end_id, 
                const std::string& mission_direction
            );
    };        
}
#endif