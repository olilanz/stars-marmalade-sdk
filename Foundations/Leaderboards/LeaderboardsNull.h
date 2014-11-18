#ifndef __LEADERBOARDSNULL_H__
#define __LEADERBOARDSNULL_H__

#include "Leaderboards.h"

class LeaderboardsNull : public Leaderboards {
public:
	virtual bool IsAvailable();
	virtual bool IsAuthenticated();
	
	virtual bool WaitForAuthentication(uint16 milliseconds);
	
	virtual void SaveScore(const std::string& leaderboardid, unsigned long score);
	virtual bool ShowLeaderboard(const std::string& leaderboardid);
};

#endif