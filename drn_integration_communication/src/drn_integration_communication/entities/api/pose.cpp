#include <drn_integration_communication/entities/api/pose.hpp>

namespace drn_integration_communication
{
    Pose::Pose(
        int id,
        const double position_x,
        const double position_y,
        const double position_z,
        const double orientation_x,
        const double orientation_y,
        const double orientation_z,
        const double orientation_w)
        : id(id),
          position_x(position_x),
          position_y(position_y),
          position_z(position_z),
          orientation_x(orientation_x),
          orientation_y(orientation_y),
          orientation_z(orientation_z),
          orientation_w(orientation_w) {}
}
