#ifndef DRN_INTEGRATION_COMUNICATION_ZONETYPES_H
#define DRN_INTEGRATION_COMUNICATION_ZONETYPES_H

#include <stdio.h>
#include <numeric> 
#include <string>

namespace drn_integration_communication
{
    class ZoneType
    {
        public:
            int id;
            std::string name;            

            ZoneType(int id, const std::string& name);
    };        
}
#endif