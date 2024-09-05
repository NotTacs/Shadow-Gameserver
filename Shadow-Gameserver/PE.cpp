#include "PE.h"

void ProcessEventHook(UObject* Object, UFunction* Function, void* Params)
{
	/*
	bool bIsUniqueFunction = true;
	if (Object && Function)
	{
		auto FunctionName = UKismetStringLibrary::Conv_NameToString(Function->Name).ToString();
		if (!FunctionName.empty())
		{
			for (auto& Functionish : Functions)
			{
				if (Functionish.first == Function)
				{
					bIsUniqueFunction = false;
				}
			}

			if (bIsUniqueFunction)
			{
				std::pair<UFunction*, void*> Pairs;
				Pairs.first = Function;
				Pairs.second = Params;
				Functions.push_back(Pairs);
			}
		}
	}
	*/

	return Object->ProcessEvent(Function, Params);
}