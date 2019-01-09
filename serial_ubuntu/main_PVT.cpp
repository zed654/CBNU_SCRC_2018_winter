// File: conserial.cpp
// Description: Serial communication console program for Windows and Linux
// WebSite: http://cool-emerald.blogspot.sg/2017/05/serial-port-programming-in-c-with.html
// MIT License (https://opensource.org/licenses/MIT)
// Copyright (c) 2018 Yan Naing Aye

#include <stdio.h>
#include "ceSerial.h"
#include <iostream>
#include <fstream>
#include <signal.h>
using namespace std;
using namespace ce;

typedef unsigned char BYTE;

volatile int j=1;

void handler(int sig){
	j=0;
	return;
}

unsigned long get_reverse(BYTE temp[96], int offset, int length) {
	unsigned long ref = 0;

	//longitude = temp[31] << 24 | temp[30] << 16 | temp[29] << 8 | temp[28]

	for (int i = 0; i < length; i++) {
		ref |= temp[offset + 4 + i] << 8*i;
	}
	return ref;
}


int main()
{
	ceSerial com("/dev/ttyACM0",9600,8,'N',1); // Linux

	printf("Opening port %s.\n",com.GetPort().c_str());
	if (com.Open() == 0) {
		printf("OK.\n");
	}
	else {
		printf("Error.\n");
		return 1;
	}

	bool successFlag;
	BYTE rx;
	BYTE temp[96];
//	ofstream out("PVT_test.txt");

//	if (!out.is_open()){
//		printf("file error!");
//	}

	// install SIGINT handler
	signal(SIGINT, handler);

	while(1) {
		rx = (BYTE) com.ReadChar(successFlag);
		if (rx != 0xB5)
            continue;
		rx = (BYTE) com.ReadChar(successFlag);
		if (rx != 0x62)
            continue;


		unsigned int check_A = 0;
		unsigned int check_B = 0;

		for (int i = 0; i < 96; i++) {
		    temp[i] = (BYTE) com.ReadChar(successFlag);
		    check_A = check_A + temp[i];
		    check_A &= 0xff;
		    check_B = check_B + check_A;
		    check_B &= 0xff;
		}

		if (temp[0] != 0x01)
		    continue;
		if (temp[1] != 0x07)
		    continue;


		BYTE valid = temp[15];
		if (valid & 0b1111 != 0b1111)
		    continue;

		rx = (BYTE) com.ReadChar(successFlag);
		if (rx != check_A)
		    continue;
		//printf("ck_A: %02x, my_caled_check_A: %02x\n", rx, check_A);
		rx = (BYTE) com.ReadChar(successFlag);
		if (rx != check_B)
		    continue;
		//printf("ck_B: %02x, my_caled_check_A: %02x\n", rx, check_B);

		// https://www.u-blox.com/sites/default/files/products/documents/u-blox8-M8_ReceiverDescrProtSpec_%28UBX-13003221%29_Public.pdf
		// according to above link (p. 328, 33.17.14 UBX-NAV-PVT (0x01 0x07)) for payload offset and length, use get_reverse function to get info such as longitude, latitude
		cout << fixed;
		cout.precision(6);
		cout << "longitude: " << (double) get_reverse(temp, 24, 4) * 1e-7 << endl;
		cout << "latitude: " << (double) get_reverse(temp, 28, 4) * 1e-7 << endl<<endl;
		//std::cout << "headVeh: " << (double)get_reverse(temp, 84, 4)*1e-5 << std::endl;

		if(j==0)
			break;
	}
//	out.close();
	return 0;

	printf("Writing.\n");
	char s[]="Hello";
	successFlag=com.Write(s); // write string
	successFlag=com.WriteChar('!'); // write a character

	printf("Waiting 3 seconds.\n");
	ceSerial::Delay(3000); // delay to wait for a character

	printf("Reading.\n");
	char c=com.ReadChar(successFlag); // read a char
	if(successFlag) printf("Rx: %c\n",c);

	printf("Closing port %s.\n",com.GetPort().c_str());
	com.Close();
	return 0;
}
