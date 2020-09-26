#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
	
	ROS_INFO_STREAM("Moving the robot based on commands");

    // Request centered joint angles [1.57, 1.57]
   ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    // Call the safe_move service and pass the requested joint angles
    if (!client.call(srv))
        ROS_ERROR("Failed to call service safe_move");
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
	
	int step=img.step;// step size of the pixel
	int height=img.height; // height or number of rows of pixel
	int left_side_boundary=1/3*(step);// Total imagee width is divided in 3 parts,left_part is between 0 to 1/3 of stepwidth
	int mid_way_boundary=2/3*(step);// mid part is 1/3 to 2/3 of step width 
	int right_side_boundary =step;  //right_part is 2/3 to 1 of step width 
	int non_white_count=0; // to count non white pixel in an image

    
 
	
	// TODO: Loop through each pixel in the image and check if there's a bright white one
    for (int i = 0; i < height * step; i++) {
		// Then, identify if this pixel falls in the left, mid, or right side of the image
        if ((img.data[i] -white_pixel)==0) { // condition to identify white pixel
			int width= i/height;// gives column index of white pixe
			if ( width>=0  && width<left_side_boundary){ // condition to check pixel falls on left side of image
				drive_robot(0.2,-0.1);     // Depending on the white ball position, call the drive_bot function and pass velocities to it

				break;
				
			}else if(width>=left_side_boundary && width<mid_way_boundary){// condition to check pixel falls on left side of image
				drive_robot(0.2,0);     // Depending on the white ball position, call the drive_bot function and pass velocities to it

				break;
				
			}else if(width>=mid_way_boundary && width<right_side_boundary){//Condition to check pixel falls on right side of image
				drive_robot(0.2,0.1);     // Depending on the white ball position, call the drive_bot function and pass velocities to it

				break;
				
			}
            
        }else {// count one for each  non white pixel
		non_white_count=non_white_count+1;
	}
		
    }
	
	if (non_white_count==((height * step) -1) ){ // if all pixel in a image are non white pixel ,stop the robot
		drive_robot(0,0);     // , call the drive_bot function and pass  zero velocities to it

	}
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

