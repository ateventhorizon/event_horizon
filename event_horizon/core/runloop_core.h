//
//  runloop.h
//  SixthView
//
//  Created byDado on 27/11/2012.
//  Copyright (c) 2012Dado. All rights reserved.
//

#pragma once

#include "di.hpp"
#include "command.hpp"

struct CommandConsole {};
class GameTime;

enum class RunLoopThreading {
    Sync,
    ASynch
};

class RunLoop {
public:
    RunLoop( CommandQueue& _cq ) : cq( _cq ) {}
	virtual ~RunLoop() {}

	void coreLoop();
	void coreFunctions();
	virtual void consolePrompt();
	virtual void runConsolePrompt();
    virtual void runSingleThread() {}

    CommandQueue& CQ() { return cq; }

protected:
	bool mbExitTriggered;
	CommandQueue& cq;
};

template< typename T > void mainLoop( uint64_t _flags = 0, RunLoopThreading rt = RunLoopThreading::Sync );

template <> void mainLoop<CommandConsole>( uint64_t _flags, RunLoopThreading rt );

void updateTime( GameTime& gt );
void daemonLoop( int _sleepSeconds );