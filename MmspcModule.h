// MmspcModule.h

#ifndef _JUST_MMSPC_MMSPC_MODULE_H_
#define _JUST_MMSPC_MMSPC_MODULE_H_

namespace just
{
    namespace mmspc
    {

        class MmspcModule 
            : public just::common::CommonModuleBase<MmspcModule>
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
} // namespace just

#endif // _JUST_MMSPC_MMSPC_MODULE_H_