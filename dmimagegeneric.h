#pragma once

namespace DataType
{
	enum DataTypes {
		BOOL = 14,
		INT_1 = 9,
		INT_2 = 1,
		INT_4 = 7,
		UINT_1 = 6,
		UINT_2 = 10,
		UINT_4 = 11,
		FLOAT = 2,
		DOUBLE = 12,
		COMPLEX_FLOAT = 3,
		COMPLEX_DOUBLE = 13,
		RGB = 23
	};
}

class DMImageGeneric
{
public:
	DMImageGeneric();
	~DMImageGeneric();
};

