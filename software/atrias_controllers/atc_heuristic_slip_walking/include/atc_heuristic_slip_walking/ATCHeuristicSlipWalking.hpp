#ifndef ATCHeuristicSlipWalking_HPP
#define ATCHeuristicSlipWalking_HPP

/**
 * @file ATCHeuristicSlipWalking.hpp
 * @author Mikhail S. Jones
 * @brief This controller is based on simulated SLIP walking gaits for ATRIAS 
 * and has been tuned manually through trial and error.
 */

// Include the ATC class
#include <atrias_control_lib/ATC.hpp>
// Our logging data type.
#include "atc_heuristic_slip_walking/controller_log_data.h"
// The type transmitted from the GUI to the controller
#include "atc_heuristic_slip_walking/controller_input.h"
// The type transmitted from the controller to the GUI
#include "atc_heuristic_slip_walking/controller_status.h"

// Our subcontroller types
#include <asc_common_toolkit/ASCCommonToolkit.hpp>
#include <asc_interpolation/ASCInterpolation.hpp>
#include <asc_leg_force/ASCLegForce.hpp>
#include <asc_hip_boom_kinematics/ASCHipBoomKinematics.hpp>
#include <asc_pd/ASCPD.hpp>
#include <asc_rate_limit/ASCRateLimit.hpp>

// Datatypes
#include <robot_invariant_defs.h>
#include <atrias_msgs/robot_state.h>
#include <atrias_shared/controller_structs.h>
#include <atrias_shared/atrias_parameters.h>

// Namespaces we're using
using namespace std;

// Our namespaces
namespace atrias {
namespace controller {

class ATCHeuristicSlipWalking : public ATC<
	atc_heuristic_slip_walking::controller_log_data_,
	atc_heuristic_slip_walking::controller_input_,
	atc_heuristic_slip_walking::controller_status_>
{
	public:
		/** 
		 * @brief The constructor for this controller.
		 * @param name The name of this component.
		 * Every top-level controller will have this name parameter,
		 * just like current controllers.
		 */
		ATCHeuristicSlipWalking(string name);

	private:
		/** 
		 * @brief This is the main function for the top-level controller.
		 */
		void controller();

		/**
		  * @brief These are sub controllers used by the top level controller.
		  */
  		ASCCommonToolkit ascCommonToolkit;
  		ASCInterpolation ascInterpolation;
		ASCLegForce ascLegForceL, ascLegForceR;
		ASCHipBoomKinematics ascHipBoomKinematics;
		ASCPD ascPDLmA, ascPDLmB, ascPDRmA, ascPDRmB, ascPDLh, ascPDRh;
		ASCRateLimit ascRateLimitLmA, ascRateLimitLmB, ascRateLimitRmA, ascRateLimitRmB;

		// Functions
		void updateState();
		void hipController();
		void standingController(atrias_msgs::robot_state_leg*, atrias_msgs::controller_output_leg*, ASCRateLimit*, ASCRateLimit*, ASCPD*, ASCPD*);
		void shutdownController();
		void stanceController(atrias_msgs::robot_state_leg*, atrias_msgs::controller_output_leg*, ASCLegForce*);
		void singleSupportEvents(atrias_msgs::robot_state_leg*, atrias_msgs::robot_state_leg*, ASCLegForce*, ASCLegForce*);
		void legSwingController(atrias_msgs::robot_state_leg*, atrias_msgs::robot_state_leg*, atrias_msgs::controller_output_leg*, ASCPD*, ASCPD*);
		void doubleSupportEvents(atrias_msgs::robot_state_leg*, atrias_msgs::robot_state_leg*, ASCLegForce*, ASCLegForce*);
		void updateExitConditions(atrias_msgs::robot_state_leg*, atrias_msgs::robot_state_leg*);

		// Variables
		int controllerState, walkingState; // State machines
		double qLh, qRh; // Hip angles
		LeftRight toePosition; // Desired toe positions measures from boom center axis

		// Motor and leg variables
		double rSl, drSl, qSl, dqSl; // Stance leg states
		double rFl, drFl, qFl, dqFl; // Flight leg states
		double qmSA, dqmSA, qmSB, dqmSB; // Stance motor states
		double qmFA, dqmFA, qmFB, dqmFB; // Flight motor states
		LegForce forceSl, forceFl;

		// Leg parameters at exit state (event trigger)
		double reSl, dreSl, qeSl, dqeSl; // Stance leg exit states
		double reFl, dreFl, qeFl, dqeFl; // Flight leg exit states
		
		// Leg parameters at target states
		double rtSl, drtSl, qtSl, dqtSl; // Stance leg target states
		double rtFl, drtFl, qtFl, dqtFl; // Flight leg target states

		// Temp leg parameters
		double ql, dql, rl, drl; 
		
		// Debug events
		bool isManualFlightLegTO, isManualFlightLegTD;
		
		// State transistion events
		bool isStanceLegTO, isFlightLegTO, isFlightLegTD, isForwardStep, isBackwardStep;
		
		// Walking gait definition values
		double r0, k, dk; // Spring parameters
		double swingLegRetraction; // The amount the leg retracts during swing
		
		// Misc margins, ratelimiters and other kludge values
		double triggerForce; // The amount of force required to trigger TD and TO
		//double triggerMarginTO;
		double legRateLimit;



};

}
}

#endif // ATCHeuristicSlipWalking_HPP
