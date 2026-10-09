#include <drn_integration_communication/services/api/pose_service.hpp>
#include <stdexcept>

namespace drn_integration_communication
{

    void PoseService::create(pqxx::connection &connection_ptr, const Pose &pose)
    {
        pqxx::nontransaction W(connection_ptr);

        std::string query = "INSERT INTO poses (position_x, position_y, position_z, orientation_x, orientation_y, orientation_z, orientation_w) VALUES (" +
                            W.esc(std::to_string(pose.position_x)) + ", " +
                            W.esc(std::to_string(pose.position_y)) + ", " +
                            W.esc(std::to_string(pose.position_z)) + ", " +
                            W.esc(std::to_string(pose.orientation_x)) + ", " +
                            W.esc(std::to_string(pose.orientation_y)) + ", " +
                            W.esc(std::to_string(pose.orientation_z)) + ", " +
                            W.esc(std::to_string(pose.orientation_w)) + ");";

        W.exec(query);
        W.commit();
    }

    void PoseService::createWithId(pqxx::connection &connection_ptr, const Pose &pose)
    {
        pqxx::nontransaction W(connection_ptr);
    
        W.exec_params(
            "INSERT INTO poses (id, position_x, position_y, position_z, "
            "                    orientation_x, orientation_y, orientation_z, orientation_w) "
            "VALUES ($1,$2,$3,$4,$5,$6,$7,$8) "
            "ON CONFLICT (id) DO UPDATE SET "
            "  position_x = EXCLUDED.position_x, "
            "  position_y = EXCLUDED.position_y, "
            "  position_z = EXCLUDED.position_z, "
            "  orientation_x = EXCLUDED.orientation_x, "
            "  orientation_y = EXCLUDED.orientation_y, "
            "  orientation_z = EXCLUDED.orientation_z, "
            "  orientation_w = EXCLUDED.orientation_w;",
            pose.id,
            pose.position_x, pose.position_y, pose.position_z,
            pose.orientation_x, pose.orientation_y, pose.orientation_z, pose.orientation_w
        );
    
        W.commit();
    }
    
    std::vector<Pose> PoseService::getAll(pqxx::connection &connection_ptr)
    {
        pqxx::nontransaction N(connection_ptr);
        std::string query = "SELECT id, position_x, position_y, position_z, orientation_x, orientation_y, orientation_z, orientation_w FROM poses;";
        pqxx::result R(N.exec(query));

        std::vector<Pose> poses;

        for (const auto &row : R)
        {
            poses.emplace_back(
                row["id"].as<int>(),
                row["position_x"].as<double>(),
                row["position_y"].as<double>(),
                row["position_z"].as<double>(),
                row["orientation_x"].as<double>(),
                row["orientation_y"].as<double>(),
                row["orientation_z"].as<double>(),
                row["orientation_w"].as<double>());
        }

        return poses;
    }

    std::optional<Pose> PoseService::getById(pqxx::connection &connection_ptr, int id)
    {
        pqxx::nontransaction N(connection_ptr);
        std::string query = "SELECT id, position_x, position_y, position_z, orientation_x, orientation_y, orientation_z, orientation_w FROM poses WHERE id = " + std::to_string(id) + ";";
        pqxx::result R(N.exec(query));

        if (R.empty())
        {
            return std::nullopt;
        }

        auto row = R[0];
        return Pose(
            row["id"].as<int>(),
            row["position_x"].as<double>(),
            row["position_y"].as<double>(),
            row["position_z"].as<double>(),
            row["orientation_x"].as<double>(),
            row["orientation_y"].as<double>(),
            row["orientation_z"].as<double>(),
            row["orientation_w"].as<double>());
    }

    void PoseService::update(pqxx::connection &connection_ptr, const Pose &pose)
    {
        pqxx::nontransaction W(connection_ptr);

        std::string query =
            "INSERT INTO poses (id, position_x, position_y, position_z, orientation_x, orientation_y, orientation_z, orientation_w) VALUES (" +
            std::to_string(pose.id) + ", " +
            W.esc(std::to_string(pose.position_x)) + ", " +
            W.esc(std::to_string(pose.position_y)) + ", " +
            W.esc(std::to_string(pose.position_z)) + ", " +
            W.esc(std::to_string(pose.orientation_x)) + ", " +
            W.esc(std::to_string(pose.orientation_y)) + ", " +
            W.esc(std::to_string(pose.orientation_z)) + ", " +
            W.esc(std::to_string(pose.orientation_w)) + ") " +
            "ON CONFLICT (id) DO UPDATE SET " +
            "position_x = EXCLUDED.position_x, " +
            "position_y = EXCLUDED.position_y, " +
            "position_z = EXCLUDED.position_z, " +
            "orientation_x = EXCLUDED.orientation_x, " +
            "orientation_y = EXCLUDED.orientation_y, " +
            "orientation_z = EXCLUDED.orientation_z, " +
            "orientation_w = EXCLUDED.orientation_w;";

        W.exec(query);
        W.commit();
    }

    void PoseService::remove(pqxx::connection &connection_ptr, int id)
    {
        pqxx::nontransaction W(connection_ptr);
        std::string query = "DELETE FROM poses WHERE id = " + std::to_string(id) + ";";
        W.exec(query);
        W.commit();
    }

}
