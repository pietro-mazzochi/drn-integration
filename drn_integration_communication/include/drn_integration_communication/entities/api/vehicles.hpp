#ifndef DRN_INTEGRATION_COMUNICATION_VEHICLES_H
#define DRN_INTEGRATION_COMUNICATION_VEHICLES_H

#include <stdio.h>
#include <numeric> 
#include <string>

namespace drn_integration_communication
{
    class Vehicle
    {
        public:
            int id;
            std::string code;
            std::string name;
            std::string model;            

            Vehicle(int id, const std::string& code, const std::string& name, const std::string& model);
    };        
}
#endif