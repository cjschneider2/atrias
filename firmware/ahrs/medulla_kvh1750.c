#include <medulla_kvh1750.h>

uint8_t unused_outbuffer[128];
uint8_t unused_inbuffer[128];
uint8_t imu_inbuffer[128];
uint8_t bytes_received = 0;
float debug_buf[10];

void setup_kvh(void)
{
	imu_port = uart_init_port(&PORTF, &USARTF0, uart_baud_921600, unused_outbuffer, 128, unused_inbuffer, 128);
	uart_connect_port(&imu_port, false);

	msync_pin = io_init_pin(&PORTF, 1);   // Initialize master sync pin for IMU.
	PORTF.DIR = PORTF.DIR | (1<<1);   // msync pin is output.
}

void read_kvh(float gyr[3], float acc[3])
{
	int i;
	PORTF.OUT = PORTF.OUT | (1<<1);   // Pull msync pin high.
	_delay_us(30);   // ..for at least 30 us.
	PORTF.OUT = PORTF.OUT & ~(1<<1);   // Pull msync pin low.

	//_delay_us(1);   // Wait for IMU to finish sending data. Scope shows about 60 us delay from rising edge of msync to first bit of packet. NOTE: This doesn't work as intended when external interrupts are on, since UART will interrupt the delay!
	// It takes 390.6 us for the IMU to send 36 bytes at 921600 baud.

	bytes_received = uart_received_bytes(&imu_port);
	uart_rx_data(&imu_port, imu_inbuffer, bytes_received);   // This takes 200 us.

	// Reverse order of bytes.
	for (i=0; i<3; i++) {
		((uint8_t*) &gyr[i])[0] = imu_inbuffer[4+4*i+3];
		((uint8_t*) &gyr[i])[1] = imu_inbuffer[4+4*i+2];
		((uint8_t*) &gyr[i])[2] = imu_inbuffer[4+4*i+1];
		((uint8_t*) &gyr[i])[3] = imu_inbuffer[4+4*i+0];
		((uint8_t*) &acc[i])[0] = imu_inbuffer[4+4*(3+i)+3];
		((uint8_t*) &acc[i])[1] = imu_inbuffer[4+4*(3+i)+2];
		((uint8_t*) &acc[i])[2] = imu_inbuffer[4+4*(3+i)+1];
		((uint8_t*) &acc[i])[3] = imu_inbuffer[4+4*(3+i)+0];
	}
}

void print_imu()
{
	int i, a=4;
	printf("%02x%02x%02x%02x  %02x%02x%02x%02x  %02x%02x%02x%02x   ",
			imu_inbuffer[a+0], imu_inbuffer[a+1], imu_inbuffer[a+2], imu_inbuffer[a+3],
			imu_inbuffer[a+4], imu_inbuffer[a+5], imu_inbuffer[a+6], imu_inbuffer[a+7],
			imu_inbuffer[a+8], imu_inbuffer[a+9], imu_inbuffer[a+10], imu_inbuffer[a+11]);

	static float buf[6];
	for (i=0; i<6; i++) {
		//for (j=0; j<4; j++) {
		//	printf("%02x", ((uint8_t*) &buf[i])[j]);
		//}
	}
	printf("\n");
}

void print_dcm(float dcm[3][3])
{
	static int i, j, k;

	for (i=0; i<3; i++) {
		for (j=0; j<3; j++) {
			printf("%4d ", (int16_t) (dcm[i][j]*1000));
			//for (k=0; k<4; k++) {
			//	printf("%c", ((uint8_t*) dcm[i])[4*j+k]);
			//}
		}
	}
	printf("\n");
	// TODO: The following kinda works, but it doesn't..
	//printf("%s\n", buf);
}

