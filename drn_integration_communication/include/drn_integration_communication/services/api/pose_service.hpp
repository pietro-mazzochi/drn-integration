#ifndef DRN_INTEGRATION_COMMUNICATION_POSE_SERVICE_HPP
#define DRN_INTEGRATION_COMMUNICATION_POSE_SERVICE_HPP

#include <pqxx/pqxx>
#include <nlohmann/json.hpp>
#include <vector>
#include <optional>
#include <drn_integration_communication/entities/api/pose.hpp>

namespace drn_integration_communication
{
    class PoseService
    {
    public:
        static void create(pqxx::connection &connection_ptr, const Pose &pose);
        static void createWithId(pqxx::connection &connection_ptr, const Pose &pose);
        static std::vector<Pose> getAll(pqxx::connection &connection_ptr);
        static std::optional<Pose> getById(pqxx::connection &connection_ptr, int id);
        static void update(pqxx::connection &connection_ptr, const Pose &pose);
        static void remove(pqxx::connection &connection_ptr, int id);
    };
}

#endif