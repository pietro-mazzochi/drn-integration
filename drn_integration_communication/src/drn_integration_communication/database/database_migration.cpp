#include <drn_integration_communication/database/database_migration.hpp>

namespace drn_integration_communication
{
    void DatabasemMigration::createTablesIfNotExist(pqxx::connection &connection_ptr)
    {
        try
        {
            pqxx::nontransaction W(connection_ptr);

            std::string createAlertsTable = R"(
            CREATE TABLE IF NOT EXISTS alerts (
                id SERIAL PRIMARY KEY,
                uuid VARCHAR(255) NOT NULL,
                active VARCHAR(255) NOT NULL,
                error_code VARCHAR(255) NOT NULL,
                data VARCHAR(255) NOT NULL,
                message VARCHAR(255) NOT NULL,
                timestamp VARCHAR(255) NOT NULL,
                supervisory_registration VARCHAR(255) NOT NULL
            );
            )";

            std::string createAnchorsTable = R"(
            CREATE TABLE IF NOT EXISTS anchors (
                id SERIAL PRIMARY KEY,
                name VARCHAR(100) NOT NULL,
                pos_x VARCHAR(100) NOT NULL,
                pos_y VARCHAR(100) NOT NULL,
                angle VARCHAR(100) NOT NULL
            );
            )";

            std::string createLogsTable = R"(
            CREATE TABLE IF NOT EXISTS logs (
                id SERIAL PRIMARY KEY,
                uuid VARCHAR(255) NOT NULL,
                data VARCHAR(255) NOT NULL,
                timestamp VARCHAR(255) NOT NULL,
                supervisory_registration VARCHAR(255) NOT NULL
            );
            )";

            std::string createRobotTable = R"(
            CREATE TABLE IF NOT EXISTS robot (
                id SERIAL PRIMARY KEY,
                uuid VARCHAR(255) NOT NULL,
                data VARCHAR(255) NOT NULL,
                timestamp VARCHAR(255) NOT NULL
            );
            )";

            std::string createMapsTable = R"(
            CREATE TABLE IF NOT EXISTS maps (
                id SERIAL PRIMARY KEY,
                name VARCHAR(100) NOT NULL,
                mode VARCHAR(100) NOT NULL,
                resolution VARCHAR(100) NOT NULL,
                origin_x VARCHAR(100) NOT NULL,
                origin_y VARCHAR(100) NOT NULL,
                origin_z VARCHAR(100) NOT NULL,
                negate VARCHAR(100) NOT NULL,
                occupied_thresh VARCHAR(100) NOT NULL,
                free_thresh VARCHAR(100) NOT NULL
            );
            )";

            std::string createNodeTypesTable = R"(
            CREATE TABLE IF NOT EXISTS node_types (
                id SERIAL PRIMARY KEY,
                name VARCHAR(100) NOT NULL
            );
            )";

            std::string createNodesTable = R"(
            CREATE TABLE IF NOT EXISTS nodes (
                id SERIAL PRIMARY KEY,
                pos_x VARCHAR(100) NOT NULL,
                pos_y VARCHAR(100) NOT NULL,
                angle VARCHAR(100) NOT NULL,
                map_id INT NOT NULL,
                node_type INT NOT NULL,
                deleted_supervisory VARCHAR(100) NOT NULL,
                direction VARCHAR(100) NOT NULL,
                label_id VARCHAR(100) NOT NULL,
                FOREIGN KEY (map_id) REFERENCES maps(id),
                FOREIGN KEY (node_type) REFERENCES node_types(id)
            );
            )";

            std::string createNodeConnectionsTable = R"(
            CREATE TABLE IF NOT EXISTS node_connections (
                id SERIAL PRIMARY KEY,
                line VARCHAR(100) NOT NULL,
                source_node_id INT NOT NULL,
                target_node_id INT NOT NULL,
                FOREIGN KEY (source_node_id) REFERENCES nodes(id),
                FOREIGN KEY (target_node_id) REFERENCES nodes(id)
            );
            )";

            std::string createZoneTypesTable = R"(
            CREATE TABLE IF NOT EXISTS zone_types (
                id SERIAL PRIMARY KEY,
                name VARCHAR(100) NOT NULL
            );
            )";

            std::string createZonesTable = R"(
            CREATE TABLE IF NOT EXISTS zones (
                id SERIAL PRIMARY KEY,
                max_velocity VARCHAR(100) NOT NULL,
                area VARCHAR(100) NOT NULL,
                map_id INT NOT NULL,
                type_zone_id INT NOT NULL,
                FOREIGN KEY (map_id) REFERENCES maps(id),
                FOREIGN KEY (type_zone_id) REFERENCES zone_types(id)
            );
            )";

            std::string createMissionsTable = R"(
            CREATE TABLE IF NOT EXISTS missions (
                id SERIAL PRIMARY KEY,
                title VARCHAR(100) NOT NULL,
                type VARCHAR(100) NOT NULL,
                end_id INT NOT NULL,
                mission_direction VARCHAR(100) NOT NULL,
                FOREIGN KEY (end_id) REFERENCES nodes(id)
            );
            )";

            std::string createMissionLogsTable = R"(
            CREATE TABLE IF NOT EXISTS mission_logs (
                id SERIAL PRIMARY KEY,
                uuid VARCHAR(255) NOT NULL,
                status VARCHAR(100) NOT NULL,
                data VARCHAR(100) NOT NULL,
                timestamp VARCHAR(255) NOT NULL,
                supervisory_registration VARCHAR(255) NOT NULL,
                mission_id INT NOT NULL,
                FOREIGN KEY (mission_id) REFERENCES missions(id)
            );
            )";

            std::string createSettingsTable = R"(
            CREATE TABLE IF NOT EXISTS settings (
                id SERIAL PRIMARY KEY,
                key VARCHAR(100) NOT NULL,
                value VARCHAR(100) NOT NULL
            );
            )";

            std::string createTelemetriesTable = R"(
            CREATE TABLE IF NOT EXISTS telemetries (
                id SERIAL PRIMARY KEY,
                data VARCHAR(100) NOT NULL,
                pos_x VARCHAR(100) NOT NULL,
                pos_y VARCHAR(100) NOT NULL,
                angle VARCHAR(100) NOT NULL,
                battery VARCHAR(100) NOT NULL,
                tension VARCHAR(100) NOT NULL,
                velocity VARCHAR(100) NOT NULL
            );
            )";

            std::string createVehiclesTable = R"(
            CREATE TABLE IF NOT EXISTS vehicles (
                id SERIAL PRIMARY KEY,
                code VARCHAR(100) NOT NULL,
                name VARCHAR(100) NOT NULL,
                model VARCHAR(100) NOT NULL
            );
            )";

            std::string createPoseTable = R"(
            CREATE TABLE IF NOT EXISTS poses (
                id SERIAL PRIMARY KEY,
                position_x DOUBLE PRECISION NOT NULL,
                position_y DOUBLE PRECISION NOT NULL,
                position_z DOUBLE PRECISION NOT NULL,
                orientation_x DOUBLE PRECISION NOT NULL,
                orientation_y DOUBLE PRECISION NOT NULL,
                orientation_z DOUBLE PRECISION NOT NULL,
                orientation_w DOUBLE PRECISION NOT NULL
            );
            )";

            std::string insertInitialPose = R"(
                INSERT INTO poses (
                    id, position_x, position_y, position_z, orientation_x, orientation_y, orientation_z, orientation_w)
                    SELECT 1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0
                    WHERE NOT EXISTS (SELECT 1 FROM poses);
            )";

            // Executar na ordem para respeitar FKs
            W.exec(createVehiclesTable);
            W.exec(createAlertsTable);
            W.exec(createAnchorsTable);
            W.exec(createLogsTable);
            W.exec(createRobotTable);
            W.exec(createMapsTable);
            W.exec(createNodeTypesTable);
            W.exec(createNodesTable);
            W.exec(createNodeConnectionsTable);
            W.exec(createSettingsTable);
            W.exec(createTelemetriesTable);
            W.exec(createZoneTypesTable);
            W.exec(createZonesTable);
            W.exec(createMissionsTable);
            W.exec(createMissionLogsTable);
            W.exec(createPoseTable);
            W.exec(insertInitialPose);

            W.commit();
            std::cout << "Tabelas criadas ou já existentes verificadas com sucesso." << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << std::endl;
        }
    }
}
