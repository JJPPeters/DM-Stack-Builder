#pragma once

#include "stdafx.h"

// As GMS 3  doesnt seem to have a debug window, I've left this using the results window
static struct DMOutDebug
{
private:
	//bool showdebug;
public:

	//void SetDebug(bool doit) { showdebug = doit; }

	DMOutDebug& operator<<(std::string value)
	{
		//if (!showdebug)
			//return;

		DM::Result(value);
		return *this;
	}

	template<typename T>
	DMOutDebug& operator<<(T value)
	{
		//if (!showdebug)
			//return;

		std::stringstream ss;
		ss << value;
		DM::Result(ss.str());
		return *this;
	}
} DMDebug;
//DMDebug.SetDebug(false);

static struct DMOutResult
{
	DMOutResult& operator<<(std::string value)
	{
		DM::Result(value);
		return *this;
	}

	template<typename T>
	DMOutResult& operator<<(T value)
	{
		std::stringstream ss;
		ss << value;
		DM::Result(ss.str());
		return *this;
	}
} DMresult;

// Very, very bad way of defining an endline
static std::string DMendl = "\n";