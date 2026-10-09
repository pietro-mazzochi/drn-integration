#ifndef DRN_INTEGRATION_COMUNICATION_SETTINGS_H
#define DRN_INTEGRATION_COMUNICATION_SETTINGS_H

#include <stdio.h>
#include <numeric> 
#include <string>

namespace drn_integration_communication
{
    class Setting
    {
        public:
            int id;
            std::string key;
            std::string value;            

            Setting(int id, const std::string& key, const std::string& value);
    };        
}
#endif