#pragma once

#include <cstdint>
#include <unordered_map>

class RubiksCube
{
public:
	uint8_t map[6 * 9];
	uint8_t dirs[6 * 9];

	RubiksCube();
	RubiksCube(const RubiksCube& in);
	void reset();

	const RubiksCube& operator=(const RubiksCube& in);
	const RubiksCube& operator*=(const RubiksCube& b);
	const RubiksCube& operator/=(const RubiksCube& b);

	bool equals(const RubiksCube& b, bool include_dirs = true) const;
	bool partial_equals(const RubiksCube& b, const uint8_t* mask, bool include_dirs = true) const;	
	void print();
	
	static RubiksCube s_cube_o;
	static RubiksCube s_bases[18];
	static RubiksCube s_RCW, s_RCCW, s_R2CW, s_R2CCW;
	static RubiksCube s_LCW, s_LCCW, s_L2CW, s_L2CCW;
	static RubiksCube s_UCW, s_UCCW, s_U2CW, s_U2CCW;
	static RubiksCube s_DCW, s_DCCW, s_D2CW, s_D2CCW;
	static RubiksCube s_FCW, s_FCCW, s_F2CW, s_F2CCW;
	static RubiksCube s_BCW, s_BCCW, s_B2CW, s_B2CCW;
	static RubiksCube s_XCW, s_XCCW, s_YCW, s_YCCW, s_ZCW, s_ZCCW;
	static RubiksCube s_MCW, s_MCCW, s_ECW, s_ECCW, s_SCW, s_SCCW;
	static std::unordered_map<char, std::pair<const RubiksCube*, const RubiksCube*>> s_op_map;

	static void s_Initialize();
	static void parse_seq(const std::string& seq, std::vector<const RubiksCube*>& operations, std::vector<std::string>& notes, std::vector<int>& groups, bool reverse = false);
	
	void exec_seq(const std::string& seq, bool reverse = false);

};

