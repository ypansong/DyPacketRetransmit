#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>


#include "lost_packets_retransmit.h"

#define TEST_NO_ERROR(METHOD) do{\
  int ret = (METHOD);\
  if(ret){\
    std::cout<< "Exception in "<<#METHOD << ", line "<< __LINE__ <<std::endl;\
    std::cout<< ", retturn value" << ret << std::endl;\
  }\
}while(0);

#define TEST_RESULT(name, y) do{\
    if(y){\
      std::cout << #name << " fails: " << (y) << std::endl;\
    } else {\
      std::cout << #name << " Pass." << std::endl;\
    }\
}while(0);

int BufferEqual(const int buffer_length, const unsigned short *buffer, const int *result_input)
{
  int i;
  if (buffer_length != result_input[0]) {
    return 1;
  }
  for (i = 0; i < buffer_length; i++) {
    if (result_input[i + 1] != buffer[i]) {
      return 1;
    }
  }
  return 0;
}
int SumInt(const int * data, const int length) {
// Only used in calculating time stamps sum.
  int i;
  int sum = 0;
  for (i = 0; i < length; i++) {
    sum += data[i];
  }
  return sum;
}

using namespace std;
struct Packet {
	int arrival_time_in_ms;
	unsigned short sequence;
	char fec_on;
	char continuous_on;
};
typedef struct Packet Packet;
int TestTimeOutFunction();
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


	//Test 1, lost 104;
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
  {// Test.
    int test_result = 0;
    unsigned short out_put_seq[100] = { 0 };
    int out_length = 0;
    LostPacketsRetransmiter lpr;
    for (i = 0; i < sizeof(test_sample_1) / sizeof(test_sample_1[0]); i++) {
      TEST_NO_ERROR(lpr.DetectGap(test_sample_1[i].sequence, test_sample_1[i].arrival_time_in_ms));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      if (104 > i) {
        int result_temp[] = { 0 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      } else {
        int result_temp[] = { 1 , 104};
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      }
    }
    TEST_RESULT(test_sample_1, test_result);
  }

	//Test 2 lost 103, 104;
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

  {// Test.
    int test_result = 0;
    unsigned short out_put_seq[100] = { 0 };
    int out_length = 0;
    LostPacketsRetransmiter lpr;
    for (i = 0; i < sizeof(test_sample_2) / sizeof(test_sample_2[0]); i++) {
      TEST_NO_ERROR(lpr.DetectGap(test_sample_2[i].sequence, test_sample_2[i].arrival_time_in_ms));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      if (103 > i) {
        int result_temp[] = { 0 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      } else {
        int result_temp[] = { 2, 103, 104 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      }
    }
    TEST_RESULT(test_sample_2, test_result);
  }

	// Test 3, lost 101, 103, 104, 105;
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

  {// Test.
    int test_result = 0;
    unsigned short out_put_seq[100] = { 0 };
    int out_length = 0;
    LostPacketsRetransmiter lpr;
    for (i = 0; i < sizeof(test_sample_3) / sizeof(test_sample_3[0]); i++) {
      TEST_NO_ERROR(lpr.DetectGap(test_sample_3[i].sequence, test_sample_3[i].arrival_time_in_ms));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      if (101 > i) {
        int result_temp[] = { 0 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      } else if( 101 == i){
        int result_temp[] = { 1, 101 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      } else {
        int result_temp[] = { 4, 101 , 103, 104, 105};
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      }
    }
    TEST_RESULT(test_sample_3, test_result);
  }

	// Test 4, lost 103, 104, 105;
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
  {// Test.
    int test_result = 0;
    unsigned short out_put_seq[100] = { 0 };
    int out_length = 0;
    LostPacketsRetransmiter lpr;
    for (i = 0; i < sizeof(test_sample_4) / sizeof(test_sample_4[0]); i++) {
      TEST_NO_ERROR(lpr.DetectGap(test_sample_4[i].sequence, test_sample_4[i].arrival_time_in_ms));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      if (103 > i) {
        int result_temp[] = { 0 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      } else {
        int result_temp[] = { 3, 103, 104, 105 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      }
    }
    TEST_RESULT(test_sample_4, test_result);
  }

	//Test 5. lost 101, 103, 105, 107, 109, 111, 113, 115, 117, 119, 121, 123, 125, 127, 129, 131, 133, 135, 137;
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

  {// Test.
    int test_result = 0;
    unsigned short out_put_seq[100] = { 0 };
    int out_length = 0;
    LostPacketsRetransmiter lpr;
    for (i = 0; i < sizeof(test_sample_5) / sizeof(test_sample_5[0]); i++) {
      TEST_NO_ERROR(lpr.DetectGap(test_sample_5[i].sequence, test_sample_5[i].arrival_time_in_ms));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      if (101 > i) {
        int result_temp[] = { 0 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      } else if(101 == i){
        int result_temp[] = { 1, 101 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      } else if (102 == i) {
        int result_temp[] = { 2, 101, 103 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      } else if (103 == i) {
        int result_temp[] = { 3, 101, 103, 105};
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      } else if (104 == i) {
        int result_temp[] = { 4, 101, 103, 105, 107};
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      } else if (105 == i) {
        int result_temp[] = { 5, 101, 103, 105, 107, 109 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      } else if (106 == i) {
        int result_temp[] = { 6, 101, 103, 105, 107, 109, 111 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      } else if (107 == i) {
        int result_temp[] = { 7, 101, 103, 105, 107, 109, 111, 113};
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      } else if (108 == i) {
        int result_temp[] = { 8, 101, 103 ,105, 107, 109, 111, 113, 115 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      } else if (109 == i) {
        int result_temp[] = { 9, 101, 103 ,105, 107, 109, 111, 113, 115, 117 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      } else if (110 == i) {
        int result_temp[] = { 9, 103 ,105, 107, 109, 111, 113, 115, 117, 119};
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      } else if (111 == i) {
        int result_temp[] = { 9, 105, 107, 109, 111, 113, 115, 117, 119, 121};
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      } else if (112 == i) {
        int result_temp[] = { 9, 107, 109, 111, 113, 115, 117, 119, 121, 123 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      } else if (113 == i) {
        int result_temp[] = { 9, 109, 111, 113, 115, 117, 119, 121, 123 , 125};
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      } else if (114 == i) {
        int result_temp[] = { 9, 111, 113, 115, 117, 119, 121, 123 , 125, 127 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      } else if (115 == i) {
        int result_temp[] = { 9, 113, 115, 117, 119, 121, 123 , 125, 127, 129 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      } else if (116 == i) {
        int result_temp[] = { 9, 115, 117, 119, 121, 123 , 125, 127, 129, 131 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      } else if (117 == i) {
        int result_temp[] = { 9, 117, 119, 121, 123 , 125, 127, 129, 131, 133 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      } else if (118 == i) {
        int result_temp[] = { 9, 119, 121, 123 , 125, 127, 129, 131, 133, 135 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      } else {
        int result_temp[] = { 9, 121, 123 , 125, 127, 129, 131, 133, 135 , 137};
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      }
    }
    TEST_RESULT(test_sample_5, test_result);
  }

	//Test6, lost none;
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
  {// Test.
    int test_result = 0;
    unsigned short out_put_seq[100] = { 0 };
    int out_length = 0;
    LostPacketsRetransmiter lpr;
    for (i = 0; i < sizeof(test_sample_6) / sizeof(test_sample_6[0]); i++) {
      TEST_NO_ERROR(lpr.DetectGap(test_sample_6[i].sequence, test_sample_6[i].arrival_time_in_ms));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 0 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    TEST_RESULT(test_sample_6, test_result);
  }


	//Test7, lost none;
	const int kTestLen_7 = 20;
	short test_seq_7[kTestLen_7] = { 100, 101, 101, 101, 102, 103, 103, 103, 104, 104, 105, 106, 107, 111, 108, 109, 110, 112, 113, 114 };
	Packet test_sample_7[kStratLength + kTestLen_7];
	memcpy(test_sample_7, packet_start, sizeof(packet_start));
	for (i = kStratLength; i < kStratLength + kTestLen_7; i++){
		test_sample_7[i].arrival_time_in_ms = i;
		test_sample_7[i].sequence = test_seq_7[i - kStratLength];
		test_sample_7[i].fec_on = kFecOn;
		test_sample_7[i].continuous_on = kContinuousOn;
	}

  {// Test.
    int test_result = 0;
    unsigned short out_put_seq[100] = { 0 };
    int out_length = 0;
    LostPacketsRetransmiter lpr;
    for (i = 0; i < sizeof(test_sample_7) / sizeof(test_sample_7[0]); i++) {
      TEST_NO_ERROR(lpr.DetectGap(test_sample_7[i].sequence, test_sample_7[i].arrival_time_in_ms));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      if (113 > i) {
        int result_temp[] = { 0 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      } else if(113 == i) {
        int result_temp[] = { 3, 108, 109, 110 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      } else if (114 == i) {
        int result_temp[] = { 2, 109, 110 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      } else if (115 == i) {
        int result_temp[] = { 1, 110 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      } else {
        int result_temp[] = {0};
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      }
    }
    TEST_RESULT(test_sample_7, test_result);
  }

	//test 8, lost 102, 104, 106;
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
  {// Test.
    int test_result = 0;
    unsigned short out_put_seq[100] = { 0 };
    int out_length = 0;
    LostPacketsRetransmiter lpr;
    for (i = 0; i < sizeof(test_sample_8) / sizeof(test_sample_8[0]); i++) {
      TEST_NO_ERROR(lpr.DetectGap(test_sample_8[i].sequence, test_sample_8[i].arrival_time_in_ms));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      if (103 > i) {
        int result_temp[] = { 0 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      }else if( 105 > i){
        int result_temp[] = { 1, 102};
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      } else if (107 > i) {
        int result_temp[] = { 2, 102, 104 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      } else {
        int result_temp[] = { 3, 102, 104, 106 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      }
    }
    TEST_RESULT(test_sample_8, test_result);
  }


	//Test 9, lost 102;
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
  {// Test.
    int test_result = 0;
    unsigned short out_put_seq[100] = { 0 };
    int out_length = 0;
    LostPacketsRetransmiter lpr;
    for (i = 0; i < sizeof(test_sample_9) / sizeof(test_sample_9[0]); i++) {
      TEST_NO_ERROR(lpr.DetectGap(test_sample_9[i].sequence, test_sample_9[i].arrival_time_in_ms));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      if (102 > i) {
        int result_temp[] = { 0 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      } else if (102 == i) {
        int result_temp[] = { 1, 102 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      } else {
        int result_temp[] = { 0 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      }
    }
    TEST_RESULT(test_sample_9, test_result);
  }

	//Test 10, lost 101, 103, 105;
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
  {// Test.
    int test_result = 0;
    unsigned short out_put_seq[100] = { 0 };
    int out_length = 0;
    LostPacketsRetransmiter lpr;
    for (i = 0; i < sizeof(test_sample_10) / sizeof(test_sample_10[0]); i++) {
      TEST_NO_ERROR(lpr.DetectGap(test_sample_10[i].sequence, test_sample_10[i].arrival_time_in_ms));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      if (101 > i) {
        int result_temp[] = { 0 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      }
      else if (101 == i) {
        int result_temp[] = { 1, 101 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      }
      else if (102 == i){
        int result_temp[] = { 0 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      }
      else if (103 == i) {
        int result_temp[] = { 1, 103 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      }
      else if (104 == i) {
        int result_temp[] = { 0 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      }
      else if (105 == i) {
        int result_temp[] = { 1, 105 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      }
      else{
        int result_temp[] = { 0 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      }
    }
    TEST_RESULT(test_sample_10, test_result);
  }

    //Test 11, lost none;
    const int kTestLen_11 = 10;
    short test_seq_11[kTestLen_11] = { 100, 101, 102, 103, 104, 105, 106, 107, 108, 109 };
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
      memmove(test_sample_11 + (remove_i - 1), test_sample_11 + remove_i, sizeof(test_sample_11) - sizeof(Packet)* remove_i);
    }
    {// Test.
      int test_result = 0;
      unsigned short out_put_seq[100] = { 0 };
      int out_length = 0;
      LostPacketsRetransmiter lpr;
      for (i = 0; i < sizeof(test_sample_11) / sizeof(test_sample_11[0]); i++) {
        TEST_NO_ERROR(lpr.DetectGap(test_sample_11[i].sequence, test_sample_11[i].arrival_time_in_ms));
        TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
        int result_temp[] = { 0 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      }
      TEST_RESULT(test_sample_11, test_result);
    }


    //Test 12, lost none;
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
    {// Test.
      int test_result = 0;
      unsigned short out_put_seq[100] = { 0 };
      int out_length = 0;
      LostPacketsRetransmiter lpr;
      for (i = 0; i < sizeof(test_sample_12) / sizeof(test_sample_12[0]); i++) {
        TEST_NO_ERROR(lpr.DetectGap(test_sample_12[i].sequence, test_sample_12[i].arrival_time_in_ms));
        TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
        int result_temp[] = { 0 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      }
      TEST_RESULT(test_sample_12, test_result);
    }

    //Test 13, lost none;
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
    {// Test.
      int test_result = 0;
      unsigned short out_put_seq[100] = { 0 };
      int out_length = 0;
      LostPacketsRetransmiter lpr;
      for (i = 0; i < sizeof(test_sample_13) / sizeof(test_sample_13[0]); i++) {
        TEST_NO_ERROR(lpr.DetectGap(test_sample_13[i].sequence, test_sample_13[i].arrival_time_in_ms));
        TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
        int result_temp[] = { 0 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      }
      TEST_RESULT(test_sample_13, test_result);
    }

    //Test 14, lost 110;
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
    {// Test.
      int test_result = 0;
      unsigned short out_put_seq[100] = { 0 };
      int out_length = 0;
      LostPacketsRetransmiter lpr;
      for (i = 0; i < sizeof(test_sample_14) / sizeof(test_sample_14[0]); i++) {
        TEST_NO_ERROR(lpr.DetectGap(test_sample_14[i].sequence, test_sample_14[i].arrival_time_in_ms));
        TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));

        if (100 > i) {
          int result_temp[] = { 0 };
          test_result += BufferEqual(out_length, out_put_seq, result_temp);
        } else if (i >= 100 && i <= 108) {
          int result_temp[] = { 1, 110 };
          test_result += BufferEqual(out_length, out_put_seq, result_temp);
        } else {
          int result_temp[] = { 0 };
          test_result += BufferEqual(out_length, out_put_seq, result_temp);
        }
      }
      TEST_RESULT(test_sample_14, test_result);
    }

    //Test 15, lost 107, 108, 109, 110;
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
    {// Test.
      int test_result = 0;
      unsigned short out_put_seq[100] = { 0 };
      int out_length = 0;
      LostPacketsRetransmiter lpr;
      for (i = 0; i < sizeof(test_sample_15) / sizeof(test_sample_15[0]); i++) {
        TEST_NO_ERROR(lpr.DetectGap(test_sample_15[i].sequence, test_sample_15[i].arrival_time_in_ms));
        TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
        if (100 > i) {
          int result_temp[] = { 0 };
          test_result += BufferEqual(out_length, out_put_seq, result_temp);
        } else if (i >= 100 && i <= 108) {
          int result_temp[] = { 4, 107, 108, 109, 110 };
          test_result += BufferEqual(out_length, out_put_seq, result_temp);
        } else {
          int result_temp[] = { 0 };
          test_result += BufferEqual(out_length, out_put_seq, result_temp);
        }
      }
      TEST_RESULT(test_sample_15, test_result);
    }

    //Test 16, lost 104-999, 1001-1199;
    const int kTestLen_16 = 10;
    short test_seq_16[kTestLen_16] = { 100, 101, 102, 103, 1000, 1200, 1201, 1202, 1203, 1204};
    Packet test_sample_16[kStratLength + kTestLen_16];
    memcpy(test_sample_16, packet_start, sizeof(packet_start));
    for (i = kStratLength; i < kStratLength + kTestLen_16; i++) {
      test_sample_16[i].arrival_time_in_ms = i;
      test_sample_16[i].sequence = test_seq_16[i - kStratLength];
      test_sample_16[i].fec_on = kFecOn;
      test_sample_16[i].continuous_on = kContinuousOn;
    }
    {// Test.
      int test_result = 0;
      unsigned short out_put_seq[100] = { 0 };
      int out_length = 0;
      LostPacketsRetransmiter lpr;
      for (i = 0; i < sizeof(test_sample_16) / sizeof(test_sample_16[0]); i++) {
        TEST_NO_ERROR(lpr.DetectGap(test_sample_16[i].sequence, test_sample_16[i].arrival_time_in_ms));
        TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
        if (104 > i) {
          int result_temp[] = { 0 };
          test_result += BufferEqual(out_length, out_put_seq, result_temp);
        } else if(104 == i){
          int k;
          int result_temp2[kMaxRetransmitBufferLength] = { 0 };
          for (k = 0; k < kMaxRetransmitBufferLength; k++) {
            result_temp2[k] = test_sample_16[i].sequence - kMaxRetransmitBufferLength + k;
          }
          int result_temp[kMaxRetransmitBufferLength + 1] = {0};
          result_temp[0] = { kMaxRetransmitBufferLength };
          memcpy(result_temp + 1, result_temp2, sizeof(result_temp2));
          test_result += BufferEqual(out_length, out_put_seq, result_temp);
        } else {
          int k;
          int result_temp2[kMaxRetransmitBufferLength] = { 0 };
          for (k = 0; k < kMaxRetransmitBufferLength; k++) {
            result_temp2[k] = test_sample_16[105].sequence - kMaxRetransmitBufferLength + k;
          }
          int result_temp[kMaxRetransmitBufferLength + 1] = { 0 };
          result_temp[0] = { kMaxRetransmitBufferLength };
          memcpy(result_temp + 1, result_temp2, sizeof(result_temp2));
          test_result += BufferEqual(out_length, out_put_seq, result_temp);
        }
      }
      TEST_RESULT(test_sample_16, test_result);
    }


    //Test 17, start at 65436-65535, 2-11;
    const int kTestLen_17 = 110;
    const int kResidue_17 = 10;
    Packet test_sample_17[kTestLen_17];
    for (i = 0; i < kTestLen_17 - kResidue_17; i++) {
      test_sample_17[i].arrival_time_in_ms = i;
      test_sample_17[i].sequence = 65535 - kTestLen_17 + i + 1 + kResidue_17;
      test_sample_17[i].fec_on = kFecOn;
      test_sample_17[i].continuous_on = kContinuousOn;
    }
    int index_17 = 2;
    for (i = kTestLen_17 - kResidue_17; i < kTestLen_17; i++) {
      test_sample_17[i].arrival_time_in_ms = i;
      test_sample_17[i].sequence = index_17;
      test_sample_17[i].fec_on = kFecOn;
      test_sample_17[i].continuous_on = kContinuousOn;
      index_17++;
    }

    {// Test.
      int test_result = 0;
      unsigned short out_put_seq[100] = { 0 };
      int out_length = 0;
      LostPacketsRetransmiter lpr;
      for (i = 0; i < sizeof(test_sample_17) / sizeof(test_sample_17[0]); i++) {
        TEST_NO_ERROR(lpr.DetectGap(test_sample_17[i].sequence, test_sample_17[i].arrival_time_in_ms));
        TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
        int result_temp[] = { 0 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      }
      TEST_RESULT(test_sample_17, test_result);
    }

    //Test 18, none, 65533, 2, 65534, 65535, 3, - 11;
    const int kTestLen_18 = 110;
    const int kResidue_18 = 10;
    Packet test_sample_18[kTestLen_18];
    for (i = 0; i < kTestLen_18 - kResidue_18; i++) {
      test_sample_18[i].arrival_time_in_ms = i;
      test_sample_18[i].sequence = 65535 - kTestLen_18 + i + 1 + kResidue_18;
      test_sample_18[i].fec_on = kFecOn;
      test_sample_18[i].continuous_on = kContinuousOn;
    }
    int index_18 = 2;
    for (i = kTestLen_18 - kResidue_18; i < kTestLen_18; i++) {
      test_sample_18[i].arrival_time_in_ms = i;
      test_sample_18[i].sequence = index_18;
      test_sample_18[i].fec_on = kFecOn;
      test_sample_18[i].continuous_on = kContinuousOn;
      index_18++;
    }
    int tmp_exchange_18 = 0;
    tmp_exchange_18 = test_sample_18[98].sequence;
    test_sample_18[98].sequence = test_sample_18[100].sequence;
    test_sample_18[100].sequence = tmp_exchange_18;

    tmp_exchange_18 = test_sample_18[99].sequence;
    test_sample_18[99].sequence = test_sample_18[100].sequence;
    test_sample_18[100].sequence = tmp_exchange_18;

    {// Test.
      int test_result = 0;
      unsigned short out_put_seq[100] = { 0 };
      int out_length = 0;
      LostPacketsRetransmiter lpr;
      for (i = 0; i < sizeof(test_sample_18) / sizeof(test_sample_18[0]); i++) {
        TEST_NO_ERROR(lpr.DetectGap(test_sample_18[i].sequence, test_sample_18[i].arrival_time_in_ms));
        TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
        int result_temp[] = { 0 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      }
      TEST_RESULT(test_sample_18, test_result);
    }

    //Test 19, none, 65530 2 65534 3 4 65535 5 - 9;
    const int kTestLen_19 = 120;
    const int kResidue_19 = 10;
    Packet test_sample_19[kTestLen_19];
    for (i = 0; i < kTestLen_19 - kResidue_19; i++) {
      test_sample_19[i].arrival_time_in_ms = i;
      test_sample_19[i].sequence = 65530 - kTestLen_19 + i + 1 + kResidue_19;
      test_sample_19[i].fec_on = kFecOn;
      test_sample_19[i].continuous_on = kContinuousOn;
    }

    test_sample_19[110].arrival_time_in_ms = 110;
    test_sample_19[110].sequence = 2;
    test_sample_19[110].fec_on = kFecOn;
    test_sample_19[110].continuous_on = kContinuousOn;

    test_sample_19[111].arrival_time_in_ms = 111;
    test_sample_19[111].sequence = 65534;
    test_sample_19[111].fec_on = kFecOn;
    test_sample_19[111].continuous_on = kContinuousOn;

    test_sample_19[112].arrival_time_in_ms = 112;
    test_sample_19[112].sequence = 3;
    test_sample_19[112].fec_on = kFecOn;
    test_sample_19[112].continuous_on = kContinuousOn;

    test_sample_19[113].arrival_time_in_ms = 113;
    test_sample_19[113].sequence = 4;
    test_sample_19[113].fec_on = kFecOn;
    test_sample_19[113].continuous_on = kContinuousOn;

    test_sample_19[114].arrival_time_in_ms = 114;
    test_sample_19[114].sequence = 65535;
    test_sample_19[114].fec_on = kFecOn;
    test_sample_19[114].continuous_on = kContinuousOn;

    for (i = 115; i < kTestLen_19; i++) {
      test_sample_19[i].arrival_time_in_ms = i;
      test_sample_19[i].sequence = kTestLen_19 - 115 + i - 115;
      test_sample_19[i].fec_on = kFecOn;
      test_sample_19[i].continuous_on = kContinuousOn;
    }
    {// Test.
      int test_result = 0;
      unsigned short out_put_seq[100] = { 0 };
      int out_length = 0;
      LostPacketsRetransmiter lpr;
      for (i = 0; i < sizeof(test_sample_19) / sizeof(test_sample_19[0]); i++) {
        TEST_NO_ERROR(lpr.DetectGap(test_sample_19[i].sequence, test_sample_19[i].arrival_time_in_ms));
        TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
        int result_temp[] = { 0 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      }
      TEST_RESULT(test_sample_19, test_result);
    }


    //Test 20, none, 65530,65534, 2, 65531, 4, 5, 3, 65532, 6 - 9;
    const int kTestLen_20 = 120;
    const int kResidue_20 = 10;
    Packet test_sample_20[kTestLen_20];
    for (i = 0; i < kTestLen_20 - kResidue_20; i++) {
      test_sample_20[i].arrival_time_in_ms = i;
      test_sample_20[i].sequence = 65530 - kTestLen_20 + i + 1 + kResidue_20;
      test_sample_20[i].fec_on = kFecOn;
      test_sample_20[i].continuous_on = kContinuousOn;
    }

    test_sample_20[110].arrival_time_in_ms = 110;
    test_sample_20[110].sequence = 65534;
    test_sample_20[110].fec_on = kFecOn;
    test_sample_20[110].continuous_on = kContinuousOn;

    test_sample_20[111].arrival_time_in_ms = 111;
    test_sample_20[111].sequence = 2;
    test_sample_20[111].fec_on = kFecOn;
    test_sample_20[111].continuous_on = kContinuousOn;

    test_sample_20[112].arrival_time_in_ms = 112;
    test_sample_20[112].sequence = 65531;
    test_sample_20[112].fec_on = kFecOn;
    test_sample_20[112].continuous_on = kContinuousOn;

    test_sample_20[113].arrival_time_in_ms = 113;
    test_sample_20[113].sequence = 4;
    test_sample_20[113].fec_on = kFecOn;
    test_sample_20[113].continuous_on = kContinuousOn;

    test_sample_20[114].arrival_time_in_ms = 114;
    test_sample_20[114].sequence = 5;
    test_sample_20[114].fec_on = kFecOn;
    test_sample_20[114].continuous_on = kContinuousOn;

    test_sample_20[115].arrival_time_in_ms = 115;
    test_sample_20[115].sequence = 3;
    test_sample_20[115].fec_on = kFecOn;
    test_sample_20[115].continuous_on = kContinuousOn;

    test_sample_20[116].arrival_time_in_ms = 116;
    test_sample_20[116].sequence = 65532;
    test_sample_20[116].fec_on = kFecOn;
    test_sample_20[116].continuous_on = kContinuousOn;

    for (i = 117; i < kTestLen_20; i++) {
      test_sample_20[i].arrival_time_in_ms = i;
      test_sample_20[i].sequence = kTestLen_20 - 115 + i - 1 - 115;
      test_sample_20[i].fec_on = kFecOn;
      test_sample_20[i].continuous_on = kContinuousOn;
    }
    {// Test.
      int test_result = 0;
      unsigned short out_put_seq[100] = { 0 };
      int out_length = 0;
      LostPacketsRetransmiter lpr;
      for (i = 0; i < sizeof(test_sample_20) / sizeof(test_sample_20[0]); i++) {
        TEST_NO_ERROR(lpr.DetectGap(test_sample_20[i].sequence, test_sample_20[i].arrival_time_in_ms));
        TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
        if (110 > i) {
          int result_temp[] = { 0 };
          test_result += BufferEqual(out_length, out_put_seq, result_temp);
        } else if (110 == i) {
          int result_temp[] = { 3, 65531, 65532, 65533 };
          test_result += BufferEqual(out_length, out_put_seq, result_temp);
        } else if (i == 111 || i == 112) {
            int result_temp[] = { 0 };
            test_result += BufferEqual(out_length, out_put_seq, result_temp);
        } else if (113 == i || i == 114) {
          int result_temp[] = { 1, 3};
          test_result += BufferEqual(out_length, out_put_seq, result_temp);
        } else if (i >= 115) {
          int result_temp[] = { 0 };
          test_result += BufferEqual(out_length, out_put_seq, result_temp);
        }

      }
      TEST_RESULT(test_sample_20, test_result);
    }

  //system("pause");

  TestTimeOutFunction();
	int tmp = 10;
	
}


int TestTimeOutFunction() {

  int i = 0;

  const char kFecOn = 0;
  const char kContinuousOn = 1;
  const int kStartTimeStamp = 70000;
  const unsigned short kStartSequence = 100;
  const int kStartPacketLength = 10000;
  const int kCheckInterval = 5;

  //const int kTimeHighBoundary = kStartTimeStamp + 
  Packet kTimeTestPackets[kStartPacketLength];
  for ( i = 0; i < kStartPacketLength; i++) {
    kTimeTestPackets[i].fec_on = kFecOn;
    kTimeTestPackets[i].continuous_on = kContinuousOn;
    kTimeTestPackets[i].sequence = i + kStartSequence;
    kTimeTestPackets[i].arrival_time_in_ms = kStartTimeStamp;
  }
  //uniform discrete distribution.
  // Test 1
  // 20 * ones(1, 100)
  // mean = 20
  // var = 0
  const int kTimeStampsLength_1 = 100;
  const int test_time_stamps_1[kTimeStampsLength_1] = { 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
    20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
    20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
    20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
    20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
    20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
    20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20 };
  Packet time_test_seq_1[kTimeStampsLength_1 + 1];
  memcpy(time_test_seq_1, kTimeTestPackets, sizeof(time_test_seq_1));
  for (i = 0; i < kTimeStampsLength_1; i++) {
    time_test_seq_1[i + 1].arrival_time_in_ms = time_test_seq_1[i].arrival_time_in_ms + test_time_stamps_1[i];
  }

  {// Test_1.
    int test_result = 0;
    unsigned short out_put_seq[100] = { 0 };
    int out_length = 0;
    LostPacketsRetransmiter lpr;
    int kArrivalTimeMean = 20;
    // Stage 1;
    for (i = 0; i < sizeof(time_test_seq_1) / sizeof(time_test_seq_1[0]); i++) {
      TEST_NO_ERROR(lpr.DetectGap(time_test_seq_1[i].sequence, time_test_seq_1[i].arrival_time_in_ms));
      TEST_NO_ERROR(lpr.DetectTimeOut(time_test_seq_1[i].arrival_time_in_ms + kCheckInterval));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 0 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    // Stage 2
    int now_time_boundary = time_test_seq_1[kTimeStampsLength_1].arrival_time_in_ms;
    int now_time = 0;
    {
      now_time = now_time_boundary + kCheckInterval;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 0 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + kArrivalTimeMean;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 0 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + kArrivalTimeMean + 1;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 0 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + 2 * kArrivalTimeMean - 1;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 0 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + 2 * kArrivalTimeMean;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 1, 201 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + 2 * kArrivalTimeMean +1;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 1,201 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + 3 * kArrivalTimeMean -1;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 1,201 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + 3 * kArrivalTimeMean;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 2, 201, 202 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + 3 * kArrivalTimeMean + 1;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 2, 201, 202 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + 4 * kArrivalTimeMean - 1;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 2, 201, 202 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + kArrivalTimeMean + 10* kArrivalTimeMean;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = {10, 102, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }

    TEST_RESULT(time_test_seq_1, test_result);
  }

  // Test 2
  // round(10:0.1 : 30 - 0.1)
  // var =  33.8283
  // mean = 19.9000
  const int kTimeStampsLength_2 = 100;
  const int test_time_stamps_2[kTimeStampsLength_2] = { 10, 10, 10, 11, 11, 11, 11, 11, 12, 12,
    12, 12, 12, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 15, 15, 15, 15,
    15, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 19,
    19, 19, 19, 19, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 22, 22, 22,
    22, 22, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25,
    26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 29, 29,
    29, 29, 29, 30, 30 };
  Packet time_test_seq_2[kTimeStampsLength_2 + 1];
  memcpy(time_test_seq_2, kTimeTestPackets, sizeof(time_test_seq_2));
  for (i = 0; i < kTimeStampsLength_2; i++) {
    time_test_seq_2[i + 1].arrival_time_in_ms = time_test_seq_2[i].arrival_time_in_ms + test_time_stamps_2[i];
  }
  {// Test_2.
    int test_result = 0;
    unsigned short out_put_seq[100] = { 0 };
    int out_length = 0;
    LostPacketsRetransmiter lpr;
    int kArrivalTimeMean = 20;
    // Stage 1;
    for (i = 0; i < sizeof(time_test_seq_2) / sizeof(time_test_seq_2[0]); i++) {
      TEST_NO_ERROR(lpr.DetectGap(time_test_seq_2[i].sequence, time_test_seq_2[i].arrival_time_in_ms));
      TEST_NO_ERROR(lpr.DetectTimeOut(time_test_seq_2[i].arrival_time_in_ms + kCheckInterval));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 0 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    // Stage 2
    int now_time_boundary = time_test_seq_2[kTimeStampsLength_2].arrival_time_in_ms;
    int now_time = 0;
    {
      now_time = now_time_boundary + kCheckInterval;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 0 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + kArrivalTimeMean;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 0 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + kArrivalTimeMean + 1;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 0 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + 2 * kArrivalTimeMean - 1;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 0 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + 2 * kArrivalTimeMean;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 1, 201 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + 2 * kArrivalTimeMean + 1;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 1,201 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + 3 * kArrivalTimeMean - 1;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 1,201 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + 3 * kArrivalTimeMean;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 2, 201, 202 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + 3 * kArrivalTimeMean + 1;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 2, 201, 202 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + 4 * kArrivalTimeMean - 1;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 2, 201, 202 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + kArrivalTimeMean + 10 * kArrivalTimeMean;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 10, 102, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }

    TEST_RESULT(time_test_seq_2, test_result);
  }


  // Test 3
  // randi([10, 30], 1, 200);
  // mean = 19.755;
  // var = 38.045;
  const int kTimeStampsLength_3 = 200;
  const int test_time_stamps_3[kTimeStampsLength_3] = { 19, 16, 14, 13, 28, 11, 30, 24, 10, 10, 27,
    19, 27, 14, 26, 15, 30, 29, 21, 29, 23, 26, 21, 14, 29, 20, 30, 17, 14,
    25, 16, 12, 28, 24, 29, 18, 27, 26, 17, 29, 16, 13, 19, 15, 10, 22, 23,
    23, 24, 27, 10, 24, 21, 24, 28, 15, 11, 16, 26, 20, 19, 13, 15, 24, 27,
    24, 22, 30, 19, 20, 15, 23, 18, 16, 22, 26, 30, 26, 22, 19, 13, 14, 24,
    30, 23, 28, 14, 24, 16, 10, 27, 19, 11, 29, 18, 22, 15, 23, 17, 20, 18,
    17, 15, 26, 15, 30, 29, 15, 14, 15, 15, 15, 18, 27, 22, 14, 26, 30, 13,
    27, 15, 18, 16, 19, 16, 12, 24, 11, 14, 20, 15, 18, 22, 11, 28, 28, 11,
    13, 10, 22, 24, 25, 14, 30, 20, 11, 13, 17, 28, 10, 17, 23, 27, 10, 26,
    11, 16, 15, 29, 22, 20, 25, 12, 28, 21, 10, 13, 15, 10, 24, 10, 20, 20,
    15, 22, 16, 29, 10, 21, 17, 17, 10, 24, 21, 26, 26, 15, 14, 17, 25, 12,
    19, 10, 11, 24, 26, 29, 29, 23, 12 };
  Packet time_test_seq_3[kTimeStampsLength_3 + 1];
  memcpy(time_test_seq_3, kTimeTestPackets, sizeof(time_test_seq_3));
  for (i = 0; i < kTimeStampsLength_3; i++) {
    time_test_seq_3[i + 1].arrival_time_in_ms = time_test_seq_3[i].arrival_time_in_ms + test_time_stamps_3[i];
  }

  {// Test_3.
    int test_result = 0;
    unsigned short out_put_seq[100] = { 0 };
    int out_length = 0;
    LostPacketsRetransmiter lpr;
    int kArrivalTimeMean = 20;
    // Stage 1;
    for (i = 0; i < sizeof(time_test_seq_3) / sizeof(time_test_seq_3[0]); i++) {
      TEST_NO_ERROR(lpr.DetectGap(time_test_seq_3[i].sequence, time_test_seq_3[i].arrival_time_in_ms));
      TEST_NO_ERROR(lpr.DetectTimeOut(time_test_seq_3[i].arrival_time_in_ms + kCheckInterval));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 0 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    // Stage 2
    int now_time_boundary = time_test_seq_3[kTimeStampsLength_3].arrival_time_in_ms;
    int now_time = 0;
    {
      now_time = now_time_boundary + kCheckInterval;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 0 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + kArrivalTimeMean;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 0 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + kArrivalTimeMean + 1;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 0 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + 2 * kArrivalTimeMean - 1;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 0 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + 2 * kArrivalTimeMean;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 1, 301 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + 2 * kArrivalTimeMean + 1;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 1,301 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + 3 * kArrivalTimeMean - 1;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 1,301 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + 3 * kArrivalTimeMean;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 2, 301, 302 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + 3 * kArrivalTimeMean + 1;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 2, 301, 302 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + 4 * kArrivalTimeMean - 1;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 2, 301, 302 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + kArrivalTimeMean + 10 * kArrivalTimeMean;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 10, 301, 302, 303, 304, 305, 306, 307, 308, 309, 310 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }

    TEST_RESULT(time_test_seq_3, test_result);
  }


  // Test 4
  // round(20 + 5.*randn(1, 200));
  // mean = 19.9600;
  // var = 24.4406;
  const int kTimeStampsLength_4 = 200;
  const int test_time_stamps_4[kTimeStampsLength_4] = { 7, 21, 20, 20, 23, 29, 20, 14, 19, 16, 18, 22,
    13, 18, 22, 22, 23, 19, 20, 20, 15, 13, 16, 21, 23, 11, 16, 11, 32, 28,
    16, 26, 32, 23, 9, 14, 22, 24, 22, 22, 17, 12, 13, 27, 22, 25, 12, 20,
    21, 14, 20, 28, 22, 22, 27, 13, 23, 20, 20, 19, 19, 14, 19, 12, 18, 11,
    25, 26, 29, 21, 20, 24, 12, 17, 27, 17, 15, 25, 15, 27, 20, 25, 18, 17,
    17, 13, 22, 18, 29, 24, 22, 24, 16, 15, 22, 24, 18, 23, 13, 21, 21, 21,
    20, 20, 21, 15, 12, 21, 18, 23, 21, 25, 24, 21, 22, 31, 10, 23, 27, 13,
    16, 17, 20, 24, 22, 22, 20, 11, 21, 15, 18, 25, 26, 12, 24, 11, 17, 16,
    22, 19, 16, 19, 22, 23, 26, 22, 16, 19, 22, 12, 20, 24, 22, 26, 21, 20,
    29, 26, 21, 20, 13, 20, 20, 27, 27, 18, 17, 21, 27, 19, 17, 16, 18, 20,
    18, 20, 20, 21, 28, 28, 15, 24, 18, 9, 15, 24, 24, 23, 25, 22, 17, 27,
    15, 17, 21, 27, 17, 19, 24, 8 };
  Packet time_test_seq_4[kTimeStampsLength_4 + 1];
  memcpy(time_test_seq_4, kTimeTestPackets, sizeof(time_test_seq_4));
  for (i = 0; i < kTimeStampsLength_4; i++) {
    time_test_seq_4[i + 1].arrival_time_in_ms = time_test_seq_4[i].arrival_time_in_ms + test_time_stamps_4[i];
  }
  {// Test_4.
    int test_result = 0;
    unsigned short out_put_seq[100] = { 0 };
    int out_length = 0;
    LostPacketsRetransmiter lpr;
    int kArrivalTimeMean = 20;
    // Stage 1;
    for (i = 0; i < sizeof(time_test_seq_4) / sizeof(time_test_seq_4[0]); i++) {
      TEST_NO_ERROR(lpr.DetectGap(time_test_seq_4[i].sequence, time_test_seq_4[i].arrival_time_in_ms));
      TEST_NO_ERROR(lpr.DetectTimeOut(time_test_seq_4[i].arrival_time_in_ms + kCheckInterval));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 0 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    // Stage 2
    int now_time_boundary = time_test_seq_4[kTimeStampsLength_4].arrival_time_in_ms;
    int now_time = 0;
    {
      now_time = now_time_boundary + kCheckInterval;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 0 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + kArrivalTimeMean;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 0 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + kArrivalTimeMean + 1;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 0 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + 2 * kArrivalTimeMean - 1;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 0 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + 2 * kArrivalTimeMean;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 1, 301 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + 2 * kArrivalTimeMean + 1;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 1,301 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + 3 * kArrivalTimeMean - 1;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 1,301 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + 3 * kArrivalTimeMean;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 2, 301, 302 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + 3 * kArrivalTimeMean + 1;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 2, 301, 302 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + 4 * kArrivalTimeMean - 1;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 2, 301, 302 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + kArrivalTimeMean + 10 * kArrivalTimeMean;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 10, 301, 302, 303, 304, 305, 306, 307, 308, 309, 310 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }

    TEST_RESULT(time_test_seq_4, test_result);
  }


  // Test 5
  // round(20 + 2.*randn(1, 200));
  // mean = 20.1000;
  // var = 4.0402;
  const int kTimeStampsLength_5 = 200;
  const int test_time_stamps_5[kTimeStampsLength_5] = { 22, 21, 19, 23, 18, 23, 21, 19, 21, 18, 20, 23, 21,
    20, 24, 21, 24, 20, 21, 16, 24, 20, 18, 20, 17, 24, 19, 21, 16, 19, 18, 19, 19, 21, 21, 23, 22, 23, 17,
    22, 19, 21, 19, 22, 21, 18, 18, 22, 19, 22, 22, 19, 20, 22, 23, 18, 20, 19, 19, 20, 24, 20, 21, 19, 19,
    19, 22, 21, 21, 19, 18, 16, 19, 22, 18, 22, 20, 15, 22, 20, 20, 21, 22, 17, 20, 15, 21, 19, 19, 19, 17,
    22, 19, 22, 17, 20, 22, 19, 22, 21, 20, 20, 21, 21, 21, 22, 19, 21, 23, 21, 21, 24, 23, 19, 17, 19, 24,
    19, 20, 21, 21, 17, 17, 20, 20, 20, 18, 18, 16, 18, 20, 22, 19, 18, 20, 19, 20, 22, 20, 22, 20, 18, 21,
    22, 18, 21, 21, 25, 19, 18, 18, 20, 18, 23, 22, 21, 16, 18, 20, 23, 25, 21, 19, 20, 18, 22, 19, 21, 19,
    20, 21, 21, 22, 20, 19, 18, 21, 17, 18, 19, 19, 20, 20, 20, 22, 19, 16, 25, 17, 20, 19, 22, 24, 18, 20,
    22, 20, 19, 20, 21 };
  Packet time_test_seq_5[kTimeStampsLength_5 + 1];
  memcpy(time_test_seq_5, kTimeTestPackets, sizeof(time_test_seq_5));
  for (i = 0; i < kTimeStampsLength_5; i++) {
    time_test_seq_5[i + 1].arrival_time_in_ms = time_test_seq_5[i].arrival_time_in_ms + test_time_stamps_5[i];
  }

  {// Test_5.
    int test_result = 0;
    unsigned short out_put_seq[100] = { 0 };
    int out_length = 0;
    LostPacketsRetransmiter lpr;
    int kArrivalTimeMean = 20;
    // Stage 1;
    for (i = 0; i < sizeof(time_test_seq_5) / sizeof(time_test_seq_5[0]); i++) {
      TEST_NO_ERROR(lpr.DetectGap(time_test_seq_5[i].sequence, time_test_seq_5[i].arrival_time_in_ms));
      TEST_NO_ERROR(lpr.DetectTimeOut(time_test_seq_5[i].arrival_time_in_ms + kCheckInterval));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 0 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    // Stage 2
    int now_time_boundary = time_test_seq_5[kTimeStampsLength_5].arrival_time_in_ms;
    int now_time = 0;
    {
      now_time = now_time_boundary + kCheckInterval;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 0 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + kArrivalTimeMean;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 0 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + kArrivalTimeMean + 1;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 0 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + 2 * kArrivalTimeMean - 1;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 0 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + 2 * kArrivalTimeMean;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 1, 301 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + 2 * kArrivalTimeMean + 1;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 1,301 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + 3 * kArrivalTimeMean - 1;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 1,301 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + 3 * kArrivalTimeMean;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 2, 301, 302 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + 3 * kArrivalTimeMean + 1;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 2, 301, 302 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + 4 * kArrivalTimeMean - 1;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 2, 301, 302 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + kArrivalTimeMean + 10 * kArrivalTimeMean;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 10, 301, 302, 303, 304, 305, 306, 307, 308, 309, 310 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }

    TEST_RESULT(time_test_seq_5, test_result);
  }




  // Test 6
  // [round(20 + 2.*randn(1, 200)), round(30 + 2.*randn(1, 200))];
  // mean(r(1:200)) = 20.1750;
  // var(r(1:200)) = 4.3059;
  // mean(r(201:400)) = 30.0850
  // var(r(201:400)) = 4.1686;
  const int kTimeStampsLength_6 = 400;
  const int test_time_stamps_6[kTimeStampsLength_6] = { 21, 18, 17, 19, 21, 20, 17, 19, 19, 22, 20, 20, 22, 22,
    20, 23, 22, 23, 22, 24, 22, 18, 20, 22, 22, 20, 20, 17, 24, 19, 20, 23, 21, 17, 25, 22, 19, 18, 19, 20, 17,
    22, 22, 19, 18, 22, 22, 19, 17, 21, 19, 19, 19, 21, 22, 21, 21, 24, 20, 18, 18, 21, 21, 19, 20, 21, 24, 22,
    22, 18, 19, 17, 20, 18, 21, 21, 24, 22, 21, 19, 21, 20, 14, 22, 19, 24, 20, 21, 19, 18, 22, 19, 21, 18, 17,
    24, 23, 22, 19, 19, 21, 21, 21, 18, 20, 19, 21, 19, 18, 18, 17, 19, 16, 18, 19, 19, 17, 20, 20, 19, 19, 18,
    20, 23, 19, 20, 20, 18, 18, 21, 21, 21, 25, 16, 21, 23, 20, 24, 22, 21, 20, 21, 20, 19, 19, 21, 18, 21, 16,
    21, 26, 22, 19, 21, 21, 21, 20, 22, 17, 21, 19, 18, 20, 24, 19, 19, 19, 23, 18, 26, 21, 15, 21, 21, 20, 20,
    20, 21, 18, 24, 20, 19, 18, 22, 17, 21, 20, 22, 21, 19, 22, 20, 20, 18, 19, 23, 17, 19, 21, 23, 28, 32, 31,
    32, 31, 28, 32, 30, 33, 32, 28, 32, 30, 28, 31, 29, 34, 30, 29, 29, 30, 28, 30, 29, 32, 28, 30, 31, 27, 27,
    33, 29, 30, 31, 31, 31, 31, 32, 29, 31, 29, 32, 32, 29, 32, 26, 27, 29, 30, 30, 35, 32, 34, 32, 26, 35, 33,
    35, 30, 32, 28, 33, 30, 27, 30, 27, 31, 31, 28, 29, 32, 32, 29, 31, 27, 30, 31, 31, 27, 29, 27, 31, 29, 30,
    31, 28, 26, 32, 29, 34, 32, 34, 28, 32, 32, 34, 31, 29, 33, 31, 29, 32, 30, 26, 27, 30, 34, 31, 31, 30, 31,
    28, 31, 33, 32, 31, 27, 31, 32, 31, 31, 31, 30, 31, 30, 31, 28, 32, 30, 27, 30, 31, 30, 33, 28, 30, 30, 29,
    29, 29, 29, 32, 30, 28, 28, 29, 30, 31, 32, 30, 30, 28, 33, 31, 31, 27, 27, 29, 27, 28, 30, 29, 28, 29, 28,
    31, 28, 29, 30, 29, 30, 28, 31, 27, 28, 31, 31, 30, 28, 24, 29, 29, 33, 25, 32, 31, 31, 30, 33, 31, 32, 29,
    27, 32, 30, 33, 30, 29, 33, 27 };
  Packet time_test_seq_6[kTimeStampsLength_6 + 1];
  memcpy(time_test_seq_6, kTimeTestPackets, sizeof(time_test_seq_6));
  for (i = 0; i < kTimeStampsLength_6; i++) {
    time_test_seq_6[i + 1].arrival_time_in_ms = time_test_seq_6[i].arrival_time_in_ms + test_time_stamps_6[i];
  }

  {// Test_6.
    int test_result = 0;
    unsigned short out_put_seq[100] = { 0 };
    int out_length = 0;
    LostPacketsRetransmiter lpr;
    int kArrivalTimeMean = 30;
    // Stage 1;
    for (i = 0; i < sizeof(time_test_seq_6) / sizeof(time_test_seq_6[0]); i++) {
      TEST_NO_ERROR(lpr.DetectGap(time_test_seq_6[i].sequence, time_test_seq_6[i].arrival_time_in_ms));
      TEST_NO_ERROR(lpr.DetectTimeOut(time_test_seq_6[i].arrival_time_in_ms + kCheckInterval));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 0 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    // Stage 2
    int now_time_boundary = time_test_seq_6[kTimeStampsLength_6].arrival_time_in_ms;
    int now_time = 0;
    {
      now_time = now_time_boundary + kCheckInterval;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 0 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + kArrivalTimeMean;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 0 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + kArrivalTimeMean + 1;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 0 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + 2 * kArrivalTimeMean - 1;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 0 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + 2 * kArrivalTimeMean;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 1, 501 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + 2 * kArrivalTimeMean + 1;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 1,501 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + 3 * kArrivalTimeMean - 1;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 1,501 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + 3 * kArrivalTimeMean;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 2, 501, 502 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + 3 * kArrivalTimeMean + 1;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 2, 501, 502 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + 4 * kArrivalTimeMean - 1;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 2, 501, 502 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }
    {
      now_time = now_time_boundary + kArrivalTimeMean + 10 * kArrivalTimeMean;
      TEST_NO_ERROR(lpr.DetectTimeOut(now_time));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      int result_temp[] = { 10, 501, 502, 503, 504, 505, 506, 507, 508, 509, 510 };
      test_result += BufferEqual(out_length, out_put_seq, result_temp);
    }

    TEST_RESULT(time_test_seq_6, test_result);
  }




  const int kStratLength = 100;
  Packet packet_start[kStratLength];
  for (i = 0; i < kStratLength; i++) {
    packet_start[i].arrival_time_in_ms = i;
    packet_start[i].sequence = i + 1;
    packet_start[i].fec_on = kFecOn;
    packet_start[i].continuous_on = kContinuousOn;
  }


  //Test 1, lost 104;
  const int kTestLen_1 = 10;
  short test_seq_1[kTestLen_1] = { 100, 101, 102, 103, 105, 106, 107, 108, 109, 110 };
  Packet test_sample_1[kStratLength + kTestLen_1];
  memcpy(test_sample_1, packet_start, sizeof(packet_start));
  for (i = kStratLength; i < kStratLength + kTestLen_1; i++) {
    test_sample_1[i].arrival_time_in_ms = i;
    test_sample_1[i].sequence = test_seq_1[i - kStratLength];
    test_sample_1[i].fec_on = kFecOn;
    test_sample_1[i].continuous_on = kContinuousOn;
  }
  {// Test.
    int test_result = 0;
    unsigned short out_put_seq[100] = { 0 };
    int out_length = 0;
    LostPacketsRetransmiter lpr;
    for (i = 0; i < sizeof(test_sample_1) / sizeof(test_sample_1[0]); i++) {
      TEST_NO_ERROR(lpr.DetectGap(test_sample_1[i].sequence, test_sample_1[i].arrival_time_in_ms));
      TEST_NO_ERROR(lpr.GetRetransmitSequences(&out_length, out_put_seq));
      if (104 > i) {
        int result_temp[] = { 0 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      }
      else {
        int result_temp[] = { 1 , 104 };
        test_result += BufferEqual(out_length, out_put_seq, result_temp);
      }
    }
    TEST_RESULT(test_sample_1, test_result);
  }
  return 0;
}