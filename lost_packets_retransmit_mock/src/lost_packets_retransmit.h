#ifndef LOST_PACKETS_RESTRANSMIT_H_
#define LOST_PACKETS_RESTRANSMIT_H_

#include <set>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#endif
#include "media_sequence.h"

const unsigned char kRetransmitVersion = 1;

const unsigned char kMaxRetransmitCount = 10; // svn is 15
const unsigned short kMaxRetransmitBufferLength = 50;
const short kTooLarge = 50 * 150; // 15s; TODO(Chaos): Need to fix test.
const short kTooSmall = -50 * 5; // 5s;

const unsigned short kMaxSendSeqBufferLength = 50;

const unsigned short kMaxUpStreamResendElemtCount = 50;
const unsigned short kMaxaPacketLength = 512;

const unsigned long kMaxTimeWithoutPack = 15000;

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

struct SendSeqElement {
    unsigned short seq;
    char data[512];
    int dataLen;
public:
    // overloaded operator <, >, ==
    bool operator < (const SendSeqElement& d) const {
        if ((seq - d.seq) < 0) {
            return true;
        }
        return false;
    }
    bool operator > (const SendSeqElement& d) const {
        if ((seq - d.seq) > 0) {
            return true;
        }
        return false;
    }
    bool operator == (const SendSeqElement& d) const {
        if (seq == d.seq) {
            return true;
        }
        return false;
    }
    void operator = (const SendSeqElement& d) {
        seq = d.seq;
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
#ifdef _WIN32
            Sleep(1);
#else
            sleep(1);
#endif // __WIN32
            
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

// downstream retransmit
class LostPacketsRetransmiter {
private:
    std::set<RetransmitElement> mRetransmitBuffer;
    std::set<SendSeqElement> mSendSeqBuffer;
    volatile int mRequestElementNum;
    volatile int mRecvCantRetransmitNum;
    int mDisorderNum;
    int mExistedSequence;
    int mDeadElement;
    volatile int mRecvRetransmitNum;

private:
    volatile char mRetransmitLock;
    volatile bool mbIsEnable;
    char mContinuousFlag;
    char mFecFlag;
    unsigned long mRecvPacketCnt;
    int mRecvValidPackCnt;

    MediaSequence mmsLastSequence;
    MediaSequence mmsLastNormalSequence;
    MediaSequence mmsUshort;
    MediaSequence mmsIndexi;

    unsigned long mLastTimestamp;
    unsigned long mLastNormalTimestamp;
    unsigned long mStartTimestamp;
    int mRecvOrderCnt;
    float mTotalArriveModel;
    float mAvgArriveModel;
    bool mbIsDisorder;
    unsigned short mRetransmitSeq;

    char mStartedFlag;

public:

    LostPacketsRetransmiter();
    ~LostPacketsRetransmiter();

    // enanle packet retransmitter or not
    void SetEnable(bool isEnable);

    // now_sequence -- input.
    int DetectGap(unsigned short now_sequence, unsigned long now_time_stamp, bool is_recv_retransmit = false);

    // current timestamp -- input.
    int DetectTimeOut(unsigned long now_time_stamp);

    // remove seq from buffer while receive the retransmitted seq
    int RemoveSequenceFromBuffer(unsigned short target_seq);

    // requested_length -- output.
    // requested_sequences -- output.
    int GetRetransmitSequences(int * requested_length, unsigned short * requested_sequences);

    void SetCurrentPlaySeq(unsigned short seq);

    int GetStatistics(int &request_retransmit_num, int &recv_retransmit_num, int &recv_cant_retransmit_num);

    unsigned short GetProtocolSeq();

    unsigned char GetVersion() {
        return kRetransmitVersion;
    };

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

    int ResetParameters();

    int ResetBuffer();

    void PrintLog(const char* format, ...);

};

// upstream retransmit
class UpstreamPacketsRetransmitter
{
public:
    UpstreamPacketsRetransmitter();
    ~UpstreamPacketsRetransmitter();

    // enanle upstream packet retransmitter or not
    void SetEnable(bool isEnable);

    // put seq in send seq buffer
    int PutSendSeqIntoBuffer(unsigned short seq, char *data, int dataLen);

    // get seq out of send seq buffer
    int GetReSendSeqFromBuffer(unsigned short seq, char *data, int *dataLen);

    int GetReSendNum();

private:
    volatile bool mbIsEnable;

    volatile char mResendSeqLock;
    unsigned short mUpStreamNewSequence;
    unsigned char mUpStreamResendBuffer[kMaxUpStreamResendElemtCount][kMaxaPacketLength + sizeof(short) + sizeof(int)];
    unsigned short mUpStreamResendBufferIndex;
    volatile int mRecvResendNum;

    bool mbGetResendSeqFlag;
};

#endif // !LOST_PACKETS_RESTRANSMIT_H_
