
#ifndef __DEBUGVIEWLOGGER_H__
#define __DEBUGVIEWLOGGER_H__

#include "OSUtilities.h"
#include "BaseLogger.h"

namespace InuCommon
{
    ////////////////////////////////////////////////////////////////////////
    /// \brief   Concrete Logger for OS standard output which can be monitored by DebugView.exe
    ///
    /// Responsibilities:
    ///      1. Implements CLogger interface
    ///
    ////////////////////////////////////////////////////////////////////////    ///////////////////////////////////////////////////////////////////////
    class CDebugViewLogger : public IBaseLogger
    {
        // It should be a friend of CLogger because of the private constructor
        friend class CLogger;
    public:
        CDebugViewLogger();
        virtual ~CDebugViewLogger();

    private:

        virtual void  Write(const std::pair<CLogger::ESeverity, std::string >& logMessage) override;
        virtual void  Flush() override { }

#if defined(__GNUC__) && !defined(__ANDROID__)
        std::string prefix;
#endif
    };
}

#endif
