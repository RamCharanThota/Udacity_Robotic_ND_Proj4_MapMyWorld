#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>
 #include <ros/console.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    
	
	ROS_INFO_STREAM("Moving the robot based on commands");

   //Requesting a service and passing the velocities to it to drive the robot
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
	int height=img.height; // height or number of rows of pixe
    int width=img.width;// width or number of columns of pixel
	int left_side_boundary=(width)/3;// Total imagee width is divided in 3 parts,left_part is between 0 to 1/3 of width
	int mid_way_boundary=(2*width)/3;// mid part is 1/3 to 2/3 of  width 
	int right_side_boundary =width;  //right_part is 2/3 to 1 of  width 
	int num_of_Wpix_left_side=0; // used to count number of pixel on left side of image
	int num_of_Wpix_right_side=0; //used to count number of pixel on right side of image
	int num_of_Wpix_mid=0;// used to count number of pixel in mida way

    
 
	
	// TODO: Loop through each pixel in the image and check if there's a bright white one
    for (int i = 0; i < height * step; i=i+3) {
		// Then, identify if this pixel falls in the left, mid, or right side of the image
        if ((img.data[i] -white_pixel)==0&&(img.data[i+1] -white_pixel)==0&&(img.data[i+2] -white_pixel)==0) { // condition to identify white pixel
			
			int colm_ind_of_pix= (i/3)%width;// gives column index of white pixel
                         ROS_DEBUG("index %d",colm_ind_of_pix ); 
			
			if ( colm_ind_of_pix>=0  && colm_ind_of_pix<left_side_boundary){ 
				// condition to check pixel falls on mid_way of image
				num_of_Wpix_left_side=num_of_Wpix_left_side+1;
                                 ROS_DEBUG("num_of_Wpix_left_side %d",num_of_Wpix_left_side );        
				
			}else if(colm_ind_of_pix>=left_side_boundary && colm_ind_of_pix<mid_way_boundary){
				// condition to check pixel falls on right side of image
				 
                                 num_of_Wpix_mid=num_of_Wpix_mid+1; 
                                ROS_DEBUG("num_of_Wpix_mid %d",num_of_Wpix_mid );   
    

				
				
			}else if(colm_ind_of_pix>=mid_way_boundary && colm_ind_of_pix<right_side_boundary){
				//Condition to check pixel falls on right side of image
				 num_of_Wpix_right_side=num_of_Wpix_right_side+1;
                                 ROS_DEBUG("num_of_Wpix_right_side %d",num_of_Wpix_right_side );
				
				
			}
            
        }
		
    }// end of for loop
	
	if (num_of_Wpix_left_side>num_of_Wpix_right_side&&num_of_Wpix_left_side>num_of_Wpix_mid){
		drive_robot(0.2,0.1);//drive left
	}else if(num_of_Wpix_mid>num_of_Wpix_left_side&&num_of_Wpix_mid>num_of_Wpix_right_side){
		drive_robot(0.2,0);// drive straight 
	}else if(num_of_Wpix_right_side>num_of_Wpix_left_side&&num_of_Wpix_right_side>num_of_Wpix_mid){
		drive_robot(0.2,-0.1);//drive right
	}else{
		drive_robot(0,0);// stop driving
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

