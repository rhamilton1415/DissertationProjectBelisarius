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
	int Connector::sessionId = -1;
	http_client Connector::client(L"http://localhost:80");
	BOMBProfiler Connector::p(&sessionId);

	//stolen from stackoverflow
	template <typename Map>
	bool map_compare(Map const &lhs, Map const &rhs)
	{
		// No predicate needed because there is operator== for pairs already.
		return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin(),rhs.end());
	}
	void Connector::establishConnection()
	{
		p.BOMBEstablishStart();
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
			r.headers().add(L"SessionId", sessionId); //if we have no connection this will be -1
			p.addNetworkOut(sizeof(r));
			pplx::task<web::http::http_response> response = client.request(r);
			p.addNetowrkIn(sizeof(response));
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
				sessionId = -1;
			}
		}
		catch (const std::exception& e)
		{
			errMessage = e.what();
			//errMessage = utility::conversions::to_utf8string(client.base_uri().to_string());
			connectionAvailable = false;
			sessionId = -1;
		}
		p.BOMBEstablishFinish();
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
			p.BOMBUpdateStart();
			updateBOMBState();
			p.BOMBUpdateFinish();
		}
	}

	BWAPI::UnitType Connector::getBOMBOrder()
	{
		try
		{
			p.addConnectionAttempt();
			p.BOMBFunctionStart();
			http_request request(methods::GET);
			request.headers().add(L"Order", 1);
			request.headers().add(L"SessionId", sessionId);
			p.addNetworkOut(sizeof(request));
			pplx::task<web::http::http_response> response = client.request(request);
			json::value v = response.get().extract_json().get();
			p.BOMBFunctionFinish();
			p.addNetowrkIn(sizeof(response));
			return BWAPI::UnitType(v.as_integer());
		}
		catch (...) //If the above fails, it may be because the server is unavailable - retry the check.
		{
			p.BOMBFunctionFinish();
			p.addConnectionFailure();
			establishConnection();
			return BWAPI::UnitTypes::None; //This will just fail the next build order frame - hopefully if the server has gone down it'll just go locally
		}
	}
	std::string Connector::updateBOMBState()
	{
		try
		{
			p.addConnectionAttempt();
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
			p.addNetworkOut(sizeof(request));
			pplx::task<web::http::http_response> response = client.request(request);
			p.addNetowrkIn(sizeof(response));
			return utility::conversions::to_utf8string(response.get().to_string());
		}
		catch (const std::exception& e)
		{
			p.BOMBUpdateFinish();
			p.addConnectionFailure();
			establishConnection();
			return e.what();
		}
		
	}
}