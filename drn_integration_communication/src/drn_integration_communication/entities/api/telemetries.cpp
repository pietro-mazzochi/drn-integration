#include <drn_integration_communication/entities/api/telemetries.hpp>

namespace drn_integration_communication
{
    Telemetrie::Telemetrie(int id, const std::string &data, const std::string& pos_x, const std::string& pos_y, const std::string& angle, const std::string& battery, const std::string& tension, const std::string& velocity)

        : id(id), data(data), pos_x(pos_x), pos_y(pos_y), angle(angle), battery(battery), tension(tension), velocity(velocity){}
}