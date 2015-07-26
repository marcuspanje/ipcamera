#include <string> 
#include <iostream> 

#include "ros/ros.h"
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <sensor_msgs/CameraInfo.h>
#include <sensor_msgs/distortion_models.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

int main(int argc, char** argv) {

   //advertise initialize publisher
   ros::init(argc, argv, "image_publisher");
   ros::NodeHandle nh("");
   image_transport::ImageTransport it(nh);
   image_transport::Publisher img_pub = it.advertise("image_raw", 1);
   ros::Publisher cam_info_pub = nh.advertise<sensor_msgs::CameraInfo>("camera_info", 1, true); 

   //load camera parameters
   //use getParam because no default values should be provided
   std::string camera;
   nh.param("camera", camera, std::string("camera"));

   //std::vector<double> K(9);
   std::vector<double> K, D, R, P; 
   
   int width, height;
   std::string distortion_model;
   sensor_msgs::CameraInfo cam_info;
   cam_info.header = std_msgs::Header();
   cam_info.header.frame_id = camera;
   
   bool got_camera_calibration = 
     nh.getParam("/image_width", width) &&
     nh.getParam("/image_height", height) &&
     nh.getParam("/distortion_model", distortion_model) &&
     nh.getParam("/camera_matrix/data", K) && 
     nh.getParam("/distortion_coefficients/data", D) && 
     nh.getParam("/rectification_matrix/data", R) && 
     nh.getParam("/projection_matrix/data", P); 
   if (!got_camera_calibration) {
     ROS_ERROR("Could not get camera calibration data");       
   }
   
   cam_info.width = width;
   cam_info.height = height;
   cam_info.distortion_model = distortion_model;
   //D has no set size, so it is a std::vector. The rest are a boost::array
   std::copy(K.begin(), K.end(), cam_info.K.begin());
   cam_info.D = D;
   std::copy(R.begin(), R.end(), cam_info.R.begin());
   std::copy(P.begin(), P.end(), cam_info.P.begin());
   
   
   //get video parameters
   std::string user, pwd, host_port, encoding;
   int rate, resolution;
   double publish_rate;
   nh.param("user", user, std::string("admin")); 
   nh.param("pwd", pwd, std::string("")); 
   nh.param("host_port", host_port, std::string("192.168.131.18:80")); 
   nh.param("rate", rate, 6);//foscam code for 10 fps 
   nh.param("resolution", resolution, 32);//foscam code for 640x480 
   nh.param("publish_rate", publish_rate, 10.0);//foscam code for 640x480 
   nh.param("encoding", encoding, std::string("bgr8"));

  //open video stream at specified url based on parameters 
   char buffer[256];
   sprintf(buffer, "http://%s/videostream.cgi?user=%s&pwd=%s&rate=%d&resolution=%d&x.mjpeg",
        host_port.c_str(), user.c_str(), pwd.c_str(), rate, resolution); 
   std::string url(buffer);
   ROS_INFO("Opening video at: %s", url.c_str());
   cv::VideoCapture vcap(url);
   if (!vcap.isOpened()) {
      ROS_WARN("Could not open video stream");
      return -1;
   }

   //publish image
   cv::Mat frame;
   sensor_msgs::ImagePtr msg;
   ros::Rate loop_rate(publish_rate);
   ROS_INFO("Publishing image at %f hz.", publish_rate);
   while(nh.ok()) {
      vcap.read(frame);
      if (!frame.empty()) {
         msg = cv_bridge::CvImage(std_msgs::Header(), encoding.c_str(), frame).toImageMsg();
         img_pub.publish(msg);
         cam_info_pub.publish(cam_info);
         cv::waitKey(1);
      }
   }
   
}
