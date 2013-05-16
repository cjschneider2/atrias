#ifndef ATRIASCONTROLLER_HPP
#define ATRIASCONTROLLER_HPP

/**
  * @file AtriasController.hpp
  * @author Ryan Van Why
  * @brief This class is subclassed by all atrias controllers.
  * This is subclassed directly by subcontrollers, and indirectly
  * through ATC for top-level controllers.
  */

// For names, and possibly a realtime debugging system in the future
#include <string>

// Orocos
#include <rtt/TaskContext.hpp>      // We're not a TaskContext, but we need to reference one.
#include <rtt/os/oro_allocator.hpp> // For accessing realtime messages.

// ROS
#include <std_msgs/Header.h> // So we can pass around ROS headers for logging.

// Our namespaces
namespace atrias {
namespace controller {

// Subcontrollers do not need to be components, so this is not a TaskContext.
class AtriasController {
	protected:
		/**
		  * @brief The normal constructor for this class.
		  * @param parent The parent controller of this subcontroller.
		  * @param name   This controller's name.
		  * This is the constructor that subcontrollers should use.
		  */
		AtriasController(const AtriasController * const parent, const std::string &name);

		/**
		  * @brief The ATC constructor for this class.
		  * @param name This component's name.
		  * This should only be called by the ATC constructor.
		  */
		AtriasController(const std::string &name);

		/**
		  * @brief This returns the value num clamped between a and b.
		  * @param num The number to be clamped
		  * @param a   One side of the interval
		  * @param b   The other side of the interval
		  * @return The clamped value (>=min, <=max)
		  * This is a convenience function for controllers. This will work on any
		  * type with a defined '<' comparison operator
		  */
		template <typename T>
		const T& clamp(const T& num, const T& a, const T& b) const {
			const T& min = std::min(a, b);
			const T& max = std::max(a, b);

			if (num < min)
				return min;

			if (max < num)
				return max;

			return num;
		}

	public:
		/**
		  * @brief This returns this controller's name.
		  * @return The name of this controller.
		  * This name includes the full hierarchy, not just the name passed in.
		  */
		const std::string& getName() const;

		/**
		  * @brief Returns a ROS header with the current timestamp.
		  * @return A ROS header for logging purposes.
		  */
		//virtual const std_msgs::Header_<RTT::os::rt_allocator<uint8_t>>& getROSHeader() const;
		virtual const std_msgs::Header& getROSHeader() const;

		/**
		  * @brief This returns the TaskContext
		  * @return A reference to the TaskContext.
		  * This should only be overridden by the ATC class
		  */
		virtual RTT::TaskContext& getTaskContext() const;

		/**
		  * @brief This returns the TLC as an AtriasController
		  * @return A reference to the top-level controller.
		  */
		AtriasController &getTLC() const;

	private:
		// This controller's (full) name
		std::string name;

		// A reference to the top-level controller as an AtriasController
		AtriasController &tlc;
};

}
}

#endif // ATRIASCONTROLLER_HPP

// vim: noexpandtab
