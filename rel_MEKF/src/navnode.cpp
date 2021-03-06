 /* \copyright This work was completed by Robert Leishman while performing official duties as 
  * a federal government employee with the Air Force Research Laboratory and is therefore in the 
  * public domain (see 17 USC § 105). Public domain software can be used by anyone for any purpose,
  * and cannot be released under a copyright license
  */

/*!
 *  \file navnode.cpp
 *  \author Robert Leishman
 *  \date June 2012
 *  \brief Implements the methods defined in navnode.h
*/


#include "rel_estimator/navnode.h"
#include "rel_estimator/navedge.h"

using namespace Eigen;


//
// Default constructor
//
NavNode::NavNode(int id_number):node_id_(id_number)
{
  //Initialize the matrices (except the truth)
  true_global_position_.setZero();
  estimate_global_position_.setZero();
  R_node_to_body_.setIdentity();
  estimate_orientation_.setIdentity();
  truth_set_ = false;
}


//
//  Construct a node with Truth information
//
NavNode::NavNode(TRUTH_message &truth_data, int id_number):node_id_(id_number)
{
  setTruePose(truth_data);
  estimate_orientation_.setIdentity();
}


//
//  Set the estimated global position and yaw
//
void NavNode::setEstimatePosition(Eigen::Vector3d &position, double psi_g, Eigen::Quaterniond &orientation)
{
  estimate_global_position_ = position; 
  //Pull out phi & theta from orientation, use the yaw, then recompute a global quaternion
  NavEdge temp;
  Vector3d true_euler = temp.computeEulerFromQuat(orientation);
  phi_i_ = 0;//true_euler(0);
  theta_i_ = 0;//true_euler(1);
  estimate_yaw_ = psi_g;

  Quaterniond q;
  q.w() = cos(estimate_yaw_/2.0)*cos(theta_i_/2.0)*cos(phi_i_/2.0) + sin(estimate_yaw_/2.0)*sin(theta_i_/2.0)*sin(phi_i_/2.0);
  q.x() = cos(estimate_yaw_/2.0)*cos(theta_i_/2.0)*sin(phi_i_/2.0) - sin(estimate_yaw_/2.0)*sin(theta_i_/2.0)*cos(phi_i_/2.0);
  q.y() = cos(estimate_yaw_/2.0)*sin(theta_i_/2.0)*cos(phi_i_/2.0) + sin(estimate_yaw_/2.0)*cos(theta_i_/2.0)*sin(phi_i_/2.0);
  q.z() = sin(estimate_yaw_/2.0)*cos(theta_i_/2.0)*cos(phi_i_/2.0) - cos(estimate_yaw_/2.0)*sin(theta_i_/2.0)*sin(phi_i_/2.0);
  //q.normalize();
  estimate_orientation_ = q;

  createRotationMatrix();
}


//
//  Set the true pose when the default constructor is used
//
void NavNode::setTruePose(TRUTH_message truth_data)
{
  true_global_position_ << truth_data.transform.translation.x, truth_data.transform.translation.y,
                           truth_data.transform.translation.z;

  true_orientation_.x() = truth_data.transform.rotation.x;
  true_orientation_.y() = truth_data.transform.rotation.y;
  true_orientation_.z() = truth_data.transform.rotation.z;
  true_orientation_.w() = truth_data.transform.rotation.w;

  NavEdge temp;
  Vector3d true_euler = temp.computeEulerFromQuat(true_orientation_);
//  phi_i_ = true_euler(0);
//  theta_i_ = true_euler(1);
  true_yaw_ = true_euler(2);

//  createRotationMatrix();
  truth_set_ = true;
}



//
//  Create the rotation matrix using phi_i and theta_i (from UAV Book: Beard and McLain)
//
void NavNode::createRotationMatrix()
{
  Matrix3d roll, pitch;
  roll << 1.d, 0.d,       0.d,
          0.d,cos(phi_i_),sin(phi_i_),
          0.d,-sin(phi_i_),cos(phi_i_);
  pitch << cos(theta_i_), 0.d, -sin(theta_i_),
           0.d,           1.d, 0.d,
           sin(theta_i_), 0.d, cos(theta_i_);

  R_node_to_body_ = roll*pitch;
}
