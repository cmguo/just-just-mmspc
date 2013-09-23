// MmspSource.cpp

#include "ppbox/mmspc/Common.h"
#include "ppbox/mmspc/MmspSource.h"

#include <util/protocol/mmsp/MmspError.h>
#include <util/protocol/mmsp/MmspSocket.hpp>
#include <util/protocol/mmsp/MmspData.h>
using namespace util::protocol;

#include <boost/asio/buffer.hpp>

#include <framework/logger/Logger.h>
#include <framework/logger/StreamRecord.h>
#include <framework/logger/StringRecord.h>

namespace ppbox
{
    namespace mmspc
    {

        FRAMEWORK_LOGGER_DECLARE_MODULE_LEVEL("ppbox.mmspc.MmspSource", framework::logger::Debug);

        MmspSource::MmspSource(
            boost::asio::io_service & io_svc)
            : util::stream::UrlSource(io_svc)
            , client_(io_svc)
            , open_step_(0){
        }

        MmspSource::~MmspSource()
        {
        }

        bool MmspSource::open(
            framework::string::Url const & url, 
            boost::uint64_t beg, 
            boost::uint64_t end, 
            boost::system::error_code & ec)
        {
            url_ = url;
            open_step_ = 1;
            is_open(ec);
            return !ec;
        }

        void MmspSource::async_open(
            framework::string::Url const & url, 
            boost::uint64_t beg, 
            boost::uint64_t end, 
            response_type const & resp)
        {
            url_ = url;
            resp_ = resp;
            handle_open(boost::system::error_code());
        }

        void MmspSource::handle_open(
            boost::system::error_code const & ec)
        {
            if (ec) {
                response(ec);
                return;
            }

            switch (open_step_) {
                case 0:
                    open_step_ = 1;
                    client_.async_connect(url_, 
                        boost::bind(&MmspSource::handle_open, this, _1));
                    break;
                case 1:
                    open_step_ = 2;
                    client_.async_play(
                        boost::bind(&MmspSource::handle_open, this, _1));
                    break;
                case 2:
                    open_step_ = 3;
                    client_.set_read_parallel(true);
                    response(ec);
                    break;
                case 4: // cancel
                    response(boost::asio::error::operation_aborted);
                    break;
                default:
                    assert(0);
            }
        }

        void MmspSource::response(
            boost::system::error_code const & ec)
        {
            response_type resp;
            resp.swap(resp_);
            resp(ec);
        }

        bool MmspSource::is_open(
            boost::system::error_code & ec)
        {
            if (open_step_ == 3) {
                ec.clear();
                return true;
            }

            switch (open_step_) {
                case 1:
                    if (client_.connect(url_, ec))
                        break;
                    open_step_ = 2;
                case 2:
                    if (client_.play(ec))
                        break;
                    open_step_ = 3;
                    client_.set_read_parallel(true);
                default:
                    assert(0);
            }

            return !ec;
        }

        bool MmspSource::close(
            boost::system::error_code & ec)
        {
            return !client_.close(ec);
        }

        bool MmspSource::cancel(
            boost::system::error_code & ec)
        {
            return !client_.cancel_forever(ec);
        }

        size_t MmspSource::private_read_some(
            buffers_t const & buffers,
            boost::system::error_code & ec)
        {
            client_.tick(ec);
            return client_.read_raw_msg(buffers, ec);
        }

        struct mmsp_source_read_handler
        {
            mmsp_source_read_handler(
                MmspSource & source, 
                util::stream::StreamMutableBuffers const & buffers, 
                util::stream::StreamHandler const & handler)
                : source_(source)
                , buffers_(buffers)
                , handler_(handler)
            {
            }

            void operator()(
                boost::system::error_code const & ec, 
                size_t bytes_transferred) const
            {
                source_.handle_read_some(buffers_, handler_, ec, bytes_transferred);
            }

        private:
            MmspSource & source_;
            util::stream::StreamMutableBuffers buffers_;
            util::stream::StreamHandler handler_;
        };

        void MmspSource::private_async_read_some(
            buffers_t const & buffers,
            handler_t const & handler)
        {
            boost::system::error_code ec;
            client_.tick(ec);
            client_.async_read_raw_msg(
                buffers, 
                mmsp_source_read_handler(*this, buffers, handler));
        }

        void MmspSource::handle_read_some(
            buffers_t const & buffers,
            handler_t const & handler, 
            boost::system::error_code const & ec, 
            size_t bytes_transferred)
        {
            if (ec == boost::asio::error::would_block) {
                private_async_read_some(buffers, handler);
                return;
            }
            handler(ec, bytes_transferred);
        }

        bool MmspSource::set_non_block(
            bool non_block, 
            boost::system::error_code & ec)
        {
            return !client_.set_non_block(non_block, ec);
        }

        bool MmspSource::set_time_out(
            boost::uint32_t time_out, 
            boost::system::error_code & ec)
        {
            return !client_.set_time_out(time_out, ec);
        }

        bool MmspSource::continuable(
            boost::system::error_code const & ec)
        {
            return ec == boost::asio::error::would_block;
        }

        bool MmspSource::is_record() const
        {
            return false;
        }

    } // namespace mmspc
} // namespace ppbox
