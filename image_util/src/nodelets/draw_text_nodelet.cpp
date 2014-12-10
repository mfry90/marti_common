// *****************************************************************************
//
// Copyright (c) 2014, Southwest Research Institute® (SwRI®)
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of Southwest Research Institute® (SwRI®) nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// *****************************************************************************

#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <ros/ros.h>
#include <nodelet/nodelet.h>
#include <image_transport/image_transport.h>
#include <sensor_msgs/image_encodings.h>
#include <sensor_msgs/Image.h>
#include <cv_bridge/cv_bridge.h>
#include <math_util/math_util.h>

namespace image_util
{
  class DrawTextNodelet : public nodelet::Nodelet
  {
  public:
      DrawTextNodelet() :
        text_("label"),
        offset_x_(0),
        offset_y_(0),
        font_scale_(1.0),
        font_thickness_(1)
    {
    }

    ~DrawTextNodelet()
    {
    }

    void onInit()
    {
      ros::NodeHandle &node = getNodeHandle();
      ros::NodeHandle &priv = getPrivateNodeHandle();

      priv.param("text", text_, text_);
      priv.param("offset_x", offset_x_, offset_x_);
      priv.param("offset_y", offset_y_, offset_y_);
      priv.param("font_scale", font_scale_, font_scale_);
      priv.param("font_thickness", font_thickness_, font_thickness_);

      image_transport::ImageTransport it(node);
      image_pub_ = it.advertise("stamped_image", 1);
      image_sub_ = it.subscribe("image", 1, &DrawTextNodelet::ImageCallback, this);
    }

    void ImageCallback(const sensor_msgs::ImageConstPtr& image)
    {
      cv_bridge::CvImagePtr cv_image = cv_bridge::toCvCopy(image);

      cv::putText(
        cv_image->image, 
        text_,
        cv::Point(offset_x_, offset_y_),
        cv::FONT_HERSHEY_SIMPLEX, 
        font_scale_,
        cv::Scalar(255, 255, 255),
        font_thickness_);

      image_pub_.publish(cv_image->toImageMsg());
    }

  private:
    std::string text_;
    double offset_x_;
    double offset_y_;
    double font_scale_;
    int font_thickness_;
    
    image_transport::Subscriber image_sub_;
    image_transport::Publisher image_pub_;
  };
}

// Register nodelet plugin
#include <pluginlib/class_list_macros.h>
PLUGINLIB_DECLARE_CLASS(
    image_util,
    draw_text,
    image_util::DrawTextNodelet,
    nodelet::Nodelet)
