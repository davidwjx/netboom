// This is the main DLL file.

//#include <codecvt>
//#include <msclr\marshal_cppstd.h>

#include "Storage.h"
#include "InuModel.h"
#include "InuModelDB.h"
#include "InuModelWrapper.h"
#include "InuModelDBWrapper.h"

using namespace InuCommon;


InuModelDBWrapper::InuModelDBWrapper()
{
}

 bool InuModelDBWrapper::Init()
 {
     std::shared_ptr<CInuModelDB> inuModelDB = CInuModelDB::GetTheInstance();
 
     if (!inuModelDB->Load())
     {
         return false;
     }
 
     const std::map<std::pair<std::string, std::string>, CInuModel>& inuModels = inuModelDB->GetAllModels();
 
     mInuModels = gcnew Dictionary<Tuple<String^, String^>^, InuModelWrapper^>();
 
     // Iterate on the models and initialize the C# properties
     for each (auto elem in inuModels)
     {
         InuModelWrapper^ imw = gcnew InuModelWrapper();
 
         if (!imw->Init(elem.second))
         {
             return false;
         }
 
         Tuple<String^, String^>^  key = gcnew Tuple<String^, String^>(gcnew System::String(elem.first.first.c_str()),
             gcnew System::String(elem.first.second.c_str()));
         mInuModels[key] = imw;
     }
 
     return true;
 }
