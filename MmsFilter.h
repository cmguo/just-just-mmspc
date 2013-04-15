// MmsFilter.h

#ifndef _PPBOX_MMSPC_RTM_FILTER_H_
#define _PPBOX_MMSPC_RTM_FILTER_H_

#include <ppbox/demux/packet/Filter.h>
#include <ppbox/demux/basic/asf/AsfParse.h>
#include <ppbox/demux/basic/asf/AsfStream.h>

#include <util/buffers/CycleBuffers.h>
#include <util/protocol/mmsp/MmspData.h>

namespace ppbox
{
    namespace mmspc
    {

        class MmsFilter
            : public ppbox::demux::Filter
        {
        public:
            MmsFilter();

            ~MmsFilter();

        public:
            virtual bool get_sample(
                ppbox::demux::Sample & sample,
                boost::system::error_code & ec);

            virtual bool get_next_sample(
                ppbox::demux::Sample & sample,
                boost::system::error_code & ec);

            virtual bool get_last_sample(
                ppbox::demux::Sample & sample,
                boost::system::error_code & ec);

            virtual bool before_seek(
                ppbox::demux::Sample & sample,
                boost::system::error_code & ec);

        public:
            bool get_header(
                boost::system::error_code & ec);

            std::vector<ppbox::demux::AsfStream> const & streams() const
            {
                return streams_;
            }

        private:
            void parse_for_time(
                ppbox::demux::Sample & sample,
                boost::system::error_code & ec);

        private:
            util::protocol::MmspDataHeader header_;
            std::vector<ppbox::demux::AsfStream> streams_;
            std::vector<size_t> stream_map_; // Map index to AsfStream
            ppbox::demux::AsfParse parse_;
            ppbox::avformat::ASF_Packet packet_;
            ppbox::avformat::ASF_PayloadHeader payload_;
            ppbox::avformat::ASF_ParseContext context_;
            bool is_save_sample_;
            ppbox::demux::Sample sample_;
            ppbox::demux::Sample sample2_;
            typedef util::buffers::CycleBuffers<
                std::deque<boost::asio::const_buffer>, boost::uint8_t
            > cycle_buffer_t ;
            cycle_buffer_t buf_;
        };

    } // namespace mux
} // namespace ppbox

#endif // _PPBOX_MMSPC_RTM_FILTER_H_
