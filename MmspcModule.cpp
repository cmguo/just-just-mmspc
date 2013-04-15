// MmspcModule.cpp

#include "ppbox/mmspc/Common.h"
#include "ppbox/mmspc/MmspcModule.h"
#include "ppbox/mmspc/ClassRegister.h"

namespace ppbox
{
    namespace mmspc
    {

        MmspcModule::MmspcModule(
            util::daemon::Daemon & daemon)
            : ppbox::common::CommonModuleBase<MmspcModule>(daemon, "MmspcModule")
        {
        }

        MmspcModule::~MmspcModule()
        {
        }

        boost::system::error_code MmspcModule::startup()
        {
            boost::system::error_code ec;
            return ec;
        }

        void MmspcModule::shutdown()
        {
        }

    } // namespace mmspc
} // namespace ppbox
