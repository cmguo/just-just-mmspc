// MmspcModule.h

#ifndef _PPBOX_MMSPC_MMSPC_MODULE_H_
#define _PPBOX_MMSPC_MMSPC_MODULE_H_

namespace ppbox
{
    namespace mmspc
    {

        class MmspSession;
        class MmspDispatcher;

        class MmspcModule 
            : public ppbox::common::CommonModuleBase<MmspcModule>
        {
        public:
            MmspcModule(
                util::daemon::Daemon & daemon);

            virtual ~MmspcModule();

        public:
            virtual boost::system::error_code startup();

            virtual void shutdown();
        };

    } // namespace mmspc
} // namespace ppbox

#endif // _PPBOX_MMSPC_MMSPC_MODULE_H_