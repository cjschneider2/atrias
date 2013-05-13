#ifndef RTMSGTYPEKITS_HPP
#define RTMSGTYPEKITS_HPP

/**
  * @file RtMsgTypekits.hpp
  * @author Ryan Van Why
  * @brief This class contains a helper function for registering typekits for ROS messages
  * Any packages that use this need to depend on rtt_rosnode!
  */

// Standard library
#include <string> // For typekit names.

// Orocos
#include <ros_msg_transporter.hpp>        // For registering ROS message transport types
#include <rtt/os/oro_allocator.hpp>       // For the realtime-safe allocator
#include <rtt/types/TemplateTypeInfo.hpp> // Allows us to create typekitsa

// Namespaces we're inside
namespace atrias {
namespace shared {

class RtMsgTypekits {
	public:
		/**
		  * @brief This registers the typekit for a given ROS type.
		  * @param name A unique name for this type, such as "rt_ops_event_"
		  */
		template <template<class> class msgType>
		static void registerType(const std::string &name);
};

// Template definitions
template <template<class> class msgType>
void RtMsgTypekits::registerType(const std::string &name) {
	// This is taken partially off the Orocos mailinglist
	// http://www.orocos.org/forum/orocos/orocos-users/cannot-transport-ros-message-rttosrtallocator
	// The steps that check if this type's already been added are custom, however.

	log(RTT::Info) << "Got type info: " << RTT::types::Types()->getTypeInfo<msgType<RTT::os::rt_allocator<uint8_t>>>() << RTT::endlog();
	// Check if this type has already been registered. If it has been, then we'll be able to obtain
	// a pointer to the type info
	if (RTT::types::Types()->getTypeInfo<msgType<RTT::os::rt_allocator<uint8_t>>>()) {
		// It's already been registered, so return immediately.
		return;
	}

	// Instantiate the TemplateTypeInfo
	auto typeInfo = new RTT::types::TemplateTypeInfo<msgType<RTT::os::rt_allocator<uint8_t>>, false>(name);
	
	// Time to register the type
	log(RTT::Info) << "Registering type " << name << ": " << RTT::types::Types()->addType(typeInfo) << RTT::endlog();
	
	log(RTT::Info) << "Adding protocol" << RTT::endlog();
	RTT::types::Types()->type(name)->addProtocol(3, new ros_integration::RosMsgTransporter<msgType<RTT::os::rt_allocator<uint8_t>>>());
}

// End namespaces
}
}

#endif // RTMSGTYPEKITS_HPP

// Tab-based indentation
// vim: noexpandtab
