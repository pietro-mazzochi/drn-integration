#ifndef DRN_INTEGRATION_COMUNICATION_POSE_H
#define DRN_INTEGRATION_COMUNICATION_POSE_H

#include <stdio.h>
#include <numeric> 

namespace drn_integration_communication
{
    class Pose
    {
        public:
            int id;
            double position_x;
            double position_y;
            double position_z;
            double orientation_x;
            double orientation_y;
            double orientation_z;
            double orientation_w;

            Pose(int id, const double position_x, const double position_y, const double position_z, const double orientation_x, const double orientation_y, const double orientation_z, const double orientation_w);
    };        
}
#endif
