
#pragma once
#include <utility>

//#include "InuModelWrapper.h"

using namespace System;
using namespace System::Collections::Generic;

using namespace std;

namespace InuCommon 
{
    ////////////////////////////////////////////////////////////////////////
    /// \brief    C# wrapper to Sensor models   
    ///
    /// Role: Each Sensor model has pre-defined parameters that are loaded from internal config file
    ///
    /// Responsibilities: 
    ///      1. Knows all Sensor models that are available
    ///      2. Knows how to load these models from internal config file 
    ////////////////////////////////////////////////////////////////////////
    public ref class InuModelDBWrapper
    {
    public:

        InuModelDBWrapper();

        /// \brief     sensor models DB
        property Dictionary<Tuple<String^, String^>^, InuModelWrapper^>^ mInuModels;

        // \brief   Initialize the sensor models DB object.
        /// 
        /// CInuModel attributes are read from internal configuration file
        /// \return    false if the object could not be loaded 
        bool Init();
    };   
}