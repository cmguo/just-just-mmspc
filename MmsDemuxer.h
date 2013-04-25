// MmsDemuxer.h

#ifndef _PPBOX_MMSPC_MMS_DEMUXER_H_
#define _PPBOX_MMSPC_MMS_DEMUXER_H_

#include "ppbox/demux/packet/PacketDemuxer.h"

namespace ppbox
{
    namespace mmspc
    {

        class MmsFilter;

        class MmsDemuxer
            : public ppbox::demux::PacketDemuxer
        {
        public:
            MmsDemuxer(
                boost::asio::io_service & io_svc, 
                ppbox::data::PacketMedia & media);

            virtual ~MmsDemuxer();

        protected:
            virtual bool check_open(
                boost::system::error_code & ec);

        private:
            MmsFilter * filter_;
        };

        PPBOX_REGISTER_PACKET_DEMUXER("mms", MmsDemuxer);

    } // namespace mmspc
} // namespace ppbox

#endif // _PPBOX_MMSPC_MMS_DEMUXER_H_
