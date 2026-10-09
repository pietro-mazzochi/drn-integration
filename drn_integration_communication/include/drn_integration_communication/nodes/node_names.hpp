#ifndef DRN_INTEGRATION_NODE_NAMES_HPP
#define DRN_INTEGRATION_NODE_NAMES_HPP

#include <stdio.h>
#include <iostream>
#include <string>
#include <pqxx/pqxx>

namespace ros_param
{
    const std::string decisionMaker = "/drn_navigation/decision_making/drn_navigation_decision_maker_node";
}

namespace service
{
    const std::string routePlanningService = "/drn_navigation/route_planning/route_planning_service";
}

#endif