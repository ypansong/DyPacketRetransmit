#ifndef LOST_PACKETS_RESTRANSMIT_H_
#define LOST_PACKETS_RESTRANSMIT_H_

#include <list>

#ifdef _WIN32
#include <Windows.h>
#else
#include "XUtil.h"
#endif

class LostPacketsRetransmiter {

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

    int GetSequencesOutFromBuffer();

    bool ComparePacketsSeq(const PacketPair& first, const PacketPair& second);

    unsigned long GetTimestamp();
    
};

#endif // !LOST_PACKETS_RESTRANSMIT_H_
