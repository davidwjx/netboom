#pragma once
using namespace System;
using namespace System::Collections::Generic;

#include "stdafx.h"
#include "InuModelWrapper.h"

using namespace std;
using namespace InuCommon;


//namespace CommonUtilitiesCSharpWrapper
//{
//    static class Program
//    {
		int main(array<System::String ^> ^args)
		{
			InuModelWrapper test;
            test.Init("M3");
            return 0;

		}
//    };
//}