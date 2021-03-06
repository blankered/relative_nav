 /* \copyright This work was completed by Robert Leishman while performing official duties as 
  * a federal government employee with the Air Force Research Laboratory and is therefore in the 
  * public domain (see 17 USC § 105). Public domain software can be used by anyone for any purpose,
  * and cannot be released under a copyright license
  */

/*!
 *  \file hex_planner.h
 *  \author Robert Leishman
 *  \date October 2012
 *  \brief This file contains the class implementation of a short-term path planner/obstacle avoider.
*/

#ifndef HEX_PLANNER_H
#define HEX_PLANNER_H


#include <ros/ros.h>
#include <tf/transform_listener.h>
#include <costmap_2d/costmap_2d_ros.h>
#include <navfn/navfn_ros.h>
#include <geometry_msgs/PoseStamped.h>
#include <vector>
#include <string>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include "rel_MEKF/edge.h"



/*!
 *  \class HexPlanner hex_planner.h "include/hex_planner.h"
 *  \brief This class implementes the costmaps_2d and navfn ROS wrapper packages to create a local planner/obstacle
 *  avoider for a hexacopter UAV.  Currently this planning is done in a rolling window of the global frame, but
 *  it needs to eventually be brought over to a relative planner using the nodes/keyframes used by the rel_MEKF package.
 *
*/
class HexPlanner
{
public:

    /*!
     *  \brief The constructor of the class.  Simply provide it a nodehandle and a costmap2DROS pointer
     *  \param nh The ROS nodehandle
     *  \param costmp Is the pointer to the costmap created in main
    */
    HexPlanner(ros::NodeHandle &nh, costmap_2d::Costmap2DROS *costmp);

    /*!
     *  \brief The destructor for the class...
     *
    */
    ~HexPlanner();


    /*!
     *  \brief This function is called by to update the plan.
     *  It publishes the plan when sucessful
    */
    bool updatePlan();


    /// \brief Return the last timestamp a plan was sucessful
    inline ros::Time getLastPlanTimestamp()
    {
      return last_plan_timestamp_;
    }


    inline bool initGoalLocationRecieved(){ return initial_goal_location_recieved_; }

private:


    /*!
     *  \brief This function is called when a goal location is published
     *  it updates the goal location and calls for a new plan to be created
    */
    void recieveGoalLocation(const geometry_msgs::PoseStamped goal_loc);


    /*!
     *  \brief This function applies edges to modify the goal location when working with relative states
    */
    void applyEdgeFromNewNode(const rel_MEKF::edge &edge_message);


    ros::Subscriber goal_subscriber_; //!< this subscribes to a goal location
    ros::Subscriber edge_sub_; //!< subscribes to relative edges to modify the goal location
    bool relative_plans_; //!< flag for when using relative plans

    costmap_2d::Costmap2DROS *costmap_; //!< a pointer to 2D costmap using the ROS wrapper.
    navfn::NavfnROS planner_; //!< the navfn ros wrapper planner
    geometry_msgs::PoseStamped goal_location_; //!< current goal location (in global coordinates)!
    bool initial_goal_location_recieved_; //!< useful flag for not trying to plan a ton right at the first
    double max_path_length_; //!< set the max path length
    ros::Time last_plan_timestamp_;

};






#endif
