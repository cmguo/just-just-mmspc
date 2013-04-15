// MmsFilter.cpp

#include "ppbox/mmspc/Common.h"
#include "ppbox/mmspc/MmsFilter.h"

#include <ppbox/avformat/stream/SampleBuffers.h>
using namespace ppbox::demux;
using namespace ppbox::avformat;

#include <util/archive/ArchiveBuffer.h>
#include <util/protocol/mmsp/MmspData.h>
using namespace util::protocol;

namespace ppbox
{
    namespace mmspc
    {

        MmsFilter::MmsFilter()
            : is_save_sample_(false)
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
            while (true) {
                if (!is_save_sample_) {
                    sample_.data.clear();
                    if (!Filter::get_sample(sample_, ec)) {
                        return false;
                    }
                    buf_ = cycle_buffer_t(sample_.data);
                    buf_.commit(sample_.size);
                    ASFIArchive ia(buf_);
                    ia.context(&context_);
                    ia >> header_ >> packet_;
                }
                ASFIArchive ia(buf_);
                ia.context(&context_);
                while (packet_.PayloadNum) {
                    ia >> payload_;
                    if (--packet_.PayloadNum == 0) {
                        is_save_sample_ = false;
                        sample2_.append(sample_);
                    }
                    parse_.add_payload(context_, payload_);
                    if (parse_.is_discontinuity()) {
                        sample.data.clear();
                    }
                    sample2_.data.insert(sample2_.data.end(), buf_.rbegin(payload_.PayloadLength), buf_.rend());
                    buf_.consume(payload_.PayloadLength);
                    if (parse_.finish()) {
                        ec.clear();
                        break;
                    }
                }
                if (parse_.finish()) {
                    break;
                }
            }

            sample.itrack = stream_map_[parse_.stream_num()];
            sample.flags = 0;
            if (parse_.is_sync_frame())
                sample.flags |= Sample::sync;
            if (parse_.is_discontinuity())
                sample.flags |= Sample::discontinuity;
            sample.dts = parse_.dts();
            sample.cts_delta = boost::uint32_t(-1);
            sample.duration = 0;
            sample.size = parse_.size();
            sample.append(sample2_);
            sample.data.swap(sample2_.data);

            parse_.clear();

            return true;
        }

        bool MmsFilter::get_header(
            boost::system::error_code & ec)
        {
            if (!Filter::get_sample(sample_, ec))
                return false;

            buf_ = cycle_buffer_t(sample_.data);
            buf_.commit(sample_.size);
            ASFIArchive ia(buf_);
            ia.context(&context_);
            ia >> header_;

            sample2_.data.insert(sample2_.data.end(), buf_.rbegin(), buf_.rend());
            sample2_.size += sample_.size - header_.HEAD_SIZE;
            sample_.data.clear();

            if (header_.AFFlags == 0x0C) {
                buf_ = cycle_buffer_t(sample2_.data);
                buf_.commit(sample2_.size);
                ASFIArchive ia(buf_);
                ppbox::avformat::ASF_Header_Object header;
                ASF_Object_Header obj_head;
                ia >> header;
                while (ia >> obj_head) {
                    if (obj_head.ObjectId == ASF_FILE_PROPERTIES_OBJECT) {
                        ppbox::avformat::ASF_File_Properties_Object_Data file;
                        ia >> file;
                        context_.max_packet_size = file.MaximumDataPacketSize;
                    } else if (obj_head.ObjectId == ASF_STREAM_PROPERTIES_OBJECT) {
                        ASF_Stream_Properties_Object_Data obj_data;
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
                sample2_.data.clear();
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
            is_save_sample_ = false;
            sample.append(sample_);
            return Filter::before_seek(sample, ec);
        }

        void MmsFilter::parse_for_time(
            Sample & sample,
            boost::system::error_code & ec)
        {
            MmspDataHeader header;
            ASF_Packet packet;
            ASF_PayloadHeader payload;
            ASF_ParseContext context(context_);
            context.packet = &packet;
            cycle_buffer_t buf(sample.data);
            buf.commit(sample.size);
            ASFIArchive ia(buf);
            ia.context(&context);
            ia >> header >> packet >> payload;
            sample.dts = payload.PresTime;
        }

    } // namespace mux
} // namespace ppbox
