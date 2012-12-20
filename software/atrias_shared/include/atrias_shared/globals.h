/*
 * globals.h
 *
 * Defines structs, enums, and global variables that need or might need to be
 * used in multiple places within the Atrias control code.
 *
 *  Created on: Jul 31, 2012
 *      Author: Michael Anderson
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <stdint.h>

#define ASSERT(condition, message) do { \
if (!(condition)) { printf((message)); } \
assert ((condition)); } while(false)

#define SECOND_IN_NANOSECONDS 1000000000LL
#define ETHERCAT_PRIO         80

//Use namespaces for bonus points (you can't win the game without bonus points)
namespace atrias {

namespace controllerManager {

typedef uint8_t UserCommand_t;

/*
 * Represents the command sent to the Controller Manager from the GUI
 */
enum class UserCommand: UserCommand_t {
    STOP = 0,
    RUN,
    E_STOP,
    UNLOAD_CONTROLLER
};

typedef uint8_t ControllerManagerState_t;

/*
 * Represents the state of the controller manager
 *
 * TODO: CONTROLLER_STARTING and CONTROLLER_STOPPING are for controller startup
 * and shutdown sequences, which will need to be implemented later
 */
enum class ControllerManagerState: ControllerManagerState_t {
    NO_CONTROLLER_LOADED = 0,
    CONTROLLER_STOPPED,
    CONTROLLER_RUNNING,
    CONTROLLER_STARTING,
    CONTROLLER_STOPPING,
    CONTROLLER_ESTOPPED
};

typedef uint8_t ControllerManagerError_t;

/*
 * Represents the type of an error encountered by the Controller Manager
 */
enum class ControllerManagerError: ControllerManagerError_t {
    NO_ERROR = 0,
    CONTROLLER_PACKAGE_NOT_FOUND,
    CONTROLLER_STATE_MACHINE_NOT_FOUND,
    CONTROLLER_STATE_MACHINE_EXCEPTION
};

}

namespace rtOps {

/** @brief This holds RT Ops's state -- also used by the Controller Manager
  * to command RT Ops into different states.
  */
typedef uint8_t RtOpsState_t;

enum class RtOpsState: RtOpsState_t {
    NO_CONTROLLER_LOADED = 0,
    DISABLED, //Controller is loaded but not enabled
    ENABLED,  //Controller is both loaded and enabled
    RESET,
    E_STOP,
    HALT
};

/** @brief Represents an RT Ops event.
  */
typedef int8_t RtOpsEvent_t;

enum class RtOpsEvent: RtOpsEvent_t {
    NO_EVENT = 0,             // Only used internally in the controller manager, should never be sent
    INVALID_CM_COMMAND,       // An invalid command was received from the Controller Manager
    INVALID_RT_OPS_STATE,     // The internal RT Ops state was somehow bad.
    MISSED_DEADLINE,          // We missed a deadline (timing overshoot). This is just a warning.
    CM_COMMAND_ESTOP,         // The controller manager sent an EStop command.
    ACK_NO_CONTROLLER_LOADED, // Acknowledges a NO_CONTROLLER_LOADED command from the CM
    ACK_DISABLE,              // Acknowledges a DISABLE command from the CM
    ACK_ENABLE,               // Acknowledges an ENABLE command from the CM
    ACK_RESET,                // Acknowledges a RESET command from the CM
    ACK_E_STOP,               // Acknowledges an E_STOP command from the CM
    ACK_HALT,                 // Acknowledges an E_STOP command from the CM
    ACK_INVALID,              // This shouldn't ever be sent... it indicates an internal inconsistency in the state machine.
    CONTROLLER_ESTOP,         // The controller commanded an estop.
    MEDULLA_ESTOP,            // Sent when any Medulla goes into error mode.
    SAFETY,                   // Sent whenever RT Ops's safety engages. Has metadata of type RtOpsEventSafetyMetadata
    CONTROLLER_CUSTOM         // This one may be sent by controllers -- they fill in their own metadata
};

/** @brief The type for RT Ops event metadata.
  */
typedef int8_t RtOpsEventMetadata_t;

/** @brief The metadata for the SAFETY event.
  * This reflects the _first_ detected reason for a halt.
  * 
  * This needs to be completed and actually implemented.
  */
enum class RtOpsEventSafetyMetadata: RtOpsEventMetadata_t {
    BOOM_MEDULLA_HALT = 1,    // The boom medulla entered halt state
    LEFT_HIP_MEDULLA_HALT,    // Likewise for left hip. The next few are similar.
    LEFT_LEG_A_MEDULLA_HALT,
    LEFT_LEG_B_MEDULLA_HALT,
    RIGHT_HIP_MEDULLA_HALT,
    RIGHT_LEG_A_MEDULLA_HALT,
    RIGHT_LEG_B_MEDULLA_HALT,
    LEFT_LEG_A_TOO_SMALL,     // RT Ops's safeties have kicked in.
    LEFT_LEG_A_TOO_LARGE,     // These signal that motors were about to hit their
    LEFT_LEG_B_TOO_SMALL,     // min and max hard stops
    LEFT_LEG_B_TOO_LARGE,
    RIGHT_LEG_A_TOO_SMALL,
    RIGHT_LEG_A_TOO_LARGE,
    RIGHT_LEG_B_TOO_SMALL,
    RIGHT_LEG_B_TOO_LARGE,
    LEFT_LEG_TOO_LONG,        // These signify that the motors or legs were about to collide
    LEFT_LEG_TOO_SHORT,       // with each other.
    RIGHT_LEG_TOO_LONG,
    RIGHT_LEG_TOO_SHORT
};

/** @brief The type for robot configuration data
  */
typedef uint8_t RobotConfiguration_t;

/** @brief Describes the "standard" robot configurations.
  * Reported by the connector.
  */
enum class RobotConfiguration: RobotConfiguration_t {
    DISABLE = 0,    // All safeties should be disabled. Zero so this is the
                    // default if a Connector doesn't implement this.
    UNKNOWN,        // Not a known configuration. All safeties enabled.
    BIPED_FULL,     // The full biped, with hips and location data.
    LEFT_LEG_NOHIP, // A single leg with no hip
    LEFT_LEG_HIP,   // A single leg with a hip
    BIPED_NOHIP,    // Two legs no hips
};

}

}

#endif /* GLOBALS_H_ */

// vim: expandtab:sts=4
