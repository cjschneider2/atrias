#ifndef ATCSlipWalking_HPP
#define ATCSlipWalking_HPP

/**
  * @file ATCSlipWalking.hpp
  * @author Outline by Ryan Van Why, control code by Andrew Peekema
  * @brief A spring loaded inverted pendulum based walking controller
  */

// Top-level controllers are components, so we need to include this.
#include <rtt/Component.hpp>

// Include the ATC class
#include <atrias_control_lib/ATC.hpp>
// No logging helper is needed -- one log port is automatically produced.

// Our logging data type.
#include "atc_slip_walking/controller_log_data.h"
// The type transmitted from the GUI to the controller
#include "atc_slip_walking/controller_input.h"
// The type transmitted from the controller to the GUI
#include "atc_slip_walking/controller_status.h"

// Our subcontroller types
#include <asc_common_toolkit/ASCCommonToolkit.hpp>
#include <asc_pd/ASCPD.hpp>
#include <asc_hip_boom_kinematics/ASCHipBoomKinematics.hpp>

// Datatypes
#include <atrias_shared/controller_structs.h>

// Namespaces we're using
using namespace std;
using namespace atc_slip_walking;

// Our namespaces
namespace atrias {
namespace controller {

class ATCSlipWalking : public ATC<atc_slip_walking::controller_log_data, controller_input, controller_status> {
    public:
        ATCSlipWalking(string name);

    private:
        void controller();

        // Sub controllers
        ASCCommonToolkit commonToolkit;
        ASCPD pdHip;
        ASCPD pdLegStance;
        ASCPD pdLegFlight;
        ASCHipBoomKinematics hipBoomKinematics;

        // Control setup
        void guiCommunication();
        int controllerState, prevState;

        // Event Angles
        double qE;  // Extension
        double qTD; // TouchDown
        double qS1; // Enter single support
        double qS2; // Exit single support
        double qTO; // TakeOff

        // Hip control
        void hipControl();
        double qlh, qrh;
        LeftRight toePosition;

        // Standing
        void standingControl();
        double qrl, qll, rrl, rll;      // SLIP model
        double qrmA, qrmB, qlmA, qlmB;  // ATRIAS motors

        // Walking
        void walkingControl();
        // Stance control

        // Flight control

        // Shutdown
        void shutdownControl();

};

}
}

#endif // ATCSlipWalking_HPP

// vim: expandtab
