#include"lost_packets_retransmit.h"

LostPacketsRetransmiter::LostPacketsRetransmiter()
{
    mContinuousFlag = -1;
    mFecFlag = -1;
    mLastSequence = -1;
    mStartTimestamp = -1;
    mExistedSequence = 0;
    mDeadOrReceived = 0;
    mDeadElement = 0;
}

int LostPacketsRetransmiter::DetectGap(unsigned short now_sequence, unsigned long now_time_stamp)
{
    bool isNeedInsert = true;

    if (mLastSequence == -1)
    {
        mLastSequence = now_sequence;
    }
    else
    {
        if ((now_sequence - mLastSequence) >= 2)
        {
            // lost packet(s)
            for (unsigned short i = mLastSequence + 1; i < now_sequence; i++)
            {
                PutSequenceIntoBuffer(i);
            }
        }
        else if ((now_sequence - mLastSequence) >= 1)
        {
            // normal, calculate arrive model
        }
        else if ((now_sequence - mLastSequence) == 0)
        {
            // repeat
            isNeedInsert = false;
        }
        else
        {
            // disorder
            isNeedInsert = false;
        }
    }

    mLastSequence = now_sequence;
    mLastTimestamp = now_time_stamp;

    if (isNeedInsert)
    {
        PacketPair t_pair;
        t_pair.seq = now_sequence;
        t_pair.timestamp = now_time_stamp;
        mArrivePackets.push_back(t_pair);
    }

    return 0;
}

int LostPacketsRetransmiter::DetectTimeOut(unsigned long now_time_stamp)
{
    if (mStartTimestamp == -1)
    {
        mStartTimestamp = now_time_stamp;
    }
    if ((now_time_stamp - mStartTimestamp) >= 2000)
    {
        unsigned long seq_interval = (now_time_stamp - mLastTimestamp) / CalculatePacketsArriveModel();
        for (unsigned short i = (mLastSequence + 1); i <= (mLastSequence + seq_interval);i++)
        {
            PutSequenceIntoBuffer(i);
        }
        return 0;
    }
    return 1;
}

int LostPacketsRetransmiter::GetRetransmitSequences(int * requested_length, unsigned short * requested_sequences)
{
    // TODO(Chaos): is GetRetransmitSequences need?
    return GetSequencesOutFromBuffer(requested_length, requested_sequences);
}

int LostPacketsRetransmiter::ResetSet()
{
    return -1;
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

unsigned long LostPacketsRetransmiter::CalculatePacketsArriveModel()
{
    unsigned short total_packets_num = 0;
    unsigned long total_time = 0;
    unsigned short first_seq = 0, second_seq = 0;
    unsigned long first_timestamp = 0, second_timestamp = 0;
    for (std::list<PacketPair>::iterator it = mArrivePackets.begin(); it != mArrivePackets.end();)
    {
        first_seq = it->seq;
        first_timestamp = it->timestamp;

        it++;
        if (it != mArrivePackets.end())
        {
            second_seq = it->seq;
            second_timestamp = it->timestamp;
            if ((second_seq - first_seq) > 0)
            {
                total_packets_num = second_seq - first_seq;
                total_time = second_timestamp - first_timestamp;
            }
        }
        it++;
    }
    return (total_time / total_packets_num);
}

int LostPacketsRetransmiter::PutSequenceIntoBuffer(unsigned short seq)
{
    RetransmitElement temp_re;
    std::set<RetransmitElement>::iterator iter;

    temp_re.seq = seq;
    temp_re.lives = kMaxRetransmitCount;

    iter = mRetransmitBuffer.find(temp_re);
    if (mRetransmitBuffer.end() != iter) {
        if (mRetransmitBuffer.size() > kMaxRetransmitBufferLength) {
            mRetransmitBuffer.erase(mRetransmitBuffer.begin());
        } else {
            //TODO(Chaos): Warring for too many elements to transmit!
        }
        mRetransmitBuffer.insert(temp_re);
    } else {
        mExistedSequence++;
    }

    return 0;
}

int LostPacketsRetransmiter::GetSequencesOutFromBuffer(int * requested_length, unsigned short * requested_sequences)
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
        if (1 <= (*temp_lives)) {
            iter = mRetransmitBuffer.erase(iter);
            mDeadElement++;
        } else {
            iter++;
        }
    }

    *requested_length = output_count;
    return 0;
}

bool LostPacketsRetransmiter::ComparePacketsSeq(const PacketPair & first, const PacketPair & second)
{
    return first.seq < second.seq;
}

unsigned long LostPacketsRetransmiter::GetTimestamp()
{
#ifdef _WIN32
    return GetTickCount();
#else
    return XGetTimestamp();
#endif
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