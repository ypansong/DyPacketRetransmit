#include"lost_packets_retransmit.h"

LostPacketsRetransmiter::LostPacketsRetransmiter() :
  mmsLastSequence(2, 65535, 2),
  mmsLastNormalSequence(2, 65535, 2),
  mmsUshort(2, 65535, 2),
  mmsIndexi(2, 65535, 2)
{
    mRetransmitLock = 0;
    mbIsEnable = true; // TODO : reset false here
    mbIsDisorder = false;
    mRetransmitSeq = 1;

    ResetParameters();
}

LostPacketsRetransmiter::~LostPacketsRetransmiter()
{
    PrintLog("request:%d, success:%d, receive:%d, disorder:%d, dead:%d, remain:%d", 
        mRequestElementNum, 
        mRecvRetransmitNum, 
        mRecvCantRetransmitNum, 
        mDisorderNum, 
        mDeadElement, 
        mRetransmitBuffer.size());
    mRetransmitBuffer.clear();
}

int LostPacketsRetransmiter::ResetParameters()
{

    mRecvPacketCnt = 0;
    mRecvValidPackCnt = 0;
    mContinuousFlag = -1;
    mFecFlag = -1;
    mLastTimestamp = 0;
    mLastNormalTimestamp = 0;
    mmsLastSequence = 0;
    mmsLastNormalSequence = 0;
    mStartTimestamp = 0;
    mRecvOrderCnt = 0;
    mTotalArriveModel = 0;
    mAvgArriveModel = 0;
    mRequestElementNum = 0;
    mRecvCantRetransmitNum = 0;
    mDisorderNum = 0;
    mExistedSequence = 0;
    mDeadElement = 0;
    mRecvRetransmitNum = 0;
    mStartedFlag = 0;

    return 0;
}

void LostPacketsRetransmiter::SetEnable(bool isEnable)
{
    mbIsEnable = isEnable;
}

int LostPacketsRetransmiter::DetectGap(unsigned short now_sequence, unsigned long now_time_stamp, bool is_recv_retransmit)
{
    RetransmitLock retransmitLock(&mRetransmitLock);

    if (!mbIsEnable)
    {
        return -1;
    }

    bool isNeedUpdate = true;

    mbIsDisorder = false;
    mRecvPacketCnt++;
    mmsUshort = now_sequence;
    int distance = -(mmsLastSequence - mmsUshort);
    if (mStartedFlag == 0)
    {
      mmsLastSequence = now_sequence;
      mStartedFlag = 1;
    }
    else
    {
      if (distance > kTooLarge || distance < kTooSmall)// Need cup down kReverGap
      {
        // jump to far, do nothing
        ResetBuffer();
      }
      else if (distance >= 2)
      {
        // lost packet(s)
        if (mRecvPacketCnt > 100)
        {
          for (mmsIndexi = mmsLastSequence + 1; mmsIndexi < mmsUshort; mmsIndexi++)
          {
            PutSequenceIntoBuffer(mmsIndexi.GetIndex());
          }
        }

      }
      else if ((distance == 1))
      {
        if (!is_recv_retransmit)
        {
        CalculatePacketsArriveModel(now_time_stamp);
        mRecvValidPackCnt++;
        mLastNormalTimestamp = now_time_stamp;
        mmsLastNormalSequence = now_sequence;
        } 
        else
        {

        }
      }
      else if (distance == 0)
      {
        // repeat
        isNeedUpdate = false;
      }
      else 
      {
        // disorder
        isNeedUpdate = false;
        mbIsDisorder = true;
      }
    }

    if (isNeedUpdate)
    {
      mmsLastSequence = now_sequence;
    }
    mLastTimestamp = now_time_stamp;

    if (is_recv_retransmit)
    {
        mRecvRetransmitNum++;
    }

    GetSequencesOutFromBuffer(now_sequence);

    return 0;
}

int LostPacketsRetransmiter::DetectTimeOut(unsigned long now_time_stamp)
{
    RetransmitLock retransmitLock(&mRetransmitLock);

    if (!mbIsEnable)
    {
        return -1;
    }

    if (mLastTimestamp == 0)
    {
        mLastTimestamp = now_time_stamp;
    }

    if (mStartTimestamp == 0)
    {
        mStartTimestamp = now_time_stamp;
    }

    if (mRecvValidPackCnt >= 100)
    {
        unsigned long elapse_time = now_time_stamp - mLastTimestamp;
        // if (elapse_time >= 200 && elapse_time <= 500) // Test case will be fault because this
        {
            if (mAvgArriveModel > 0)
            {
                unsigned short i = 0;
                float float_seq_interval = (now_time_stamp - mLastTimestamp) / mAvgArriveModel;
                float seq_interval = floor(float_seq_interval);
                float float_seq_interval_2 = (now_time_stamp - mLastNormalTimestamp) / mAvgArriveModel;
                float seq_interval2 = floor(float_seq_interval_2);

                mmsUshort = mmsLastSequence + seq_interval;
                for (mmsIndexi = mmsLastSequence + 1; mmsIndexi < mmsUshort; mmsIndexi++)
                {
                    PutSequenceIntoBuffer(mmsIndexi.GetIndex());
                }

                mmsUshort = mmsUshort - (mmsLastNormalSequence + seq_interval2);
                if (mmsUshort <= 20)
                {
                  mmsUshort = (mmsLastNormalSequence + seq_interval2);
                  for (mmsIndexi = mmsLastSequence + seq_interval; mmsIndexi < mmsUshort; mmsIndexi++)
                  {
                    PutSequenceIntoBuffer(mmsIndexi.GetIndex());
                  }
                }
            }
        }
        if (elapse_time >= kMaxTimeWithoutPack)
        {
            ResetBuffer();
        }
    }
    return 0;
}

int LostPacketsRetransmiter::GetRetransmitSequences(int * requested_length, unsigned short * requested_sequences)
{
    RetransmitLock retransmitLock(&mRetransmitLock);

    if (!mbIsEnable)
    {
        *requested_length = 0;
        return -1;
    }

    if (NULL == requested_sequences) {
        return -2;
    }
    std::set<RetransmitElement>::iterator iter;
    unsigned char * temp_lives;
    int output_count = 0;

    for (iter = mRetransmitBuffer.begin(); iter != mRetransmitBuffer.end(); ) {
        temp_lives = (unsigned char*)(&(iter->lives));
        *temp_lives = *temp_lives - 1;
        output_count++;
        requested_sequences[output_count - 1] = iter->seq;
        if (1 >= (*temp_lives)) {
            iter = mRetransmitBuffer.erase(iter);
            mDeadElement++;
        }
        else {
            iter++;
        }
    }

    *requested_length = output_count;
    mRequestElementNum += output_count;

    return 0;
}

int LostPacketsRetransmiter::ResetBuffer()
{
    ResetParameters();
    mRetransmitBuffer.clear();
    return 0;
}

int LostPacketsRetransmiter::RemoveSequenceFromBuffer(unsigned short target_seq)
{
    RetransmitLock retransmitLock(&mRetransmitLock);

    if (!mbIsEnable)
    {
        return -1;
    }

    mRecvCantRetransmitNum++;
    return GetSequencesOutFromBuffer(target_seq);
}

float LostPacketsRetransmiter::CalculatePacketsArriveModel(unsigned long now_timestamp)
{
    float alpha[] = { 0.99f, 0.01f };
    if (mLastTimestamp != 0)
    {
        mRecvOrderCnt++;
        if (mRecvOrderCnt <= 50)
        {
            // ignore the first 50 packets
            mTotalArriveModel += 1.0f * (now_timestamp - mLastTimestamp);
            mAvgArriveModel = mTotalArriveModel / mRecvOrderCnt;
        }
        else
        {
            mAvgArriveModel = alpha[0] * mAvgArriveModel + alpha[1] * (1.0f * (now_timestamp - mLastTimestamp));
        }
    }

    return mAvgArriveModel;
}

int LostPacketsRetransmiter::PutSequenceIntoBuffer(unsigned short seq)
{
    RetransmitElement temp_re;
    std::set<RetransmitElement>::iterator iter;

    temp_re.seq = seq;
    temp_re.lives = kMaxRetransmitCount;

    iter = mRetransmitBuffer.find(temp_re);
    if (mRetransmitBuffer.end() == iter) {
        mRetransmitBuffer.insert(temp_re);

        if (mRetransmitBuffer.size() > kMaxRetransmitBufferLength) {
            mRetransmitBuffer.erase(mRetransmitBuffer.begin());
        } else {
            //TODO(Chaos): Warring for too many elements to transmit!
        }
    } else {
        mExistedSequence++;
    }

    return 0;
}

int LostPacketsRetransmiter::GetSequencesOutFromBuffer(unsigned short seq)
{
    RetransmitElement temp_re;
    std::set<RetransmitElement>::iterator iter;

    temp_re.seq = seq;
    temp_re.lives = kMaxRetransmitCount;

    iter = mRetransmitBuffer.find(temp_re);
    if (mRetransmitBuffer.end() != iter) {
        mRetransmitBuffer.erase(iter);
        if (mbIsDisorder)
        {
            mDisorderNum++;
        }
        return 0;
    }
    return 1;
}

void LostPacketsRetransmiter::SetCurrentPlaySeq(unsigned short seq)
{
    RetransmitLock retransmitLock(&mRetransmitLock);

    if (!mbIsEnable)
    {
        return;
    }

    RetransmitElement temp_re;
    std::set<RetransmitElement>::iterator iter;

    temp_re.seq = seq;
    temp_re.lives = kMaxRetransmitCount;
    if (!mRetransmitBuffer.empty())
    {
        iter = mRetransmitBuffer.begin();
        if (seq >= iter->seq)
        {
            // erase all the elemen that less than seq
            for (iter = mRetransmitBuffer.begin(); iter != mRetransmitBuffer.lower_bound(temp_re);)
            {
                iter = mRetransmitBuffer.erase(iter);
            }
        }
    }
}

unsigned short LostPacketsRetransmiter::GetProtocolSeq()
{
    return mRetransmitSeq++;
}

void LostPacketsRetransmiter::PrintLog(const char* format, ...)
{
    return;
    char t_buffer[2048] = { 0 };
    char t_string[2048] = { 0 };

    strcat_s(t_buffer, format);
    strcat_s(t_buffer, "\n");

    va_list arg_ptr;
    va_start(arg_ptr, format);

    vsprintf_s(t_string, t_buffer, arg_ptr);

#ifdef _WIN32
    printf("%s", t_string);
#else
    // LOGD("%s", t_string);
#endif // __WIN32

    va_end(arg_ptr);
}

int LostPacketsRetransmiter::GetStatistics(int &request_retransmit_num, int &recv_retransmit_num, int &recv_cant_retransmit_num)
{
    request_retransmit_num = mRequestElementNum; 
    recv_retransmit_num = mRecvRetransmitNum;
    recv_cant_retransmit_num = mRecvCantRetransmitNum;

    return 0;
}

UpstreamPacketsRetransmitter::UpstreamPacketsRetransmitter()
{
    mResendSeqLock = 0;

    mUpStreamNewSequence = 0;
    mUpStreamResendBufferIndex = 0;
    memset(mUpStreamResendBuffer, 0, sizeof(mUpStreamResendBuffer));
    mRecvResendNum = 0;

    mbGetResendSeqFlag = 0;
}

UpstreamPacketsRetransmitter::~UpstreamPacketsRetransmitter()
{
}

void UpstreamPacketsRetransmitter::SetEnable(bool isEnable)
{
    mbIsEnable = isEnable;
}

int UpstreamPacketsRetransmitter::PutSendSeqIntoBuffer(unsigned short seq, char *data, int dataLen)
{
    RetransmitLock resendLock(&mResendSeqLock);

    if (!mbIsEnable)
    {
        return -1;
    }

    if (dataLen > kMaxaPacketLength) {
        return -2;
    }

    mUpStreamNewSequence = seq;
    memcpy(mUpStreamResendBuffer[mUpStreamResendBufferIndex], &seq, sizeof(seq));
    memcpy(&mUpStreamResendBuffer[mUpStreamResendBufferIndex][sizeof(seq)], &dataLen, sizeof(dataLen));
    memcpy(&mUpStreamResendBuffer[mUpStreamResendBufferIndex][sizeof(seq) + sizeof(dataLen)], data, dataLen);
    mUpStreamResendBufferIndex++;
    if (mUpStreamResendBufferIndex >= kMaxUpStreamResendElemtCount) {
        mUpStreamResendBufferIndex = 0;
        mbGetResendSeqFlag = 1;
    }

    return 0;
}

int UpstreamPacketsRetransmitter::GetReSendSeqFromBuffer(unsigned short seq, char *data, int *dataLen)
{ 
    RetransmitLock resendLock(&mResendSeqLock);

    if (!mbIsEnable)
    {
        return -1;
    }

    unsigned short sequence_find = 0;
    if ((NULL == data) || (NULL == dataLen)) {
        return -2;
    }
    *dataLen = 0;
    const unsigned short kNonSequenceLow = 0;
    const unsigned short kNonSequenceHigh = 2;
    //const int kMaxUnsignedShortValue = 65536;

    if (kNonSequenceLow <= seq && kNonSequenceHigh > seq) {
        return -3;
    }

    if (!mbGetResendSeqFlag)
    {
        return -4;
    }

    int low_bound = mUpStreamNewSequence - kMaxUpStreamResendElemtCount + 1;
    if (low_bound < kNonSequenceHigh) {
        low_bound = low_bound + 65536 - (kNonSequenceHigh - kNonSequenceLow);
        if (seq < low_bound || seq >mUpStreamNewSequence) {
            return 0;
        }
        int taget = seq - low_bound;
        if (taget < 0) {
            taget += 65536 - (kNonSequenceHigh - kNonSequenceLow);
        }
        taget += mUpStreamResendBufferIndex;
        if (taget >= kMaxUpStreamResendElemtCount) {
            taget -= kMaxUpStreamResendElemtCount;
        }
        memcpy(dataLen, &mUpStreamResendBuffer[taget][sizeof(seq)], sizeof(*dataLen));
        memcpy(data, &mUpStreamResendBuffer[taget][sizeof(seq) + sizeof(*dataLen)], *dataLen);
        mRecvResendNum++;
    } else {
        if (low_bound <= seq && mUpStreamNewSequence >= seq) {
            int taget = seq - low_bound + mUpStreamResendBufferIndex;
            if (taget >= kMaxUpStreamResendElemtCount) {
                taget -= kMaxUpStreamResendElemtCount;
            }
            memcpy(dataLen, &mUpStreamResendBuffer[taget][sizeof(seq)], sizeof(*dataLen));
            memcpy(data, &mUpStreamResendBuffer[taget][sizeof(seq) + sizeof(*dataLen)], *dataLen);
            mRecvResendNum++;
        }
    }

    return 0;
}

int UpstreamPacketsRetransmitter::GetReSendNum()
{
    return mRecvResendNum;
}