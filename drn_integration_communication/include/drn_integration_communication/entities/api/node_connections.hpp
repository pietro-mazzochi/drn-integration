#ifndef DRN_INTEGRATION_COMUNICATION_NODECONNECTIONS_H
#define DRN_INTEGRATION_COMUNICATION_NODECONNECTIONS_H

#include <stdio.h>
#include <numeric> 
#include <string>
#include <vector> 

namespace drn_integration_communication
{
    class NodeConnection
    {
        public:
            int id;
            std::vector<std::vector<double>> line;
            int source_node_id;
            int target_node_id;

            NodeConnection(int id, const std::vector<std::vector<double>>& line, int source_node_id, int target_node_id);
    };        
}
#endif