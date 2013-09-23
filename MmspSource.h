// MmspSource.h

#ifndef _PPBOX_MMSPC_MMSP_SOURCE_H_
#define _PPBOX_MMSPC_MMSP_SOURCE_H_

#include <util/stream/UrlSource.h>
#include <util/protocol/mmsp/MmspClient.h>

namespace ppbox
{
    namespace mmspc
    {

        struct mmsp_source_read_handler;

        class MmspSource
            : public util::stream::UrlSource
        {
        public:
            MmspSource(
                boost::asio::io_service & io_svc);

            virtual ~MmspSource();

        public:
            virtual bool open(
                framework::string::Url const & url, 
                boost::uint64_t beg, 
                boost::uint64_t end, 
                boost::system::error_code & ec);

            using util::stream::UrlSource::open;

            virtual void async_open(
                framework::string::Url const & url, 
                boost::uint64_t beg, 
                boost::uint64_t end, 
                response_type const & resp);

            using util::stream::UrlSource::async_open;

            virtual bool is_open(
                boost::system::error_code & ec);

            virtual bool close(
                boost::system::error_code & ec);

        public:
            virtual bool cancel(
                boost::system::error_code & ec);

        public:
            virtual bool set_non_block(
                bool non_block, 
                boost::system::error_code & ec);

            virtual bool set_time_out(
                boost::uint32_t time_out, 
                boost::system::error_code & ec);

            virtual bool continuable(
                boost::system::error_code const & ec);

        public:
            bool is_record() const;

        private:
            // implement util::stream::Source
            virtual std::size_t private_read_some(
                buffers_t const & buffers,
                boost::system::error_code & ec);

            virtual void private_async_read_some(
                buffers_t const & buffers,
                handler_t const & handler);

        private:
            void handle_open(
                boost::system::error_code const & ec);

            void response(
                boost::system::error_code const & ec);

            friend struct mmsp_source_read_handler;

            void handle_read_some(
                buffers_t const & buffers,
                handler_t const & handler, 
                boost::system::error_code const & ec, 
                size_t bytes_transferred);

        private:
            util::protocol::MmspClient client_;
            response_type resp_;
            size_t open_step_;
            framework::string::Url url_;
        };

        UTIL_REGISTER_URL_SOURCE("mmsp", MmspSource);

    } // namespace mmspc
} // namespace ppbox

#endif // _PPBOX_MMSPC_MMSP_SOURCE_H_
