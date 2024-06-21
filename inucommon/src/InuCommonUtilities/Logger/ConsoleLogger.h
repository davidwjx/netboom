
#ifndef __CONSOLELOGGER_H__
#define __CONSOLELOGGER_H__

#include "BaseLogger.h"
#include "OSUtilities.h"
#include <fstream>

namespace InuCommon
{

    ////////////////////////////////////////////////////////////////////////
    /// \brief   Concrete Logger for standard output
    ////////////////////////////////////////////////////////////////////////
    class CConsoleLogger: public IBaseLogger
    {
        // It should be a friend of CLogger because of the private constructor 
        friend class CLogger;
        
    public:

        CConsoleLogger();
        virtual ~CConsoleLogger();

    private:

        virtual void  Write(const std::pair<CLogger::ESeverity, std::string >& logMessage) override;
        virtual void  Flush() override { }

        //std::string mLastMessageSeverity;
    };

}

#endif // __TEXTFILELOGGER_H__

