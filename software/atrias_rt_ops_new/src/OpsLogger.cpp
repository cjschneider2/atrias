#include "atrias_rt_ops/OpsLogger.h"

namespace atrias {

namespace rtOps {

OpsLogger::OpsLogger(RTT::OutputPort<atrias_msgs::rt_ops_cycle>*  log_cyclic_out,
                     RTT::OutputPort<atrias_msgs::rt_ops_cycle>*  gui_cyclic_out,
                     RTT::OutputPort<atrias_msgs::rt_ops_event>* event_out) :
                     guiPublishTimer() {
	logCyclicOut = log_cyclic_out;
	guiCyclicOut = gui_cyclic_out;
	eventOut     = event_out;
}

void OpsLogger::beginCycle() {
	RTT::os::TimeService::nsecs startTime = RTT::os::TimeService::Instance()->getNSecs();
	
	// Check for a controller overshoot. If the controllers' finish time is earlier
	// than the start time, then it must be from a previous iteration (and there must
	// therefore have been an overshoot.
	if (rtOpsCycle.startTime > rtOpsCycle.endTime)
		sendEvent(controllerManager::RtOpsEvent::MISSED_DEADLINE);
	
	// Send our 1 kHz log stream.
	logCyclicOut->write(rtOpsCycle);
	if (guiPublishTimer.readyToSend()) {
		// Time to send on the 50 Hz port.
		guiCyclicOut->write(rtOpsCycle);
	}
	
	rtOpsCycle.startTime = startTime;
}

void OpsLogger::logControllerOutput(atrias_msgs::controller_output& output) {
	rtOpsCycle.controllerOutput = output;
}

void OpsLogger::logClampedControllerOutput(
	atrias_msgs::controller_output& clamped_output) {
	rtOpsCycle.commandedOutput = clamped_output;
}

void OpsLogger::endCycle() {
	rtOpsCycle.endTime = RTT::os::TimeService::Instance()->getNSecs();
}

void OpsLogger::logRobotState(atrias_msgs::robot_state& state) {
	rtOpsCycle.robotState = state;
	rtOpsCycle.header     = state.header;
}

void OpsLogger::sendEvent(controllerManager::RtOpsEvent event) {
	atrias_msgs::rt_ops_event event_msg;
	event_msg.event = (uint8_t) event;
	eventOut->write(event_msg);
}

}

}
