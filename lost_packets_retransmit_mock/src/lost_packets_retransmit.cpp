#include"lost_packets_retransmit.h"

LostPacketsRetransmiter::LostPacketsRetransmiter()
{
    mContinuousFlag = -1;
    mFecFlag = -1;
    mLastSequence = -1; 
}

int LostPacketsRetransmiter::DetectGap(unsigned short now_sequence)
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
            // disorder deal with 5 packets
            mDisorderPackets.push_back(now_sequence);
            if (mDisorderPackets.size() >= 5)
            {
                mDisorderPackets.sort();
                unsigned short first_seq = 0, second_seq = 0;
                for (std::list<unsigned short>::iterator it = mDisorderPackets.begin(); it != mDisorderPackets.end();)
                {
                    first_seq = *it;
                    if ((mLastSequence - first_seq) <= 10)
                    {
                        it++;
                        if (it != mDisorderPackets.end())
                        {
                            second_seq = *it;
                            for (unsigned short i = (first_seq + 1); i < second_seq;i++)
                            {
                                PutSequenceIntoBuffer(i);
                            }
                        }
                    }
                    it++;
                }
                mDisorderPackets.clear();
            }
        }
    }

    mLastSequence = now_sequence;

    if (isNeedInsert)
    {
        PacketPair t_pair;
        t_pair.seq = now_sequence;
        t_pair.timestamp = GetTimestamp();
        mArrivePackets.push_back(t_pair);
    }

    return 0;
}

int LostPacketsRetransmiter::DetectTimeOut(int now_time_stamp)
{
    PacketPair t_pair;
    t_pair.seq = 0;
    t_pair.timestamp = now_time_stamp;
    mArrivePackets.push_back(t_pair);
    
    return 0;
}

int LostPacketsRetransmiter::GetRetransmitSequences(int * requested_length, unsigned short * requested_sequences)
{
    return -1;
}

int LostPacketsRetransmiter::ResetSet()
{
    return -1;
}

int LostPacketsRetransmiter::RemoveSequenceFromBuffer(unsigned short target_seq)
{
    return 0;
}

unsigned long LostPacketsRetransmiter::CalculatePacketsArriveModel()
{
    unsigned short total_packets_num = 0;
    unsigned long total_time = 0;
    unsigned short first_seq = 0, second_seq = 0;
    unsigned long first_timestamp = 0, second_timestamp = 0;
    if (mLastSequence >= 100)
    {
        // TODO : Must erase mArrivePackets
        for (std::list<PacketPair>::iterator it = mArrivePackets.begin(); it != mArrivePackets.end();)
        {
            first_seq = it->seq;
            first_timestamp = it->timestamp;

            it++;
            if (it != mArrivePackets.end())
            {
                second_seq = it->seq;
                second_timestamp = it->timestamp;
                unsigned short sequence_interval = second_seq - first_seq;
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
    else
    {
        return -1;
    }
}

int LostPacketsRetransmiter::PutSequenceIntoBuffer(unsigned short seq)
{
    return -1;
}

int LostPacketsRetransmiter::GetSequencesOutFromBuffer()
{
    return -1;
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

void printLog(const char* format)
{
#ifdef _WIN32
    printf("%s\n", format);
#else
    LOGD(content);
#endif // __WIN32
}