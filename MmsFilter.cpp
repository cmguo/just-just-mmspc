// MmsFilter.cpp

#include "just/mmspc/Common.h"
#include "just/mmspc/MmsFilter.h"

#include <just/demux/base/DemuxError.h>
using namespace just::demux;

using namespace just::avformat;
using namespace just::avformat::error;

#include <just/avbase/stream/SampleBuffers.h>
using namespace just::avbase;

#include <util/archive/ArchiveBuffer.h>
#include <util/protocol/mmsp/MmspData.h>
using namespace util::protocol;

namespace just
{
    namespace mmspc
    {

        MmsFilter::MmsFilter()
            : packet_index_(0)
        {
            context_.packet = &packet_;
        }

        MmsFilter::~MmsFilter()
        {
        }

        bool MmsFilter::get_sample(
            Sample & sample,
            boost::system::error_code & ec)
        {
            while (next_payload(ec)) {
                if (payload_.StreamNum >= stream_map_.size()) {
                    ec = bad_media_format;
                    return false;
                }
                size_t index = stream_map_[payload_.StreamNum];
                if (index >= streams_.size()) {
                    ec = bad_media_format;
                    return false;
                }
                PayloadParse & parse(parses_[index]);
                parse.add_payload(context_, payload_, buf_);
                if (!parse.finish()) {
                    continue;
                }
                sample.itrack = index;
                sample.flags = 0;
                if (parse.is_sync_frame())
                    sample.flags |= Sample::f_sync;
                if (parse.is_discontinuity())
                    sample.flags |= Sample::f_discontinuity;
                sample.dts = parse.dts();
                sample.cts_delta = boost::uint32_t(-1);
                sample.duration = 0;
                sample.size = parse.size();
                parse.clear(sample.data);

                for (size_t i = parse.first_packet - packet_index_; i < packet_memory_.size(); ++i) {
                    --packet_memory_[i].first;
                }
                size_t packet_index = packet_index_;
                for (size_t i = 0; i < packet_memory_.size(); ++i) {
                    if (packet_memory_[i].first == 0) {
                        ++packet_index_;
                        sample.append(packet_memory_[i].second);
                    } else {
                        break;
                    }
                }
                packet_memory_.erase(packet_memory_.begin(), packet_memory_.begin() + (packet_index_ - packet_index));
                parse.first_packet = packet_index_ + packet_memory_.size();

                break;
            }

            return true;
        }

        bool MmsFilter::get_header(
            boost::system::error_code & ec)
        {
            if (!Filter::get_sample(sample_, ec))
                return false;

            buf_ = cycle_buffer_t(sample_.data);
            buf_.commit(sample_.size);
            AsfIArchive ia(buf_);
            ia.context(&context_);
            ia >> header_;

            sample2_.data.insert(sample2_.data.end(), buf_.rbegin(), buf_.rend());
            sample2_.size += sample_.size - header_.HEAD_SIZE;
            sample_.data.clear();
            sample2_.append(sample_); // lock memory

            if (header_.AFFlags == 0x0C) {
                buf_ = cycle_buffer_t(sample2_.data);
                buf_.commit(sample2_.size);
                AsfIArchive ia(buf_);
                just::avformat::AsfHeaderObject header;
                AsfObjectHeader obj_head;
                ia >> header;
                while (ia >> obj_head) {
                    if (obj_head.ObjectId == ASF_FILE_PROPERTIES_OBJECT) {
                        just::avformat::AsfFilePropertiesObjectData file;
                        ia >> file;
                        context_.max_packet_size = file.MaximumDataPacketSize;
                    } else if (obj_head.ObjectId == ASF_STREAM_PROPERTIES_OBJECT) {
                        AsfStreamPropertiesObjectData obj_data;
                        ia >> obj_data;
                        size_t index = streams_.size();
                        if ((size_t)obj_data.Flag.StreamNumber + 1 > stream_map_.size()) {
                            stream_map_.resize(obj_data.Flag.StreamNumber + 1, size_t(-1));
                            stream_map_[obj_data.Flag.StreamNumber] = index;
                        }
                        streams_.push_back(AsfStream(obj_data));
                        streams_.back().index = index;
                    } else {
                        ia.seekg(obj_head.ObjLength - 24, std::ios::cur);
                    }
                }
                parses_.resize(streams_.size());
                sample2_.data.clear();
                sample_.append(sample2_); // release memory on next get_sample
                return true;
            }

            return false;
        }

        bool MmsFilter::get_next_sample(
            Sample & sample,
            boost::system::error_code & ec)
        {
            if (!Filter::get_next_sample(sample, ec))
                return false;

            parse_for_time(sample, ec);

            return true;
        }

        bool MmsFilter::get_last_sample(
            Sample & sample,
            boost::system::error_code & ec)
        {
            if (!Filter::get_last_sample(sample, ec))
                return false;

            parse_for_time(sample, ec);

            return true;
        }

        bool MmsFilter::before_seek(
            Sample & sample,
            boost::system::error_code & ec)
        {
            for (size_t i = 0; i < parses_.size(); ++i) {
                parses_[i].first_packet = 0;
                std::deque<boost::asio::const_buffer> data;
                parses_[i].clear(data);
            }
            for (size_t i = 0; i < packet_memory_.size(); ++i) {
                sample.append(packet_memory_[i].second);
            }
            sample.append(sample_);
            sample.append(sample2_);
            packet_memory_.clear();
            packet_index_ = 0;
            packet_.PayloadNum = 0;

            return Filter::before_seek(sample, ec);
        }

        bool MmsFilter::next_payload(
            boost::system::error_code & ec)
        {
            if (packet_.PayloadNum == 0) {
                sample_.data.clear();
                if (!Filter::get_sample(sample_, ec)) {
                    return false;
                }
                packet_memory_.push_back(std::make_pair(streams_.size() + 1, sample_.memory));
                sample_.memory = NULL;
                buf_ = cycle_buffer_t(sample_.data);
                buf_.commit(sample_.size);
                AsfIArchive ia(buf_);
                ia.context(&context_);
                ia >> header_ >> packet_;
            }
            AsfIArchive ia(buf_);
            ia.context(&context_);
            ia >> payload_;
            if (--packet_.PayloadNum == 0) {
                --packet_memory_.back().first;
            }
            if (ia) {
                ec.clear();
                return true;
            } else {
                ec = bad_media_format;
                return false;
            }
        }

        void MmsFilter::parse_for_time(
            Sample & sample,
            boost::system::error_code & ec)
        {
            MmspDataHeader header;
            AsfPacket packet;
            AsfPayloadHeader payload;
            AsfParseContext context(context_);
            context.packet = &packet;
            cycle_buffer_t buf(sample.data);
            buf.commit(sample.size);
            AsfIArchive ia(buf);
            ia.context(&context);
            ia >> header >> packet >> payload;
            sample.dts = payload.PresTime;
        }

    } // namespace mmspc
} // namespace just
