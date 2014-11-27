// MmsFilter.h

#ifndef _JUST_MMSPC_MMS_FILTER_H_
#define _JUST_MMSPC_MMS_FILTER_H_

#include <just/demux/packet/Filter.h>
#include <just/demux/basic/asf/AsfParse.h>
#include <just/demux/basic/asf/AsfStream.h>

#include <util/buffers/CycleBuffers.h>
#include <util/protocol/mmsp/MmspData.h>

namespace just
{
    namespace mmspc
    {

        class MmsFilter
            : public just::demux::Filter
        {
        public:
            MmsFilter();

            ~MmsFilter();

        public:
            virtual bool get_sample(
                just::demux::Sample & sample,
                boost::system::error_code & ec);

            virtual bool get_next_sample(
                just::demux::Sample & sample,
                boost::system::error_code & ec);

            virtual bool get_last_sample(
                just::demux::Sample & sample,
                boost::system::error_code & ec);

            virtual bool before_seek(
                just::demux::Sample & sample,
                boost::system::error_code & ec);

        public:
            bool get_header(
                boost::system::error_code & ec);

            std::vector<just::demux::AsfStream> const & streams() const
            {
                return streams_;
            }

        private:
            bool next_payload(
                boost::system::error_code & ec);

            void parse_for_time(
                just::demux::Sample & sample,
                boost::system::error_code & ec);

        private:
            typedef std::pair<
                size_t, 
                just::data::MemoryLock *
            > packet_memory;

            typedef util::buffers::CycleBuffers<
                std::deque<boost::asio::const_buffer>, boost::uint8_t
            > cycle_buffer_t ;

            struct PayloadParse
                : just::demux::AsfParse
            {
                PayloadParse()
                    : first_packet(0)
                {
                }

                size_t first_packet;

                bool add_payload(
                    just::avformat::AsfParseContext const & context, 
                    just::avformat::AsfPayloadHeader const & payload, 
                    cycle_buffer_t & buffer)
                {
                    bool result = just::demux::AsfParse::add_payload(context, payload);
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
                    just::demux::AsfParse::clear();
                }

            private:
                std::deque<boost::asio::const_buffer> data_;
            };

        private:
            util::protocol::MmspDataHeader header_;
            std::vector<size_t> stream_map_; // Map index to AsfStream
            std::vector<just::demux::AsfStream> streams_;
            std::vector<PayloadParse> parses_;

            just::demux::Sample sample_;
            just::demux::Sample sample2_;
            cycle_buffer_t buf_;
            just::avformat::AsfPacket packet_;
            just::avformat::AsfPayloadHeader payload_;
            just::avformat::AsfParseContext context_;
            size_t packet_index_;
            std::deque<packet_memory> packet_memory_;
        };

    } // namespace mmspc
} // namespace just

#endif // _JUST_MMSPC_MMS_FILTER_H_
