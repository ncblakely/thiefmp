#pragma once

namespace AITools
{
	bool SetAIAwareness(int objAI, int objAwareOf, int awarenessLevel);
	bool SetAIAwareness(interface IAI* pAI, int objAwareOf, int awarenessLevel);
}