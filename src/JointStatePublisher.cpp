#include <robotnik_joint_state_publisher/JointStatePublisher.h>
#include <boost/algorithm/string.hpp>
JointStatePublisher::JointStatePublisher(ros::NodeHandle h) : RComponent(h), nh_(h), pnh_("~")
{
  component_name.assign(pnh_.getNamespace());

  rosReadParams();
}

JointStatePublisher::~JointStatePublisher()
{
}

void JointStatePublisher::rosReadParams()
{
  std::string joint_state_topics = "";
  readParam(pnh_, "joint_state_topics", joint_state_topics, joint_state_topics);
  boost::split(joint_state_topics_, joint_state_topics, boost::is_any_of(" "));
}

int JointStatePublisher::rosSetup()
{
  RComponent::rosSetup();

  // Subscribers
  joint_state_subs_.resize(joint_state_topics_.size());
  joint_state_msgs_.resize(joint_state_topics_.size());
  for (size_t i = 0; i < joint_state_topics_.size(); i++)
  {
    joint_state_subs_[i] = nh_.subscribe<sensor_msgs::JointState>(joint_state_topics_[i], 1, 
                            boost::bind(&JointStatePublisher::jointStateCb, this, _1, i));
  }

  joint_state_pub_ = pnh_.advertise<sensor_msgs::JointState>("joint_states", 1);
  
}

void JointStatePublisher::initState()
{
  switchToState(robotnik_msgs::State::STANDBY_STATE);
}

void  JointStatePublisher::standbyState()
{
  switchToState(robotnik_msgs::State::READY_STATE);
}

void  JointStatePublisher::readyState()
{
  updateJointState();
}

void JointStatePublisher::rosPublish()
{
  RComponent::rosPublish();
  joint_state_pub_.publish(joint_state_);
}

void JointStatePublisher::failureState()
{

}

void JointStatePublisher::emergencyState()
{

}

void JointStatePublisher::jointStateCb(const sensor_msgs::JointStateConstPtr& input, int index)
{
  joint_state_msgs_[index] = *input;
}

void JointStatePublisher::updateJointState()
{
  joint_state_ = sensor_msgs::JointState();

  for ( int i = 0; i < joint_state_msgs_.size(); i++ )
  {
    for (int j = 0; j < joint_state_msgs_[i].name.size(); j++)
    {
      joint_state_.name.push_back(joint_state_msgs_[i].name[j]);
      joint_state_.position.push_back(joint_state_msgs_[i].position[j]);
      joint_state_.velocity.push_back(joint_state_msgs_[i].velocity[j]);
      joint_state_.effort.push_back(joint_state_msgs_[i].effort[j]);
    }
  }
}

