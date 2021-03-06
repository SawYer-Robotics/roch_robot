/**
*      _____
*     /  _  \
*    / _/ \  \
*   / / \_/   \
*  /  \_/  _   \  ___  _    ___   ___   ____   ____   ___   _____  _   _
*  \  / \_/ \  / /  _\| |  | __| / _ \ | ++ \ | ++ \ / _ \ |_   _|| | | |
*   \ \_/ \_/ /  | |  | |  | ++ | |_| || ++ / | ++_/| |_| |  | |  | +-+ |
*    \  \_/  /   | |_ | |_ | ++ |  _  || |\ \ | |   |  _  |  | |  | +-+ |
*     \_____/    \___/|___||___||_| |_||_| \_\|_|   |_| |_|  |_|  |_| |_|
*             ROBOTICS™
*
*  File: Message_cmd.cpp
*  Desc: Implements Set Message subclasses.
*  Auth: Iain Peet
*
*  Copyright (c) 2010, Clearpath Robotics, Inc.
*  All Rights Reserved
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of Clearpath Robotics, Inc. nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL CLEARPATH ROBOTICS, INC. BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Please send comments, questions, or patches to skynet@clearpathrobotics.com
*
*/

#include "roch_base/core/Message_cmd.h"


#include <cstring>
#include <iostream>
#include <typeinfo>

using namespace std;

#include "roch_base/core/Number.h"
// Conditions on the below to handle compiling for nonstandard hardware
#ifdef LOGGING_AVAIL
#include "roch_base/core/Logger.h"
#endif
#ifdef DEBUG_INFO
#include "roch_base/roch_hardware.h"//test for ROS_DEBUG_STREAM
#endif
namespace sawyer
{

  long CmdMessage::total_destroyed = 0;
  long CmdMessage::total_sent = 0;

  CmdMessage::~CmdMessage()
  {
    ++total_destroyed;
    if (!is_sent)
    {
#ifdef LOGGING_AVAIL
        CPR_WARN() << "Command message destroyed without being sent. Type: "
            << "0x" << hex << getType() << dec 
            << ". Total unsent: " << (total_destroyed-total_sent) << endl;
#endif
    }
    else
    {
      total_sent++;
    }
  }


  CmdProcessorReset::CmdProcessorReset() : CmdMessage()
  {
    setPayloadLength(2);
    utob(getPayloadPointer(), 2, (uint16_t) 0x3A18);
    setType(CMD_PROCESSOR_RESET);
    makeValid();
  }

  CmdProcessorReset::CmdProcessorReset(const CmdProcessorReset &other) : CmdMessage(other)
  {
  }


  CmdRestoreSettings::CmdRestoreSettings(enum restoreFlags flags) : CmdMessage()
  {
#ifdef DEBUG_INFO
    ROS_DEBUG_STREAM("CmdRestoreSettings: flags:%x",(uint8_t) (flags));
#endif
    setPayloadLength(3);

    utob(getPayloadPointer(), 2, (uint16_t) (0x3a18));
    *getPayloadPointer(2) = (uint8_t) (flags);
    setType(CMD_RESTORE_SETTINGS);

    makeValid();
  }

  CmdRestoreSettings::CmdRestoreSettings(const CmdRestoreSettings &other) : CmdMessage(other)
  {
  }


  CmdStoreSettings::CmdStoreSettings() : CmdMessage()
  {
    setPayloadLength(2);

    utob(getPayloadPointer(), 2, (uint16_t) (0x3a18));
    setType(CMD_STORE_SETTINGS);

    makeValid();
  }

  CmdStoreSettings::CmdStoreSettings(const CmdStoreSettings &other) : CmdMessage(other)
  {
  }


  SetAckermannOutput::SetAckermannOutput(double steer, double throt, double brake) : CmdMessage()
  {
#ifdef DEBUG_INFO
    ROS_DEBUG_STREAM("SetAckermannOutput: steer:%.2lf, throt:%.2lf, brake:%.2lf",steer,throt,brake);
#endif
    setPayloadLength(PAYLOAD_LEN);

    ftob(getPayloadPointer(STEERING), 2, steer, 100);
    ftob(getPayloadPointer(THROTTLE), 2, throt, 100);
    ftob(getPayloadPointer(BRAKE), 2, brake, 100);

    setType(SET_ACKERMANN_SETPT);
    makeValid();
  }

  SetAckermannOutput::SetAckermannOutput(const SetAckermannOutput &other) : CmdMessage(other)
  {
  }


  SetDifferentialControl::SetDifferentialControl(
      double p,
      double i,
      double d)
      : CmdMessage()
  { 
#ifdef DEBUG_INFO
    ROS_DEBUG_STREAM("SetDifferentialControl: P:%.2lf,I:%.2lf,D:%.2lf.", p, i, d);
#endif
  
    setPayloadLength(PAYLOAD_LEN);

    ftob(getPayloadPointer(LEFT_P), 2, p, 10000);
    ftob(getPayloadPointer(LEFT_I), 2, i, 10000);
    ftob(getPayloadPointer(LEFT_D), 2, d, 10000);

    ftob(getPayloadPointer(RIGHT_P), 2, p, 10000);
    ftob(getPayloadPointer(RIGHT_I), 2, i, 10000);
    ftob(getPayloadPointer(RIGHT_D), 2, d, 10000);

    setType(SET_DIFF_CTRL_CONSTS);
    makeValid();
  }

  SetDifferentialControl::SetDifferentialControl(
      double left_p,
      double left_i,
      double left_d,
      double right_p,
      double right_i,
      double right_d)
      : CmdMessage()
  {
#ifdef DEBUG_INFO
    ROS_DEBUG_STREAM("SetDifferentialControl: left_p:%.2lf,left_i:%.2lf,left_d:%.2lf"
				     "right_p:%.2lf,right_i:%.2lf,right_d:%.2lf.", left_p,
       left_i,
       left_d,
       right_p,
       right_i,
       right_d);
#endif
    setPayloadLength(PAYLOAD_LEN);

    ftob(getPayloadPointer(LEFT_P), 2, left_p, 10000);
    ftob(getPayloadPointer(LEFT_I), 2, left_i, 10000);
    ftob(getPayloadPointer(LEFT_D), 2, left_d, 10000);

    ftob(getPayloadPointer(RIGHT_P), 2, right_p, 10000);
    ftob(getPayloadPointer(RIGHT_I), 2, right_i, 10000);
    ftob(getPayloadPointer(RIGHT_D), 2, right_d, 10000);

    setType(SET_DIFF_CTRL_CONSTS);
    makeValid();
  }

  SetDifferentialControl::SetDifferentialControl(const SetDifferentialControl &other)
      : CmdMessage(other)
  {
  }


  SetDifferentialOutput::SetDifferentialOutput(double left, double right) : CmdMessage()
  {
#ifdef DEBUG_INFO
    ROS_DEBUG_STREAM("SetDifferentialOutput: left:%.2lf,right:%.2lf",left,right);
#endif
    setPayloadLength(PAYLOAD_LEN);
    ftob(getPayloadPointer(LEFT), 2, left, 100);
    ftob(getPayloadPointer(RIGHT), 2, right, 100);

    setType(SET_DIFF_WHEEL_SETPTS);
    makeValid();
  }

  SetDifferentialOutput::SetDifferentialOutput(const SetDifferentialOutput &other)
      : CmdMessage(other)
  {
  }


  SetDifferentialSpeed::SetDifferentialSpeed(double left_speed, double right_speed,
      double left_accel, double right_accel)
      : CmdMessage()
  {
    setPayloadLength(PAYLOAD_LEN);
    ftob(getPayloadPointer(LEFT_SPEED), 2, left_speed, 100);
    ftob(getPayloadPointer(LEFT_ACCEL), 2, left_accel, 100);
    ftob(getPayloadPointer(RIGHT_SPEED), 2, right_speed, 100);
    ftob(getPayloadPointer(RIGHT_ACCEL), 2, right_accel, 100);
#ifdef DEBUG_SEND_SPEED
    ROS_DEBUG_STREAM("SetDifferentialSpeed: left_speed:%.2lf,right_speed:%.2lf,left_accel:%.2lf,right_accel:%.2lf",left_speed,right_speed,left_accel,right_accel);
#endif
    setType(SET_DIFF_WHEEL_SPEEDS);
    makeValid();
  }

  SetDifferentialSpeed::SetDifferentialSpeed(const SetDifferentialSpeed &other) : CmdMessage(other)
  {
  }


  SetGear::SetGear(uint8_t gear) : CmdMessage()
  {
#ifdef DEBUG_INFO 
    ROS_DEBUG_STREAM("SetGear: gear:%x",gear);
#endif
    setPayloadLength(1);
    getPayloadPointer()[0] = gear;
    setType(SET_GEAR_SETPOINT);
    makeValid();
  }

  SetGear::SetGear(const SetGear &other) : CmdMessage(other)
  {
  }


  SetMaxAccel::SetMaxAccel(double max_fwd, double max_rev) : CmdMessage()
  {
    setPayloadLength(PAYLOAD_LEN);

    ftob(getPayloadPointer(MAX_FWD), 2, max_fwd, 100);
    ftob(getPayloadPointer(MAX_REV), 2, max_rev, 100);
#ifdef DEBUG_INFO
    ROS_DEBUG_STREAM("SetMaxAccel: max_fwd:%.2lf, max_rev:%.2lf",max_fwd,max_rev);
#endif
    setType(SET_MAX_ACCEL);
    makeValid();
  }

  SetMaxAccel::SetMaxAccel(const SetMaxAccel &other) : CmdMessage(other)
  {
  }


  SetMaxSpeed::SetMaxSpeed(double max_fwd, double max_rev) : CmdMessage()
  {
    setPayloadLength(PAYLOAD_LEN);

    ftob(getPayloadPointer(MAX_FWD), 2, max_fwd, 100);
    ftob(getPayloadPointer(MAX_REV), 2, max_rev, 100);
#ifdef DEBUG_INFO
    ROS_DEBUG_STREAM("SetMaxSpeed: max_fwd:%.2lf, max_rev:%.2lf",max_fwd,max_rev);
#endif
    setType(SET_MAX_SPEED);
    makeValid();
  }

  SetMaxSpeed::SetMaxSpeed(const SetMaxSpeed &other) : CmdMessage(other)
  {
  }


  SetPlatformName::SetPlatformName(const char *name) : CmdMessage()
  {
    size_t cpy_len = strlen(name);
    size_t max_len = MAX_MSG_LENGTH - HEADER_LENGTH - CRC_LENGTH - 1 /* for size field */;
    if (cpy_len > max_len) { cpy_len = max_len; }
#ifdef DEBUG_INFO
    ROS_DEBUG_STREAM("SetPlatformName: name:%s",name);
#endif
    setPayloadLength(cpy_len + 1);
    getPayloadPointer()[0] = cpy_len;
    memcpy(getPayloadPointer(1), name, cpy_len);

    setType(SET_PLATFORM_NAME);

    makeValid();
  }

  SetPlatformName::SetPlatformName(const SetPlatformName &other) : CmdMessage(other)
  {
  }


  SetPlatformTime::SetPlatformTime(uint32_t time) : CmdMessage()
  {
#ifdef DEBUG_INFO  
    ROS_DEBUG_STREAM("SetPlatformTime: name:%d",time);
#endif
    setPayloadLength(4);
    utob(getPayloadPointer(), 4, time);
    setType(SET_PLATFORM_TIME);
    makeValid();
  }

  SetPlatformTime::SetPlatformTime(const SetPlatformTime &other) : CmdMessage(other)
  {
  }


  SetSafetySystem::SetSafetySystem(uint16_t flags) : CmdMessage()
  {
#ifdef DEBUG_INFO 
    ROS_DEBUG_STREAM("SetSafetySystem: flags:%x",flags);
#endif 
    setPayloadLength(2);
    utob(getPayloadPointer(), 2, flags);
    setType(SET_SAFETY_SYSTEM);
    makeValid();
  }

  SetSafetySystem::SetSafetySystem(const SetSafetySystem &other) : CmdMessage(other)
  {
  }


  SetTurn::SetTurn(double trans, double rad, double accel) : CmdMessage()
  {
    setPayloadLength(PAYLOAD_LEN);
#ifdef DEBUG_INFO 
    ROS_DEBUG_STREAM("SetTurn: rad:%.2lf, accel:%.2lf",trans,accel);
#endif 
  
    ftob(getPayloadPointer(TRANSLATIONAL), 2, trans, 100);
    ftob(getPayloadPointer(TURN_RADIUS), 2, rad, 100);
    ftob(getPayloadPointer(TRANS_ACCEL), 2, accel, 100);

    setType(SET_TURN_SETPT);
    makeValid();
  }

  SetTurn::SetTurn(const SetTurn &other) : CmdMessage(other)
  {
  }

  SetWheelInfo::SetWheelInfo(double wheel_gauge, double wheel_diameter) : CmdMessage()
  {
    setPayloadLength(PAYLOAD_LEN);
#ifdef DEBUG_INFO
    ROS_DEBUG_STREAM("Set Wheel gauge: %.2lf, wheel diameter: %.2lf .",wheel_gauge,wheel_diameter);
#endif

    ftob(getPayloadPointer(WHEEL_GAUGE), 2, wheel_gauge, 10000);
    ftob(getPayloadPointer(WHEEL_DIAMETER), 2, wheel_diameter, 10000);

    setType(SET_WHEEL_INFO);
    makeValid();
  }
 
  SetWheelInfo::SetWheelInfo(const SetWheelInfo &other) : CmdMessage(other)
  {
  }

  SetVelocity::SetVelocity(double trans, double rot, double accel) : CmdMessage()
  {
    setPayloadLength(PAYLOAD_LEN);
#ifdef DEBUG_INFO 
    ROS_DEBUG_STREAM("SetVelocity: trans:%.2lf, rot:%.2lf, accel:%.2lf",trans,rot,accel);
#endif 
    
    ftob(getPayloadPointer(TRANSLATIONAL), 2, trans, 100);
    ftob(getPayloadPointer(ROTATIONAL), 2, rot, 100);
    ftob(getPayloadPointer(TRANS_ACCEL), 2, accel, 100);

    setType(SET_VELOCITY_SETPT);
    makeValid();
  }

  SetVelocity::SetVelocity(const SetVelocity &other) : CmdMessage(other)
  {
  }


}; // namespace sawyer

