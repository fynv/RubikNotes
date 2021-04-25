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
#include "crc64.h"
#include "json.hpp"

#define MODE_PACK_BASEDATA 1
#define MODE_STOCHASTIC_SEARCH 100

#define MODE_PLL_PREPARE 2
#define MODE_PLL_DISCOVER 3

#define MODE_OLL_PREPARE 4
#define MODE_OLL_DISCOVER 5

#define MODE MODE_PLL_DISCOVER

#if MODE!=MODE_PACK_BASEDATA
#include "base_data.hpp"

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

	uint64_t hash() const
	{
		return crc64(0, map, 54);
	}

	uint64_t hash_oll() const
	{
		uint8_t tmp[54];
		for (int i = 0; i < 54; i++)
		{
			uint8_t v = map[i];
			uint8_t x = v / 9;
			uint8_t y = v % 9;
			if (x == 2)
			{
				if (y == 0 || y == 2 || y == 6 || y == 8)
				{
					v = (uint8_t)(-1);
				}
				else if (y == 1 || y == 3 || y == 5 || y == 7)
				{
					v = (uint8_t)(-2);
				}
			}
			else if (x == 0 || x == 1 || x == 4 || x == 5)
			{
				if (y == 0) v = (uint8_t)(-4);
				if (y == 1) v = (uint8_t)(-5);
				if (y == 2) v = (uint8_t)(-3);
			}
			tmp[i] = v;
		}
		return crc64(0, tmp, 54);
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

std::unordered_set<int> get_avoid_ids(int id)
{
	std::unordered_set<int> ret;
	if (id <= 0) return ret;
	id--;
	if (id % 2 == 0)
	{
		ret.insert(id + 1);
	}
	if (id % 4 < 2)
	{
		int g = id / 4 * 4 + 1;
		ret.insert(g + 2);
		ret.insert(g + 3);
	}
	
	return ret;
}

std::unordered_set<int> get_avoid_ids2(int id)
{
	std::unordered_set<int> ret;
	if (id <= 0) return ret;
	id--;
	if (id % 2 == 1)
	{
		ret.insert(id + 1);
	}
	if (id % 4 >= 2)
	{
		int g = id / 4 * 4 + 1;
		ret.insert(g);
		ret.insert(g + 1);
	}
	
	return ret;

}

#endif // MODE!=MODE_PACK_BASEDATA

#if MODE == MODE_PLL_PREPARE
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

	std::unordered_map<uint64_t, uint64_t> reached;
	std::queue<Record> active;

	FILE* fp = fopen("PLL8.dat", "wb");

	uint64_t hs = RubiksCube::s_cube_o.hash();
	uint64_t code = 0;
	reached[hs] = 0;
	active.push({ code, RubiksCube::s_cube_o });

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

		auto avoid_ids = get_avoid_ids(suffix.code%13);

		int max_id = is_last ? 3 : 13;
		for (int id = max_id-1; id>0; id--)
		{
			if (avoid_ids.find(id)!=avoid_ids.end()) continue;

			uint64_t code = id + suffix.code * 13;
			RubiksCube cube = *ops[id];
			cube *= suffix.state;

			hs = cube.hash();		
			auto iter = reached.find(hs);
			if (iter == reached.end() || iter->second==level)
			{
				if (!is_last)
				{
					reached[hs] = level;
					active.push({ code, cube });
				}
				if (is_last)
				{
					fwrite(&hs, sizeof(uint64_t), 1, fp);
					fwrite(&code, sizeof(uint64_t), 1, fp);
				}
				
			}
		}
	}

	fclose(fp);

	return 0;
}

#endif 

#if MODE == MODE_PLL_DISCOVER
int main()
{
	RubiksCube::s_Initialize();
	printf("Calculating PLL cases\n");

	std::string pll_seqs[] =
	{
		"z'(UL'UL)(ULUL')(U'L'U2)z",
		"z(U'RU'R')(U'R'U'R)(URU2)z'",
		"(r'2R2U)(r'2R2U2)(r'2R2U)(r'2R2)",
		"(R'U')(R'FRF')(UR)b'(R'U'RU)b",

		"x'R2D2(R'U'R)D2(R'UR')x",
		"(lU'R)D2(R'UR)D2R'2x",
		"x'(RU'R'D)(RUR'D')RUR'z'(RU)(L'U'R')zx",

		"(R'U'RU)(RB'R'2U)(RUR'U')(RB)",
		"(R'U)(RU'R'l')d'(R'UR)y'(RUR'U'R2)x'",
		"(R'Ul'f')'x(R'Ul'f')U2(R'U'RUR)x",
		"B'(R'URUR'U')(RB)(R'U'RU)(RB'R'B)",

		"(L'UR'U2)(LU'L'U2)(LRU')",
		"(RU'LU2)(R'URU2)(R'L'U)",
		"(L'U2)(LU2)(L'BLU)(L'U'L'B'L2U')",
		"(RU2)(R'U2)(RB'R'U')(RURBR'2U)",

		"y'(R2u'RU'R)(UR'uR'2)y(RU'R')",
		"(RUR')y'(R2u'RU')(R'UR'uR'2)y",
		"y'(R'2uR'UR')(U'Ru'R2)y(L'UL)",
		"(L'U'L)y'(R'2uR'U)(RU'Ru'R2)y",

		"(RU'LU2)(R'UR)(L'U'LU2)(R'UL')",
		"(L'UR'U2)(LU'L')(RUR'U2)(LU'R)",
	};

	int count_pll_seqs = (int)(sizeof(pll_seqs) / sizeof(std::string));

	std::vector<std::vector<RubiksCube>> pll_lsts(count_pll_seqs);

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
				cube /= cube_mod;

				uint64_t hs = cube.hash();
				auto iter = collection.find(hs);
				if (iter == collection.end())
				{
					collection[hs] = cube;
					pll_lsts[i].push_back(cube);
				}				
				cube_mod *= RubiksCube::s_UCW;
			}
		}
	}

	size_t found_pll = 0;

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

	std::unordered_map<uint64_t, std::vector<uint64_t>> prefix_map;
	{
		FILE* fp = fopen("PLL8.dat", "rb");
		fseek(fp, 0, SEEK_END);
		size_t len = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		while (ftell(fp) < len)
		{
			uint64_t hs, code;
			fread(&hs, sizeof(uint64_t), 1, fp);
			fread(&code, sizeof(uint64_t), 1, fp);
			prefix_map[hs].push_back(code);
		}
		fclose(fp);
	}

	std::vector<int> pll_level(count_pll_seqs, INT_MAX);
	std::vector<std::vector<std::vector<uint8_t>>> opt_seqs_pll(count_pll_seqs);

	struct Record
	{
		uint64_t code;
		RubiksCube state;
	};
	
	std::unordered_map<uint64_t, uint64_t> reached;
	std::queue<Record> active;

	uint64_t hs = RubiksCube::s_cube_o.hash();
	uint64_t code = 0;
	reached[hs] = 0;
	active.push({ code, RubiksCube::s_cube_o });

	int max_level = 8;
	int level = 0;
	int code_level_min = 0;
	while (active.size() > 0)
	{
		if (_kbhit() != 0 && _getch() == 27) break;

		Record suffix = active.front();
		active.pop();
		if (suffix.code >= code_level_min)
		{
			if (found_pll == count_pll_seqs) break;
			level++;			
			code_level_min = code_level_min * 13 + 1;
			printf("Scanning, %d moves.\n", 8 + level);
		}

		bool is_last = level == max_level;

		auto avoid_ids = get_avoid_ids2(suffix.code%13);

		for (int id = 1; id < 13; id++)
		{
			if (avoid_ids.find(id) != avoid_ids.end()) continue;

			uint64_t code = id + suffix.code * 13;
			RubiksCube cube = *ops[id];
			cube *= suffix.state;

			hs = cube.hash();
			auto iter = reached.find(hs);
			if (iter == reached.end() || iter->second == level)
			{
				if (!is_last)
				{
					reached[hs] = level;
					active.push({ code, cube });
				}

				bool has_found = false;
				for (int i = 0; i < count_pll_seqs; i++)
				{
					if (pll_level[i] < level) continue;

					std::vector<RubiksCube>& sub_lst = pll_lsts[i];
					for (size_t j = 0; j < sub_lst.size(); j++)
					{
						RubiksCube cube_dst = sub_lst[j];
						cube_dst *= cube;
						hs = cube_dst.hash();
						auto iter2 = prefix_map.find(hs);
						if (iter2 != prefix_map.end())
						{
							std::vector<uint8_t> suffix = decode_seq(code);
							const std::vector<uint64_t>& prefixes = iter2->second;
							for (size_t k = 0; k < prefixes.size(); k++)
							{
								std::vector<uint8_t> prefix = decode_seq(prefixes[k]);
								reverse_seq(prefix);
								auto avoid_id2 = get_avoid_ids2((int)prefix[0]);
								if (avoid_id2.find((int)suffix[suffix.size() - 1]) != avoid_id2.end()) continue;
								std::vector<uint8_t> suffix_cpy = suffix;
								suffix_cpy.insert(suffix_cpy.end(), prefix.begin(), prefix.end());
								opt_seqs_pll[i].push_back(suffix_cpy);
								std::string str = seq2str(suffix_cpy);
								printf("pll %d: %s\n", i + 1, str.c_str());
							}

							if (pll_level[i] > level)
							{
								pll_level[i] = level;
								found_pll++;
								has_found = true;
							}							
						}
					}
				}
				if (has_found)
				{
					printf("%zu/%zu\n", found_pll, count_pll_seqs);
				}
			}
		}
	}

	
	{
		FILE* fp = fopen("found_pll.txt", "w");

		for (int i = 0; i < count_pll_seqs; i++)
		{
			fprintf(fp, "PLL %d:\n", i + 1);
			std::vector<std::vector<uint8_t>>& sub_lst_seq = opt_seqs_pll[i];		
			for (size_t j = 0; j < sub_lst_seq.size(); j++)
			{
				std::string str = seq2str(sub_lst_seq[j]);
				fprintf(fp, "%s\n", str.c_str());
			}
			fprintf(fp, "\n");
		}

		fclose(fp);
	}

	return 0;
}

#endif

#if MODE == MODE_OLL_PREPARE
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

	std::unordered_map<uint64_t, uint64_t> reached;
	std::unordered_map<uint64_t, uint64_t> level_map;	
	std::queue<Record> active;

	FILE* fp = fopen("OLL8.dat", "wb");

	uint64_t hs = RubiksCube::s_cube_o.hash();
	uint64_t hs_oll = RubiksCube::s_cube_o.hash_oll();
	uint64_t code = 0;
	reached[hs] = 0;
	level_map[hs_oll] = 0;
	active.push({ code, RubiksCube::s_cube_o });	

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
		auto avoid_ids = get_avoid_ids(suffix.code % 13);

		int max_id = is_last ? 3 : 13;
		for (int id = max_id-1; id>0; id--)
		{
			if (avoid_ids.find(id) != avoid_ids.end()) continue;			

			uint64_t code = id + suffix.code * 13;
			RubiksCube cube = *ops[id];
			cube *= suffix.state;

			hs = cube.hash();
			hs_oll = cube.hash_oll();

			auto iter = reached.find(hs);
			if (iter == reached.end() || iter->second == level)
			{
				int cur_level = INT_MAX;
				auto iter_l = level_map.find(hs_oll);
				if (iter_l != level_map.end())
				{
					cur_level = iter_l->second;
				}
				if (!is_last)
				{
					reached[hs] = level;
					if (level < cur_level) level_map[hs_oll] = level;
					active.push({ code, cube });
				}
				if (id < 3 && level <= cur_level)
				{
					fwrite(&hs_oll, sizeof(uint64_t), 1, fp);
					fwrite(&code, sizeof(uint64_t), 1, fp);
				}
			}
		}
	}

	fclose(fp);

	return 0;
}
#endif

#if MODE == MODE_OLL_DISCOVER
int main()
{
	RubiksCube::s_Initialize();
	printf("Calculating OLL cases\n");

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
	std::vector<std::vector<RubiksCube>> oll_lsts(count_oll_seqs);

	{
		std::unordered_map<uint64_t, RubiksCube> collection;
		for (int i = 0; i < count_oll_seqs; i++)
		{
			RubiksCube cube;
			cube.exec_seq(oll_seqs[i], true);

			for (int j = 0; j < 4; j++)
			{
				uint64_t hs = cube.hash_oll();
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

	std::unordered_map<uint64_t, std::vector<uint64_t>> prefix_map;
	{
		FILE* fp = fopen("OLL8.dat", "rb");
		fseek(fp, 0, SEEK_END);
		size_t len = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		while (ftell(fp) < len)
		{
			uint64_t hs, code;
			fread(&hs, sizeof(uint64_t), 1, fp);
			fread(&code, sizeof(uint64_t), 1, fp);
			prefix_map[hs].push_back(code);
		}
		fclose(fp);
	}

	std::vector<int> oll_level(count_oll_seqs, INT_MAX);
	std::vector<std::vector<std::vector<uint8_t>>> opt_seqs_oll(count_oll_seqs);

	struct Record
	{
		uint64_t code;
		RubiksCube state;
	};

	std::unordered_map<uint64_t, uint64_t> reached;
	std::queue<Record> active;

	uint64_t hs = RubiksCube::s_cube_o.hash();
	uint64_t code = 0;
	reached[hs];
	active.push({ code, RubiksCube::s_cube_o });

	printf("Scanning, sub-8 moves\n");

	for (int i = 0; i < count_oll_seqs; i++)
	{
		std::vector<RubiksCube>& sub_lst = oll_lsts[i];
		for (size_t j = 0; j < sub_lst.size(); j++)
		{
			RubiksCube cube_dst = sub_lst[j];
			hs = cube_dst.hash_oll();
			auto iter2 = prefix_map.find(hs);
			if (iter2 != prefix_map.end())
			{
				const std::vector<uint64_t>& prefixes = iter2->second;
				for (size_t k = 0; k < prefixes.size(); k++)
				{
					std::vector<uint8_t> prefix = decode_seq(prefixes[k]);
					reverse_seq(prefix);
					opt_seqs_oll[i].push_back(prefix);
					std::string str = seq2str(prefix);
					printf("oll %d: %s\n", i + 1, str.c_str());
				}

				if (oll_level[i] > 0)
				{
					oll_level[i] = 0;
					found_oll++;
				}
			}
		}
	}

	int max_level = 6;
	int level = 0;
	int code_level_min = 0;
	while (active.size() > 0)
	{
		if (_kbhit() != 0 && _getch() == 27) break;

		Record suffix = active.front();
		active.pop();
		if (suffix.code >= code_level_min)
		{
			if (found_oll == count_oll_seqs) break;
			level++;
			code_level_min = code_level_min * 13 + 1;
			printf("Scanning, %d moves.\n", 8 + level);
		}

		bool is_last = level == max_level;
		auto avoid_ids = get_avoid_ids2(suffix.code % 13);

		for (int id = 1; id < 13; id++)
		{
			if (avoid_ids.find(id) != avoid_ids.end()) continue;

			uint64_t code = id + suffix.code * 13;
			RubiksCube cube = *ops[id];
			cube *= suffix.state;

			hs = cube.hash();
			auto iter = reached.find(hs);
			if (iter == reached.end() || iter->second == level)
			{
				if (!is_last)
				{
					reached[hs] = level;
					active.push({ code, cube });
				}

				bool has_found = false;
				for (int i = 0; i < count_oll_seqs; i++)
				{
					if (oll_level[i] < level) continue;

					std::vector<RubiksCube>& sub_lst = oll_lsts[i];
					for (size_t j = 0; j < sub_lst.size(); j++)
					{
						RubiksCube cube_dst = sub_lst[j];
						cube_dst *= cube;
						hs = cube_dst.hash_oll();
						auto iter2 = prefix_map.find(hs);
						if (iter2 != prefix_map.end())
						{
							std::vector<uint8_t> suffix = decode_seq(code);
							const std::vector<uint64_t>& prefixes = iter2->second;
							for (size_t k = 0; k < prefixes.size(); k++)
							{
								std::vector<uint8_t> prefix = decode_seq(prefixes[k]);
								reverse_seq(prefix);
								auto avoid_id2 = get_avoid_ids2((int)prefix[0]);
								if (avoid_id2.find((int)suffix[suffix.size() - 1]) != avoid_id2.end()) continue;
								std::vector<uint8_t> suffix_cpy = suffix;
								suffix_cpy.insert(suffix_cpy.end(), prefix.begin(), prefix.end());
								opt_seqs_oll[i].push_back(suffix_cpy);
								std::string str = seq2str(suffix_cpy);
								printf("oll %d: %s\n", i + 1, str.c_str());
							}

							if (oll_level[i] > level)
							{
								oll_level[i] = level;
								found_oll++;
								has_found = true;
							}
						}
					}
				}
				if (has_found)
				{
					printf("%zu/%zu\n", found_oll, count_oll_seqs);
				}
			}
		}
	}

	{
		FILE* fp = fopen("found_oll.txt", "w");

		for (int i = 0; i < count_oll_seqs; i++)
		{
			fprintf(fp, "OLL %d:\n", i + 1);
			std::vector<std::vector<uint8_t>>& sub_lst_seq = opt_seqs_oll[i];
			for (size_t j = 0; j < sub_lst_seq.size(); j++)
			{
				std::string str = seq2str(sub_lst_seq[j]);
				fprintf(fp, "%s\n", str.c_str());
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
		keys.insert(cube.hash());
		cube *= RubiksCube::s_UCW;
		keys.insert(cube.hash());
		cube *= RubiksCube::s_UCW;
		keys.insert(cube.hash());
		cube *= RubiksCube::s_UCW;
		keys.insert(cube.hash());
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
				uint64_t hs = cube.hash();
				auto iter = keys.find(hs);
				if (iter == keys.end())
				{
					keys.insert(hs);
					cube *= RubiksCube::s_UCW;
					keys.insert(cube.hash());
					cube *= RubiksCube::s_UCW;
					keys.insert(cube.hash());
					cube *= RubiksCube::s_UCW;
					keys.insert(cube.hash());

					printf("%s\n", seq.c_str());
				}
				break;
			}

		}

	}	

	return 0;
}

#endif
