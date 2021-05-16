#include "RubiksCube.h"
#include <memory.h>
#include <cstdio>
#include "base_data.hpp"

RubiksCube RubiksCube::s_cube_o;
RubiksCube RubiksCube::s_bases[18];
RubiksCube RubiksCube::s_RCW, RubiksCube::s_RCCW, RubiksCube::s_R2CW, RubiksCube::s_R2CCW;
RubiksCube RubiksCube::s_LCW, RubiksCube::s_LCCW, RubiksCube::s_L2CW, RubiksCube::s_L2CCW;
RubiksCube RubiksCube::s_UCW, RubiksCube::s_UCCW, RubiksCube::s_U2CW, RubiksCube::s_U2CCW;
RubiksCube RubiksCube::s_DCW, RubiksCube::s_DCCW, RubiksCube::s_D2CW, RubiksCube::s_D2CCW;
RubiksCube RubiksCube::s_FCW, RubiksCube::s_FCCW, RubiksCube::s_F2CW, RubiksCube::s_F2CCW;
RubiksCube RubiksCube::s_BCW, RubiksCube::s_BCCW, RubiksCube::s_B2CW, RubiksCube::s_B2CCW;
RubiksCube RubiksCube::s_XCW, RubiksCube::s_XCCW, RubiksCube::s_YCW, RubiksCube::s_YCCW, RubiksCube::s_ZCW, RubiksCube::s_ZCCW;
RubiksCube RubiksCube::s_MCW, RubiksCube::s_MCCW, RubiksCube::s_ECW, RubiksCube::s_ECCW, RubiksCube::s_SCW, RubiksCube::s_SCCW;
std::unordered_map<char, std::pair<const RubiksCube*, const RubiksCube*>>  RubiksCube::s_op_map;

RubiksCube::RubiksCube()
{
	reset();
}

RubiksCube::RubiksCube(const RubiksCube& in)
{
	memcpy(map, in.map, sizeof(map));
	memcpy(dirs, in.dirs, sizeof(dirs));
}

void RubiksCube::reset()
{
	for (uint8_t i = 0; i < 6 * 9; i++)
	{
		map[i] = i;
		dirs[i] = 0;
	}
}

const RubiksCube& RubiksCube::operator=(const RubiksCube& in)
{
	memcpy(map, in.map, sizeof(map));
	memcpy(dirs, in.dirs, sizeof(dirs));
	return *this;
}

const RubiksCube& RubiksCube::operator*=(const RubiksCube& b)
{
	uint8_t map_old[6 * 9];
	uint8_t dirs_old[6 * 9];
	memcpy(map_old, map, sizeof(map_old));
	memcpy(dirs_old, dirs, sizeof(dirs_old));

	for (uint8_t i = 0; i < 6 * 9; i++)
	{
		uint8_t j = b.map[i];
		uint8_t ddir = b.dirs[i];
		map[i] = map_old[j];
		dirs[i] = (dirs_old[j] + ddir) % 4;
	}
	return *this;
}

const RubiksCube& RubiksCube::operator/=(const RubiksCube& b)
{
	uint8_t map_old[6 * 9];
	uint8_t dirs_old[6 * 9];
	memcpy(map_old, map, sizeof(map_old));
	memcpy(dirs_old, dirs, sizeof(dirs_old));

	for (uint8_t i = 0; i < 6 * 9; i++)
	{
		uint8_t j = b.map[i];
		uint8_t ddir = b.dirs[i];
		map[j] = map_old[i];
		dirs[j] = (dirs_old[i] + 4 - ddir) % 4;
	}

	return *this;
}

bool RubiksCube::equals(const RubiksCube& b, bool include_dirs) const
{
	for (uint8_t i = 0; i < 6 * 9; i++)
	{
		if (map[i] != b.map[i]) return false;
		if (include_dirs && dirs[i] != b.dirs[i]) return false;
	}
	return true;
}

bool RubiksCube::partial_equals(const RubiksCube& b, const uint8_t* mask, bool include_dirs) const
{
	for (uint8_t i = 0; i < 6 * 9; i++)
	{
		if (mask[i] == 0) continue;
		if (map[i] != b.map[i]) return false;
		if (include_dirs && dirs[i] != b.dirs[i]) return false;
	}
	return true;
}

void RubiksCube::print()
{
	for (int i = 0; i < 54; i++)
		printf("%u ", (unsigned)map[i]);
	printf("\n");
	for (int i = 0; i < 54; i++)
		printf("%u ", (unsigned)dirs[i]);
	printf("\n");
}

void RubiksCube::s_Initialize()
{
	uint8_t* p_in = (uint8_t*)base_data;
	for (int i = 0; i < 18; i++, p_in += 54)
		memcpy(s_bases[i].map, p_in, 54);
	for (int i = 0; i < 18; i++, p_in += 54)
		memcpy(s_bases[i].dirs, p_in, 54);

	s_RCW.reset();
	s_RCW *= s_bases[0];

	s_RCCW.reset();
	s_RCCW *= s_bases[5];

	s_R2CW.reset();
	s_R2CW *= s_bases[0];
	s_R2CW *= s_bases[1];

	s_R2CCW.reset();
	s_R2CCW *= s_bases[5];
	s_R2CCW *= s_bases[4];

	s_LCW.reset();
	s_LCW *= s_bases[3];

	s_LCCW.reset();
	s_LCCW *= s_bases[2];

	s_L2CW.reset();
	s_L2CW *= s_bases[3];
	s_L2CW *= s_bases[4];

	s_L2CCW.reset();
	s_L2CCW *= s_bases[2];
	s_L2CCW *= s_bases[1];

	s_UCW.reset();
	s_UCW *= s_bases[6];

	s_UCCW.reset();
	s_UCCW *= s_bases[11];

	s_U2CW.reset();
	s_U2CW *= s_bases[6];
	s_U2CW *= s_bases[7];

	s_U2CCW.reset();
	s_U2CCW *= s_bases[11];
	s_U2CCW *= s_bases[10];

	s_DCW.reset();
	s_DCW *= s_bases[9];

	s_DCCW.reset();
	s_DCCW *= s_bases[8];

	s_D2CW.reset();
	s_D2CW *= s_bases[9];
	s_D2CW *= s_bases[10];

	s_D2CCW.reset();
	s_D2CCW *= s_bases[8];
	s_D2CCW *= s_bases[7];

	s_FCW.reset();
	s_FCW *= s_bases[12];

	s_FCCW.reset();
	s_FCCW *= s_bases[17];

	s_F2CW.reset();
	s_F2CW *= s_bases[12];
	s_F2CW *= s_bases[13];

	s_F2CCW.reset();
	s_F2CCW *= s_bases[17];
	s_F2CCW *= s_bases[16];

	s_BCW.reset();
	s_BCW *= s_bases[15];

	s_BCCW.reset();
	s_BCCW *= s_bases[14];

	s_B2CW.reset();
	s_B2CW *= s_bases[15];
	s_B2CW *= s_bases[16];

	s_B2CCW.reset();
	s_B2CCW *= s_bases[14];
	s_B2CCW *= s_bases[13];

	s_XCW.reset();
	s_XCW *= s_bases[0];
	s_XCW *= s_bases[1];
	s_XCW *= s_bases[2];

	s_XCCW.reset();
	s_XCCW *= s_bases[3];
	s_XCCW *= s_bases[4];
	s_XCCW *= s_bases[5];

	s_YCW.reset();
	s_YCW *= s_bases[6];
	s_YCW *= s_bases[7];
	s_YCW *= s_bases[8];

	s_YCCW.reset();
	s_YCCW *= s_bases[9];
	s_YCCW *= s_bases[10];
	s_YCCW *= s_bases[11];

	s_ZCW.reset();
	s_ZCW *= s_bases[12];
	s_ZCW *= s_bases[13];
	s_ZCW *= s_bases[14];

	s_ZCCW.reset();
	s_ZCCW *= s_bases[15];
	s_ZCCW *= s_bases[16];
	s_ZCCW *= s_bases[17];

	s_MCW.reset();
	s_MCW *= s_bases[4];

	s_MCCW.reset();
	s_MCCW *= s_bases[1];

	s_ECW.reset();
	s_ECW *= s_bases[10];

	s_ECCW.reset();
	s_ECCW *= s_bases[7];

	s_SCW.reset();
	s_SCW *= s_bases[13];

	s_SCCW.reset();
	s_SCCW *= s_bases[16];

	s_op_map['R'] = { &s_RCW, &s_RCCW }; s_op_map['r'] = { &s_R2CW, &s_R2CCW };
	s_op_map['L'] = { &s_LCW, &s_LCCW }; s_op_map['l'] = { &s_L2CW, &s_L2CCW };
	s_op_map['U'] = { &s_UCW, &s_UCCW }; s_op_map['u'] = { &s_U2CW, &s_U2CCW };
	s_op_map['D'] = { &s_DCW, &s_DCCW }; s_op_map['d'] = { &s_D2CW, &s_D2CCW };
	s_op_map['F'] = { &s_FCW, &s_FCCW }; s_op_map['f'] = { &s_F2CW, &s_F2CCW };
	s_op_map['B'] = { &s_BCW, &s_BCCW }; s_op_map['b'] = { &s_B2CW, &s_B2CCW };
	s_op_map['x'] = { &s_XCW, &s_XCCW }; s_op_map['y'] = { &s_YCW, &s_YCCW }; s_op_map['z'] = { &s_ZCW, &s_ZCCW };
	s_op_map['E'] = { &s_ECW, &s_ECCW }; s_op_map['M'] = { &s_MCW, &s_MCCW }; s_op_map['S'] = { &s_SCW, &s_SCCW };
}

void RubiksCube::parse_seq(const std::string& seq, std::vector<const RubiksCube*>& operations, std::vector<std::string>& notes, std::vector<int>& groups, bool reverse)
{
	operations.clear();
	notes.clear();
	groups.clear();
	int group_id = 0;
	bool in_group = false;
	for (size_t i = 0; i < seq.length(); i++)
	{
		char c = seq[i];
		const RubiksCube* op = nullptr;
		std::string note;

		auto iter = s_op_map.find(c);
		if (iter != s_op_map.end())
		{
			if (i < seq.length() - 1 && (seq[i + 1] == '\'' || seq[i + 1] == '`'))
			{
				if (reverse)
				{
					op = iter->second.first;
					note = std::string(1, c);
				}
				else
				{
					op = iter->second.second;
					note = std::string(1, c) + "\'";
				}
				i++;
			}
			else
			{
				if (reverse)
				{
					op = iter->second.second;
					note = std::string(1, c) + "\'";
				}
				else
				{
					op = iter->second.first;
					note = std::string(1, c);
				}
			}
			int count = 1;
			if (i < seq.length() - 1 && seq[i + 1] == '2')
			{
				count = 2;
				i++;
			}

			if (!in_group) group_id++;

			for (int j = 0; j < count; j++)
			{
				operations.push_back(op);
				notes.push_back(note);
				groups.push_back(group_id);
			}
		}
		else if (c == '(')
		{
			in_group = true;
			group_id++;
		}
		else if (c == ')')
		{
			in_group = false;
		}
	}

	if (reverse)
	{
		std::reverse(operations.begin(), operations.end());
		std::reverse(notes.begin(), notes.end());
		std::reverse(groups.begin(), groups.end());
	}
}

void RubiksCube::exec_seq(const std::string& seq, bool reverse)
{
	std::vector<const RubiksCube*> operations;
	std::vector<std::string> notes;
	std::vector<int> groups;
	parse_seq(seq, operations, notes, groups, reverse);
	for (size_t i = 0; i < operations.size(); i++)
		(*this) *= *operations[i];
}

