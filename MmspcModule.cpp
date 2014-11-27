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

        boost::system::error_code MmspcModule::startup()
        {
            boost::system::error_code ec;
            return ec;
        }

        void MmspcModule::shutdown()
        {
        }

    } // namespace mmspc
} // namespace just
