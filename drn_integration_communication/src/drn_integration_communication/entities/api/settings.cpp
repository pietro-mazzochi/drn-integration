#include <drn_integration_communication/entities/api/settings.hpp>

namespace drn_integration_communication
{
    Setting::Setting(int id, const std::string &key, const std::string &value)
        : id(id), key(key), value(value) {}
}