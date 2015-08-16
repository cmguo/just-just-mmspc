// MmspcModule.cpp

#include "just/mmspc/Common.h"
#include "just/mmspc/MmspcModule.h"
#include "just/mmspc/ClassRegister.h"

namespace just
{
    namespace mmspc
    {

        MmspcModule::MmspcModule(
            util::daemon::Daemon & daemon)
            : just::common::CommonModuleBase<MmspcModule>(daemon, "MmspcModule")
        {
        }

        MmspcModule::~MmspcModule()
        {
        }

        bool MmspcModule::startup(
           boost::system::error_code & ec)
        {
            return true;
        }

        bool MmspcModule::shutdown(
           boost::system::error_code & ec)
        {
            return true;
        }

    } // namespace mmspc
} // namespace just
