// MmsDemuxer.h

#ifndef _JUST_MMSPC_MMS_DEMUXER_H_
#define _JUST_MMSPC_MMS_DEMUXER_H_

#include "just/demux/packet/PacketDemuxer.h"

namespace just
{
    namespace mmspc
    {

        class MmsFilter;

        class MmsDemuxer
            : public just::demux::PacketDemuxer
        {
        public:
            MmsDemuxer(
                boost::asio::io_service & io_svc, 
                just::data::PacketMedia & media);

            virtual ~MmsDemuxer();

        protected:
            virtual bool check_open(
                boost::system::error_code & ec);

        private:
            MmsFilter * filter_;
        };

        JUST_REGISTER_PACKET_DEMUXER("mms", MmsDemuxer);

    } // namespace mmspc
} // namespace just

#endif // _JUST_MMSPC_MMS_DEMUXER_H_
