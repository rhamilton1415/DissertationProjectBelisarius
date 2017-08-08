#include "Connector.h"
using namespace std;
#define BUFSIZE 4096 

namespace Connectors
{
	std::map<BWAPI::UnitType, int> Connector::playerState;
	std::map<BWAPI::UnitType, int> Connector::queued; 
	bool Connector::connectionAvailable = false;
	std::string Connector::errMessage = "";
	bool Connector::BOMOrderBlock = false;
	int Connector::sessionId = 0;
	http_client Connector::client(L"http://localhost:80");

	//stolen from stackoverflow
	template <typename Map>
	bool map_compare(Map const &lhs, Map const &rhs)
	{
		// No predicate needed because there is operator== for pairs already.
		return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin(),rhs.end());
	}
	void Connector::establishConnection()
	{
		try
		{
			ifstream myReadFile;
			std::string address;
			//uri u;
			myReadFile.open("ClientConfig.txt");
			if (myReadFile.is_open()) {
				while (!myReadFile.eof()) 
				{
					myReadFile >> address;
					client = http_client(utility::conversions::to_string_t(address));
				}
			}
			myReadFile.close();
			//client = http_client(u);
			http_request r(methods::POST);
			r.headers().add(L"Init", 1);
			pplx::task<web::http::http_response> response = client.request(r);
			//If this succeeds, we can use the cloud server - else the AI will use local maths. 
			if (response.get().status_code() == http::status_codes::OK)
			{
				sessionId = response.get().extract_json().get().as_integer();
				connectionAvailable = true;
				errMessage = "";
			}
			else
			{
				connectionAvailable = false;
			}
		}
		catch (const std::exception& e)
		{
			errMessage = e.what();
			//errMessage = utility::conversions::to_utf8string(client.base_uri().to_string());
			connectionAvailable = false;
		}
	}
	void Connector::endSession()
	{
		try
		{
			http_request request(methods::DEL);
			request.headers().add(L"SessionId", sessionId);
			pplx::task<web::http::http_response> response = client.request(request);
		}
		catch(...)
		{

		}
	}
	void Connector::updateState(std::map<BWAPI::UnitType, int> inQueued, std::map<BWAPI::UnitType, int> inPlayerState)
	{
		//If there hasn't actually been an update, don't bother
		if (!map_compare(queued, inQueued) || !map_compare(playerState, inPlayerState))
		{
			queued = inQueued;
			playerState = inPlayerState;
			updateBOMBState();
		}
	}

	BWAPI::UnitType Connector::getBOMBOrder()
	{
		try
		{
			http_request request(methods::GET);
			request.headers().add(L"Order", 1);
			request.headers().add(L"SessionId", sessionId);
			pplx::task<web::http::http_response> response = client.request(request);
			json::value v = response.get().extract_json().get();
			return BWAPI::UnitType(v.as_integer());
		}
		catch (...) //If the above fails, it may be because the server is unavailable - retry the check.
		{
			establishConnection();
			return BWAPI::UnitTypes::None; //This will just fail the next build order frame - hopefully if the server has gone down it'll just go locally
		}
	}
	std::string Connector::updateBOMBState()
	{
		try
		{
			//Build JSON objects of the "Complete State" of both PlayerState and Queued maps
			json::value completeState; //me_irl
			json::value playerStateJSONobj;
			for (std::map<BWAPI::UnitType, int>::const_iterator it = playerState.begin(); it != playerState.end(); ++it)
			{ 
				const std::wstring key = std::to_wstring(it->first);
				playerStateJSONobj[key] = json::value::number(playerState.at(it->first)); //build the json object for the map
			}
			json::value queuedJSONobj;
			for (std::map<BWAPI::UnitType, int>::const_iterator it = queued.begin(); it != queued.end(); ++it)
			{
				const std::wstring key = std::to_wstring(it->first);
				queuedJSONobj[key] = json::value::number(queued.at(it->first)); //build the json object for the map
			}
			completeState[L"PlayerState"] = playerStateJSONobj;
			completeState[L"Queued"] = queuedJSONobj;

			// Manually build up an HTTP request with header and request URI.
			http_request request(methods::POST);
			request.headers().add(L"State", 1);
			request.headers().set_content_type(L"application/json");
			request.headers().add(L"SessionId", sessionId);
			request.set_body(completeState);
			pplx::task<web::http::http_response> response = client.request(request);
			return utility::conversions::to_utf8string(response.get().to_string());
		}
		catch (const std::exception& e)
		{
			establishConnection();
			return e.what();
		}
		
	}

	/*int Connector::Connect(std::map<BWAPI::UnitType, int> queued, std::map<BWAPI::UnitType, int> playerState)
	{
		return 0;
	}*/
}