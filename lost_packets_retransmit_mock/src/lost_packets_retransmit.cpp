#include"lost_packets_retransmit.h"

LostPacketsRetransmiter::LostPacketsRetransmiter()
{
    mContinuousFlag = -1;
    mFecFlag = -1;
    mRecvPacketCnt = 0;
    mLastSequence = 0;
    mLastTimestamp = 0;
    mStartTimestamp = 0;
    mRecvOrderCnt = 0;
    mTotalArriveModel = 0;
    mAvgArriveModel = 0;
    mExistedSequence = 0;
    mDeadOrReceived = 0;
    mDeadElement = 0;
}

int LostPacketsRetransmiter::DetectGap(unsigned short now_sequence, unsigned long now_time_stamp)
{
    bool isNeedUpdate = true;
    mRecvPacketCnt++;

    if (mLastSequence == 0)
    {
        mLastSequence = now_sequence;
    }
    else
    {
        if ((now_sequence - mLastSequence) > kReverGap)
        {
            // jump to far, do nothing
            isNeedUpdate = false;
        }
        else if ((now_sequence - mLastSequence) >= 2)
        {
            // lost packet(s)
            for (unsigned short i = mLastSequence + 1; i < now_sequence; i++)
            {
                if (mRecvPacketCnt > 100)
                {
                    PutSequenceIntoBuffer(i);
                }
            }
        }
        else if ((now_sequence - mLastSequence) == 1)
        {
            CalculatePacketsArriveModel(now_time_stamp);
        }
        else if ((now_sequence - mLastSequence) == 0)
        {
            // repeat
            isNeedUpdate = false;
        }
        else if ((now_sequence - mLastSequence) <= (-kReverGap))
        {
            // turnover, reset the buffer
            ResetBuffer();
        }
        else
        {
            // normal, disorder
            isNeedUpdate = false;
        }
    }

    if (isNeedUpdate)
    {
        mLastSequence = now_sequence;
    }

    GetSequencesOutFromBuffer(now_sequence);

    mLastTimestamp = now_time_stamp;

    return 0;
}

int LostPacketsRetransmiter::DetectTimeOut(unsigned long now_time_stamp)
{
    if (mStartTimestamp == 0)
    {
        mStartTimestamp = now_time_stamp;
    }
    if ((now_time_stamp - mStartTimestamp) >= 2000)
    {
        if (mAvgArriveModel > 0)
        {
            float float_seq_interval = (now_time_stamp - mLastTimestamp) / mAvgArriveModel;
            float seq_interval = floor(float_seq_interval);
            for (unsigned short i = (mLastSequence + 1); i < (mLastSequence + seq_interval);i++)
            {
                PutSequenceIntoBuffer(i);
            }
            return 0;
        }
    }
    return 0;
}

int LostPacketsRetransmiter::GetRetransmitSequences(int * requested_length, unsigned short * requested_sequences)
{
    if (NULL == requested_sequences) {
        return -1;
    }
    std::set<RetransmitElement>::iterator iter;
    unsigned char * temp_lives;
    int output_count = 0;

    for (iter = mRetransmitBuffer.begin(); iter != mRetransmitBuffer.end(); ) {
        temp_lives = &((unsigned char)iter->lives);
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
    return 0;
}

int LostPacketsRetransmiter::ResetBuffer()
{
    mAvgArriveModel = 0;
    mRetransmitBuffer.clear();
    return 0;
}

int LostPacketsRetransmiter::RemoveSequenceFromBuffer(unsigned short target_seq)
{
    RetransmitElement temp_re;
    std::set<RetransmitElement>::iterator iter;
    temp_re.seq = target_seq;
    iter = mRetransmitBuffer.find(temp_re);
    if (mRetransmitBuffer.end() != iter) {
        mRetransmitBuffer.erase(iter);
    } else {
        mDeadOrReceived++;
    }
    return 0;
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
        // calculate what we erase
        return 0;
    }
    return 1;
}

void LostPacketsRetransmiter::printLog(const char* format, ...)
{
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
    LOGD("%s", t_string);
#endif // __WIN32

    va_end(arg_ptr);
}