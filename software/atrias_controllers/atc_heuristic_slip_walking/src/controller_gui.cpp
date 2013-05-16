/*! \file controller_gui.cpp
 *  \author Mikhail Jones
 */

#include <atc_heuristic_slip_walking/controller_gui.h>

void flight_td_pressed()  {controllerDataOut.flight_td = 1;}
void flight_td_released() {controllerDataOut.flight_td = 0;}
void flight_to_pressed()  {controllerDataOut.flight_to = 1;}
void flight_to_released() {controllerDataOut.flight_to = 0;}

//! \brief Initialize the GUI.
bool guiInit(Glib::RefPtr<Gtk::Builder> gui) {
	// Get widgets
	gui->get_widget("walking_state_spinbutton", walking_state_spinbutton);
	gui->get_widget("standing_leg_spinbutton", standing_leg_spinbutton);
	gui->get_widget("slip_leg_spinbutton", slip_leg_spinbutton);
	gui->get_widget("swing_leg_retraction_spinbutton", swing_leg_retraction_spinbutton);
	gui->get_widget("event_trigger_force_spinbutton", event_trigger_force_spinbutton);
	gui->get_widget("stance_leg_target_spinbutton", stance_leg_target_spinbutton);
	gui->get_widget("flight_leg_target_spinbutton", flight_leg_target_spinbutton);
	gui->get_widget("leg_pos_kp_spinbutton", leg_pos_kp_spinbutton);
	gui->get_widget("leg_for_kp_spinbutton", leg_for_kp_spinbutton);
	gui->get_widget("leg_for_kd_spinbutton", leg_for_kd_spinbutton);
	gui->get_widget("leg_pos_kd_spinbutton", leg_pos_kd_spinbutton);
	gui->get_widget("hip_pos_kp_spinbutton", hip_pos_kp_spinbutton);
	gui->get_widget("hip_pos_kd_spinbutton", hip_pos_kd_spinbutton);
	gui->get_widget("main_controller_combobox", main_controller_combobox);
	gui->get_widget("flight_td_button", flight_td_button);
	gui->get_widget("flight_to_button", flight_to_button);

	// Set ranges
	walking_state_spinbutton->set_range(0, 3);
	standing_leg_spinbutton->set_range(0.75, 0.95);
	slip_leg_spinbutton->set_range(0.75, 0.95);
	swing_leg_retraction_spinbutton->set_range(0.0, 0.15);
	event_trigger_force_spinbutton->set_range(0.0, 50.0);
	stance_leg_target_spinbutton->set_range(M_PI/2.0, M_PI/2.0 + 0.30);
	flight_leg_target_spinbutton->set_range(M_PI/2.0 - 0.30, M_PI/2.0);
	leg_pos_kp_spinbutton->set_range(0.0, 1500.0);
	leg_for_kp_spinbutton->set_range(0.0, 1500.0);
	leg_for_kd_spinbutton->set_range(0.0, 50.0);
	leg_pos_kd_spinbutton->set_range(0.0, 100.0);
	hip_pos_kp_spinbutton->set_range(0.0, 300.0);
	hip_pos_kd_spinbutton->set_range(0.0, 50.0);

	// Set increments
	walking_state_spinbutton->set_increments(1, 0);
	standing_leg_spinbutton->set_increments(0.01, 0.0);
	slip_leg_spinbutton->set_increments(0.01, 0.0);
	swing_leg_retraction_spinbutton->set_increments(0.01, 0.0);
	event_trigger_force_spinbutton->set_increments(1.0, 0.0);
	stance_leg_target_spinbutton->set_increments(0.01, 0.0);
	flight_leg_target_spinbutton->set_increments(0.01, 0.0);
	leg_pos_kp_spinbutton->set_increments(10.0, 0.0);
	leg_for_kp_spinbutton->set_increments(10.0, 0.0);
	leg_for_kd_spinbutton->set_increments(1.0, 0.0);
	leg_pos_kd_spinbutton->set_increments(1.0, 0.0);
	hip_pos_kp_spinbutton->set_increments(10.0, 0.0);
	hip_pos_kd_spinbutton->set_increments(1.0, 0.0);

	// Set values
	walking_state_spinbutton->set_value(0);
	standing_leg_spinbutton->set_value(0.90);
	slip_leg_spinbutton->set_value(0.90);
	swing_leg_retraction_spinbutton->set_value(0.10);
	event_trigger_force_spinbutton->set_value(25.0);
	stance_leg_target_spinbutton->set_value(M_PI/2.0 + 0.06);
	flight_leg_target_spinbutton->set_value(M_PI/2.0 - 0.157);
	leg_pos_kp_spinbutton->set_value(300.0);
	leg_pos_kd_spinbutton->set_value(15.0);
	leg_for_kp_spinbutton->set_value(300.0);
	leg_for_kd_spinbutton->set_value(6.0);
	hip_pos_kp_spinbutton->set_value(150.0);
	hip_pos_kd_spinbutton->set_value(10.0);
	
	// Connect buttons to functions
	flight_td_button->signal_pressed().connect(sigc::ptr_fun((void(*)())flight_td_pressed));
	flight_td_button->signal_released().connect(sigc::ptr_fun((void(*)())flight_td_released));
	flight_to_button->signal_pressed().connect(sigc::ptr_fun((void(*)())flight_to_pressed));
	flight_to_button->signal_released().connect(sigc::ptr_fun((void(*)())flight_to_released));

	// Set up subscriber and publisher.
	sub = nh.subscribe("ATCHeuristicSlipWalking_status", 0, controllerCallback);
	pub = nh.advertise<atc_heuristic_slip_walking::controller_input>("ATCHeuristicSlipWalking_input", 0);
	return true;
}

//! \brief Update our local copy of the controller status.
void controllerCallback(const atc_heuristic_slip_walking::controller_status &status) {
	controllerDataIn = status;
}

//! \brief Get parameters from the server and configure GUI accordingly.
void getParameters() {
	// Get parameters in the atrias_gui namespace
	nh.getParam("/atrias_gui/walking_state", controllerDataOut.walking_state);
	nh.getParam("/atrias_gui/standing_leg", controllerDataOut.standing_leg);
	nh.getParam("/atrias_gui/slip_leg", controllerDataOut.slip_leg);
	nh.getParam("/atrias_gui/swing_leg_retraction", controllerDataOut.swing_leg_retraction);
	nh.getParam("/atrias_gui/event_trigger_force", controllerDataOut.event_trigger_force);
	nh.getParam("/atrias_gui/stance_leg_target", controllerDataOut.stance_leg_target);
	nh.getParam("/atrias_gui/flight_leg_target", controllerDataOut.flight_leg_target);
	nh.getParam("/atrias_gui/leg_pos_kp", controllerDataOut.leg_pos_kp);
	nh.getParam("/atrias_gui/leg_for_kp", controllerDataOut.leg_for_kp);
	nh.getParam("/atrias_gui/leg_for_kd", controllerDataOut.leg_for_kd);
	nh.getParam("/atrias_gui/leg_pos_kd", controllerDataOut.leg_pos_kd);
	nh.getParam("/atrias_gui/hip_pos_kp", controllerDataOut.hip_pos_kp);
	nh.getParam("/atrias_gui/hip_pos_kd", controllerDataOut.hip_pos_kd);
	int main_controller;
	nh.getParam("/atrias_gui/main_controller", main_controller);
	controllerDataOut.main_controller = (uint8_t)main_controller;

	// Configure the GUI
	walking_state_spinbutton->set_value(controllerDataOut.walking_state);
	standing_leg_spinbutton->set_value(controllerDataOut.standing_leg);
	slip_leg_spinbutton->set_value(controllerDataOut.slip_leg);
	leg_pos_kp_spinbutton->set_value(controllerDataOut.leg_pos_kp);
	leg_for_kp_spinbutton->set_value(controllerDataOut.leg_for_kp);
	leg_for_kd_spinbutton->set_value(controllerDataOut.leg_for_kd);
	leg_pos_kd_spinbutton->set_value(controllerDataOut.leg_pos_kd);
	hip_pos_kp_spinbutton->set_value(controllerDataOut.hip_pos_kp);
	hip_pos_kd_spinbutton->set_value(controllerDataOut.hip_pos_kd);
	main_controller_combobox->set_active(controllerDataOut.main_controller);
}

//! \brief Set parameters on server according to current GUI settings.
void setParameters() {
	nh.setParam("/atrias_gui/walking_state", controllerDataOut.walking_state);
	nh.setParam("/atrias_gui/standing_leg", controllerDataOut.standing_leg);
	nh.setParam("/atrias_gui/slip_leg", controllerDataOut.slip_leg);
	nh.setParam("/atrias_gui/swing_leg_retraction", controllerDataOut.swing_leg_retraction);
	nh.setParam("/atrias_gui/event_trigger_force", controllerDataOut.event_trigger_force);
	nh.setParam("/atrias_gui/stance_leg_target", controllerDataOut.stance_leg_target);
	nh.setParam("/atrias_gui/flight_leg_target", controllerDataOut.flight_leg_target);
	nh.setParam("/atrias_gui/leg_pos_kp", controllerDataOut.leg_pos_kp);
	nh.setParam("/atrias_gui/leg_for_kp", controllerDataOut.leg_for_kp);
	nh.setParam("/atrias_gui/leg_for_kd", controllerDataOut.leg_for_kd);
	nh.setParam("/atrias_gui/leg_pos_kd", controllerDataOut.leg_pos_kd);
	nh.setParam("/atrias_gui/hip_pos_kp", controllerDataOut.hip_pos_kp);
	nh.setParam("/atrias_gui/hip_pos_kd", controllerDataOut.hip_pos_kd);
	nh.setParam("/atrias_gui/main_controller", controllerDataOut.main_controller);
}

//! \brief Update the GUI.
void guiUpdate() {
	// Set GUI values from controllerDataOut
	walking_state_spinbutton->set_value(controllerDataIn.walking_state);
	
	// Set values in controllerDataOut variable here
	controllerDataOut.standing_leg = standing_leg_spinbutton->get_value();
	controllerDataOut.slip_leg = slip_leg_spinbutton->get_value();
	controllerDataOut.swing_leg_retraction = swing_leg_retraction_spinbutton->get_value();
	controllerDataOut.event_trigger_force = event_trigger_force_spinbutton->get_value();
	controllerDataOut.stance_leg_target = stance_leg_target_spinbutton->get_value();
	controllerDataOut.flight_leg_target = flight_leg_target_spinbutton->get_value();
	controllerDataOut.leg_pos_kp = leg_pos_kp_spinbutton->get_value();
	controllerDataOut.leg_for_kp = leg_for_kp_spinbutton->get_value();
	controllerDataOut.leg_for_kd = leg_for_kd_spinbutton->get_value();
	controllerDataOut.leg_pos_kd = leg_pos_kd_spinbutton->get_value();
	controllerDataOut.hip_pos_kp = hip_pos_kp_spinbutton->get_value();
	controllerDataOut.hip_pos_kd = hip_pos_kd_spinbutton->get_value();
	controllerDataOut.main_controller = (uint8_t)main_controller_combobox->get_active_row_number();

	// publish the controller input
	pub.publish(controllerDataOut);
}

//! \brief Take down the GUI.
void guiTakedown() {
}
