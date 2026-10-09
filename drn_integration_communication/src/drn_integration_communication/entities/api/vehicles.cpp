#include <drn_integration_communication/entities/api/vehicles.hpp>

namespace drn_integration_communication
{
    Vehicle::Vehicle(int id, const std::string& code, const std::string &name, const std::string &model)
        : id(id), code(code), name(name), model(model) {}
}