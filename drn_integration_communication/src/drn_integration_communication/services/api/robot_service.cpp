#include <drn_integration_communication/services/api/robot_service.hpp>
#include <stdexcept>
#include <optional>

namespace drn_integration_communication
{
    void RobotService::create(pqxx::connection &connection_ptr, const Robot &Robot)
    {
        pqxx::nontransaction W(connection_ptr);
        std::string query = "INSERT INTO Robot (uuid, data, timestamp) VALUES ('" + W.esc(Robot.uuid) + "', '" + W.esc(Robot.data) + "', '" + W.esc(Robot.timestamp) + "');";
        W.exec(query);
        W.commit();
    }

    void RobotService::robot(pqxx::connection &connection_ptr, const Robot &Robot)
    {
        pqxx::nontransaction W(connection_ptr);
        std::string query = "INSERT INTO Robot (id, uuid, data, timestamp) VALUES (" +
                            W.esc(std::to_string(Robot.id)) + ", '" +
                            W.esc(Robot.uuid) + "', '" +
                            W.esc(Robot.data) + "', '" +
                            W.esc(Robot.timestamp) + "')" +
                            " ON CONFLICT (id) DO UPDATE SET data = EXCLUDED.data, timestamp = EXCLUDED.timestamp;";
        W.exec(query);
        W.commit();
    }

    std::vector<Robot> RobotService::getAll(pqxx::connection &connection_ptr)
    {
        pqxx::nontransaction N(connection_ptr);
        std::string query = "SELECT id, uuid, data, timestamp FROM Robot;";
        pqxx::result R(N.exec(query));

        std::vector<Robot> Robot;

        for (const auto &row : R)
        {
            Robot.emplace_back(
                row[0].as<int>(),
                row[1].as<std::string>(),
                row[2].as<std::string>(),
                row[3].as<std::string>());
        }

        return Robot;
    }

    Robot RobotService::getById(pqxx::connection &connection_ptr, int id)
    {
        try
        {
            pqxx::nontransaction N(connection_ptr);
            pqxx::result R = N.exec_params(
                "SELECT id, uuid, data, timestamp FROM Robot WHERE id = $1", id);

            if (R.empty())
            {
                throw std::runtime_error("Robot not found");
            }

            auto row = R[0];
            return Robot(row[0].as<int>(), row[1].as<std::string>(), row[2].as<std::string>(), row[3].as<std::string>());
        }
        catch (const std::exception &e)
        {
            throw std::runtime_error(std::string("Database query error: ") + e.what());
        }
    }

    void RobotService::update(pqxx::connection &connection_ptr, const Robot &Robot)
    {
        pqxx::nontransaction W(connection_ptr);
        std::string query = "UPDATE Robot SET uuid = '" + W.esc(Robot.uuid) + "', data = '" + W.esc(Robot.data) + "', timestamp = '" + W.esc(Robot.timestamp) + "' WHERE id = " + std::to_string(Robot.id) + ";";
        W.exec(query);
        W.commit();
    }

    void RobotService::remove(pqxx::connection &connection_ptr, int id)
    {
        pqxx::nontransaction W(connection_ptr);
        std::string query = "DELETE FROM Robot WHERE id = " + std::to_string(id) + ";";
        W.exec(query);
        W.commit();
    }

    bool RobotService::existId(pqxx::connection &connection_ptr, int id)
    {
        pqxx::nontransaction W(connection_ptr);

        pqxx::result R = W.exec("SELECT id FROM Robot WHERE id = " + W.esc(std::to_string(id)) + ";");

        W.commit();

        return !R.empty();
    }

}