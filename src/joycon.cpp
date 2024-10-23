#include <ros/ros.h>
#include <sensor_msgs/Joy.h>
#include <geometry_msgs/Twist.h>


class JoyCtrlRover
{
public:
  JoyCtrlRover();
  void joyPub();

private:
  void joyCallback(const sensor_msgs::Joy::ConstPtr& joy);
  

  ros::NodeHandle nh_;

  int linear_x_,linear_y_, angular_, safety_;
  double l_scale_, a_scale_;
  ros::Publisher vel_pub_;
  ros::Subscriber joy_sub_;
  geometry_msgs::Twist twist_;

};


JoyCtrlRover::JoyCtrlRover():
  linear_x_(1),
  linear_y_(0),
  angular_(3),
  a_scale_(0.8),
  l_scale_(0.6),
  safety_(7)
{

  nh_.param("/joycon/axis_linear_x", linear_x_, linear_x_);
  nh_.param("/joycon/axis_linear_y", linear_y_, linear_y_);
  nh_.param("/joycon/axis_angular", angular_, angular_);
  nh_.param("/joycon/scale_angular", a_scale_, a_scale_);
  nh_.param("/joycon/scale_linear", l_scale_, l_scale_);
  nh_.param("/joycon/safety_button", safety_, safety_);


  vel_pub_ = nh_.advertise<geometry_msgs::Twist>("rover_twist", 1);


  joy_sub_ = nh_.subscribe<sensor_msgs::Joy>("joy", 1, &JoyCtrlRover::joyCallback, this);

}

void JoyCtrlRover::joyCallback(const sensor_msgs::Joy::ConstPtr& joy)
{
  if(joy->buttons[safety_]){
    twist_.angular.z = a_scale_*joy->axes[angular_];
    twist_.linear.x = l_scale_*joy->axes[linear_x_];
    twist_.linear.y = l_scale_*joy->axes[linear_y_];
  }else{
    twist_.angular.z =0;
    twist_.linear.x = 0;
    twist_.linear.y = 0;
  }
  
}

void JoyCtrlRover::joyPub()
{
  vel_pub_.publish(twist_);
}


int main(int argc, char** argv)
{
  ros::init(argc, argv, "joycon");
  JoyCtrlRover joy_ctrl_mecanumrover;

  ros::NodeHandle n;

  ros::Rate r(10);
  while(n.ok()){
    ros::spinOnce();
    joy_ctrl_mecanumrover.joyPub();
    r.sleep();
  }
}
