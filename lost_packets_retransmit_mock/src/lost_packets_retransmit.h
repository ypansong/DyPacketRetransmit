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

struct RetransmitElement {
    unsigned short seq;
    unsigned char lives;
public:
    // overloaded operator <, >, ==
    bool operator < (const RetransmitElement& d) const {
        if ((seq - d.seq) < 0) {
            return true;
        }
        return false;
    }
    bool operator > (const RetransmitElement& d) const {
        if ((seq - d.seq) > 0) {
            return true;
        }
        return false;
    }
    bool operator == (const RetransmitElement& d) const {
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

class RetransmitLock
{
public:
    explicit RetransmitLock(volatile char* lock)
        :mLock(lock)
    {
        while (*mLock)
        {
            Sleep(1);
        }
        *mLock = 1;
    };

    explicit RetransmitLock(volatile char& lock)
        :mLock(&lock)
    {
        while (*mLock)
        {
            Sleep(1);
        }
        *mLock = 1;
    };

    ~RetransmitLock()
    {
        if (*mLock)
        {
            *mLock = 0;
        }
    };

private:
    volatile char* mLock;
};

class LostPacketsRetransmiter {
private:
    std::set<RetransmitElement> mRetransmitBuffer;
    int mRequestElementNum;
    int mReceiveElementNum;
    int mDisorderNum;
    int mExistedSequence;
    int mDeadElement;

private:
    volatile char mRetransmitLock;
    char mContinuousFlag;
    char mFecFlag;
    unsigned long mRecvPacketCnt;
    unsigned short mLastSequence;
    unsigned long mLastTimestamp;
    unsigned long mStartTimestamp;
    int mRecvOrderCnt;
    float mTotalArriveModel;
    float mAvgArriveModel;
    bool mbIsDisorder;
public:

    LostPacketsRetransmiter();
    ~LostPacketsRetransmiter();

    // now_sequence -- input.
    int DetectGap(unsigned short now_sequence, unsigned long now_time_stamp);

    // current timestamp -- input.
    int DetectTimeOut(unsigned long now_time_stamp);

    // remove seq from buffer while receive the retransmitted seq
    int RemoveSequenceFromBuffer(unsigned short target_seq);

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

private:
    float CalculatePacketsArriveModel(unsigned long now_timestamp);

    int PutSequenceIntoBuffer(unsigned short seq);

    int GetSequencesOutFromBuffer(unsigned short seq);

    int ResetBuffer();

    void PrintLog(const char* format, ...);

};

#endif // !LOST_PACKETS_RESTRANSMIT_H_
