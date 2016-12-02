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

const unsigned char kRetransmitVersion = 0;

const unsigned char kMaxRetransmitCount = 10;
const unsigned short kMaxRetransmitBufferLength = 50;
const unsigned short kReverGap = 65535 / 2;

const unsigned short kMaxSendSeqBufferLength = 50;

const unsigned short kMaxUpStreamResendElemtCount = 50;
const unsigned short kMaxaPacketLength = 512;

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

class LostPacketsRetransmiter {
private:
    std::set<RetransmitElement> mRetransmitBuffer;
    std::set<SendSeqElement> mSendSeqBuffer;
    int mRequestElementNum;
    int mReceiveElementNum;
    int mDisorderNum;
    int mExistedSequence;
    int mDeadElement;

private:
    volatile char mRetransmitLock;
    volatile char mResendSeqLock;
    bool mbIsEnable;
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
    unsigned short mRetransmitSeq;
    unsigned char mUpStreamResendBuffer[kMaxUpStreamResendElemtCount][kMaxaPacketLength + sizeof(short) + sizeof (int)];
    unsigned short mUpStreamResendBufferIndex;
    unsigned short mUpStreamNewSequence;
public:

    LostPacketsRetransmiter();
    ~LostPacketsRetransmiter();

    // enanle packet retransmitter or not
    void SetEnable(bool isEnable);

    // now_sequence -- input.
    int DetectGap(unsigned short now_sequence, unsigned long now_time_stamp);

    // current timestamp -- input.
    int DetectTimeOut(unsigned long now_time_stamp);

    // remove seq from buffer while receive the retransmitted seq
    int RemoveSequenceFromBuffer(unsigned short target_seq);

    // requested_length -- output.
    // requested_sequences -- output.
    int GetRetransmitSequences(int * requested_length, unsigned short * requested_sequences);

    // put seq in send seq buffer
    int PutSendSeqIntoBuffer(unsigned short seq, char *data, int dataLen);

    // get seq out of send seq buffer
    int GetReSendSeqFromBuffer(unsigned short seq, char *data, int *dataLen);

    // put seq in send seq buffer
    int PutSendSeqIntoBuffer2(unsigned short seq, unsigned char *data, int dataLen);

    // get seq out of send seq buffer
    int GetReSendSeqFromBuffer2(unsigned short seq, unsigned char *data, int *dataLen);

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

    int ResetBuffer();

    void PrintLog(const char* format, ...);

};

#endif // !LOST_PACKETS_RESTRANSMIT_H_
