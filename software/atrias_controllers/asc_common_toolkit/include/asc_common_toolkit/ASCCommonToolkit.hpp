#ifndef ASC_COMMON_TOOLKIT_HPP
#define ASC_COMMON_TOOLKIT_HPP
 
/**
  * @file ASC_COMMON_TOOLKIT.hpp
  * @author Mikhail Jones
  * @brief This implements common functionality required by most controllers.
  */

// The include for the controller class
#include <atrias_control_lib/AtriasController.hpp>

// And for the logging helper class
#include <atrias_control_lib/LogPort.hpp>

// Our log data
#include "asc_common_toolkit/controller_log_data.h"

// Datatypes
#include <atrias_shared/controller_structs.h>
#include <atrias_shared/atrias_parameters.h>

// Namespaces we're using
using namespace std;

// Our namespaces
namespace atrias {
namespace controller {

// The subcontroller class itself
class ASCCommonToolkit : public AtriasController {
	public:
		/**
		  * @brief The constructor for this subcontroller
		  * @param parent The instantiating, "parent" controller.
		  * @param name The name for this controller.
		  */                  
		ASCCommonToolkit(AtriasController *parent, string name);

		/**
		  * @brief Computes the current axial leg stiffness.
		  * @param r The current leg length.
		  * @param dr The current leg length velocity.
		  * @param r0 The initial leg length.
		  * @return k The computed virtual leg stiffness.
		  * @return k The computed virtual leg stiffness derivative.
		  */
		std::tuple<double, double> legStiffness(double r, double dr, double r0);
		double k, dk;

		/**
		  * @brief Converts motor position to leg position.
		  * @param qmA The motor A angular position.
		  * @param qmB The motor B agular position.
		  * @return ql The computed leg angle.
		  * @return rl The computed leg length.
		  */
		std::tuple<double, double> motorPos2LegPos(double qmA, double qmB);
		double ql, rl;

		/**
		  * @brief Converts leg position to motor position.
		  * @param ql The current leg angle.
		  * @param rl The initial leg length.
		  * @return qmA The computed motor A angular position
		  * @return qmB The computed motor B angular position.
		  */
		std::tuple<double, double> legPos2MotorPos(double ql, double rl);
		double qmA, qmB;

		/**
		  * @brief Converts motor velocity to leg velocity.
		  * @param qmA The current motor A angle.
		  * @param qmB The current motor B angle.
		  * @param dqmA The current motor A velocity.
		  * @param dqmB The current motor B velocity.
		  * @return drl The computed leg length velocity.
		  * @return dql The computed leg angle velocity.
		  */
		std::tuple<double, double> motorVel2LegVel(double qmA, double qmB, double dqmA, double dqmB);
		double drl, dql;

		/**
		  * @brief Converts leg velocity to motor velocity.
		  * @param rl The current leg length.
		  * @param dql The current leg angular velocity.
		  * @param drl The current leg length velocity.
		  * @return dqmA The computed motor A velocity.
		  * @return dqmB The computed motor B velocity.
		  */
		std::tuple<double, double> legVel2MotorVel(double rl, double dql, double drl);
		double dqmA, dqmB;

		/**
		  * @brief Converts radians to degrees.
		  * @param rad The current angle in radians.
		  * @return deg The computed angle in degrees.
		  */
		double rad2Deg(double rad);
		double deg;

		/**
		  * @brief Converts degrees to radians.
		  * @param deg The current angle in degrees.
		  * @return rad The computed angle in radians.
		  */
		double deg2Rad(double deg);
		double rad;

		/**
		  * @brief Converts cartesian coordiantes to polar coordinates.
		  * @param x The x cartesian coordinate.
		  * @param z The z cartesian coordinate.
		  * @return q The q polar coordinate.
		  * @return r The r polar coordinate.
		  */
		std::tuple<double, double> cartPos2PolPos(double x, double z);
		double q, r;

		/**
		  * @brief Converts polar coordiantes to cartesian coordinates.
		  * @param q The q polar coordinate.
		  * @param r The r polar coordinate.
		  * @return x The x cartesian coordinate.
		  * @return z The z cartesian coordinate.
		  */
		std::tuple<double, double> polPos2CartPos(double q, double r);
		double x, z;
		
		/**
		  * @brief Converts cartesian velocity to polar velocity.
		  * @param q The angular polar position.
		  * @param r The radial polar position.
		  * @param dx The x cartesian velocity.
		  * @param dz The z cartesian velocity.
		  * @return dq The angular polar velocity.
		  * @return dr The radial polar velocity.
		  */
		std::tuple<double, double> cartVel2PolVel(double q, double r, double dx, double dz);
		double dq, dr;

		/**
		  * @brief Converts polar velocity to cartesian velocity.
		  * @param q The angular polar position.
		  * @param r The radial polar position.
  		  * @param dq The angular polar velocity.
		  * @param dr The radial polar velocity.
		  * @return dx The x cartesian velocity.
		  * @return dz The z cartesian velocity.
		  */
		std::tuple<double, double> polVel2CartVel(double q, double r, double dq, double dr);
		double dx, dz;

	private:
		/** 
		  * @brief This is our logging port.
		  * You may have as many of these as you'd like of various types.
		  */
		LogPort<asc_common_toolkit::controller_log_data_> log_out;
};

}
}

#endif // ASC_COMMON_TOOLKIT_HPP
