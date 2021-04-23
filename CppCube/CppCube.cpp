#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <ctime>
#include <memory.h>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <vector>
#include <queue>
#include <fstream>
#include <conio.h>
#include "base_data.hpp"
#include "crc64.h"
#include "json.hpp"

#define MODE_PREPARE 1
#define MODE_DISCOVER 2
#define MODE_PACK_BASEDATA 3
#define MODE_STOCHASTIC_SEARCH 4

#define MODE MODE_DISCOVER

using json = nlohmann::json;

class RubiksCube
{
public:
	uint8_t map[6 * 9];
	uint8_t dirs[6 * 9];

	RubiksCube()
	{
		reset();
	}

	RubiksCube(const RubiksCube& in)
	{
		memcpy(map, in.map, sizeof(map));
		memcpy(dirs, in.dirs, sizeof(dirs));
	}

	void reset()
	{
		for (uint8_t i = 0; i < 6 * 9; i++)
		{
			map[i] = i;
			dirs[i] = 0;
		}
	}

	const RubiksCube& operator=(const RubiksCube& in)
	{
		memcpy(map, in.map, sizeof(map));
		memcpy(dirs, in.dirs, sizeof(dirs));
		return *this;
	}
	
	const RubiksCube& operator*=(const RubiksCube& b)
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

	const RubiksCube& operator/=(const RubiksCube& b)
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

	bool equals(const RubiksCube& b, bool include_dirs = true) const
	{
		for (uint8_t i = 0; i < 6 * 9; i++)
		{
			if (map[i] != b.map[i]) return false;
			if (include_dirs && dirs[i] != b.dirs[i]) return false;
		}
		return true;
	}

	bool partial_equals(const RubiksCube& b, const uint8_t* mask, bool include_dirs = true) const
	{
		for (uint8_t i = 0; i < 6 * 9; i++)
		{
			if (mask[i] == 0) continue;
			if (map[i] != b.map[i]) return false;
			if (include_dirs && dirs[i] != b.dirs[i]) return false;
		}
		return true;
	}

	void print()
	{
		for (int i = 0; i < 54; i++)
			printf("%u ", (unsigned)map[i]);
		printf("\n");
		for (int i = 0; i < 54; i++)
			printf("%u ", (unsigned)dirs[i]);
		printf("\n");
	}

	void to_json(const char* fn) const
	{
		json j_map;
		json j_dirs;
		for (int i = 0; i < 54; i++)
		{
			j_map[i] = map[i];
			j_dirs[i] = dirs[i];
		}
		json j;
		j["map"] = j_map;
		j["dirs"] = j_dirs;
		std::ofstream fo(fn);
		fo << j;
	}

	void from_json(const char* fn)
	{
		std::ifstream fi(fn);
		json j;
		fi >> j;
		json j_map = j["map"];
		json j_dirs = j["dirs"];
		for (int i = 0; i < 54; i++)
		{
			map[i] = j_map[i];
			dirs[i] = j_dirs[i];
		}
	}

	static void s_to_json(const RubiksCube* cubes, int count, const char* fn)
	{	
		
		json j_root;
		for (int i = 0; i < count; i++)
		{
			json j_map;
			json j_dirs;
			for (int j = 0; j < 54; j++)
			{
				j_map[j] = cubes[i].map[j];
				j_dirs[j] = cubes[i].dirs[j];
			}
			json j_cube;
			j_cube["map"] = j_map;
			j_cube["dirs"] = j_dirs;
			j_root[i] = j_cube;
		}
		std::ofstream fo(fn);
		fo << j_root;
	}

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

	static void s_Initialize()
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

	static void parse_seq(const std::string& seq, std::vector<const RubiksCube*>& operations, std::vector<std::string>& notes, std::vector<int>& groups, bool reverse = false)
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

	void exec_seq(const std::string& seq, bool reverse = false)
	{
		std::vector<const RubiksCube*> operations;
		std::vector<std::string> notes;
		std::vector<int> groups;
		parse_seq(seq, operations, notes, groups, reverse);
		for (size_t i = 0; i < operations.size(); i++)
			(*this) *= *operations[i];
	}

};

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

uint64_t encode_seq(const std::vector<uint8_t>& seq)
{
	uint64_t res = 0;
	uint64_t unit = 1;
	for (size_t i = 0; i < seq.size(); i++)
	{
		res += seq[i] * unit;		
		unit *= 13;
	}
	return res;
}

std::vector<uint8_t> decode_seq(uint64_t code)
{
	std::vector<uint8_t> seq;
	while (code != 0)
	{
		seq.push_back(code % 13);
		code = code / 13;
	}
	return seq;
}

void reverse_seq(std::vector<uint8_t>& seq)
{
	std::reverse(seq.begin(), seq.end());
	for (size_t i = 0; i < seq.size(); i++)
	{
		int x = (seq[i] - 1) / 2;
		int y = (seq[i] - 1) % 2;
		seq[i] = x * 2 + (1 - y) + 1;
	}
}

std::string seq2str(const std::vector<uint8_t>& seq)
{
	static std::string s_ops[] = { "", "R","R\'","L","L\'","U","U\'","D","D\'","F","F\'","B","B\'" };
	std::string str;
	for (size_t i = 0; i < seq.size(); i++)
		str += s_ops[seq[i]];
	return str;
}

#if MODE == MODE_PREPARE
int main()
{
	RubiksCube::s_Initialize();

	const RubiksCube* ops[] =
	{
		&RubiksCube::s_cube_o,
		&RubiksCube::s_RCW,
		&RubiksCube::s_RCCW,
		&RubiksCube::s_LCW,
		&RubiksCube::s_LCCW,
		&RubiksCube::s_UCW,
		&RubiksCube::s_UCCW,
		&RubiksCube::s_DCW,
		&RubiksCube::s_DCCW,
		&RubiksCube::s_FCW,
		&RubiksCube::s_FCCW,
		&RubiksCube::s_BCW,
		&RubiksCube::s_BCCW
	};

	struct Record
	{
		uint64_t code;
		RubiksCube state;
	};

	std::unordered_set<uint64_t> reached;
	std::queue<Record> active;

	FILE* fp = fopen("Records_8.dat", "wb");

	uint64_t hs = crc64(0, RubiksCube::s_cube_o.map, 54);
	uint64_t code = 0;
	reached.insert(hs);
	active.push({ code, RubiksCube::s_cube_o });
	fwrite(&hs, sizeof(uint64_t), 1, fp);
	fwrite(&code, sizeof(uint64_t), 1, fp);

	int max_level = 8;
	int level = 0;
	int code_level_min = 0;	

	while (active.size() > 0)
	{
		Record suffix = active.front();						
		active.pop();
		if (suffix.code >= code_level_min)
		{
			level++;
			code_level_min = code_level_min * 13 + 1;
			printf("Scanning level %d.\n", level);
		}

		bool is_last = level == max_level;
		
		int max_id = is_last ? 3 : 13;
		for (int id = 1; id < max_id; id++)
		{
			uint64_t code = id + suffix.code * 13;
			RubiksCube cube = *ops[id];
			cube *= suffix.state;

			hs = crc64(0, cube.map, 54);
			auto iter = reached.find(hs);
			if (iter == reached.end())
			{
				reached.insert(hs);
				if (id < 3)
				{
					fwrite(&hs, sizeof(uint64_t), 1, fp);
					fwrite(&code, sizeof(uint64_t), 1, fp);
				}
				if (!is_last)
				{
					active.push({ code, cube });
				}
			}

		}
	}	
	fclose(fp);

	return 0;
}

#endif


#if MODE == MODE_DISCOVER
int main()
{
	RubiksCube::s_Initialize();

	printf("Calculating OLL/PLL cases\n");

	std::string pll_seqs[] =
	{
		"",
		"(L'UR'U2)(LU'L'U2)(LRU')",
		"(RU'LU2)(R'URU2)(R'L'U)",
		"z'(UL'UL)(ULUL')(U'L'U2)z",
		"z(U'RU'R')(U'R'U'R)(URU2)z'",	
		"(RU2)(R'U2)(RB'R'U')(RURBR'2U)",
		"(L'U2)(LU2)(L'BLU)(L'U'L'B'L2U')",
		"(r'2R2U)(r'2R2U2)(r'2R2U)(r'2R2)",
		"(R'U')(R'FRF')(UR)b'(R'U'RU)b",
		"x'R2D2(R'U'R)D2(R'UR')x",
		"(lU'R)D2(R'UR)D2R'2x",
		"x'(RU'R'D)(RUR'D')RUR'z'(RU)(L'U'R')zx",
		"(R'U'RU)(RB'R'2U)(RUR'U')(RB)",
		"B'(R'URUR'U')(RB)(R'U'RU)(RB'R'B)",
		"(U'R'U)(RU'R'l')d'(R'UR)y'(RUR'U'R2)x'",
		"(R'Ul'f')'x(R'Ul'f')U2(R'U'RUR)x",
		"(L'UR'U2)(LU'L')(RUR'U2)(LU'RU')",
		"(RU'LU2)(R'UR)(L'U'LU2)(R'UL'U)",
		"(RUR')y'(R2u'RU')(R'UR'uR'2)y",
		"y'(R2u'RU'R)(UR'uR'2)y(RU'R')",
		"(L'U'L)y'(R'2uR'U)(RU'Ru'R2)y",
		"y'(R'2uR'UR')(U'Ru'R2)y(L'UL)"
	};

	int count_pll_seqs = (int)(sizeof(pll_seqs) / sizeof(std::string));

	std::string oll_seqs[] =
	{
		"(R U'U')(R2' F R F')U2(R' F R F')",
		"(F R U R' U' F')(f R U R' U' f')",
		"f(RUR'U')f'U'F(RUR'U')F'",
		"f(RUR'U')yx(R'F)(RUR'U')F'y'",
		"(r' U2)(R U R'U) r",
		"(rU'U')(R'U'RU'r')",
		"rUR'URU'U'r'",
		"r'U'RU'R'U2r",
		"(R' U' R) y' x' (R U')(R'F)(R U R') x y",
		"(RUR'U)(R'FRF')(RU'U'R')",
		"r'(R2UR'U)(RU'U'R'U)(rR')",
		"(rR'2U'RU')(R'U2RU'R)r'",
		"(rU'r'U')(rUr')(F'UF)",
		"R' F R U R' F'R (F U' F')",
		"(r' U' r)(R'U'R U)(r' U r)",
		"(r U r')(R U R' U')(r U' r')",
		"(R U R' U)(R' F R F'U2)R' F R F'",
		"F (R U R' d)(R' U2)(R' F R F') y",
		"R' U2 F R U R'U' y'R2 U'U' R B y",
		"r'(R U)(R U R'U' r2)(R2'U) (R U') r'",
		"(R U'U')(R' U' R U R' U') (R U' R')",
		"R U'U' (R'2 U')(R2 U')R'2 U' U'R",
		"(R2 D') (R U'U') (R' D) (R U'U' R)",
		"x' R U R' z'(R U)(L' U' R') x y",
		"F'(rUR'U')(r'FR)",
		"R U'U' R' U'R U' R'",
		"R' U2 R U R' U R",
		"(r U R' U')(r' R U)(R U' R')",
		"(R U R' U')(R U' R' F' U' F)(R U R')",
		"(R2 U R' B')(RU')(R2' U)(R B R')",
		"(r'F'UF)(LF'L'U'r)",
		"(RU)(B'U')(R'URBR')",
		"(R U R' U')(R' F R F')",
		"(R'U'R U) x' z'(R U)(L'U')r R' y",
		"R U'U'R2' F R F'(R U'U'R')",
		"R'U'R U' R'U R U l U'R'U x",
		"F (R U' R'U'R U) (R' F')",
		"(R U R'U)(RU'R'U')(R'F R F')",
		"(r U' r' U' r)y(R U R' f') y'",
		"(R' F R U R'U')(F' U R)",
		"RU'R'U2RUyRU'R'U'F'y'",
		"(R'U2)(R U R' U R2)y(R U R' U')F'y'",
		"(B' U')(R' U R B)",
		"f (R U R' U')f'",
		"F(RUR'U')F'",
		"(R' U') R' F R F' (U R)",
		"B'(R' U' R U) (R' U' R U) B",
		"F (R U R' U') (R U R' U') F'",
		"R B'(R2 F)(R2 B) R2 F' R",
		"L'B (L2 F')(L2B')L2 F L'",
		"f (R U R' U') (R U R' U') f'",
		"R'U' R U' R' d R' U l U x y",
		"(r' U2)(R U R'U')(R U R'U) r ",
		"(r U'U')(R' U' R U R' U')(R U' r')",
		"(R U'U') (R'2 U') R U' R'U2 (F R F')",
		"F (R U R'U')(R F')(r U R'U')r'",
		"(R U R' U' r)(R' U)(R U' r')"
	};

	int count_oll_seqs = (int)(sizeof(oll_seqs) / sizeof(std::string));	
	
	std::vector<RubiksCube> pll_lst;
	std::vector<std::vector<RubiksCube>> oll_lsts(count_oll_seqs);

	{
		std::unordered_map<uint64_t, RubiksCube> collection;
		for (int i = 0; i < count_pll_seqs; i++)
		{
			RubiksCube cube_pll;
			cube_pll.exec_seq(pll_seqs[i], true);

			RubiksCube cube_mod;

			for (int j = 0; j < 4; j++)
			{
				RubiksCube cube = cube_mod;
				cube *= cube_pll;

				for (int k = 0; k < 4; k++)
				{
					uint64_t hs = crc64(0, cube.map, 54);
					auto iter = collection.find(hs);
					if (iter == collection.end())
					{
						collection[hs] = cube;
						pll_lst.push_back(cube);
					}
					cube *= RubiksCube::s_UCW;
				}
				cube_mod *= RubiksCube::s_UCW;
			}
		}

		for (int i = 0; i < count_oll_seqs; i++)
		{
			for (size_t j = 0; j < pll_lst.size(); j++)
			{
				RubiksCube cube = pll_lst[j];
				cube.exec_seq(oll_seqs[i], true);

				for (int k = 0; k < 4; k++)
				{
					uint64_t hs = crc64(0, cube.map, 54);
					auto iter = collection.find(hs);
					if (iter == collection.end())
					{
						collection[hs] = cube;
						oll_lsts[i].push_back(cube);
					}
					cube *= RubiksCube::s_UCW;
				}
			}
		}
	}

#if 0
	printf("%d\n", pll_lst.size());
	RubiksCube::s_to_json(pll_lst.data(), (int)pll_lst.size(), "../../all_pll.json");

	std::vector<RubiksCube> oll_v = pll_lst;
	for (int i = 0; i < count_oll_seqs; i++)
	{
		std::vector<RubiksCube>& sub_lst = oll_lsts[i];
		for (size_t j = 0; j < sub_lst.size(); j++)
			oll_v.push_back(sub_lst[j]);
	}

	printf("%d\n", oll_v.size());
	RubiksCube::s_to_json(oll_v.data(), (int)oll_v.size(), "../../all_oll.json");
#endif

	size_t count_oll = 0;
	for (int i = 0; i < count_oll_seqs; i++)
	{
		std::vector<RubiksCube>& sub_lst = oll_lsts[i];
		count_oll += sub_lst.size();
	}

	size_t found_pll = 0;
	size_t found_oll = 0;

	printf("Loading prefixes\n");
	

	const RubiksCube* ops[] =
	{
		&RubiksCube::s_cube_o,
		&RubiksCube::s_RCW,
		&RubiksCube::s_RCCW,
		&RubiksCube::s_LCW,
		&RubiksCube::s_LCCW,
		&RubiksCube::s_UCW,
		&RubiksCube::s_UCCW,
		&RubiksCube::s_DCW,
		&RubiksCube::s_DCCW,
		&RubiksCube::s_FCW,
		&RubiksCube::s_FCCW,
		&RubiksCube::s_BCW,
		&RubiksCube::s_BCCW
	};
	
	std::unordered_map<uint64_t, uint64_t> prefix_map;
	{
		FILE* fp = fopen("Records_8.dat", "rb");
		fseek(fp, 0, SEEK_END);
		size_t len = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		while (ftell(fp) < len)
		{
			uint64_t hs, code;
			fread(&hs, sizeof(uint64_t), 1, fp);
			fread(&code, sizeof(uint64_t), 1, fp);
			prefix_map[hs] = code;
		}
		fclose(fp);
	}

	std::vector<std::vector<uint8_t>> opt_seqs_pll(pll_lst.size());
	std::vector<std::vector<std::vector<uint8_t>>> opt_seqs_oll(count_oll_seqs);
	for (int i = 0; i < count_oll_seqs; i++)
	{
		std::vector<RubiksCube>& sub_lst = oll_lsts[i];
		opt_seqs_oll[i].resize(sub_lst.size());
	}

	struct Record
	{
		uint64_t code;
		RubiksCube state;
	};

	std::unordered_set<uint64_t> reached;
	std::queue<Record> active;

	uint64_t hs = crc64(0, RubiksCube::s_cube_o.map, 54);
	uint64_t code = 0;
	reached.insert(hs);
	active.push({ code, RubiksCube::s_cube_o });

	printf("Scanning, sub-8 moves\n");
	for (size_t j = 0; j < pll_lst.size(); j++)
	{
		RubiksCube cube_dst = pll_lst[j];
		uint64_t hs = crc64(0, cube_dst.map, 54);
		auto iter = prefix_map.find(hs);
		if (iter != prefix_map.end())
		{			
			auto seq = decode_seq(iter->second);		
			reverse_seq(seq);
			if (seq.size()<1 || seq[0] == 5 || seq[0] == 6)
			{
				opt_seqs_pll[j] = { 0 };
			}
			else
			{
				opt_seqs_pll[j] = seq;
				std::string str = seq2str(seq);
				printf("pll %d: %s\n", j, str.c_str());
			}
			found_pll++;
		}
	}

	for (int i = 0; i < count_oll_seqs; i++)
	{
		std::vector<RubiksCube>& sub_lst = oll_lsts[i];
		std::vector<std::vector<uint8_t>>& sub_lst_seq = opt_seqs_oll[i];
		for (size_t j = 0; j < sub_lst.size(); j++)
		{
			RubiksCube cube_dst = sub_lst[j];
			uint64_t hs = crc64(0, cube_dst.map, 54);
			auto iter = prefix_map.find(hs);
			if (iter != prefix_map.end())
			{				
				auto seq = decode_seq(iter->second);
				reverse_seq(seq);

				if (seq.size() < 1 || seq[0] == 5 || seq[0] == 6)
				{
					sub_lst_seq[j] = { 0 };
				}
				else
				{
					sub_lst_seq[j] = seq;
					std::string str = seq2str(seq);
					printf("oll %d: %s\n", i + 1, str.c_str());
				}
				found_oll++;
			}
		}
	}

	int level = 0;
	int code_level_min = 0;
	while (active.size() > 0)
	{
		if (_kbhit() != 0 && _getch() == 27) break;
		printf("pll: %zu/%zu oll:%zu/%zu\n", found_pll, pll_lst.size(), found_oll, count_oll);
		
		Record suffix = active.front();
		active.pop();
		if (suffix.code >= code_level_min)
		{
			level++;
			code_level_min = code_level_min * 13 + 1;
			printf("Scanning, %d moves.\n", 8 + level);
		}
		for (int id = 1; id < 13; id++)
		{
			uint64_t code = id + suffix.code * 13;
			RubiksCube cube = *ops[id];
			cube *= suffix.state;

			hs = crc64(0, cube.map, 54);
			auto iter = reached.find(hs);
			if (iter == reached.end())
			{
				reached.insert(hs);				
				active.push({ code, cube });				

				for (size_t j = 0; j < pll_lst.size(); j++)
				{
					if (opt_seqs_pll[j].size()>0) continue;
					RubiksCube cube_dst = pll_lst[j];
					cube_dst *= cube;
					uint64_t hs = crc64(0, cube_dst.map, 54);
					auto iter = prefix_map.find(hs);
					if (iter != prefix_map.end())
					{
						auto suffix = decode_seq(code);						
						auto seq = decode_seq(iter->second);
						if (seq.size() < 1 || suffix[0] == 5 || suffix[0] == 6)
						{
							opt_seqs_pll[j] = { 0 };
						}
						else
						{
							reverse_seq(seq);
							suffix.insert(suffix.end(), seq.begin(), seq.end());
							opt_seqs_pll[j] = suffix;
							std::string str = seq2str(suffix);
							printf("pll %d: %s\n", j, str.c_str());
						}											
						found_pll++;
					}
				}

				for (int i = 0; i < count_oll_seqs; i++)
				{
					std::vector<RubiksCube>& sub_lst = oll_lsts[i];
					std::vector<std::vector<uint8_t>>& sub_lst_seq = opt_seqs_oll[i];
					for (size_t j = 0; j < sub_lst.size(); j++)
					{
						if (sub_lst_seq[j].size()>0) continue;
						RubiksCube cube_dst = sub_lst[j];
						cube_dst *= cube;						
						uint64_t hs = crc64(0, cube_dst.map, 54);
						auto iter = prefix_map.find(hs);
						if (iter != prefix_map.end())
						{													
							auto suffix = decode_seq(code);
							auto seq = decode_seq(iter->second);
							if (seq.size() < 1 || suffix[0] == 5 || suffix[0] == 6)
							{
								sub_lst_seq[j] = { 0 };
							}
							else
							{
								reverse_seq(seq);
								suffix.insert(suffix.end(), seq.begin(), seq.end());
								sub_lst_seq[j] = suffix;
								std::string str = seq2str(suffix);
								printf("oll %d: %s\n", i + 1, str.c_str());
							}
							found_oll++;
						}
					}
				}
			}
		}
	}

	struct compare {
		inline bool operator()(const std::vector<uint8_t>& first,
			const std::vector<uint8_t>& second) const
		{
			return first.size() < second.size();
		}
	} c;	

	{
		FILE* fp = fopen("found_seqs.txt", "w");

		fprintf(fp, "PLL:\n");
		std::sort(opt_seqs_pll.begin(), opt_seqs_pll.end(), c);
		for (size_t j = 0; j < opt_seqs_pll.size(); j++)
		{
			if (opt_seqs_pll[j].size()>0 && opt_seqs_pll[j][0]>0)
			{				
				std::string str = seq2str(opt_seqs_pll[j]);
				fprintf(fp, "%s\n", str.c_str());
			}
		}
		fprintf(fp, "\n");

		for (int i = 0; i < count_oll_seqs; i++)
		{
			fprintf(fp, "OLL %d:\n", i + 1);
			std::vector<std::vector<uint8_t>>& sub_lst_seq = opt_seqs_oll[i];
			std::sort(sub_lst_seq.begin(), sub_lst_seq.end(), c);
			for (size_t j = 0; j < sub_lst_seq.size(); j++)
			{
				if (sub_lst_seq[j].size()>0 && sub_lst_seq[j][0]>0)
				{
					std::string str = seq2str(sub_lst_seq[j]);
					fprintf(fp, "%s\n", str.c_str());
				}
			}
			fprintf(fp, "\n");
		}

		fclose(fp);
	}
		

	return 0;
}
#endif

#if MODE == MODE_PACK_BASEDATA

void pack(const char* fn_data, const char* fn_hpp, const char* name_arr)
{
	FILE *fp_hpp = fopen(fn_hpp, "w");

	FILE *fp_data = fopen(fn_data, "rb");
	fseek(fp_data, 0, SEEK_END);
	size_t size = (size_t)ftell(fp_data);
	fseek(fp_data, 0, SEEK_SET);
	std::vector<char> buf((size + 3)&(~3));
	fread(buf.data(), 1, size, fp_data);
	fclose(fp_data);

	fprintf(fp_hpp, "const static size_t %s_size = %llu;\n", name_arr, size);

	size_t num_dwords = buf.size() >> 2;
	unsigned *dwords = (unsigned*)buf.data();

	fprintf(fp_hpp, "static unsigned %s[]={\n", name_arr);
	for (size_t j = 0; j < num_dwords; j++)
	{
		fprintf(fp_hpp, "0x%x,", dwords[j]);
		if (j % 10 == 9)
			fputs("\n", fp_hpp);
	}
	fputs("};\n\n", fp_hpp);

	fclose(fp_hpp);

}

int main()
{
	pack("base.data", "base_data.hpp", "base_data");
}
#endif

#if MODE == MODE_STOCHASTIC_SEARCH
int main()
{
	srand((unsigned)time(nullptr));
	RubiksCube::s_Initialize();

	uint8_t oll_mask[54];
	memset(oll_mask, 0, 54);
	memset(oll_mask + 3, 1, 6);
	memset(oll_mask + 12, 1, 6);
	memset(oll_mask + 27, 1, 9);
	memset(oll_mask + 39, 1, 6);
	memset(oll_mask + 48, 1, 6);

	std::unordered_set<uint64_t> keys;

	{
		RubiksCube cube;
		keys.insert(crc64(0, cube.map, 54));
		cube *= RubiksCube::s_UCW;
		keys.insert(crc64(0, cube.map, 54));
		cube *= RubiksCube::s_UCW;
		keys.insert(crc64(0, cube.map, 54));
		cube *= RubiksCube::s_UCW;
		keys.insert(crc64(0, cube.map, 54));
	}

	const RubiksCube* ops[] =
	{
		&RubiksCube::s_RCW,
		&RubiksCube::s_RCCW,
		&RubiksCube::s_LCW,
		&RubiksCube::s_LCCW,
		&RubiksCube::s_UCW,
		&RubiksCube::s_UCCW,
		&RubiksCube::s_DCW,
		&RubiksCube::s_DCCW,
		&RubiksCube::s_FCW,
		&RubiksCube::s_FCCW,
		&RubiksCube::s_BCW,
		&RubiksCube::s_BCCW
	};

	std::string s_ops[] = { "R","R\'","L","L\'","U","U\'","D","D\'","F","F\'","B","B\'" };

	while (true)
	{
		RubiksCube cube;
		std::string seq = "";
		for (int i = 0; i < 14; i++)
		{
			int last_j;
			int j;
			if (i == 0)
			{
				j = rand() % 2;
			}
			else
			{
				while (true)
				{
					j = rand() % 12;
					if (j / 2 != last_j / 2 || j % 2 == last_j % 2) break;
				}
			}
			
			cube *= *ops[j];
			seq += s_ops[j];
			last_j = j;
			if (cube.partial_equals(RubiksCube::s_cube_o, oll_mask, false))
			{
				uint64_t hs = crc64(0, cube.map, 54);
				auto iter = keys.find(hs);
				if (iter == keys.end())
				{
					keys.insert(hs);
					cube *= RubiksCube::s_UCW;
					keys.insert(crc64(0, cube.map, 54));
					cube *= RubiksCube::s_UCW;
					keys.insert(crc64(0, cube.map, 54));
					cube *= RubiksCube::s_UCW;
					keys.insert(crc64(0, cube.map, 54));

					printf("%s\n", seq.c_str());
				}
				break;
			}

		}

	}	

	return 0;
}

#endif
