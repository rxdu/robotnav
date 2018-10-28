#include <iostream>

#include "canvas/cv_draw.hpp"

using namespace librav;

int main()
{
    // Mat image;
    // image = imread("/home/rdu/Workspace/librav/data/intensity.jpg", CV_LOAD_IMAGE_COLOR);   // Read the file
    std::string file_name = "/home/rdu/Workspace/librav/data/intensity.jpg";

    CvDraw::ShowImage(file_name, "test greyscale");

    return 0;
}