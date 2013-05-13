/*
 * controller_gui.cpp
 *
 * atc_eq_point controller
 *
 *  Created on: May 5, 2012
 *	  Author: Michael Anderson
 */

#include <atc_eq_point/controller_gui.h>

void gc_l_pressed()  {controllerDataOut.gc_l = 1;}
void gc_l_released() {controllerDataOut.gc_l = 0;}
void gc_r_pressed()  {controllerDataOut.gc_r = 1;}
void gc_r_released() {controllerDataOut.gc_r = 0;}

//! \brief Initialize the GUI.
bool guiInit(Glib::RefPtr<Gtk::Builder> gui) {
	gui->get_widget("control_combobox", control_combobox);
	gui->get_widget("leftHipPos",       lhip_pos_spinbutton);
	gui->get_widget("rightHipPos",      rhip_pos_spinbutton);
	gui->get_widget("gc_l_button", gc_l_button);
	gui->get_widget("gc_r_button", gc_r_button);
	gui->get_widget("aea_spinbutton", aea_spinbutton);
	gui->get_widget("pea_spinbutton", pea_spinbutton);
	gui->get_widget("l_leg_fl_spinbutton", l_leg_fl_spinbutton);
	gui->get_widget("p_af_spinbutton", p_af_spinbutton);
	gui->get_widget("d_af_spinbutton", d_af_spinbutton);
	gui->get_widget("p_lf_spinbutton", p_lf_spinbutton);
	gui->get_widget("d_lf_spinbutton", d_lf_spinbutton);
	gui->get_widget("l_fl_spinbutton", l_fl_spinbutton);
	gui->get_widget("l_leg_st_spinbutton", l_leg_st_spinbutton);
	gui->get_widget("d_as_spinbutton", d_as_spinbutton);
	gui->get_widget("p_ls_spinbutton", p_ls_spinbutton);
	gui->get_widget("d_ls_spinbutton", d_ls_spinbutton);
	gui->get_widget("l_st_spinbutton", l_st_spinbutton);

	if (control_combobox && lhip_pos_spinbutton && rhip_pos_spinbutton && gc_l_button && gc_r_button &&
			aea_spinbutton && pea_spinbutton &&
			l_leg_fl_spinbutton && p_af_spinbutton && d_af_spinbutton && p_lf_spinbutton && d_lf_spinbutton && l_fl_spinbutton &&
			l_leg_st_spinbutton && d_as_spinbutton && p_ls_spinbutton && d_ls_spinbutton && l_st_spinbutton) {
		// Set ranges.
		lhip_pos_spinbutton->set_range(2.0, 2.30);
		rhip_pos_spinbutton->set_range(2.30, 2.60);
		aea_spinbutton->set_range(0.5, 1.57);
		pea_spinbutton->set_range(1.57, 2.5);
		l_leg_fl_spinbutton->set_range(0.5, 1.0);
		p_af_spinbutton->set_range(0, 5000);
		d_af_spinbutton->set_range(0, 500);
		p_lf_spinbutton->set_range(0, 5000);
		d_lf_spinbutton->set_range(0, 500);
		l_fl_spinbutton->set_range(0, 1);
		l_leg_st_spinbutton->set_range(0.5, 1.0);
		d_as_spinbutton->set_range(0, 0.5);
		p_ls_spinbutton->set_range(0, 6000);
		d_ls_spinbutton->set_range(0, 500);
		l_st_spinbutton->set_range(-60, 60);

		// Set default values.
		lhip_pos_spinbutton->set_value(2.15);
		rhip_pos_spinbutton->set_value(2.45);
		aea_spinbutton->set_value(1.00);
		pea_spinbutton->set_value(2.00);
		l_leg_fl_spinbutton->set_value(0.8);
		l_leg_st_spinbutton->set_value(0.9);

		// Connect buttons to functions.
		gc_l_button->signal_pressed().connect(sigc::ptr_fun((void(*)())gc_l_pressed));
		gc_l_button->signal_released().connect(sigc::ptr_fun((void(*)())gc_l_released));
		gc_r_button->signal_pressed().connect(sigc::ptr_fun((void(*)())gc_r_pressed));
		gc_r_button->signal_released().connect(sigc::ptr_fun((void(*)())gc_r_released));

		// Set up subscriber and publisher.
		sub = nh.subscribe("ATCEqPoint_status", 0, controllerCallback);
		pub = nh.advertise<atc_eq_point::controller_input>("ATCEqPoint_input", 0);
		return true;
	}
	return false;
}

//! \brief Update our local copy of the controller status.
void controllerCallback(const atc_eq_point::controller_status &status) {
	controllerDataIn = status;
}

//! \brief Get parameters from the server and configure GUI accordingly.
void getParameters() {
	// Get parameters.
	nh.getParam("/atrias_gui/lhip_pos", controllerDataOut.lhip_pos);
	nh.getParam("/atrias_gui/rhip_pos", controllerDataOut.rhip_pos);
	nh.getParam("/atrias_gui/aea", controllerDataOut.aea);
	nh.getParam("/atrias_gui/pea", controllerDataOut.pea);
	nh.getParam("/atrias_gui/l_leg_fl", controllerDataOut.l_leg_fl);
	nh.getParam("/atrias_gui/p_af", controllerDataOut.p_af);
	nh.getParam("/atrias_gui/d_af", controllerDataOut.d_af);
	nh.getParam("/atrias_gui/p_lf", controllerDataOut.p_lf);
	nh.getParam("/atrias_gui/d_lf", controllerDataOut.d_lf);
	nh.getParam("/atrias_gui/l_fl", controllerDataOut.l_fl);
	nh.getParam("/atrias_gui/l_leg_st", controllerDataOut.l_leg_st);
	nh.getParam("/atrias_gui/d_as", controllerDataOut.d_as);
	nh.getParam("/atrias_gui/p_ls", controllerDataOut.p_ls);
	nh.getParam("/atrias_gui/d_ls", controllerDataOut.d_ls);
	nh.getParam("/atrias_gui/l_st", controllerDataOut.l_st);

	// Configure the GUI.
	lhip_pos_spinbutton->set_value(controllerDataOut.lhip_pos);
	rhip_pos_spinbutton->set_value(controllerDataOut.rhip_pos);
	aea_spinbutton->set_value(controllerDataOut.aea);
	pea_spinbutton->set_value(controllerDataOut.pea);
	l_leg_fl_spinbutton->set_value(controllerDataOut.l_leg_fl);
	p_af_spinbutton->set_value(controllerDataOut.p_af);
	d_af_spinbutton->set_value(controllerDataOut.d_af);
	p_lf_spinbutton->set_value(controllerDataOut.p_lf);
	d_lf_spinbutton->set_value(controllerDataOut.d_lf);
	l_fl_spinbutton->set_value(controllerDataOut.l_fl);
	l_leg_st_spinbutton->set_value(controllerDataOut.l_leg_st);
	d_as_spinbutton->set_value(controllerDataOut.d_ls);
	p_ls_spinbutton->set_value(controllerDataOut.p_ls);
	d_ls_spinbutton->set_value(controllerDataOut.d_ls);
	l_st_spinbutton->set_value(controllerDataOut.l_st);
}

//! \brief Set parameters on server according to current GUI settings.
void setParameters() {
	nh.setParam("/atrias_gui/lhip_pos", controllerDataOut.lhip_pos);
	nh.setParam("/atrias_gui/rhip_pos", controllerDataOut.rhip_pos);
	nh.setParam("/atrias_gui/aea", controllerDataOut.aea);
	nh.setParam("/atrias_gui/pea", controllerDataOut.pea);
	nh.setParam("/atrias_gui/l_leg_fl", controllerDataOut.l_leg_fl);
	nh.setParam("/atrias_gui/p_af", controllerDataOut.p_af);
	nh.setParam("/atrias_gui/d_af", controllerDataOut.d_af);
	nh.setParam("/atrias_gui/p_lf", controllerDataOut.p_lf);
	nh.setParam("/atrias_gui/d_lf", controllerDataOut.d_lf);
	nh.setParam("/atrias_gui/l_fl", controllerDataOut.l_fl);
	nh.setParam("/atrias_gui/l_leg_st", controllerDataOut.l_leg_st);
	nh.setParam("/atrias_gui/d_as", controllerDataOut.d_as);
	nh.setParam("/atrias_gui/p_ls", controllerDataOut.p_ls);
	nh.setParam("/atrias_gui/d_ls", controllerDataOut.d_ls);
	nh.setParam("/atrias_gui/l_st", controllerDataOut.l_st);
}

//! \brief Update the GUI.
void guiUpdate() {
	controllerDataOut.control  = control_combobox->get_active_row_number();
	controllerDataOut.lhip_pos = lhip_pos_spinbutton->get_value();
	controllerDataOut.rhip_pos = rhip_pos_spinbutton->get_value();
	controllerDataOut.aea      = aea_spinbutton->get_value();
	controllerDataOut.pea      = pea_spinbutton->get_value();
	controllerDataOut.l_leg_fl = l_leg_fl_spinbutton->get_value();
	controllerDataOut.p_af     = p_af_spinbutton->get_value();
	controllerDataOut.d_af     = d_af_spinbutton->get_value();
	controllerDataOut.p_lf     = p_lf_spinbutton->get_value();
	controllerDataOut.d_lf     = d_lf_spinbutton->get_value();
	controllerDataOut.l_fl     = l_fl_spinbutton->get_value();
	controllerDataOut.l_leg_st = l_leg_st_spinbutton->get_value();
	controllerDataOut.d_as     = d_as_spinbutton->get_value();
	controllerDataOut.p_ls     = p_ls_spinbutton->get_value();
	controllerDataOut.d_ls     = d_ls_spinbutton->get_value();
	controllerDataOut.l_st     = l_st_spinbutton->get_value();

	pub.publish(controllerDataOut);
}

//! \brief Take down the GUI.
void guiTakedown() {
}

