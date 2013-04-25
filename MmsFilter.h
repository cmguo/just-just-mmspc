// MmsFilter.h

#ifndef _PPBOX_MMSPC_MMS_FILTER_H_
#define _PPBOX_MMSPC_MMS_FILTER_H_

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
            bool next_payload(
                boost::system::error_code & ec);

            void parse_for_time(
                ppbox::demux::Sample & sample,
                boost::system::error_code & ec);

        private:
            typedef std::pair<
                size_t, 
                ppbox::data::MemoryLock *
            > packet_memory;

            typedef util::buffers::CycleBuffers<
                std::deque<boost::asio::const_buffer>, boost::uint8_t
            > cycle_buffer_t ;

            struct PayloadParse
                : ppbox::demux::AsfParse
            {
                PayloadParse()
                    : first_packet(0)
                {
                }

                size_t first_packet;

                bool add_payload(
                    ppbox::avformat::ASF_ParseContext const & context, 
                    ppbox::avformat::ASF_PayloadHeader const & payload, 
                    cycle_buffer_t & buffer)
                {
                    bool result = ppbox::demux::AsfParse::add_payload(context, payload);
                    if (payloads().size() == 1) {
                        data_.clear();
                    }
                    data_.insert(data_.end(), buffer.rbegin(payload.PayloadLength), buffer.rend());
                    buffer.consume(payload.PayloadLength);
                    return result;
                }

                void clear(
                    std::deque<boost::asio::const_buffer> & data)
                {
                    data.swap(data_);
                    data_.clear();
                    ppbox::demux::AsfParse::clear();
                }

            private:
                std::deque<boost::asio::const_buffer> data_;
            };

        private:
            util::protocol::MmspDataHeader header_;
            std::vector<size_t> stream_map_; // Map index to AsfStream
            std::vector<ppbox::demux::AsfStream> streams_;
            std::vector<PayloadParse> parses_;

            ppbox::demux::Sample sample_;
            ppbox::demux::Sample sample2_;
            cycle_buffer_t buf_;
            ppbox::avformat::ASF_Packet packet_;
            ppbox::avformat::ASF_PayloadHeader payload_;
            ppbox::avformat::ASF_ParseContext context_;
            size_t packet_index_;
            std::deque<packet_memory> packet_memory_;
        };

    } // namespace mmspc
} // namespace ppbox

#endif // _PPBOX_MMSPC_MMS_FILTER_H_
