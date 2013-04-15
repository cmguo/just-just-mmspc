// MmsDemuxer.cpp

#include "ppbox/mmspc/Common.h"
#include "ppbox/mmspc/MmsDemuxer.h"
#include "ppbox/mmspc/MmsFilter.h"

#include <ppbox/demux/basic/flv/FlvStream.h>
using namespace ppbox::demux;

#include <ppbox/avformat/flv/FlvTagType.h>
using namespace ppbox::avformat;

#include <util/buffers/BuffersCopy.h>

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.mmspc.MmsDemuxer", framework::logger::Debug);

namespace ppbox
{
    namespace mmspc
    {

        MmsDemuxer::MmsDemuxer(
            boost::asio::io_service & io_svc, 
            ppbox::data::PacketMedia & media)
            : ppbox::demux::PacketDemuxer(io_svc, media)
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
} // namespace ppbox
