#ifndef DRN_INTEGRATION_COMUNICATION_NODES_H
#define DRN_INTEGRATION_COMUNICATION_NODES_H

#include <stdio.h>
#include <numeric>
#include <string>

namespace drn_integration_communication
{
    class Node
    {
    public:
        int id;
        std::string pos_x;
        std::string pos_y;
        std::string angle;
        std::string deleted_supervisory;
        int map_id;
        int node_type;
        std::string direction;
        std::string label_id;

        Node(
            int id, 
            const std::string& pos_x, 
            const std::string& pos_y, 
            const std::string& angle, 
            const std::string& deleted_supervisory, 
            int map_id, 
            int node_type, 
            const std::string& direction, 
            const std::string& label_id
        );
    };
}
#endif