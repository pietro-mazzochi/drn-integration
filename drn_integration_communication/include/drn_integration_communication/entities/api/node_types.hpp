#ifndef DRN_INTEGRATION_COMUNICATION_NODETYPES_H
#define DRN_INTEGRATION_COMUNICATION_NODETYPES_H

#include <stdio.h>
#include <numeric> 
#include <string>

namespace drn_integration_communication
{
    class NodeType
    {
        public:
            int id;
            std::string name;

            NodeType(int id, const std::string& name);
    };        
}
#endif