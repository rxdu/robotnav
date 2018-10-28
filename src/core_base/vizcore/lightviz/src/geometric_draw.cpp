/* 
 * geometric_draw.cpp
 * 
 * Created on: Aug 10, 2018 09:18
 * Description: 
 * 
 * Copyright (c) 2018 Ruixiang Du (rdu)
 */

#include "lightviz/details/geometric_draw.hpp"

#include <cassert>

#include "lightviz/details/matrix_draw.hpp"

using namespace librav;
using namespace LightViz;
using namespace CvDraw;

cv::Mat GeometryDraw::CreateCanvas(double xmin, double xmax, double ymin, double ymax, cv::Scalar bg_color)
{
    assert(xmax > xmin && ymax > ymin);

    xmin_ = xmin;
    xmax_ = xmax;
    ymin_ = ymin;
    ymax_ = ymax;

    // create canvas
    xspan_ = xmax - xmin;
    yspan_ = ymax - ymin;
    canvas_size_x_ = xspan_ * pixel_per_meter;
    canvas_size_y_ = yspan_ * pixel_per_meter;

    cv::Mat canvas(canvas_size_y_, canvas_size_x_, CV_8UC3, bg_color);

    std::cout << "canvas size: " << canvas_size_x_ << " , " << canvas_size_y_ << std::endl;

    return canvas;
}

cv::Mat GeometryDraw::CreateCanvasWithMatrixColorMap(const Eigen::MatrixXd &matrix, int32_t ppu)
{
    cv::Mat color_img = LightViz::CreateColorMapFromEigenMatrix(matrix);
    pixel_per_meter = ppu;

    return color_img;
}

cv::Mat GeometryDraw::DrawPolyline(cv::Mat canvas, const Polyline &polyline, bool show_dot, cv::Scalar ln_color, int32_t ln_width)
{
    std::size_t pt_num = polyline.GetPointNumer();

    if (pt_num == 0)
    {
        return canvas;
    }
    else if (pt_num == 1)
    {
        if (show_dot)
        {
            auto pt = ConvertCartisianToPixel(polyline.GetPoint(0).x, polyline.GetPoint(0).y);
            DrawPoint(canvas, cv::Point(pt.x, pt.y), CvDrawColors::red_color);
        }
    }
    else
    {
        for (std::size_t i = 0; i < pt_num - 1; ++i)
        {
            auto pt1 = ConvertCartisianToPixel(polyline.GetPoint(i).x, polyline.GetPoint(i).y);
            auto pt2 = ConvertCartisianToPixel(polyline.GetPoint(i + 1).x, polyline.GetPoint(i + 1).y);
            DrawLine(canvas, cv::Point(pt1.x, pt1.y), cv::Point(pt2.x, pt2.y), ln_color, ln_width);
        }
        if (show_dot)
        {
            for (std::size_t i = 0; i < pt_num; ++i)
            {
                auto pt1 = ConvertCartisianToPixel(polyline.GetPoint(i).x, polyline.GetPoint(i).y);
                DrawPoint(canvas, cv::Point(pt1.x, pt1.y), CvDrawColors::red_color);
            }
        }
    }

    return canvas;
}

cv::Mat GeometryDraw::DrawCubicSpline(cv::Mat canvas, const CSpline &spline, double step, cv::Scalar ln_color, int32_t ln_width)
{
    std::vector<cv::Point2d> pts;
    std::vector<CSpline::Knot> knots(spline.GetAllKnots());
    for (double x = knots.front().x; x < knots.back().x; x += step)
        pts.emplace_back(x, spline.Evaluate(x));

    std::cout << "intermediate points: " << pts.size() << std::endl;

    for (std::size_t i = 0; i < pts.size() - 1; ++i)
    {
        auto pt1 = ConvertCartisianToPixel(pts[i].x, pts[i].y);
        auto pt2 = ConvertCartisianToPixel(pts[i + 1].x, pts[i + 1].y);
        DrawLine(canvas, cv::Point(pt1.x, pt1.y), cv::Point(pt2.x, pt2.y), ln_color, ln_width);
    }

    return canvas;
}

cv::Mat GeometryDraw::DrawParametricCurve(cv::Mat canvas, const ParametricCurve &pcurve, double step, cv::Scalar ln_color, int32_t ln_width)
{
    std::vector<cv::Point2d> pts;

    for (double s = 0; s < pcurve.GetTotalLength(); s += step)
        pts.emplace_back(pcurve.GetXSpline().Evaluate(s), pcurve.GetYSpline().Evaluate(s));

    // std::cout << "intermediate points: " << pts.size() << std::endl;

    for (std::size_t i = 0; i < pts.size() - 1; ++i)
    {
        auto pt1 = ConvertCartisianToPixel(pts[i].x, pts[i].y);
        auto pt2 = ConvertCartisianToPixel(pts[i + 1].x, pts[i + 1].y);
        DrawLine(canvas, cv::Point(pt1.x, pt1.y), cv::Point(pt2.x, pt2.y), ln_color, ln_width);
    }

    return canvas;
}

cv::Mat GeometryDraw::DrawCurvilinearGrid(cv::Mat canvas, const CurvilinearGrid &grid, double step, bool show_center, cv::Scalar ln_color, int32_t ln_width)
{
    // draw center line
    if (show_center)
        canvas = DrawParametricCurve(canvas, grid.curve_, step, CvDrawColors::gray_color, ln_width);

    // draw normal lines
    auto spt1 = ConvertCartisianToPixel(grid.grid_tiles_.front().back()->vertices[2].position.x, grid.grid_tiles_.front().back()->vertices[2].position.y);
    auto spt2 = ConvertCartisianToPixel(grid.grid_tiles_.front().front()->vertices[3].position.x, grid.grid_tiles_.front().front()->vertices[3].position.y);
    DrawLine(canvas, cv::Point(spt1.x, spt1.y), cv::Point(spt2.x, spt2.y), ln_color, ln_width);
    for (auto &row : grid.grid_tiles_)
    {
        auto pt1 = ConvertCartisianToPixel(row.back()->vertices[0].position.x, row.back()->vertices[0].position.y);
        auto pt2 = ConvertCartisianToPixel(row.front()->vertices[1].position.x, row.front()->vertices[1].position.y);
        DrawLine(canvas, cv::Point(pt1.x, pt1.y), cv::Point(pt2.x, pt2.y), ln_color, ln_width);
    }

    // draw tangential lines
    for (auto &row : grid.grid_tiles_)
    {
        for (auto &cell : row)
        {
            auto pt1 = ConvertCartisianToPixel(cell->vertices[1].position.x, cell->vertices[1].position.y);
            auto pt2 = ConvertCartisianToPixel(cell->vertices[3].position.x, cell->vertices[3].position.y);
            DrawLine(canvas, cv::Point(pt1.x, pt1.y), cv::Point(pt2.x, pt2.y), ln_color, ln_width);
        }
        auto fpt1 = ConvertCartisianToPixel(row.back()->vertices[0].position.x, row.back()->vertices[0].position.y);
        auto fpt2 = ConvertCartisianToPixel(row.back()->vertices[2].position.x, row.back()->vertices[2].position.y);
        DrawLine(canvas, cv::Point(fpt1.x, fpt1.y), cv::Point(fpt2.x, fpt2.y), ln_color, ln_width);
    }

    return canvas;
}

cv::Mat GeometryDraw::DrawPolygon(cv::Mat canvas, const Polygon &polygon, bool show_dot, cv::Scalar ln_color, int32_t ln_width)
{
    std::size_t pt_num = polygon.GetPointNumer();

    if (pt_num < 3)
        return canvas;

    for (std::size_t i = 0; i < pt_num - 1; ++i)
    {
        auto pt1 = ConvertCartisianToPixel(polygon.GetPoint(i).x, polygon.GetPoint(i).y);
        auto pt2 = ConvertCartisianToPixel(polygon.GetPoint(i + 1).x, polygon.GetPoint(i + 1).y);
        DrawLine(canvas, cv::Point(pt1.x, pt1.y), cv::Point(pt2.x, pt2.y), ln_color, ln_width);
    }
    auto last_pt = ConvertCartisianToPixel(polygon.GetPoint(pt_num - 1).x, polygon.GetPoint(pt_num - 1).y);
    auto first_pt = ConvertCartisianToPixel(polygon.GetPoint(0).x, polygon.GetPoint(0).y);
    DrawLine(canvas, cv::Point(last_pt.x, last_pt.y), cv::Point(first_pt.x, first_pt.y), ln_color, ln_width);

    if (show_dot)
    {
        for (std::size_t i = 0; i < pt_num; ++i)
        {
            auto pt1 = ConvertCartisianToPixel(polygon.GetPoint(i).x, polygon.GetPoint(i).y);
            DrawPoint(canvas, cv::Point(pt1.x, pt1.y), CvDrawColors::red_color);
        }
    }

    return canvas;
}

/* 
 * Assumptions:
 *  1. Convext polygon
 *  2. Front line: first two points
*/
cv::Mat GeometryDraw::DrawPolygonDirection(cv::Mat canvas, const Polygon &polygon, cv::Scalar ln_color, int32_t ln_width)
{
    if (polygon.GetPointNumer() < 3)
        return canvas;

    std::vector<SimplePoint> points;
    for (int i = 0; i < polygon.GetPointNumer(); ++i)
        points.push_back(ConvertCartisianToPixel(polygon.GetPoint(i).x, polygon.GetPoint(i).y));

    double center_x = 0;
    double center_y = 0;

    for (auto &pt : points)
    {
        center_x += pt.x;
        center_y += pt.y;
    }
    center_x = center_x / polygon.GetPointNumer();
    center_y = center_y / polygon.GetPointNumer();

    double front_x = (points[0].x + points[1].x) / 2.0;
    double front_y = (points[0].y + points[1].y) / 2.0;

    DrawArrow(canvas, cv::Point(center_x, center_y), cv::Point(front_x, front_y), ln_color, ln_width);

    return canvas;
}

cv::Mat GeometryDraw::DrawFilledPolygon(cv::Mat canvas, const Polygon &polygon, bool show_dot, cv::Scalar fill_color, cv::Scalar ln_color, int32_t ln_width)
{
    std::size_t pt_num = polygon.GetPointNumer();

    if (pt_num < 3)
        return canvas;

    cv::Point pts[1][100];
    for (int i = 0; i < polygon.GetPointNumer(); ++i)
    {
        auto pt1 = ConvertCartisianToPixel(polygon.GetPoint(i).x, polygon.GetPoint(i).y);
        pts[0][i] = cv::Point(pt1.x, pt1.y);
    }

    const cv::Point *ppt[1] = {pts[0]};
    int npt[] = {static_cast<int>(polygon.GetPointNumer())};

    cv::fillPoly(canvas, ppt, npt, 1, fill_color);

    return canvas;
}

cv::Mat GeometryDraw::WritePointPosition(cv::Mat canvas, const std::vector<SimplePoint> &points)
{
    for (auto &pt : points)
    {
        auto pt1 = ConvertCartisianToPixel(pt.x, pt.y);
        std::string pos_str = "(" + std::to_string(static_cast<int32_t>(pt.x)) + "," + std::to_string(static_cast<int32_t>(pt.y)) + ")";
        WriteText(canvas, pos_str, cv::Point(pt1.x, pt1.y));
    }
}

cv::Mat GeometryDraw::DrawDistribution(cv::Mat canvas, double cx, double cy, double xspan, double yspan, std::function<double(double, double)> dist_fun)
{
    // std::cout << "distribution: " << cx << " , " << cy << " ; " << xmin_ << " , " << xmax_ << " , " << ymin_ << " , " << ymax_ << std::endl;

    assert(cx >= xmin_ && cx < xmax_ && cy >= ymin_ && cy < ymax_);

    // distributions coverage x/y limits
    double dxmin = cx - xspan / 2.0;
    double dxmax = cx + xspan / 2.0;
    double dymin = cy - yspan / 2.0;
    double dymax = cy + yspan / 2.0;

    // crop distribution to canvas area
    if (dxmin < xmin_)
        dxmin = xmin_;
    if (dxmax > xmax_)
        dxmax = xmax_;
    if (dymin < ymin_)
        dymin = ymin_;
    if (dymax > ymax_)
        dymax = ymax_;

    double dxspan = dxmax - dxmin;
    double dyspan = dymax - dymin;
    int32_t x_size = dxspan * pixel_per_meter;
    int32_t y_size = dyspan * pixel_per_meter;

    Eigen::MatrixXd threat_matrix = Eigen::MatrixXd::Zero(y_size, x_size);
    int32_t meter_per_pixel = 1 / pixel_per_meter;
    for (int32_t i = 0; i < x_size; ++i)
        for (int32_t j = 0; j < y_size; ++j)
        {
            // convert to cartisian coordinate
            double x = dxmin + static_cast<double>(i) / pixel_per_meter;
            double y = dymin + static_cast<double>(j) / pixel_per_meter;

            threat_matrix(j, i) = dist_fun(x, y);
        }

    cv::Mat threat_vis = CreateColorMapFromEigenMatrix(threat_matrix, true);

    // merge threat distribution to canvas
    auto top_left_pixel = ConvertCartisianToPixel(dxmin, dymax); // y inverted in cartesian coordinate
    threat_vis.copyTo(canvas(cv::Rect(top_left_pixel.x, top_left_pixel.y, threat_vis.cols, threat_vis.rows)));

    std::cout << int32_t(threat_vis.at<cv::Vec3b>(0, 0)[0]) << " , "
              << int32_t(threat_vis.at<cv::Vec3b>(0, 0)[1]) << " , "
              << int32_t(threat_vis.at<cv::Vec3b>(0, 0)[2]) << std::endl;

    // std::cout << "------------" << std::endl;
    // std::cout << "dx: " << dxmin << " , " << dxmax << std::endl;
    // std::cout << "dy: " << dymin << " , " << dymax << std::endl;
    // std::cout << "size: " << x_size << " , " << y_size << std::endl;
    // std::cout << "threat: " << threat_vis.cols << " , " << threat_vis.rows << std::endl;
    // std::cout << "top left coordinate: " << top_left_pixel.x << " , " << top_left_pixel.y << std::endl;

    return canvas;
}

SimplePoint GeometryDraw::ConvertCartisianToPixel(double xi, double yi)
{
    int32_t x = (xi - xmin_) / xspan_ * canvas_size_x_;
    int32_t y = canvas_size_y_ - (yi - ymin_) / yspan_ * canvas_size_y_;

    if (x < 0)
        x = 0;
    if (y < 0)
        y = 0;
    if (x >= canvas_size_x_)
        x = canvas_size_x_ - 1;
    if (y >= canvas_size_y_)
        y = canvas_size_y_ - 1;
    return SimplePoint(x, y);
}