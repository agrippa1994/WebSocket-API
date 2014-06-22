#include "Client.h"


/// Global websocket_client pointer (shared)
std::shared_ptr<websocket_client> g_pClient = nullptr;

/// Global connection hdl which is used for sending data and stuff like that
websocketpp::connection_hdl g_cConnection;

/// Global array of addresses which are used for callbacks which are registered by the interface
uint32_t g_dwCallbackAdresses[callback_numElements] = { 0x0 };


/// Connect to an endpoint
/**
* Create and initialize a connection to a specific endpoint
*
* @param szServer Address of an endpoint, example: (ws://localhost:9000/)
*/
EXPORT bool websocket_connect(const char *szServer)
{
	if (g_pClient)
		return false;

	// Create a new instance
	g_pClient = std::shared_ptr<websocket_client>(new websocket_client());

	// Disable logging
	g_pClient->clear_access_channels(websocketpp::log::alevel::none);
	g_pClient->clear_error_channels(websocketpp::log::alevel::none);

	// Initialize Boost ASIO
	g_pClient->init_asio();

	// Callback handlers
	{
		g_pClient->set_open_handler([&](websocketpp::connection_hdl hdl)
		{
			uint32_t address = 0;
			if ((address = g_dwCallbackAdresses[callback_on_connect]))
				__asm call address
		});

		g_pClient->set_fail_handler([&](websocketpp::connection_hdl hdl)
		{
			uint32_t address = 0;
			if ((address = g_dwCallbackAdresses[callback_on_fail]))
				__asm call address

		});

		g_pClient->set_close_handler([&](websocketpp::connection_hdl hdl)
		{
			uint32_t address = 0;
			if ((address = g_dwCallbackAdresses[callback_on_disconnect]))
				__asm call address

		});

		g_pClient->set_message_handler([&](websocketpp::connection_hdl hdl, websocket_message msg)
		{
			uint32_t address = 0;
			if ((address = g_dwCallbackAdresses[callback_on_message]))
			{
				try
				{
					msg->get_payload(); // Might throw
					auto c_str = msg->get_payload();

					__asm call c_str
					__asm call address
				}
				catch (...)
				{
				}
			}

		});
	}
	

	websocketpp::lib::error_code ec;
	websocket_client::connection_ptr ptr = g_pClient->get_connection(std::string(szServer), ec);
	if (ec)
	{
		// Reset the client
		g_pClient = nullptr;
		return false;
	}

	// Get handle
	g_cConnection = ptr->get_handle();

	// Connect
	g_pClient->connect(ptr);

	// Start a thread for asynchronious operations
	websocketpp::lib::thread(&websocket_client::run, g_pClient);

	return true;
}

EXPORT bool websocket_disconnect()
{
	if (!g_pClient)
		return false;

	// Stop asynchronious operations
	while(!g_pClient->stopped())
		g_pClient->stop();
	
	// Delete the instance and use RAII for deinitialization
	g_pClient = nullptr;

	return true;
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

	// Set the address
	g_dwCallbackAdresses[dwType] = dwAddress;

	return true;
}
