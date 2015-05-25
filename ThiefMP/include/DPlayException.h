#pragma once

// DirectPlay exception object. Contains HRESULT error code and optional plain-text representation of the error.
class DPlayException
{
public:
	DPlayException(const char* errorMessage, HRESULT errorCode) { m_errorMessage = errorMessage; m_errorCode = errorCode; }

	const char* GetMessage() { return m_errorMessage; }
	HRESULT GetCode() { return m_errorCode; }

protected:
	const char* m_errorMessage;
	HRESULT m_errorCode;
};
