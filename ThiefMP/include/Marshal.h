#pragma once

#include "Engine\inc\MarshalBuffer.h"
#include "Defines.h"

class CDarkMarshalBuffer : public CMarshalBuffer
{
public:
	void CopyGlobalFromObj(int obj);
	void CopyReactionEvent(const sReactionEvent& reactEvent);
	void CopyMultiParm(const sMultiParm& parm);
	void CopyChainedEvent(const sChainedEvent& event);
	void CopyReactionParam(const sReactionParam& reactParams);

	int GetObjFromGlobal();
	void GetReactionEvent(sReactionEvent& reactEvent);
	void GetMultiParm(sMultiParm& parm);
	void GetChainedEvent(sChainedEvent& event);
	void GetReactionParam(sReactionParam& reactParams);
};