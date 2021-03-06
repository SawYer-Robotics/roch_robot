#include <ros/ros.h>
#include "roch_base/roch_hardware.h"

#include <boost/assign/list_of.hpp>
#include <float.h>
#define polling_timeout_ 20
using namespace std;
namespace
{
  const uint8_t LEFT = 0, RIGHT = 1;
};

double speed_x;
double speed_z;
void publishRawData()
{
#if 1
        std::ostringstream ostream;
      sawyer::base_data::RawData data =sawyer::Transport::instance().getdata();
      ostream << " { " ;
      ostream << std::setfill('0') << std::uppercase;
      for (unsigned int i=0; i < data.length; i++)
          ostream << std::hex << std::setw(2) << static_cast<unsigned int>(data.data[i]) << " " << std::dec;
      ostream << "}";
      std_msgs::StringPtr msg(new std_msgs::String);
      msg->data = ostream.str();
      ROS_INFO("reveive command:%s",msg->data.c_str());
#endif
}

void controloverallSpeed(double lin_vel, double ang_vel, double accel_left, double accel_right)
  {
    bool success = false;
    
    double acc = (accel_left + accel_right)*0.5;
    
    while (!success)
    {
      try
      {
//	sawyer::SetVelocity(lin_vel,ang_vel,acc).send();
    sawyer::SetWheelInfo(lin_vel,ang_vel).send();
        success = true;
      }
      catch (sawyer::Exception *ex)
      {
        ROS_ERROR_STREAM("Error sending Wheel Info command: " << ex->message);
       
      }
    }
  }
void speedCallBack(const geometry_msgs::Twist::ConstPtr& speed){
  
    //cout<<"setspeed "<<speed_x<<"  "<<speed_z<<endl;
  speed_x = speed->linear.x;
  speed_z = speed->angular.z;
  
  
    controloverallSpeed(speed_x, speed_z, 0, 0);   
      
    publishRawData();
  core::Channel<sawyer::DataWheelInfo>::Ptr wheelData = core::Channel<sawyer::DataWheelInfo>::requestData(
     polling_timeout_);
   publishRawData();
     if(wheelData){
      ROS_INFO_STREAM("Received  Wheel data information, Wheel Gauge: "<<wheelData->getWheelGauge()<<", Wheel Diameter: "<<wheelData->getWheelDiameter());    
      
    }
    else{
       ROS_ERROR("Could not get Wheel Data form MCU.");
    }
#if 0
    core::Channel<sawyer::DataVelocity>::Ptr overallspeed = core::Channel<sawyer::DataVelocity>::requestData(
      polling_timeout_);
      publishRawData();
    if (overallspeed)
    {
      ROS_DEBUG_STREAM("Received  speed information (speed:" << overallspeed->getTranslational() << " Rotational:" << overallspeed->getRotational()<<" Acceleration:"<<overallspeed->getTransAccel() << ")");

      ROS_INFO("Received  speed information,  speed:%.2lf, Rotational:%.2lf, Acceleration:%.2lf",overallspeed->getTranslational(),overallspeed->getRotational(),overallspeed->getTransAccel());     

    }
#endif
}
ros::Subscriber speedSubscriber_;
int main(int argc, char** argv){
  
  ros::init(argc,argv,"test_imu");
  std::string port = "/dev/roch";
  core::connect(port);
  
  ros::NodeHandle nh;
  speedSubscriber_ = nh.subscribe("/twist_mux/keyboard_teleop/cmd_vel",100000,&speedCallBack);

    ROS_INFO("begin send requestData");
 
   ros::spin();
  return 0;
}
