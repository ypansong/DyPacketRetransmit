#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;
struct Packet {
	int arrival_time_in_ms;
	short sequence;
	char fec_on;
	char continuous_on;
};
typedef struct Packet Packet;

void main()
{
	int i;
	const char kFecOn = 0;
	const char kContinuousOn = 1;
	const int kStratLength = 100;
	Packet packet_start[kStratLength];
	for (i = 0; i < kStratLength; i++){
		packet_start[i].arrival_time_in_ms = i;
		packet_start[i].sequence = i + 1;
		packet_start[i].fec_on = kFecOn;
		packet_start[i].continuous_on = kContinuousOn;
	}

	// lost 104;
	const int kTestLen_1 = 10;
	short test_seq_1[kTestLen_1] = { 100, 101, 102, 103, 105, 106, 107, 108, 109, 110 };
	Packet test_sample_1[kStratLength + kTestLen_1];
	memcpy(test_sample_1, packet_start, sizeof(packet_start));
	for (i = kStratLength; i < kStratLength + kTestLen_1; i ++){
		test_sample_1[i].arrival_time_in_ms = i;
		test_sample_1[i].sequence = test_seq_1[i - kStratLength];
		test_sample_1[i].fec_on = kFecOn;
		test_sample_1[i].continuous_on = kContinuousOn;
	}

	// lost 103, 104;
	const int kTestLen_2 = 10;
	short test_seq_2[kTestLen_2] = { 100, 101, 102, 105, 106, 107, 108, 109, 110, 111 };
	Packet test_sample_2[kStratLength + kTestLen_2];
	memcpy(test_sample_2, packet_start, sizeof(packet_start));
	for (i = kStratLength; i < kStratLength + kTestLen_2; i++){
		test_sample_2[i].arrival_time_in_ms = i;
		test_sample_2[i].sequence = test_seq_2[i - kStratLength];
		test_sample_2[i].fec_on = kFecOn;
		test_sample_2[i].continuous_on = kContinuousOn;
	}

	// lost 101, 103, 104, 105;
	const int kTestLen_3 = 10;
	short test_seq_3[kTestLen_3] = { 100, 102, 106, 107, 108, 109, 110, 111, 112, 113};
	Packet test_sample_3[kStratLength + kTestLen_3];
	memcpy(test_sample_3, packet_start, sizeof(packet_start));
	for (i = kStratLength; i < kStratLength + kTestLen_3; i++){
		test_sample_3[i].arrival_time_in_ms = i;
		test_sample_3[i].sequence = test_seq_3[i - kStratLength];
		test_sample_3[i].fec_on = kFecOn;
		test_sample_3[i].continuous_on = kContinuousOn;
	}

	// lost 103, 104, 105;
	const int kTestLen_4 = 10;
	short test_seq_4[kTestLen_4] = { 100, 101, 102, 106, 107, 108, 109, 110, 111, 112 };
	Packet test_sample_4[kStratLength + kTestLen_4];
	memcpy(test_sample_4, packet_start, sizeof(packet_start));
	for (i = kStratLength; i < kStratLength + kTestLen_4; i++){
		test_sample_4[i].arrival_time_in_ms = i;
		test_sample_4[i].sequence = test_seq_4[i - kStratLength];
		test_sample_4[i].fec_on = kFecOn;
		test_sample_4[i].continuous_on = kContinuousOn;
	}

	// lost 101, 103, 105, 107, 109, 111, 113, 115, 117, 119, 121, 123, 125, 127, 129, 131, 133, 135, 137;
	const int kTestLen_5 = 20;
	short test_seq_5[kTestLen_5] = { 100, 102, 104, 106, 108, 110, 112, 114, 116, 118, 120, 122, 124, 126, 128, 130, 132, 134, 136, 138 };
	Packet test_sample_5[kStratLength + kTestLen_5];
	memcpy(test_sample_5, packet_start, sizeof(packet_start));
	for (i = kStratLength; i < kStratLength + kTestLen_5; i++){
		test_sample_5[i].arrival_time_in_ms = i;
		test_sample_5[i].sequence = test_seq_5[i - kStratLength];
		test_sample_5[i].fec_on = kFecOn;
		test_sample_5[i].continuous_on = kContinuousOn;
	}

	// lost none;
	const int kTestLen_6 = 10;
	short test_seq_6[kTestLen_6] = { 100, 101, 102, 103, 104, 105, 106, 107, 108, 109 };
	Packet test_sample_6[kStratLength + kTestLen_6];
	memcpy(test_sample_6, packet_start, sizeof(packet_start));
	for (i = kStratLength; i < kStratLength + kTestLen_6; i++){
		test_sample_6[i].arrival_time_in_ms = i;
		test_sample_6[i].sequence = test_seq_6[i - kStratLength];
		test_sample_6[i].fec_on = kFecOn;
		test_sample_6[i].continuous_on = kContinuousOn;
	}

	// lost none;
	const int kTestLen_7 = 20;
	short test_seq_7[kTestLen_7] = { 100, 101, 101, 101, 102, 103, 103, 103, 104, 104, 105, 106, 107, 108, 108, 108, 109, 109, 109, 109 };
	Packet test_sample_7[kStratLength + kTestLen_7];
	memcpy(test_sample_7, packet_start, sizeof(packet_start));
	for (i = kStratLength; i < kStratLength + kTestLen_7; i++){
		test_sample_7[i].arrival_time_in_ms = i;
		test_sample_7[i].sequence = test_seq_7[i - kStratLength];
		test_sample_7[i].fec_on = kFecOn;
		test_sample_7[i].continuous_on = kContinuousOn;
	}

	// lost 102, 104, 106;
	const int kTestLen_8 = 10;
	short test_seq_8[kTestLen_8] = { 100, 101, 101, 103, 103, 105, 105, 107, 108, 109 };
	Packet test_sample_8[kStratLength + kTestLen_8];
	memcpy(test_sample_8, packet_start, sizeof(packet_start));
	for (i = kStratLength; i < kStratLength + kTestLen_8; i++){
		test_sample_8[i].arrival_time_in_ms = i;
		test_sample_8[i].sequence = test_seq_8[i - kStratLength];
		test_sample_8[i].fec_on = kFecOn;
		test_sample_8[i].continuous_on = kContinuousOn;
	}

	// lost 102;
	const int kTestLen_9 = 10;
	short test_seq_9[kTestLen_9] = { 100, 101, 103, 102, 104, 105, 106, 107, 108, 109 };
	Packet test_sample_9[kStratLength + kTestLen_9];
	memcpy(test_sample_9, packet_start, sizeof(packet_start));
	for (i = kStratLength; i < kStratLength + kTestLen_9; i++){
		test_sample_9[i].arrival_time_in_ms = i;
		test_sample_9[i].sequence = test_seq_9[i - kStratLength];
		test_sample_9[i].fec_on = kFecOn;
		test_sample_9[i].continuous_on = kContinuousOn;
	}

	// lost 101, 103, 105;
	const int kTestLen_10 = 10;
	short test_seq_10[kTestLen_10] = { 100, 102, 101, 104, 103, 106, 105, 107, 108, 109 };
	Packet test_sample_10[kStratLength + kTestLen_10];
	memcpy(test_sample_10, packet_start, sizeof(packet_start));
	for (i = kStratLength; i < kStratLength + kTestLen_10; i++){
		test_sample_10[i].arrival_time_in_ms = i;
		test_sample_10[i].sequence = test_seq_10[i - kStratLength];
		test_sample_10[i].fec_on = kFecOn;
		test_sample_10[i].continuous_on = kContinuousOn;
	}

    // lost none;
    const int kTestLen_11 = 10;
    short test_seq_11[kTestLen_11] = { 100, 101,  102, 103, 104, 105, 106, 107, 108, 109 };
    Packet test_sample_11[kStratLength + kTestLen_11];
    memcpy(test_sample_11, packet_start, sizeof(packet_start));
    for (i = kStratLength; i < kStratLength + kTestLen_11; i++){
      test_sample_11[i].arrival_time_in_ms = i;
      test_sample_11[i].sequence = test_seq_11[i - kStratLength];
      test_sample_11[i].fec_on = kFecOn;
      test_sample_11[i].continuous_on = kContinuousOn;
    }
    unsigned short remove_table_11[] = { 1, 2 , 30, 31, 32, 40, 41};
    int remove_table_length_11 = sizeof(remove_table_11) / sizeof(unsigned short);
    for (i = 0; i < remove_table_length_11; i++) {
      int remove_i = remove_table_11[remove_table_length_11 - i - 1];
      memmove(test_sample_11 + (remove_i - 1), test_sample_11 + remove_i,
        sizeof(test_sample_11)-sizeof(Packet)* remove_i);
    }


    // lost none;
    const int kTestLen_12 = 10;
    //short test_seq_12[kTestLen_12] = { 100, 101, 102, 103, 104, 105, 106, 107, 108, 109 };
    Packet test_sample_12[kStratLength + kTestLen_12];
    memcpy(test_sample_12, packet_start, sizeof(packet_start));

    unsigned short remove_table_12[] = { 1, 2, 30, 31, 32, 40, 41 };
    int remove_table_length_12 = sizeof(remove_table_12) / sizeof(unsigned short);
    for (i = 0; i < remove_table_length_12; i++) {
      int remove_i = remove_table_12[remove_table_length_12 - i - 1];
      memmove(test_sample_12 + (remove_i - 1), test_sample_12 + remove_i,
        sizeof(test_sample_12)-sizeof(Packet)* remove_i);
    }
    int remainder_start_12 = (sizeof(packet_start) / sizeof(Packet)) -
      (sizeof(remove_table_12) / sizeof(remove_table_12[0]));

    for (i = remainder_start_12; i < sizeof(test_sample_12)/ sizeof(Packet); i++){
      test_sample_12[i].arrival_time_in_ms = test_sample_12[i - 1].arrival_time_in_ms + 1;
      test_sample_12[i].sequence = test_sample_12[i - 1].sequence + 1;
      test_sample_12[i].fec_on = kFecOn;
      test_sample_12[i].continuous_on = kContinuousOn;
    }

    // lost none;
    const int kTestLen_13 = 20;
    Packet test_sample_13[kStratLength + kTestLen_13];
    for (i = 0; i < sizeof(test_sample_13)/ sizeof(test_sample_13[0]); i++){
      test_sample_13[i].arrival_time_in_ms = i;
      test_sample_13[i].sequence = i + 1;
      test_sample_13[i].fec_on = kFecOn;
      test_sample_13[i].continuous_on = kContinuousOn;
    }

    unsigned short remove_table_13[] = { 1, 2, 30, 31, 32, 40, 41 , 107, 108};
    int remove_table_length_13 = sizeof(remove_table_13) / sizeof(unsigned short);
    for (i = 0; i < remove_table_length_13; i++) {
      int remove_i = remove_table_13[remove_table_length_13 - i - 1];
      memmove(test_sample_13 + (remove_i - 1), test_sample_13 + remove_i,
        sizeof(test_sample_13)-sizeof(Packet)* remove_i);
    }

    int remainder_start_13 = (sizeof(test_sample_13) / sizeof(Packet)) -
      (sizeof(remove_table_13) / sizeof(remove_table_13[0]));
    for (i = remainder_start_13; i < sizeof(test_sample_13) / sizeof(Packet); i++){
      test_sample_13[i].arrival_time_in_ms = test_sample_13[i - 1].arrival_time_in_ms + 1;
      test_sample_13[i].sequence = test_sample_13[i - 1].sequence + 1;
      test_sample_13[i].fec_on = kFecOn;
      test_sample_13[i].continuous_on = kContinuousOn;
    }

    // lost 110;
    const int kTestLen_14 = 20;
    Packet test_sample_14[kStratLength + kTestLen_14];
    for (i = 0; i < sizeof(test_sample_14) / sizeof(test_sample_14[0]); i++){
      test_sample_14[i].arrival_time_in_ms = i;
      test_sample_14[i].sequence = i + 1;
      test_sample_14[i].fec_on = kFecOn;
      test_sample_14[i].continuous_on = kContinuousOn;
    }

    unsigned short remove_table_14[] = { 1, 2, 30, 31, 32, 40, 41, 107, 108 , 110};
    int remove_table_length_14 = sizeof(remove_table_14) / sizeof(unsigned short);
    for (i = 0; i < remove_table_length_14; i++) {
      int remove_i = remove_table_14[remove_table_length_14 - i - 1];
      memmove(test_sample_14 + (remove_i - 1), test_sample_14 + remove_i,
        sizeof(test_sample_14)-sizeof(Packet)* remove_i);
    }

    int remainder_start_14 = (sizeof(test_sample_14) / sizeof(Packet)) -
      (sizeof(remove_table_14) / sizeof(remove_table_14[0]));
    for (i = remainder_start_14; i < sizeof(test_sample_14) / sizeof(Packet); i++){
      test_sample_14[i].arrival_time_in_ms = test_sample_14[i - 1].arrival_time_in_ms + 1;
      test_sample_14[i].sequence = test_sample_14[i - 1].sequence + 1;
      test_sample_14[i].fec_on = kFecOn;
      test_sample_14[i].continuous_on = kContinuousOn;
    }

    // lost 107, 108, 109, 110;
    const int kTestLen_15 = 20;
    Packet test_sample_15[kStratLength + kTestLen_15];
    for (i = 0; i < sizeof(test_sample_15) / sizeof(test_sample_15[0]); i++){
      test_sample_15[i].arrival_time_in_ms = i;
      test_sample_15[i].sequence = i + 1;
      test_sample_15[i].fec_on = kFecOn;
      test_sample_15[i].continuous_on = kContinuousOn;
    }

    unsigned short remove_table_15[] = { 2, 30, 31, 32, 40, 41, 107, 108, 109, 110 };
    int remove_table_length_15 = sizeof(remove_table_15) / sizeof(unsigned short);
    for (i = 0; i < remove_table_length_15; i++) {
      int remove_i = remove_table_15[remove_table_length_15 - i - 1];
      memmove(test_sample_15 + (remove_i - 1), test_sample_15 + remove_i,
        sizeof(test_sample_15)-sizeof(Packet)* remove_i);
    }

    int remainder_start_15 = (sizeof(test_sample_15) / sizeof(Packet)) -
      (sizeof(remove_table_15) / sizeof(remove_table_15[0]));
    for (i = remainder_start_15; i < sizeof(test_sample_15) / sizeof(Packet); i++){
      test_sample_15[i].arrival_time_in_ms = test_sample_15[i - 1].arrival_time_in_ms + 1;
      test_sample_15[i].sequence = test_sample_15[i - 1].sequence + 1;
      test_sample_15[i].fec_on = kFecOn;
      test_sample_15[i].continuous_on = kContinuousOn;
    }

	int tmp = 10;
	
}