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
    std::list<unsigned short> mDisorderPackets;
    
    char mContinuousFlag;
    char mFecFlag;
    unsigned short mLastSequence;
    unsigned long mLastTimestamp;

public:

    LostPacketsRetransmiter();

    // now_sequence -- input.
    int DetectGap(unsigned short now_sequence);

    // now_time_stamp -- input.
    int DetectTimeOut(int now_time_stamp);

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

    int ResetSet();

    int RemoveSequenceFromBuffer(unsigned short target_seq);

private:
    unsigned long CalculatePacketsArriveModel();

    int PutSequenceIntoBuffer(unsigned short seq);

    int GetSequencesOutFromBuffer(int * requested_length, unsigned short * requested_sequences);

    bool ComparePacketsSeq(const PacketPair& first, const PacketPair& second);

    unsigned long GetTimestamp();
    
};

#endif // !LOST_PACKETS_RESTRANSMIT_H_
