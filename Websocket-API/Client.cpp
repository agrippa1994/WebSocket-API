#include "Client.h"


std::shared_ptr<websocket_client> g_pClient = nullptr;
websocketpp::connection_hdl g_cConnection;

uint32_t g_dwCallbackAdresses[callback_numElements] = { 0x0 };


EXPORT bool websocket_connect(const char *szServer)
{
	if (g_pClient)
		return false;

	// create a new instance
	g_pClient = std::shared_ptr<websocket_client>(new websocket_client());

	// Disable logging
	g_pClient->clear_access_channels(websocketpp::log::alevel::none);
	g_pClient->clear_error_channels(websocketpp::log::alevel::none);

	g_pClient->init_asio();

	g_pClient->set_open_handler([&](websocketpp::connection_hdl hdl)
	{
		g_cConnection = hdl;


	});

	g_pClient->set_close_handler([&](websocketpp::connection_hdl hdl)
	{
		g_cConnection = hdl;
	});

	g_pClient->set_message_handler([&](websocketpp::connection_hdl hdl, websocket_message msg)
	{
		g_cConnection = hdl;
	});

}

EXPORT bool websocket_disconnect()
{
	if (!g_pClient)
		return false;
}


EXPORT bool websocket_send(const char *szMessage, const size_t dwLen)
{
	if (!g_pClient)
		return false;

}


EXPORT bool websocket_isconnected()
{
	if (!g_pClient)
		return false;

	
}


EXPORT bool websocket_registerCallback(uint32_t dwType, uint32_t dwAddress)
{
	if (dwType < 0 || dwType >= callback_numElements)
		return false;
}
