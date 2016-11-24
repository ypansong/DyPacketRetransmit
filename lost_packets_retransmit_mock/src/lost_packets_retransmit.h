#ifndef LOST_PACKETS_RESTRANSMIT_H_
#define LOST_PACKETS_RESTRANSMIT_H_

#include <list>
#include <set>

#ifdef _WIN32
#include <Windows.h>
#else
#include "XUtil.h"
#endif
const unsigned char kMaxRetransmitCount = 10;
const unsigned short kMaxRetransmitBufferLength = 50;
const unsigned short kReverGap = 65535 / 2;

struct RetransmitElement{
  unsigned short seq;
  unsigned char lives;
public:
  // overloaded operator <, >, ==
  bool operator < (const RetransmitElement& d) const{
    if ((seq - d.seq) < 0) {
      return true;
    }
    return false;
  }
  bool operator > (const RetransmitElement& d) const{
    if ((seq - d.seq) > 0) {
      return true;
    }
    return false;
  }
  bool operator == (const RetransmitElement& d) const{
    if (seq == d.seq) {
      return true;
    }
    return false;
  }
  void operator = (const RetransmitElement& d) {
    seq = d.seq;
    lives = d.lives;
  }
};




class LostPacketsRetransmiter {
private:
  std::set<RetransmitElement> mRetransmitBuffer;
  int mExistedSequence;
  int mDeadOrReceived;
  int mDeadElement;

  
private:
    typedef struct
    {
        unsigned short seq;
        unsigned long timestamp;
    }PacketPair;
    std::list<PacketPair> mArrivePackets;
    
    char mContinuousFlag;
    char mFecFlag;
    unsigned long mRecvPacketCnt;
    unsigned short mLastSequence;
    unsigned long mLastTimestamp;
    unsigned long mStartTimestamp;

public:

    LostPacketsRetransmiter();

    // now_sequence -- input.
    int DetectGap(unsigned short now_sequence, unsigned long now_time_stamp);

    // current timestamp -- input.
    int DetectTimeOut(unsigned long now_time_stamp);

    // requested_length -- output.
    // requested_sequences -- output.
    int GetRetransmitSequences(int * requested_length, unsigned short * requested_sequences);

    void SetFecOn(int fec) {
        mFecFlag = fec == 1 ? 1 : 0;
    };

    void SetContinuousOn(int continunous) {
        mContinuousFlag = continunous == 1 ? 1 : 0;
    };

    int GetFecOn() {
        return mFecFlag;
    };

    int GetContinuousOn() {
        return mContinuousFlag;
    };

    int ResetBuffer();

    int RemoveSequenceFromBuffer(unsigned short target_seq);

private:
    unsigned long CalculatePacketsArriveModel();

    int PutSequenceIntoBuffer(unsigned short seq);

    int GetSequencesOutFromBuffer(unsigned short seq);

    bool ComparePacketsSeq(const PacketPair& first, const PacketPair& second);

    unsigned long GetTimestamp();
    
    void printLog(const char* format, ...);

};

#endif // !LOST_PACKETS_RESTRANSMIT_H_
