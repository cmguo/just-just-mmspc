// MmsDemuxer.cpp

#include "just/mmspc/Common.h"
#include "just/mmspc/MmsDemuxer.h"
#include "just/mmspc/MmsFilter.h"

using namespace just::demux;

#include <util/buffers/BuffersCopy.h>

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("just.mmspc.MmsDemuxer", framework::logger::Debug);

namespace just
{
    namespace mmspc
    {

        MmsDemuxer::MmsDemuxer(
            boost::asio::io_service & io_svc, 
            just::data::PacketMedia & media)
            : just::demux::PacketDemuxer(io_svc, media)
            , filter_(NULL)
        {
        }

        MmsDemuxer::~MmsDemuxer()
        {
            if (filter_) {
                delete filter_;
            }
        }

        bool MmsDemuxer::check_open(
            boost::system::error_code & ec)
        {
            if (filter_ == NULL) {
                filter_ = new MmsFilter;
                add_filter(filter_);
            }

            if (filter_->get_header(ec)) {
                stream_infos_.insert(stream_infos_.end(), filter_->streams().begin(), filter_->streams().end());
                return true;
            }

            return false;
        }

    } // namespace mmspc
} // namespace just
