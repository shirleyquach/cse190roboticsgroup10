#include <ros/ros.h>
#include <iostream>
#include <assignment_5/Mode.h>
#include <termios.h>
#include <cstdlib>
#include <signal.h>

using namespace std;

int getch()
{
  static struct termios oldt, newt;
  tcgetattr( STDIN_FILENO, &oldt);           // save old settings
  newt = oldt;
  newt.c_lflag &= ~(ICANON);                 // disable buffering
  tcsetattr( STDIN_FILENO, TCSANOW, &newt);  // apply new settings

  int c = getchar();  // read character (non-blocking)

  tcsetattr( STDIN_FILENO, TCSANOW, &oldt);  // restore old settings
  return c;
}

void quit(int sig)
{
    ros::shutdown();
    exit(EXIT_SUCCESS);
}

int main( int argc, char * argv[] )
{
    static int mode;
    char input;

    ros::init(argc, argv, "motion_mode_keyboard");
    ros::NodeHandle node;
    ros::ServiceClient client = node.serviceClient<assignment_5::Mode>("Mode");

    signal(SIGINT, quit);


    /* Prompt for input */
    printf( "\nSelect a mode: " );
    printf( "\n\t1\tFarneback Optical Flow." );
    printf( "\n\t2\tImproved Mixture of Guassian (MOG2) Background Subtration.\n" );
    while ( ros::ok() )
    {
        assignment_5::Mode srv;
        input = getch();
        if ( input - '0' != mode && (input == '1' || input == '2') )
        {
            /* Converts input to int and sets the request to this int */
            srv.request.mode = input - '0';

            if( client.call(srv) )
            {
                mode = input - '0';
                if ( mode == 1 )
                {
                    ROS_INFO("Farneback Optical Flow Mode Requested");
                }
                else if ( mode == 2 )
                {
                    ROS_INFO("Improved Mixture of Gaussian (MOG2) Background Subtraction Mode Requested");
                }
                ROS_INFO("Request succesfully received");
            }
            else
            {
                ROS_ERROR("Failed to call service Mode");
                return EXIT_FAILURE;
            }
        }
    }
    return EXIT_SUCCESS;
}
