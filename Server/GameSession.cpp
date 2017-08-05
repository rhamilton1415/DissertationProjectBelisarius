#include "stdafx.h"
#include "GameSession.h"

void handle_get(http_request request);
void handle_post(http_request request);
void updatePlayerState(json::value completePlayerState);
GameSession::GameSession()
{
}


GameSession::~GameSession()
{
}
