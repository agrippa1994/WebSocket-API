#pragma once
#include "Websocket.h"

#define EXPORT extern "C" __declspec(dllexport)

enum class eCallbackType : uint32_t
{
	on_connect, 
	on_disconnect, 
	on_message,
	size
};

extern uint32_t g_dwCallbackAdresses[( uint32_t(eCallbackType::size) )];

extern std::shared_ptr<websocket_client> g_pClient;

EXPORT void websocket_connect(const char *szServer);
EXPORT bool websocket_disconnect();

EXPORT bool websocket_send(const char *szMessage, const size_t dwLen);

EXPORT bool websocket_isconnected();

EXPORT bool websocket_registerCallback(eCallbackType eType, uint32_t dwAddress);