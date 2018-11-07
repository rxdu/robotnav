#include <memory>
#include <iostream>

#include "road_map/road_map.hpp"

#include "lightviz/lightviz.hpp"
#include "ugvnav_viz/ugvnav_viz.hpp"

#include "stopwatch/stopwatch.h"

using namespace librav;

int main()
{
    stopwatch::StopWatch timer;

    std::shared_ptr<RoadMap> map = std::make_shared<RoadMap>("/home/rdu/Workspace/librav/data/road_map/natick_roundabout_full.osm");

    if (!map->MapReady())
    {
        std::cout << "map didn't load correctly" << std::endl;
        return -1;
    }

    map->PrintInfo();

    std::cout << "map loaded in " << timer.toc() << " seconds" << std::endl;

    // auto ids = map->FindOccupiedLanelet(CartCooridnate(55, 56));
    // std::cout << "occupied laneles: " << ids.size() << std::endl;

    RoadMapViz::SetupRoadMapViz(map);
    RoadMapViz::ShowLanes();

    // LightViz::ShowPolygon(map->GetAllLanePolygons(), 10);
    // LightViz::ShowPolyline(map->GetAllLaneBoundPolylines(), 10);
    // LightViz::ShowLanePolylines(map->GetAllLaneBoundPolylines(), map->GetAllLaneCenterPolylines());
    // LightViz::ShowPolylinePosition(map->GetAllLaneCenterPolylines(), 10);

    // std::vector<Polygon> roi;
    // roi.push_back(map->GetLanePolygon("s1"));
    // LightViz::ShowFilledPolygon(roi, map->GetAllLanePolygons());

    return 0;
}