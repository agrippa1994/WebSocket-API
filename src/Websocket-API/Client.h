#pragma once
#include "Websocket.h"

#define EXPORT extern "C" __declspec(dllexport)

/// Enumeration which is used for callback identification
enum eCallbackType : uint32_t
{
	callback_on_connect = 0,
	callback_on_fail,
	callback_on_disconnect,
	callback_on_message,
	callback_numElements
};

EXPORT bool websocket_connect(const char *szServer);
EXPORT bool websocket_disconnect();

EXPORT bool websocket_send(const char *szMessage, const size_t dwLen, bool isBinary);

EXPORT bool websocket_isconnected();

EXPORT bool websocket_registerCallback(uint32_t dwType, uint32_t dwAddress);