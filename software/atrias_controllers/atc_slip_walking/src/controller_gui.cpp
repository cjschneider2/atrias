#include <atc_slip_walking/controller_gui.h>

bool guiInit(Glib::RefPtr<Gtk::Builder> gui) {
    gui->get_widget("standing_leg_length_spinbutton", standing_leg_length_spinbutton);
    gui->get_widget("touchdown_angle_spinbutton", touchdown_angle_spinbutton);
    gui->get_widget("takeoff_angle_spinbutton", takeoff_angle_spinbutton);
    gui->get_widget("slip_leg_length_spinbutton", slip_leg_length_spinbutton);
    gui->get_widget("linear_spring_constant_spinbutton", linear_spring_constant_spinbutton);
    gui->get_widget("leg_stance_kp_spinbutton", leg_stance_kp_spinbutton);
    gui->get_widget("leg_stance_kd_spinbutton", leg_stance_kd_spinbutton);
    gui->get_widget("leg_flight_kp_spinbutton", leg_flight_kp_spinbutton);
    gui->get_widget("leg_flight_kd_spinbutton", leg_flight_kd_spinbutton);
    gui->get_widget("hip_kp_spinbutton", hip_kp_spinbutton);
    gui->get_widget("hip_kd_spinbutton", hip_kd_spinbutton);
    gui->get_widget("main_controller_combobox", main_controller_combobox);
    gui->get_widget("spring_type_combobox", spring_type_combobox);
    gui->get_widget("stance_controller_combobox", stance_controller_combobox);

    // Ranges
    standing_leg_length_spinbutton->set_range(0.5, 0.95);
    touchdown_angle_spinbutton->set_range(M_PI/8.0, M_PI/2.0);
    takeoff_angle_spinbutton->set_range(M_PI/2.0, M_PI*7.0/8.0);
    slip_leg_length_spinbutton->set_range(0.5, 0.95);
    linear_spring_constant_spinbutton->set_range(0.0, 50000.0);
    leg_stance_kp_spinbutton->set_range(0.0, 2000.0);
    leg_flight_kp_spinbutton->set_range(0.0, 2000.0);
    leg_stance_kd_spinbutton->set_range(0.0, 50.0);
    leg_flight_kd_spinbutton->set_range(0.0, 50.0);
    hip_kp_spinbutton->set_range(0.0, 250.0);
    hip_kd_spinbutton->set_range(0.0, 25.0);

    // Increments
    standing_leg_length_spinbutton->set_increments(0.01, 0.0);
    touchdown_angle_spinbutton->set_increments(M_PI/32.0, 0.0);
    takeoff_angle_spinbutton->set_increments(M_PI/32.0, 0.0);
    slip_leg_length_spinbutton->set_increments(0.01, 0.0);
    linear_spring_constant_spinbutton->set_increments(100.0, 0.0);
    leg_stance_kp_spinbutton->set_increments(10.0, 0.0);
    leg_flight_kp_spinbutton->set_increments(10.0, 0.0);
    leg_stance_kd_spinbutton->set_increments(1.0, 0.0);
    leg_flight_kd_spinbutton->set_increments(1.0, 0.0);
    hip_kp_spinbutton->set_increments(10.0, 0.0);
    hip_kd_spinbutton->set_increments(1.0, 0.0);

    // Set default values
    standing_leg_length_spinbutton->set_value(0.90);
    touchdown_angle_spinbutton->set_value(M_PI/4.0);
    takeoff_angle_spinbutton->set_value(M_PI*3.0/4.0);
    slip_leg_length_spinbutton->set_value(0.85);
    linear_spring_constant_spinbutton->set_value(28000.0);
    leg_stance_kp_spinbutton->set_value(1000.0);
    leg_flight_kp_spinbutton->set_value(500.0);
    leg_stance_kd_spinbutton->set_value(25.0);
    leg_flight_kd_spinbutton->set_value(8.0);
    hip_kp_spinbutton->set_value(100.0);
    hip_kd_spinbutton->set_value(8.0);

    // Set up subscriber and publisher
    sub = nh.subscribe("ATCSlipWalking_status", 0, controllerCallback);
    pub = nh.advertise<atc_slip_walking::controller_input>("ATCSlipWalking_input", 0);
    return true;
}

void controllerCallback(const atc_slip_walking::controller_status &status) {
    controllerDataIn = status;
}

// Get parameters from the server and configure GUI accordingly
void getParameters() {
    // Get parameters in the atrias_gui namespace
    nh.getParam("/atrias_gui/standing_leg_length", controllerDataOut.standing_leg_length);
    nh.getParam("/atrias_gui/touchdown_angle", controllerDataOut.touchdown_angle);
    nh.getParam("/atrias_gui/takeoff_angle", controllerDataOut.takeoff_angle);
    nh.getParam("/atrias_gui/slip_leg_length", controllerDataOut.slip_leg_length);
    nh.getParam("/atrias_gui/linear_spring_constant", controllerDataOut.linear_spring_constant);
    nh.getParam("/atrias_gui/leg_stance_kp", controllerDataOut.leg_stance_kp);
    nh.getParam("/atrias_gui/leg_stance_kd", controllerDataOut.leg_stance_kd);
    nh.getParam("/atrias_gui/leg_flight_kp", controllerDataOut.leg_flight_kp);
    nh.getParam("/atrias_gui/leg_flight_kd", controllerDataOut.leg_flight_kd);
    nh.getParam("/atrias_gui/hip_kp", controllerDataOut.hip_kp);
    nh.getParam("/atrias_gui/hip_kd", controllerDataOut.hip_kd);
    int main_controller;
    nh.getParam("/atrias_gui/main_controller", main_controller);
    controllerDataOut.main_controller = (uint8_t)main_controller;
    int spring_type;
    nh.getParam("/atrias_gui/spring_type", spring_type);
    controllerDataOut.spring_type = (uint8_t)spring_type;
    int stance_controller;
    nh.getParam("/atrias_gui/stance_controller", stance_controller);
    controllerDataOut.stance_controller = (uint8_t)stance_controller;

    // Configure the GUI
    standing_leg_length_spinbutton->set_value(controllerDataOut.standing_leg_length);
    touchdown_angle_spinbutton->set_value(controllerDataOut.touchdown_angle);
    takeoff_angle_spinbutton->set_value(controllerDataOut.takeoff_angle);
    slip_leg_length_spinbutton->set_value(controllerDataOut.slip_leg_length);
    linear_spring_constant_spinbutton->set_value(controllerDataOut.linear_spring_constant);
    leg_stance_kp_spinbutton->set_value(controllerDataOut.leg_stance_kp);
    leg_stance_kd_spinbutton->set_value(controllerDataOut.leg_stance_kd);
    leg_flight_kp_spinbutton->set_value(controllerDataOut.leg_flight_kp);
    leg_flight_kd_spinbutton->set_value(controllerDataOut.leg_flight_kd);
    hip_kp_spinbutton->set_value(controllerDataOut.hip_kp);
    hip_kd_spinbutton->set_value(controllerDataOut.hip_kd);
    main_controller_combobox->set_active(controllerDataOut.main_controller);
    spring_type_combobox->set_active(controllerDataOut.spring_type);
    stance_controller_combobox->set_active(controllerDataOut.stance_controller);
}

// Set parameters on the ROS server according to current GUI settings
void setParameters() {
    nh.setParam("/atrias_gui/standing_leg_length", controllerDataOut.standing_leg_length);
    nh.setParam("/atrias_gui/touchdown_angle", controllerDataOut.touchdown_angle);
    nh.setParam("/atrias_gui/takeoff_angle", controllerDataOut.takeoff_angle);
    nh.setParam("/atrias_gui/slip_leg_length", controllerDataOut.slip_leg_length);
    nh.setParam("/atrias_gui/linear_spring_constant", controllerDataOut.linear_spring_constant);
    nh.setParam("/atrias_gui/leg_stance_kp", controllerDataOut.leg_stance_kp);
    nh.setParam("/atrias_gui/leg_stance_kd", controllerDataOut.leg_stance_kd);
    nh.setParam("/atrias_gui/leg_flight_kp", controllerDataOut.leg_flight_kp);
    nh.setParam("/atrias_gui/leg_flight_kd", controllerDataOut.leg_flight_kd);
    nh.setParam("/atrias_gui/hip_kp", controllerDataOut.hip_kp);
    nh.setParam("/atrias_gui/hip_kd", controllerDataOut.hip_kd);
    nh.setParam("/atrias_gui/main_controller", controllerDataOut.main_controller);
    nh.setParam("/atrias_gui/spring_type", controllerDataOut.spring_type);
    nh.setParam("/atrias_gui/stance_controller", controllerDataOut.stance_controller);
}

void guiUpdate() {
    controllerDataOut.standing_leg_length = standing_leg_length_spinbutton->get_value();
    controllerDataOut.touchdown_angle = touchdown_angle_spinbutton->get_value();
    controllerDataOut.takeoff_angle = takeoff_angle_spinbutton->get_value();
    controllerDataOut.slip_leg_length = slip_leg_length_spinbutton->get_value();
    controllerDataOut.linear_spring_constant = linear_spring_constant_spinbutton->get_value();
    controllerDataOut.leg_stance_kp = leg_stance_kp_spinbutton->get_value();
    controllerDataOut.leg_stance_kd = leg_stance_kd_spinbutton->get_value();
    controllerDataOut.leg_flight_kp = leg_flight_kp_spinbutton->get_value();
    controllerDataOut.leg_flight_kd = leg_flight_kd_spinbutton->get_value();
    controllerDataOut.hip_kp = hip_kp_spinbutton->get_value();
    controllerDataOut.hip_kd = hip_kd_spinbutton->get_value();
    controllerDataOut.main_controller = (uint8_t)main_controller_combobox->get_active_row_number();
    controllerDataOut.spring_type = (uint8_t)spring_type_combobox->get_active_row_number();
    controllerDataOut.stance_controller = (uint8_t)stance_controller_combobox->get_active_row_number();

    pub.publish(controllerDataOut);
}

void guiTakedown() {
}

