#ifndef ATC_HPP
#define ATC_HPP

/**
  * @file ATC.hpp
  * @author Ryan Van Why
  * @brief This should be subclassed by top-level controllers.
  * This provides interfaces to make writing controllers easier.
  */

// Orocos includes
#include <rtt/Component.hpp>       // We need this since we're a component.
#include <rtt/ConnPolicy.hpp>      // Allows us to establish ROS connections.
#include <rtt/InputPort.hpp>       // So we can receive data.
#include <rtt/OperationCaller.hpp> // Lets us send events and call other operations
#include <rtt/OutputPort.hpp>      // So we can send data.
#include <rtt/TaskContext.hpp>     // We're a component aka TaskContext

// Robot state and controller output
#include <atrias_msgs/controller_output.h>
#include <atrias_msgs/robot_state.h>
#include <atrias_msgs/unused.h>

// For the medulla state enum
#include <robot_invariant_defs.h>
// For the maximum and minimum torques, used by the startup controller
#include <robot_variant_defs.h>
// And to check RT Ops's state
#include <atrias_shared/globals.h>
// For our publishing timer
#include <atrias_shared/GuiPublishTimer.h>
// To let us register typekits for the messages
#include <atrias_shared/RtMsgTypekits.hpp>

// We subclass this, so let's include it
#include "atrias_control_lib/AtriasController.hpp"

// Our namespaces
namespace atrias {
namespace controller {

/**
  * @brief This detected if a type is not equal to Unused.
  * @return False if this is of type Unused, true otherwise
  */
template <template <class> class T>
inline bool notUnused() {
	return true;
}

// Utilize template specialization to detect the difference
template <>
inline bool notUnused<atrias_msgs::unused_>() {
	return false;
}

// This is a component, so we subclass TaskContext;
// as a controller, this subclasses AtriasController
// Also, we're a template...
template <template <class> class logType    = atrias_msgs::unused_,
          template <class> class guiInType  = atrias_msgs::unused_,
          template <class> class guiOutType = atrias_msgs::unused_>
class ATC : public RTT::TaskContext, public AtriasController {
	public:
		/**
		  * @brief The constructor for this class.
		  * @param name This component's name.
		  * The name should be passed directly -- it's not the controller's
		  * choice.
		  */
		ATC(const std::string &name);

		/**
		  * @brief Returns a ROS header with the current timestamp.
		  * @return A ROS header for logging purposes.
		  */
		const std_msgs::Header_<RTT::os::rt_allocator<uint8_t>>& getROSHeader() const;

		/**
		  * @brief This returns the TaskContext
		  * @return A reference to the TaskContext.
		  * This should only be overridden by the ATC class
		  */
		RTT::TaskContext& getTaskContext() const;

	protected:
		/**
		  * @brief This may be used by controller to command an EStop
		  */
		void commandEStop();

		/**
		  * @brief This may be used by controller to command a halt
		  */
		void commandHalt();

		/**
		  * @brief Returns whether or not the robot is enabled.
		  * @return True if the robot is enabled, false otherwise.
		  */
		bool isEnabled() const;

		/**
		  * @brief This enables or disables the default startup controller
		  * @param enable Whether or not to enable the default startup controller
		  * @return Whether or not the default startup controller has been enabled
		  */
		bool setStartupEnabled(bool enable);

		/**
		  * @brief Returns whether or not the startup controller is running
		  * @return True if the startup controller is running, false otherwise
		  */
		bool isStarting() const;

		// These member variables should be set/read from by
		// the controllers themselves.
		logType<RTT::os::rt_allocator<uint8_t>>    logOut;
		guiInType<RTT::os::rt_allocator<uint8_t>>  guiIn;
		guiOutType<RTT::os::rt_allocator<uint8_t>> guiOut;

		// Here is the robot state
		atrias_msgs::robot_state rs;

		// And the controller output
		atrias_msgs::controller_output co;

		/**
		  * @brief This allows controllers to send an event
		  * @param metadata The metadata for this event
		  * This only allows transmission of CONTROLLER_CUSTOM events
		  */
		void sendEvent(rtOps::RtOpsEventMetadata_t metadata = 0);

	private:
		/**
		  * @brief This is the actual controller function.
		  * This should be overloaded by the actual controller.
		  */
		virtual void controller() = 0;

		/**
		  * @brief This allows subcontrollers to access the TaskContext
		  * @return A reference to the TaskContext.
		  */
		//RTT::TaskContext& getTaskContext() const;

		// Port for input data from the GUI
		RTT::InputPort<guiInType<RTT::os::rt_allocator<uint8_t>>>   guiInPort;

		// Port to send data to the GUI
		RTT::OutputPort<guiOutType<RTT::os::rt_allocator<uint8_t>>> guiOutPort;

		// Temporary header copy, so getROSHeader() can return a reference
		// In the new event system, everything will be RT-safe, so this won't be necessary
		std_msgs::Header_<RTT::os::rt_allocator<uint8_t>> header;

		// Port for logging controller data
		RTT::OutputPort<logType<RTT::os::rt_allocator<uint8_t>>>    logOutPort;

		/**
		  * @brief This callback is executed when data is received from the GUI
		  */
		void guiInCallback(RTT::base::PortInterface* portInterface);

		// This times our publishing to the GUI for is
		atrias::shared::GuiPublishTimer publishTimer;

		/**
		  * @brief This is the operation called cyclically by RT Ops
		  * This runs the controller.
		  * @param robotState The new robot state
		  * @return           The new controller output.
		  */
		atrias_msgs::controller_output& runController(atrias_msgs::robot_state& robotState);

		// This lets us send RT Ops events
		RTT::OperationCaller<void(rtOps::RtOpsEvent, rtOps::RtOpsEventMetadata_t)> sendEventOp;

		/**
		  * @brief This is the state enum for the startup/shutdown state machine.
		  */
		enum class State : int8_t {
			RUN = 0, // Normal operation
			STARTUP  // Running the default startup controller
		};

		// The state we are currently in
		State mode;

		/**
		  * @brief This runs the startup controller
		  */
		void startupController();

		// Whether or not the startup controller is enabled
		bool startupEnabled;

		// Remaining time for startup controller
		double startupTimeRem;

		/**
		  * @brief This connects to RT Ops, so it can call this controller.
		  */
		bool configureHook();
};

template <template <class> class logType,
          template <class> class guiInType,
          template <class> class guiOutType>
ATC<logType, guiInType, guiOutType>::ATC(const std::string &name) :
	RTT::TaskContext(name),
	AtriasController(name),
	publishTimer(50), // The parameter is the transmit period in ms
	sendEventOp("sendEvent")
{
	// We initialize to run mode
	this->mode = State::RUN;

	// By default, the startup controller is disabled
	this->startupEnabled = false;

	// Register the operation runController()
	this->provides("atc")
		->addOperation("runController", &ATC<logType, guiInType, guiOutType>::runController, this, RTT::ClientThread)
		.doc("Run the controller. Takes in the robot state and returns a controller output.");

	// Connect with the sendEvent operation
	this->requires("rtOps")->addOperationCaller(this->sendEventOp);

	// Set up the event port for incoming GUI data (if there is incoming GUI data)
	if (notUnused<guiInType>()) {
		log(RTT::Info) << "[" << this->AtriasController::getName()
		               << "] Setting up GUI input port." << RTT::endlog();

		// Add typekits for this message type
		shared::RtMsgTypekits::registerType<guiInType>(this->AtriasController::getName() + "_input");

		this->addEventPort("guiInput", guiInPort, boost::bind(&ATC<logType, guiInType, guiOutType>::guiInCallback, this, _1));

		// We need to use a ConnPolicy to connect this port.
		RTT::ConnPolicy policy = RTT::ConnPolicy();

		// 3 signals that this is a ROS transport
		policy.transport = 3;

		// Set the name
		policy.name_id = std::string("/") + this->AtriasController::getName() + "_input";
		log(RTT::Info) << "[" << this->AtriasController::getName()
		               << "] Connecting GUI input to topic " << policy.name_id << RTT::endlog();

		this->guiInPort.createStream(policy);
	}

	// Set up the port for outgoing GUI data (it it exists)
	if (notUnused<guiOutType>()) {
		log(RTT::Info) << "/" << this->AtriasController::getName()
		               << "] Setting up GUI output port." << RTT::endlog();

		// Add typekits for this message type
		shared::RtMsgTypekits::registerType<guiOutType>(this->AtriasController::getName() + "_status");

		this->addPort("guiOutput", guiOutPort);

		// Let's connect this port to ROS
		// No buffer necessary, since this is going to the GUI
		RTT::ConnPolicy policy = RTT::ConnPolicy();

		// 3 == ROS transport
		policy.transport = 3;

		// Set the name
		policy.name_id = "/" + this->AtriasController::getName() + "_status";
		log(RTT::Info) << "[" << this->AtriasController::getName()
		               << "] Connecting GUI output to topic " << policy.name_id << RTT::endlog();

		// Set the port's policy
		this->guiOutPort.createStream(policy);
	}

	// Initialize the logging port (if necessary)
	if (notUnused<logType>()) {
		log(RTT::Info) << "/" << this->AtriasController::getName()
		               << "] Setting up logging port." << RTT::endlog();

		// Add typekits for this message type
		shared::RtMsgTypekits::registerType<logType>(this->AtriasController::getName() + "_log");
		
		this->addPort("log", logOutPort);

		// Let's connect this port to ROS
		// Buffer 10 seconds worth of data, since this is for logging
		RTT::ConnPolicy policy = RTT::ConnPolicy::buffer(10000);

		// Set the transport to ROS
		policy.transport = 3;

		// Set the topic's name
		policy.name_id = "/" + this->AtriasController::getName() + "_log";
		log(RTT::Info) << "[" << this->AtriasController::getName()
		               << "] Connecting logging output to topic " << policy.name_id << RTT::endlog();

		// Set the policy
		this->logOutPort.createStream(policy);
	}
}

template <template <class> class logType,
          template <class> class guiInType,
          template <class> class guiOutType>
const std_msgs::Header_<RTT::os::rt_allocator<uint8_t>>& ATC<logType, guiInType, guiOutType>::getROSHeader() const {
	return this->header;
}

template <template <class> class logType,
          template <class> class guiInType,
          template <class> class guiOutType>
void ATC<logType, guiInType, guiOutType>::commandEStop() {
	this->co.command = medulla_state_error;
}

template <template <class> class logType,
          template <class> class guiInType,
          template <class> class guiOutType>
void ATC<logType, guiInType, guiOutType>::commandHalt() {
	// Ignore the command if an EStop has already been commanded
	if (co.command == medulla_state_error)
		return;

	this->co.command = medulla_state_halt;
}

template <template <class> class logType,
          template <class> class guiInType,
          template <class> class guiOutType>
bool ATC<logType, guiInType, guiOutType>::isEnabled() const {
	return (rtOps::RtOpsState) rs.rtOpsState == rtOps::RtOpsState::ENABLED;
}

template <template <class> class logType,
          template <class> class guiInType,
          template <class> class guiOutType>
RTT::TaskContext& ATC<logType, guiInType, guiOutType>::getTaskContext() const {
	return *((RTT::TaskContext*) this);
}

template <template <class> class logType,
          template <class> class guiInType,
          template <class> class guiOutType>
void ATC<logType, guiInType, guiOutType>::sendEvent(rtOps::RtOpsEventMetadata_t metadata) {
	this->sendEventOp(rtOps::RtOpsEvent::CONTROLLER_CUSTOM, metadata);
}

template <template <class> class logType,
          template <class> class guiInType,
          template <class> class guiOutType>
void ATC<logType, guiInType, guiOutType>::guiInCallback(RTT::base::PortInterface* portInterface) {
	this->guiInPort.read(this->guiIn);
}

template <template <class> class logType,
          template <class> class guiInType,
          template <class> class guiOutType>
atrias_msgs::controller_output& ATC<logType, guiInType, guiOutType>::runController(atrias_msgs::robot_state &robotState) {
	// Check for change in state (to trigger the change to startup mode).
	if (this->startupEnabled                         &&
	    this->rs.rtOpsState != robotState.rtOpsState &&
	    (rtOps::RtOpsState) robotState.rtOpsState == rtOps::RtOpsState::ENABLED)
	{
		// We should switch to startup mode.
		this->mode           = State::STARTUP;
		this->startupTimeRem = STARTUP_TIME;
	}

	// Save the robot state so the controller (and this class) can access it.
	this->rs = robotState;

	// And update header timestamp.
	this->header.stamp = rs.header.stamp;

	// Default to run -- this is changed if the
	// controller commands a change
	this->co.command = medulla_state_run;

	// And zero torques, in case the controller doesn't
	// set them (intentionally or otherwise)
	co.lLeg.motorCurrentA   = 0.0;
	co.lLeg.motorCurrentB   = 0.0;
	co.lLeg.motorCurrentHip = 0.0;
	co.rLeg.motorCurrentA   = 0.0;
	co.rLeg.motorCurrentB   = 0.0;
	co.rLeg.motorCurrentHip = 0.0;

	// Run the controller
	this->controller();

	// Transmit the status to the GUI, if it's time.
	if (notUnused<guiOutType>()) {
		if (publishTimer.readyToSend()) {
			// Set the header (for timestamping)
			this->guiOut.header = this->getROSHeader();

			// Send the status
			this->guiOutPort.write(this->guiOut);
		}
	}

	// Send the controller's logging data
	if (notUnused<logType>()) {
		// Set the header
		this->logOut.header = this->getROSHeader();

		// And actually send it out
		this->logOutPort.write(this->logOut);
	}

	// Run the startup controller, if in the startup state
	if (this->mode == State::STARTUP)
		this->startupController();

	// Finally, return the controller output
	return this->co;
}

template <template <class> class logType,
          template <class> class guiInType,
          template <class> class guiOutType>
void ATC<logType, guiInType, guiOutType>::startupController() {

	// Clamp the controller outputs
	this->co.lLeg.motorCurrentA   = clamp(this->co.lLeg.motorCurrentA,   MIN_MTR_CURRENT_CMD,     MAX_MTR_CURRENT_CMD);
	this->co.lLeg.motorCurrentB   = clamp(this->co.lLeg.motorCurrentB,   MIN_MTR_CURRENT_CMD,     MAX_MTR_CURRENT_CMD);
	this->co.lLeg.motorCurrentHip = clamp(this->co.lLeg.motorCurrentHip, MIN_HIP_MTR_CURRENT_CMD, MAX_HIP_MTR_CURRENT_CMD);
	this->co.rLeg.motorCurrentA   = clamp(this->co.rLeg.motorCurrentA,   MIN_MTR_CURRENT_CMD,     MAX_MTR_CURRENT_CMD);
	this->co.rLeg.motorCurrentB   = clamp(this->co.rLeg.motorCurrentB,   MIN_MTR_CURRENT_CMD,     MAX_MTR_CURRENT_CMD);
	this->co.rLeg.motorCurrentHip = clamp(this->co.rLeg.motorCurrentHip, MIN_HIP_MTR_CURRENT_CMD, MAX_HIP_MTR_CURRENT_CMD);

	// Determine the scaling value
	double scale = (STARTUP_TIME - startupTimeRem) / STARTUP_TIME;

	// Scale the outputs
	this->co.lLeg.motorCurrentA   *= scale;
	this->co.lLeg.motorCurrentB   *= scale;
	this->co.lLeg.motorCurrentHip *= scale;
	this->co.rLeg.motorCurrentA   *= scale;
	this->co.rLeg.motorCurrentB   *= scale;
	this->co.rLeg.motorCurrentHip *= scale;

	// End the controller after the specified amount of time
	this->startupTimeRem -= ((double) CONTROLLER_LOOP_PERIOD_NS) / ((double) SECOND_IN_NANOSECONDS);
	if (this->startupTimeRem <= 0.0)
		this->mode = State::RUN;
}

template <template <class> class logType,
          template <class> class guiInType,
          template <class> class guiOutType>
bool ATC<logType, guiInType, guiOutType>::setStartupEnabled(bool enable) {
	return this->startupEnabled = enable;
}

template <template <class> class logType,
          template <class> class guiInType,
          template <class> class guiOutType>
bool ATC<logType, guiInType, guiOutType>::isStarting() const {
	return (this->mode == State::STARTUP);
}

template <template <class> class logType,
          template <class> class guiInType,
          template <class> class guiOutType>
bool ATC<logType, guiInType, guiOutType>::configureHook() {
	// Connect services with RT Ops so it can see our "atc" service
	this->connectServices(this->getPeer("atrias_rt"));
	return true;
}

}
}

#endif // ATC_HPP

// vim: noexpandtab
