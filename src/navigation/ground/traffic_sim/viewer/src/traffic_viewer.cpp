/* 
 * traffic_viewer.cpp
 * 
 * Created on: Nov 21, 2018 01:07
 * Description: 
 * 
 * Copyright (c) 2018 Ruixiang Du (rdu)
 */

#include "traffic_viewer/traffic_viewer.hpp"

#include <cassert>

#include "lightview/viewer_utils.hpp"
#include "lightview/lightwidgets.hpp"
#include "lightviz/details/cartesian_canvas.hpp"
#include "ugvnav_viz/details/roadmap_draw.hpp"
#include "ugvnav_viz/details/vehicle_draw.hpp"

#include "traffic_map/map_loader.hpp"
#include "cav_common/cav_datalink.hpp"

using namespace librav;

TrafficViewer::TrafficViewer(std::string map_file, int32_t ppu) : LightViewer(), ppu_(ppu)
{
    MapLoader loader(map_file);
    road_map_ = loader.road_map;
    traffic_map_ = loader.traffic_map;

    CalcCanvasSize(road_map_);

    // setup communication link
    data_link_ = std::make_shared<LCMLink>();
    if (!data_link_->good())
        std::cerr << "ERROR: Failed to initialize LCM." << std::endl;
    data_link_ready_ = true;

    data_link_->subscribe(CAV_COMMON_CHANNELS::VEHICLE_ESTIMATIONS_CHANNEL, &TrafficViewer::HandleVehicleEstimationsMsg, this);
    data_link_->subscribe(CAV_COMMON_CHANNELS::EGO_VEHICLE_STATE_CHANNEL, &TrafficViewer::HandleEgoVehicleStateMsg, this);
}

void TrafficViewer::CalcCanvasSize(std::shared_ptr<RoadMap> map)
{
    assert(map != nullptr);

    std::vector<double> xmins, xmaxs, ymins, ymaxs;

    for (auto &polyline : map->GetAllLaneBoundPolylines())
    {
        xmins.push_back(polyline.GetMinX());
        xmaxs.push_back(polyline.GetMaxX());
        ymins.push_back(polyline.GetMinY());
        ymaxs.push_back(polyline.GetMaxY());
    }
    for (auto &polyline : map->GetAllLaneCenterPolylines())
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

    xmin_ = bd_xl - xspan * 0.1;
    xmax_ = bd_xr + xspan * 0.1;
    ymin_ = bd_yb - yspan * 0.1;
    ymax_ = bd_yt + yspan * 0.1;

    x_span_ = xmax_ - xmin_;
    y_span_ = ymax_ - ymin_;
    aspect_ratio_ = y_span_ / x_span_;
}

cv::Mat TrafficViewer::CropImageToROI(cv::Mat img, double cx, double cy, double xspan, double yspan)
{
    double rxmin = cx - xspan / 2.0;
    double rxmax = cx + xspan / 2.0;
    double rymin = cy - yspan / 2.0;
    double rymax = cy + yspan / 2.0;

    if (rxmin < xmin_)
        rxmin = xmin_;
    if (rxmax > xmax_)
        rxmax = xmax_;
    if (rymin < ymin_)
        rymin = ymin_;
    if (rymax > ymax_)
        rymax = ymax_;

    std::cout << "canvas size: " << xmin_ << " , " << xmax_ << " , " << ymin_ << " , " << ymax_ << std::endl;

    int tl_x = rxmin * ppu_;
    int tl_y = rymin * ppu_;
    int width = (rxmax - rxmin) * ppu_;
    int height = (rymax - rymin) * ppu_;

    return img(cv::Rect(tl_x, tl_y, width, height));
}

void TrafficViewer::HandleEgoVehicleStateMsg(const librav::ReceiveBuffer *rbuf, const std::string &chan, const librav_lcm_msgs::VehicleState *msg)
{
    ego_vehicle_state_ = VehicleState(msg->id, {msg->position[0], msg->position[1], msg->theta}, msg->speed);
    ego_state_updated_ = true;
}

void TrafficViewer::HandleVehicleEstimationsMsg(const librav::ReceiveBuffer *rbuf, const std::string &chan, const librav_lcm_msgs::VehicleEstimations *msg)
{
    surrounding_vehicles_.clear();
    for (int i = 0; i < msg->vehicle_num; ++i)
    {
        auto est = msg->estimations[i];
        surrounding_vehicles_.emplace_back(VehicleState(est.id, {est.position[0], est.position[1], est.theta}, est.speed));
    }
}

void TrafficViewer::Start()
{
    std::cout << "traffic viewer started" << std::endl;

    bool show_another_window = false;

    while (!glfwWindowShouldClose(window_))
    {
        PreHouseKeeping();

        // process LCM callbacks first
        data_link_->handleTimeout(0);

        // create image according to user selection
        //-------------------------------------------------------------//

        static float f = 0.0f;
        static int counter = 0;

        static bool show_center_line = false;
        static bool use_jetcolor_bg = false;
        static bool centered_at_ego_vehicle = true;
        static float zoom_in_ratio = 0.56f;
        static bool save_image = false;

        //-------------------------------------------------------------//

        ImGui::SetNextWindowPos(ImVec2(0, 0), 0, ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(100, 700), ImGuiCond_FirstUseEver);

        ImGui::Begin("Settings:");

        ImGui::Text("Road Map:");
        ImGui::Checkbox("Show Centerline", &show_center_line);
        ImGui::Checkbox("Jetcolor Map", &use_jetcolor_bg);

        ImGui::Text("Traffic:");
        ImGui::Checkbox("Centered at Ego Vehicle", &centered_at_ego_vehicle);
        ImGui::SliderFloat("Zoom-in Ratio", &zoom_in_ratio, 0.0f, 1.0f);
        ImGui::Text("Save Result:");
        static char img_name_buf[32] = "traffic_viewer";
        ImGui::InputText("", img_name_buf, IM_ARRAYSIZE(img_name_buf));
        ImGui::SameLine();
        if (ImGui::Button("Save Image"))
            save_image = true;
        // ImGui::SameLine();

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        ImGui::End();

        //-------------------------------------------------------------//

        CartesianCanvas canvas(ppu_);
        if (use_jetcolor_bg)
            canvas.SetupCanvas(xmin_, xmax_, ymin_, ymax_, CvDrawColors::jet_colormap_lowest);
        else
            canvas.SetupCanvas(xmin_, xmax_, ymin_, ymax_);

        RoadMapDraw road_draw(road_map_, canvas);
        road_draw.DrawLanes(show_center_line);

        VehicleDraw veh_draw(canvas);
        for (auto &veh : surrounding_vehicles_)
        {
            Polygon fp = VehicleFootprint(veh.GetPose()).polygon;
            veh_draw.DrawVehicle(fp, veh.id_);
        }

        if (ego_state_updated_)
        {
            Polygon fp = VehicleFootprint(ego_vehicle_state_.GetPose()).polygon;
            veh_draw.DrawVehicle(fp, CvDrawColors::blue_color);
        }

        //-------------------------------------------------------------//

        // draw image to window
        // cv::Mat vis_img = cv::imread("/home/rdu/Pictures/lanelets_light.png");
        // cv::Mat vis_img = canvas.paint_area;
        // cv::Mat vis_img = canvas.GetROIofPaintArea(80, 100, 100, 100);

        cv::Mat vis_img;
        if (centered_at_ego_vehicle && ego_state_updated_)
        {
            auto pos = ego_vehicle_state_.GetPose().position;
            vis_img = canvas.GetROIofPaintArea(pos.x, pos.y, zoom_in_ratio);
        }
        else
        {
            vis_img = canvas.paint_area;
        }

        LightWidget::DrawOpenCVImageToBackground(vis_img);

        // save image if requested
        if (save_image)
        {
            std::string name(img_name_buf);
            cv::imwrite(name + ".png", vis_img);
            save_image = false;
        }

        vis_img.release();

        //-------------------------------------------------------------//

        PostHousekeeping();
    }
}