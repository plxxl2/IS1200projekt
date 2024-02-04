/* 




uint8_t reverse (uint8_t input){
	uint8_t output = 0;
	int i;
	for (i=0; i < 8; i++){
		int a = (input >> i ) & 0x01;
		if (a == 1) output = output | a << (7-i);
	}
	return output;
} */