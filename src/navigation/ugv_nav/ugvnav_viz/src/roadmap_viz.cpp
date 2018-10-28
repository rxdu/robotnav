/* 
 * roadmap_viz.cpp
 * 
 * Created on: Aug 21, 2018 07:38
 * Description: 
 * 
 * Copyright (c) 2018 Ruixiang Du (rdu)
 */

#include "ugvnav_viz/roadmap_viz.hpp"

#include "lightviz/details/geometric_draw.hpp"

using namespace librav;
using namespace CvDraw;

RoadMapViz::RoadMapViz()
{
    xmin_ = 0.0;
    xmax_ = 1.0;
    ymin_ = 0.0;
    ymax_ = 1.0;
}

RoadMapViz &RoadMapViz::GetInstance()
{
    static RoadMapViz viz;
    return viz;
}

void RoadMapViz::SetupRoadMapViz(std::shared_ptr<RoadMap> map)
{
    RoadMapViz &viz = RoadMapViz::GetInstance();

    viz.road_map_ = map;
    viz.boundary_lines_ = map->GetAllLaneBoundPolylines();
    viz.center_lines_ = map->GetAllLaneCenterPolylines();

    std::vector<double> xmins, xmaxs, ymins, ymaxs;

    for (auto &polyline : viz.boundary_lines_)
    {
        xmins.push_back(polyline.GetMinX());
        xmaxs.push_back(polyline.GetMaxX());
        ymins.push_back(polyline.GetMinY());
        ymaxs.push_back(polyline.GetMaxY());
    }
    for (auto &polyline : viz.center_lines_)
    {
        xmins.push_back(polyline.GetMinX());
        xmaxs.push_back(polyline.GetMaxX());
        ymins.push_back(polyline.GetMinY());
        ymaxs.push_back(polyline.GetMaxY());
    }
    double bd_xl = *std::min_element(xmins.begin(), xmins.end());
    double bd_xr = *std::max_element(xmaxs.begin(), xmaxs.end());
    double bd_yb = *std::min_element(ymins.begin(), ymins.end());
    double bd_yt = *std::max_element(ymaxs.begin(), ymaxs.end());

    double xspan = bd_xr - bd_xl;
    double yspan = bd_yt - bd_yb;

    viz.xmin_ = bd_xl - xspan * 0.1;
    viz.xmax_ = bd_xr + xspan * 0.1;
    viz.ymin_ = bd_yb - yspan * 0.1;
    viz.ymax_ = bd_yt + yspan * 0.1;
}

void RoadMapViz::ShowLanes(bool show_center_line, int32_t pixel_per_unit, std::string window_name, bool save_img)
{
    RoadMapViz &viz = RoadMapViz::GetInstance();

    GeometryDraw gdraw(pixel_per_unit);

    cv::Mat canvas = gdraw.CreateCanvas(viz.xmin_, viz.xmax_, viz.ymin_, viz.ymax_, CvDrawColors::jet_colormap_lowest);

    for (auto &polyline : viz.boundary_lines_)
        canvas = gdraw.DrawPolyline(canvas, polyline, false, CvDrawColors::silver_color);

    if (show_center_line)
    {
        for (auto &polyline : viz.center_lines_)
            canvas = gdraw.DrawPolyline(canvas, polyline, false, CvDrawColors::black_color);
    }

    ShowImage(canvas, window_name, save_img);
}

void RoadMapViz::ShowTrafficChannel(TrafficChannel &channel, int32_t pixel_per_unit, std::string window_name, bool save_img)
{
    RoadMapViz &viz = RoadMapViz::GetInstance();

    GeometryDraw gdraw(pixel_per_unit);

    cv::Mat canvas = gdraw.CreateCanvas(viz.xmin_, viz.xmax_, viz.ymin_, viz.ymax_, CvDrawColors::jet_colormap_lowest);

    for (auto &polyline : viz.boundary_lines_)
        canvas = gdraw.DrawPolyline(canvas, polyline, false, CvDrawColors::silver_color);

    // canvas = gdraw.DrawPolyline(canvas, channel.center_line_, true, CvDrawColors::gray_color);
    if (channel.grid_ != nullptr)
        canvas = gdraw.DrawCurvilinearGrid(canvas, *channel.grid_.get());

    canvas = gdraw.DrawParametricCurve(canvas, channel.center_curve_, 0.1, CvDrawColors::black_color);

    ShowImage(canvas, window_name, save_img);
}

void RoadMapViz::ShowLatticeInTrafficChannel(std::vector<StateLattice> &lattice, TrafficChannel &channel, int32_t pixel_per_unit, std::string window_name, bool save_img)
{
    RoadMapViz &viz = RoadMapViz::GetInstance();

    GeometryDraw gdraw(pixel_per_unit);

    cv::Mat canvas = gdraw.CreateCanvas(viz.xmin_, viz.xmax_, viz.ymin_, viz.ymax_, CvDrawColors::jet_colormap_lowest);

    for (auto &polyline : viz.boundary_lines_)
        canvas = gdraw.DrawPolyline(canvas, polyline, false, CvDrawColors::silver_color);

    // canvas = gdraw.DrawPolyline(canvas, channel.center_line_, true, CvDrawColors::gray_color);
    if (channel.grid_ != nullptr)
        canvas = gdraw.DrawCurvilinearGrid(canvas, *channel.grid_.get());

    canvas = gdraw.DrawParametricCurve(canvas, channel.center_curve_, 0.1, CvDrawColors::black_color);

    // draw state lattice
    double step = 0.1;
    for (auto &sl : lattice)
    {
        std::vector<MotionState> states;
        for (double s = 0; s <= sl.GetLength(); s += step)
            states.push_back(sl.Evaluate(s, step / 5.0));
        for (int32_t i = 0; i < states.size() - 1; ++i)
        {
            MotionState st1 = states[i];
            MotionState st2 = states[i + 1];

            auto pt1 = gdraw.ConvertCartisianToPixel(st1.x, st1.y);
            auto pt2 = gdraw.ConvertCartisianToPixel(st2.x, st2.y);
            DrawLine(canvas, cv::Point(pt1.x, pt1.y), cv::Point(pt2.x, pt2.y), CvDrawColors::orange_color, 1);
        }
    }

    ShowImage(canvas, window_name, save_img);
}

void RoadMapViz::ShowTrafficChannelCenterline(TrafficChannel &channel, int32_t pixel_per_unit, std::string window_name, bool save_img)
{
    RoadMapViz &viz = RoadMapViz::GetInstance();

    GeometryDraw gdraw(pixel_per_unit);

    cv::Mat canvas = gdraw.CreateCanvas(viz.xmin_, viz.xmax_, viz.ymin_, viz.ymax_, CvDrawColors::jet_colormap_lowest);

    for (auto &polyline : viz.boundary_lines_)
        canvas = gdraw.DrawPolyline(canvas, polyline, false, CvDrawColors::silver_color);

    canvas = gdraw.DrawPolyline(canvas, channel.center_line_, true, CvDrawColors::black_color);

    ShowImage(canvas, window_name, save_img);
}

void RoadMapViz::ShowVehicle(Polygon &polygon, int32_t pixel_per_unit, std::string window_name, bool save_img)
{
    RoadMapViz &viz = RoadMapViz::GetInstance();

    GeometryDraw gdraw(pixel_per_unit);

    cv::Mat canvas = gdraw.CreateCanvas(viz.xmin_, viz.xmax_, viz.ymin_, viz.ymax_, CvDrawColors::jet_colormap_lowest);

    for (auto &polyline : viz.boundary_lines_)
        canvas = gdraw.DrawPolyline(canvas, polyline, false, CvDrawColors::silver_color);

    for (auto &polyline : viz.center_lines_)
        canvas = gdraw.DrawPolyline(canvas, polyline, false, CvDrawColors::black_color);

    canvas = gdraw.DrawPolygon(canvas, polygon, false, CvDrawColors::cyan_color);
    canvas = gdraw.DrawPolygonDirection(canvas, polygon);

    ShowImage(canvas, window_name, save_img);
}

void RoadMapViz::ShowVehiclePath(std::vector<Polyline> &path, std::vector<Polygon> polygons, int32_t pixel_per_unit, std::string window_name, bool save_img)
{
    RoadMapViz &viz = RoadMapViz::GetInstance();

    GeometryDraw gdraw(pixel_per_unit);

    cv::Mat canvas = gdraw.CreateCanvas(viz.xmin_, viz.xmax_, viz.ymin_, viz.ymax_, CvDrawColors::jet_colormap_lowest);

    for (auto &polyline : viz.boundary_lines_)
        canvas = gdraw.DrawPolyline(canvas, polyline, false, CvDrawColors::silver_color);

    for (auto &polyline : viz.center_lines_)
        canvas = gdraw.DrawPolyline(canvas, polyline, false, CvDrawColors::black_color);

    for (auto &polygon : polygons)
    {
        canvas = gdraw.DrawPolygon(canvas, polygon, false, CvDrawColors::cyan_color);
        canvas = gdraw.DrawPolygonDirection(canvas, polygon, CvDrawColors::red_color, 2);
    }

    for (auto &polyline : path)
        canvas = gdraw.DrawPolyline(canvas, polyline, false, CvDrawColors::lime_color, 2);

    ShowImage(canvas, window_name, save_img);
}

void RoadMapViz::ShowVehicleFootprints(std::vector<Polygon> &polygons, int32_t pixel_per_unit, std::string window_name, bool save_img)
{
    RoadMapViz &viz = RoadMapViz::GetInstance();

    GeometryDraw gdraw(pixel_per_unit);

    cv::Mat canvas = gdraw.CreateCanvas(viz.xmin_, viz.xmax_, viz.ymin_, viz.ymax_, CvDrawColors::jet_colormap_lowest);

    for (auto &polyline : viz.boundary_lines_)
        canvas = gdraw.DrawPolyline(canvas, polyline, false, CvDrawColors::silver_color);

    for (auto &polyline : viz.center_lines_)
        canvas = gdraw.DrawPolyline(canvas, polyline, false, CvDrawColors::black_color);

    for (auto &polygon : polygons)
    {
        canvas = gdraw.DrawPolygon(canvas, polygon, false, CvDrawColors::cyan_color);
        canvas = gdraw.DrawPolygonDirection(canvas, polygon, CvDrawColors::red_color, 2);
    }

    ShowImage(canvas, window_name, save_img);
}

void RoadMapViz::ShowConflictingZone(std::vector<Polygon> &highlight, std::vector<Polygon> &polygons, int32_t pixel_per_unit, std::string window_name, bool save_img)
{
    RoadMapViz &viz = RoadMapViz::GetInstance();

    GeometryDraw gdraw(pixel_per_unit);

    cv::Mat canvas = gdraw.CreateCanvas(viz.xmin_, viz.xmax_, viz.ymin_, viz.ymax_, CvDrawColors::jet_colormap_lowest);

    for (auto &polyline : viz.boundary_lines_)
        canvas = gdraw.DrawPolyline(canvas, polyline, false, CvDrawColors::silver_color);

    for (auto &polyline : viz.center_lines_)
        canvas = gdraw.DrawPolyline(canvas, polyline, false, CvDrawColors::black_color);

    for (auto &polygon : highlight)
    {
        canvas = gdraw.DrawFilledPolygon(canvas, polygon, false, CvDrawColors::olive_color);
    }

    for (auto &polygon : polygons)
    {
        canvas = gdraw.DrawPolygon(canvas, polygon, false, CvDrawColors::cyan_color);
        canvas = gdraw.DrawPolygonDirection(canvas, polygon, CvDrawColors::red_color, 2);
    }

    ShowImage(canvas, window_name, save_img);
}

// void RoadMapViz::ShowLabledTrafficFlows(std::vector<TrafficFlow *> &flows, int32_t pixel_per_unit, std::string window_name, bool save_img)
// {
//     RoadMapViz &viz = RoadMapViz::GetInstance();

//     GeometryDraw gdraw(pixel_per_unit);

//     cv::Mat canvas = gdraw.CreateCanvas(viz.xmin_, viz.xmax_, viz.ymin_, viz.ymax_, CvDrawColors::jet_colormap_lowest);

//     for (auto &polyline : viz.boundary_lines_)
//         canvas = gdraw.DrawPolyline(canvas, polyline, false, CvDrawColors::silver_color);

//     for (auto &polyline : viz.center_lines_)
//         canvas = gdraw.DrawPolyline(canvas, polyline, false, CvDrawColors::black_color);

//     std::vector<Polygon> highlight, all;

//     for (auto &tf : flows)
//     {
//         auto blks = tf->GetAllLaneBlocks();
//         all.insert(all.end(), blks.begin(), blks.end());

//         auto cblks = tf->GetConflictingLaneBlocks();
//         highlight.insert(highlight.end(), cblks.begin(), cblks.end());
//     }

//     for (auto &polygon : highlight)
//     {
//         canvas = gdraw.DrawFilledPolygon(canvas, polygon, false, CvDrawColors::olive_color);
//     }

//     for (auto &polygon : all)
//     {
//         canvas = gdraw.DrawPolygon(canvas, polygon, false, CvDrawColors::cyan_color);
//         canvas = gdraw.DrawPolygonDirection(canvas, polygon, CvDrawColors::red_color, 2);
//     }

//     ShowImage(canvas, window_name, save_img);
// }

// void RoadMapViz::ShowLabledTrafficFlows(TrafficFlow *ego_flow, std::vector<TrafficFlow *> &flows, int32_t pixel_per_unit, std::string window_name, bool save_img)
// {
//     RoadMapViz &viz = RoadMapViz::GetInstance();

//     GeometryDraw gdraw(pixel_per_unit);

//     cv::Mat canvas = gdraw.CreateCanvas(viz.xmin_, viz.xmax_, viz.ymin_, viz.ymax_, CvDrawColors::jet_colormap_lowest);

//     for (auto &polyline : viz.boundary_lines_)
//         canvas = gdraw.DrawPolyline(canvas, polyline, false, CvDrawColors::silver_color);

//     for (auto &polyline : viz.center_lines_)
//         canvas = gdraw.DrawPolyline(canvas, polyline, false, CvDrawColors::black_color);

//     std::vector<Polygon> highlight, all;

//     for (auto &tf : flows)
//     {
//         auto blks = tf->GetAllLaneBlocks();
//         all.insert(all.end(), blks.begin(), blks.end());

//         auto cblks = tf->GetConflictingLaneBlocks();
//         highlight.insert(highlight.end(), cblks.begin(), cblks.end());
//     }

//     for (auto &polygon : highlight)
//     {
//         canvas = gdraw.DrawFilledPolygon(canvas, polygon, false, CvDrawColors::olive_color);
//     }

//     for (auto &polygon : all)
//     {
//         canvas = gdraw.DrawPolygon(canvas, polygon, false, CvDrawColors::cyan_color);
//         canvas = gdraw.DrawPolygonDirection(canvas, polygon, CvDrawColors::red_color, 2);
//     }

//     std::vector<Polygon> ego_polys = ego_flow->GetAllLaneBlocks();
//     for (auto &polygon : ego_polys)
//     {
//         canvas = gdraw.DrawPolygon(canvas, polygon, false, CvDrawColors::cyan_color);
//         canvas = gdraw.DrawPolygonDirection(canvas, polygon, CvDrawColors::green_color, 2);
//     }

//     ShowImage(canvas, window_name, save_img);
// }