// MmspMedia.h

#ifndef _PPBOX_MMSPC_MMSP_MEDIA_H_
#define _PPBOX_MMSPC_MMSP_MEDIA_H_

#include "ppbox/mmspc/MmspSource.h"

#include <ppbox/data/packet/PacketMedia.h>

#include <util/protocol/mmsp/MmspClient.h>

namespace ppbox
{
    namespace mmspc
    {

        class MmspMedia
            : public ppbox::data::PacketMedia
        {
        public:
            MmspMedia(
                boost::asio::io_service & io_svc,
                framework::string::Url const & url);

            virtual ~MmspMedia();

        public:
            virtual void async_open(
                response_type const & resp);

            virtual void cancel(
                boost::system::error_code & ec);

            virtual void close(
                boost::system::error_code & ec);

        public:
            virtual bool get_basic_info(
                ppbox::data::MediaBasicInfo & info,
                boost::system::error_code & ec) const;

            virtual bool get_info(
                ppbox::data::MediaInfo & info,
                boost::system::error_code & ec) const;

        public:
            virtual bool get_packet_feature(
                ppbox::data::PacketFeature & feature,
                boost::system::error_code & ec) const;

            virtual util::stream::Source & source();

        private:
            void handle_open(
                boost::system::error_code const & ec, 
                MediaBase::response_type const & resp);

        private:
            MmspSource source_;
            ppbox::data::MediaInfo info_;
        };

        PPBOX_REGISTER_MEDIA_BY_PROTOCOL("mms", MmspMedia);

    } // mmspc
} // ppbox

#endif // _PPBOX_MMSPC_MMSP_MEDIA_H_
