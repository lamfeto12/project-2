#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
    ROS_INFO_STREAM("Try to move the robot");
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    if (!client.call(srv))
        ROS_ERROR("failed to call service");
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
    enum SIDE {left, right, mid, no_ball};
    auto which_side = no_ball;
    // TODO: Loop through each pixel in the image and check if there's a bright white one
    for (int i = 0; i < img.height * img.step; i+=3){
        auto red = img.data[i];
        auto blue = img.data[i+1];
        auto green = img.data[i+2];
        // Then, identify if this pixel falls in the left, mid, or right side of the image
        if ((red == white_pixel) && (blue == white_pixel) && (green == white_pixel)){
            auto column_number = i % img.step;
            if (column_number < img.step*1/3){
                which_side = left;}
                
            else if(column_number > img.step*2/3){         
                which_side = right;}

            else{              
                which_side = mid;} 
            break;
       }                   
    }
   
    // Depending on the white ball position, call the drive_bot function and pass velocities to it

    if (which_side == left){
        drive_robot(0.5,1.0);}
    else if (which_side == right){
        drive_robot(0.5,-1.0);}
    else if (which_side == mid){
        drive_robot(0.5,0);}
    // Request a stop when there's no white ball seen by the camera
    else{
        drive_robot(0,0);}
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
