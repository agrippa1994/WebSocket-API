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
* @param szServer Address of an endpoint, example: (ws://localhost:9000/). The string MUST NOT be null!
* @return Returns a boolean which indicates if the initialization was successfull. The real connection status can be determined with additional callbacks.
*/
EXPORT bool websocket_connect(const char *szServer)
{
	if (g_pClient || szServer == NULL)
		return false;

	// Create a new instance
	g_pClient = std::shared_ptr<websocket_client>(new websocket_client());

	// Disable logging
	g_pClient->clear_access_channels(websocketpp::log::alevel::none);
	g_pClient->clear_error_channels(websocketpp::log::alevel::none);

	// Initialize Boost ASIO
	g_pClient->init_asio();

	// Callback handlers. The function convention which is used for these callbacks is "__cdecl"
	{
		g_pClient->set_open_handler([&](websocketpp::connection_hdl hdl)
		{
			uint32_t address = 0x0;
			if ((address = g_dwCallbackAdresses[callback_on_connect]))
				__asm call address
		});

		g_pClient->set_fail_handler([&](websocketpp::connection_hdl hdl)
		{
			// Deinitialize the connection.
			websocket_disconnect();

			uint32_t address = 0x0;
			if ((address = g_dwCallbackAdresses[callback_on_fail]))
				__asm call address

		});

		g_pClient->set_close_handler([&](websocketpp::connection_hdl hdl)
		{
			// Deinitialize the connection.
			websocket_disconnect();

			uint32_t address = 0x0;
			if ((address = g_dwCallbackAdresses[callback_on_disconnect]))
				__asm call address

		});

		g_pClient->set_message_handler([&](websocketpp::connection_hdl hdl, websocket_message msg)
		{
			uint32_t address = 0x0;
			if ((address = g_dwCallbackAdresses[callback_on_message]))
			{
				try
				{
					msg->get_payload(); // Might throw if the pointer is invalid
					auto c_str = msg->get_payload();

					__asm call c_str
					__asm call address
					__asm add esp, 4 // Clean up the stack
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

/// Close the connection to the endpoint
/**
* Close the connection and deinitialize the client.
*
* @return Returns a boolean which indicates if the deinitialization was successfull.
*/
EXPORT bool websocket_disconnect()
{
	if (!g_pClient)
		return false;

	// Stop asynchronious operations
	try
	{
		while (!g_pClient->stopped())
			g_pClient->stop();
	}
	catch (...)
	{
	}
	
	// Delete the instance and use RAII for deinitialization
	g_pClient = nullptr;

	return true;
}

/// Send a message
/**
* Send a message to the endpoint which can be sent in text or binary format.
*
* @param szMessage Pointer to a string or to a binary buffer.
* @param dwLen Length of the message.
* @param isBinary Boolean which indicates if the data is binary or not.
*
* @return Returns a boolean which indicates if the deinitialization was successfull.
*/
EXPORT bool websocket_send(const char *szMessage, const size_t dwLen, bool isBinary)
{
	if (!g_pClient || szMessage == 0 || dwLen == 0)
		return false;

	try
	{
		auto p = g_pClient->get_con_from_hdl(g_cConnection);
		if (p->send(std::string(szMessage, dwLen), isBinary ? websocketpp::frame::opcode::text : websocketpp::frame::opcode::binary))
			return false;

		return true;
	}
	catch (...)
	{
	}

	return false;
}

/// Check the connection status
/**
* This function checks if the socket is connected to an endpoint.
*
* @return Returns a boolean which is true if the connection is established otherwise false.
*/
EXPORT bool websocket_isconnected()
{
	if (!g_pClient)
		return false;

	return true;
}

/// Register a callback
/**
* This function checks if the socket is connected to an endpoint.
*
* @param dwType Type of the callback (@see eCallbackType)
* @param dwAddress Address of the function which the interface will call if the callback is triggered (depends on the type)
*
* @return Returns a boolean which indicates if the callback has been registered.
*
* @see eCallbackType
*/
EXPORT bool websocket_registerCallback(uint32_t dwType, uint32_t dwAddress)
{
	if (dwType < 0 || dwType >= callback_numElements)
		return false;

	// Set the address
	g_dwCallbackAdresses[dwType] = dwAddress;

	return true;
}
