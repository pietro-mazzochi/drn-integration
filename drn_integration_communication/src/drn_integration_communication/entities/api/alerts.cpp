#include <drn_integration_communication/entities/api/alerts.hpp>

namespace drn_integration_communication
{
    Alert::Alert(int id, const std::string& uuid, const std::string& active, const std::string& error_code, const std::string& data, const std::string& message, const std::string& timestamp, const std::string& supervisory_registration)
        : id(id), uuid(uuid), active(active), error_code(error_code), data(data), message(message), timestamp(timestamp), supervisory_registration(supervisory_registration) {}
}