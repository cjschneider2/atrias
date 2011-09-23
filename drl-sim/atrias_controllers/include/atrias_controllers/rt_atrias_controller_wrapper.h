// Devin Koepl

#ifndef FUNCS_H_EC_CONTROL_WRAPPER
#define FUNCS_H_EC_CONTROL_WRAPPER

/****************************************************************************/

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

// RTAI
#include <rtai_lxrt.h>

// EtherCAT
#include <ecrt.h>

// ROS
#include <ros/ros.h>

// ATRIAS robot
#include <atrias/ucontroller.h>

// ATRIAS controllers
#include <atrias_controllers/controller.h>
#include <atrias_controllers/control_switcher_state_machine.h>
#include <atrias_controllers/simulation_ecat_interface.h>
#include <atrias_controllers/controller_wrapper_states.h>

/*****************************************************************************/

#define MEDULLA_BOOM_POS	0, 0
#define MEDULLA_B_POS		0, 1
#define MEDULLA_A_POS		0, 2
#define MEDULLA_HIP_POS		0, 3

#define VENDOR_ID		0x00000777
#define PRODUCT_CODE		0x02628111

#define BITS_IN_A_BYTE 		8

/*****************************************************************************/

// EtherCAT
ec_master_t *master = NULL;
ec_master_state_t master_state = {};

ec_domain_t *domain1 = NULL;
ec_domain_state_t domain1_state = {};

// Slave Configurations
ec_slave_config_t *sc_medulla_boom;
ec_slave_config_t *sc_medullaB;
ec_slave_config_t *sc_medullaA;
ec_slave_config_t *sc_medulla_hip;

/*****************************************************************************/

// process data
uint8_t *domain1_pd; // process data memory

// Slave offsets
unsigned int off_medulla_boom_rx;
unsigned int off_medullaB_rx;
unsigned int off_medullaA_rx;
unsigned int off_medulla_hip_rx;

unsigned int off_medulla_boom_tx;
unsigned int off_medullaB_tx;
unsigned int off_medullaA_tx;
unsigned int off_medulla_hip_tx;

const ec_pdo_entry_reg_t domain_regs[] =
{
	{MEDULLA_BOOM_POS,	VENDOR_ID, PRODUCT_CODE, 0x6126, 0x21, &off_medulla_boom_rx},
	{MEDULLA_BOOM_POS,	VENDOR_ID, PRODUCT_CODE, 0x6130, 0x01, &off_medulla_boom_tx},
	{MEDULLA_B_POS	 ,	VENDOR_ID, PRODUCT_CODE, 0x6126, 0x22, &off_medullaB_rx},
	{MEDULLA_B_POS	 ,	VENDOR_ID, PRODUCT_CODE, 0x6130, 0x02, &off_medullaB_tx},
	{MEDULLA_A_POS	 ,	VENDOR_ID, PRODUCT_CODE, 0x6126, 0x23, &off_medullaA_rx},
	{MEDULLA_A_POS	 ,	VENDOR_ID, PRODUCT_CODE, 0x6130, 0x03, &off_medullaA_tx},
	{MEDULLA_HIP_POS ,	VENDOR_ID, PRODUCT_CODE, 0x6126, 0x24, &off_medulla_hip_rx},
	{MEDULLA_HIP_POS ,	VENDOR_ID, PRODUCT_CODE, 0x6130, 0x04, &off_medulla_hip_tx},
	{}
};

/*****************************************************************************/

// RX PDO entries
ec_pdo_entry_info_t medulla_boom_rxpdo_entries = {0x6126, 0x21,  BITS_IN_A_BYTE * sizeof(uControllerInput)};
ec_pdo_entry_info_t medullaB_rxpdo_entries = {0x6126, 0x22,  BITS_IN_A_BYTE * sizeof(uControllerInput)};
ec_pdo_entry_info_t medullaA_rxpdo_entries = {0x6126, 0x23,  BITS_IN_A_BYTE * sizeof(uControllerInput)};
ec_pdo_entry_info_t medulla_hip_rxpdo_entries = {0x6126, 0x24,  BITS_IN_A_BYTE * sizeof(uControllerInput)};

// TX PDO Entries
ec_pdo_entry_info_t medulla_boom_txpdo_entries = {0x6130, 0x01, BITS_IN_A_BYTE * sizeof(uControllerOutput)};
ec_pdo_entry_info_t medullaB_txpdo_entries = {0x6130, 0x02, BITS_IN_A_BYTE * sizeof(uControllerOutput)};
ec_pdo_entry_info_t medullaA_txpdo_entries = {0x6130, 0x03, BITS_IN_A_BYTE * sizeof(uControllerOutput)};
ec_pdo_entry_info_t medulla_hip_txpdo_entries = {0x6130, 0x04, BITS_IN_A_BYTE * sizeof(uControllerOutput)};

// RX PDO Info
ec_pdo_info_t medulla_boom_rx_pdos = {0x1600, 1, &medulla_boom_rxpdo_entries};
ec_pdo_info_t medullaB_rx_pdos = {0x1601, 1, &medullaB_rxpdo_entries};
ec_pdo_info_t medullaA_rx_pdos = {0x1602, 1, &medullaA_rxpdo_entries};
ec_pdo_info_t medulla_hip_rx_pdos = {0x1603, 1, &medulla_hip_rxpdo_entries};

// TX PDO Info
ec_pdo_info_t medulla_boom_tx_pdos = {0x1A00, 1, &medulla_boom_txpdo_entries};
ec_pdo_info_t medullaB_tx_pdos = {0x1A01, 1, &medullaB_txpdo_entries};
ec_pdo_info_t medullaA_tx_pdos = {0x1A02, 1, &medullaA_txpdo_entries};
ec_pdo_info_t medulla_hip_tx_pdos = {0x1A03, 1, &medulla_hip_txpdo_entries};

/*****************************************************************************/

ec_sync_info_t medulla_boom_sync[] = 
{
	{0, EC_DIR_OUTPUT, 0, NULL,	EC_WD_DISABLE},
	{1, EC_DIR_INPUT , 0, NULL,	EC_WD_DISABLE},
	{2, EC_DIR_OUTPUT, 1, &medulla_boom_rx_pdos, EC_WD_ENABLE},
	{3, EC_DIR_INPUT , 1, &medulla_boom_tx_pdos, EC_WD_ENABLE},
	{0xff}
};

ec_sync_info_t medullaB_sync[] = 
{
	{0, EC_DIR_OUTPUT, 0, NULL,	EC_WD_DISABLE},
	{1, EC_DIR_INPUT , 0, NULL,	EC_WD_DISABLE},
	{2, EC_DIR_OUTPUT, 1, &medullaB_rx_pdos, EC_WD_ENABLE},
	{3, EC_DIR_INPUT , 1, &medullaB_tx_pdos, EC_WD_ENABLE},
	{0xff}
};

ec_sync_info_t medullaA_sync[] = 
{
	{0, EC_DIR_OUTPUT, 0, NULL,	EC_WD_DISABLE},
	{1, EC_DIR_INPUT , 0, NULL,	EC_WD_DISABLE},
	{2, EC_DIR_OUTPUT, 1, &medullaA_rx_pdos, EC_WD_ENABLE},
	{3, EC_DIR_INPUT , 1, &medullaA_tx_pdos, EC_WD_ENABLE},
	{0xff}
};

ec_sync_info_t medulla_hip_sync[] = 
{
	{0, EC_DIR_OUTPUT, 0, NULL,	EC_WD_DISABLE},
	{1, EC_DIR_INPUT , 0, NULL,	EC_WD_DISABLE},
	{2, EC_DIR_OUTPUT, 1, &medulla_hip_rx_pdos, EC_WD_ENABLE},
	{3, EC_DIR_INPUT , 1, &medulla_hip_tx_pdos, EC_WD_ENABLE},
	{0xff}
};

/*****************************************************************************/

#endif // FUNCS_H_EC_CONTROL_WRAPPER
