/**
 * @brief SetpointActuatorControl plugin
 * @file setpoint_actuator_control.cpp
 * @author Marcel Stüttgen <stuettgen@fh-aachen.de>
 *
 * @addtogroup plugin
 * @{
 */
/*
 * Copyright 2015 Marcel Stüttgen.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <mavros/mavros_plugin.h>
#include <pluginlib/class_list_macros.h>

#include<mavros/ActuatorControl.h>

namespace mavplugin {

/**
 * @brief Setpoint actuator control plugin
 *
 * Send setpoint actuator control to FCU controller.
 */
class ActuatorControlPlugin : public MavRosPlugin {

 public:

  //constructor
  ActuatorControlPlugin() : uas_(nullptr) { };

  //init function
  void initialize(UAS &uas, ros::NodeHandle &nh, diagnostic_updater::Updater &diag_updater) {

    uas_ = &uas;
    actuator_controls_sub_ = nh.subscribe("actuator_controls", 10, &ActuatorControlPlugin::actuator_control_cb, this);

  }

  //name of object
  const std::string get_name() const {
    return "SetpointActuatorControl";
  }

  const message_map get_rx_handlers() {
    return { /* Rx disabled */ };
  }

private:
 
  UAS *uas_;

  ros::Subscriber actuator_controls_sub_;


  
  /* -*- low-level send -*- */
  /* message definiton here: https://pixhawk.ethz.ch/mavlink/#SET_ACTUATOR_CONTROL_TARGET */

  void set_actuator_control_target(uint32_t time_boot_ms, uint8_t group_mix, const  float controls[8]) {

    mavlink_message_t msg;
    //todo: get correckt pack chan msg
    /*
    mavlink_msg_set_actuator_control_target_pack_chan(UAS_PACK_CHAN(uas), &msg, 
                                                      time_boot_ms,
                                                      UAS_PACK_TGT(uas),
                                                      group_mix,
                                                      controls)
    */
    UAS_FCU(uas_)->send_message(&msg);
  }

  
  /* -*- callbacks -*- */
  
  void actuator_control_cb(const mavros::ActuatorControl::ConstPtr &req) {

    // about groups, mixing and channels:
    // https://pixhawk.org/dev/mixing

    //call low level send
    set_actuator_control_target(ros::Time::now().toNSec()/1000000,
                                req->group_mix,
                                req->controls.data()); //"controls" is of type boost::array, so .data() will return the pointer                           
  }

};

}; // namespace mavplugin

PLUGINLIB_EXPORT_CLASS(mavplugin::ActuatorControlPlugin, mavplugin::MavRosPlugin)
