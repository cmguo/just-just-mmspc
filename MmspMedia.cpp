// MmspMedia.cpp

#include "ppbox/mmspc/Common.h"
#include "ppbox/mmspc/MmspMedia.h"

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>
#include <framework/string/Algorithm.h>

#include <boost/bind.hpp>

FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.mmspc.MmspMedia", framework::logger::Debug);

namespace ppbox
{
    namespace mmspc
    {

        MmspMedia::MmspMedia(
            boost::asio::io_service & io_svc,
            framework::string::Url const & url)
            : PacketMedia(io_svc, url)
            , source_(io_svc)
        {
            boost::system::error_code ec;
            PacketMedia::get_basic_info(info_, ec);
            info_.type = info_.live;
            info_.format = "mms";
        }

        MmspMedia::~MmspMedia()
        {
        }

        void MmspMedia::async_open(
            MediaBase::response_type const & resp)
        {
            source_.async_open(url_, 
                boost::bind(&MmspMedia::handle_open, this, _1, resp));
        }

        void MmspMedia::handle_open(
            boost::system::error_code const & ec, 
            MediaBase::response_type const & resp)
        {
            if (!ec && source_.is_record()) {
                ((ppbox::data::MediaInfo &)info_).type = info_.vod;
                info_.flags |= info_.f_seekable;
                info_.flags |= info_.f_pauseable;
            }
            resp(ec);
        }

        void MmspMedia::cancel(
            boost::system::error_code & ec)
        {
            source_.cancel(ec);
        }

        void MmspMedia::close(
            boost::system::error_code & ec)
        {
            source_.close(ec);
        }

        bool MmspMedia::get_basic_info(
            ppbox::data::MediaBasicInfo & info,
            boost::system::error_code & ec) const
        {
            info = info_;
            ec.clear();
            return true;
        }

        bool MmspMedia::get_info(
            ppbox::data::MediaInfo & info,
            boost::system::error_code & ec) const
        {
            info = info_;
            ec.clear();
            return PacketMedia::get_info(info, ec);
        }

        bool MmspMedia::get_packet_feature(
            ppbox::data::PacketFeature & feature,
            boost::system::error_code & ec) const
        {
            feature.piece_size = 1024;
            feature.packet_max_size = 1024 * 100;
            feature.buffer_size = 1024 * 1024 * 2; // 2M
            feature.prepare_size = 1024 * 10;
            ec.clear();
            return true;
        }

        util::stream::Source & MmspMedia::source()
        {
            return source_;
        }


    } // mmspc
} // ppbox
