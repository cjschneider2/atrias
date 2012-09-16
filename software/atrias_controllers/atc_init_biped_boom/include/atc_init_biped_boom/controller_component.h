#ifndef __ATC_BIPED_BOOM_INIT_TEST_H__
#define __ATC_BIPED_BOOM_INIT_TEST_H__

/*! \file controller_component.h
 *  \author Andrew Peekema
 *  \brief Orocos Component header for atc_init_biped_boom controller.
 */

// Orocos 
#include <rtt/os/main.h>
#include <rtt/RTT.hpp>
#include <rtt/Logger.hpp>
#include <rtt/TaskContext.hpp>
#include <rtt/OperationCaller.hpp>
#include <rtt/Component.hpp>

// C
#include <stdlib.h>

#include <atrias_shared/GuiPublishTimer.h>
#include <atrias_shared/globals.h>
#include <robot_invariant_defs.h>

// Datatypes
#include <atc_init_biped_boom/controller_input.h>
#include <atc_init_biped_boom/controller_status.h>
#include <atc_init_biped_boom/controller_log_data.h>
#include <atrias_msgs/robot_state.h>
#include <atrias_msgs/controller_output.h>
#include <atrias_shared/controller_structs.h>

using namespace RTT;
using namespace Orocos;
using namespace atc_init_biped_boom;

namespace atrias {
using namespace shared;
namespace controller {

class ATCInitBipedBoomTest : public TaskContext {
private:
    // This Operation is called by the RT Operations Manager.
    atrias_msgs::controller_output runController(atrias_msgs::robot_state rs);

    // Controller variables
    atrias_msgs::controller_output co;
    double cycle;
    bool initStatus, runStatus;
    RobotPos pos;

    // Subcontroller names
    std::string initBipedBoom0Name;

    // Subcontroller components
    TaskContext *initBipedBoom0; 

    // Subcontroller operations
    OperationCaller<bool(atrias_msgs::robot_state rs, RobotPos pos)> initBipedBoom0Init;
    OperationCaller<atrias_msgs::controller_output(atrias_msgs::robot_state rs)> initBipedBoom0Run;
    OperationCaller<bool(void)> initBipedBoom0Done;

    // Logging
    controller_log_data              logData;
    OutputPort<controller_log_data>  logPort;

    // For the GUI
    shared::GuiPublishTimer                         *pubTimer;
    controller_input                                guiIn;
    InputPort<controller_input>                     guiDataIn;

public:
    // Constructor
    ATCInitBipedBoomTest(std::string name);

    // Standard Orocos hooks
    bool configureHook();
    bool startHook();
    void updateHook();
    void stopHook();
    void cleanupHook();
};
}
}

#endif
