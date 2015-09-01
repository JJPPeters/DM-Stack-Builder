#pragma once

#include "boost/variant.hpp"
#include <cstdint>

typedef boost::variant<
	bool,
	int8_t,
	int16_t,
	int32_t,
	uint8_t,
	uint16_t,
	uint32_t,
	float,
	double,
	std::complex<float>,
	std::complex<double>
> numbertype;

class are_strict_equals : public boost::static_visitor<bool>
{
public:

	template <typename T, typename U>
	bool operator()(const T &, const U &) const
	{
		return false; // cannot compare different types
	}

	template <typename T>
	bool operator()(const T & lhs, const T & rhs) const
	{
		// might want to change this to a difference is less than thing
		return lhs == rhs;
	}

};