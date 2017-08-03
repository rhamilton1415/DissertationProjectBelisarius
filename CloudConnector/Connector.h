#pragma once
#include <vector>
#include <exception>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <Windows.h>
using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency;
using namespace concurrency::streams;
namespace Connectors
{
	class Connector
	{
	public:
		static int Connect();
	};
}
