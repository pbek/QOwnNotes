#include "num_cvt.h"
#include "utf8_strings.h"
#include <vector>

static std::vector<litehtml::tchar_t> latin_lower = { _t('a'), _t('b'), _t('c'), _t('d'), _t('e'), _t('f'), _t('g'), _t('h'), _t('i'), _t('j'), _t('k'), _t('l'), _t('m'), _t('n'), _t('o'), _t('p'), _t('q'), _t('r'), _t('s'), _t('t'), _t('u'), _t('v'), _t('w'), _t('x'), _t('y'), _t('z') };
static std::vector<litehtml::tchar_t> latin_upper = { _t('A'), _t('B'), _t('C'), _t('D'), _t('E'), _t('F'), _t('G'), _t('H'), _t('I'), _t('J'), _t('K'), _t('L'), _t('M'), _t('N'), _t('O'), _t('P'), _t('Q'), _t('R'), _t('S'), _t('T'), _t('U'), _t('V'), _t('W'), _t('X'), _t('Y'), _t('Z') };
static std::vector<std::wstring> greek_lower = { L"α", L"β", L"γ", L"δ", L"ε", L"ζ", L"η", L"θ", L"ι", L"κ", L"λ", L"μ", L"ν", L"ξ", L"ο", L"π", L"ρ", L"σ", L"τ", L"υ", L"φ", L"χ", L"ψ", L"ω" };

static litehtml::tstring to_mapped_alpha(int num, const std::vector<litehtml::tchar_t>& map)
{
	int dividend = num;
	litehtml::tstring out;
	int modulo;

	while (dividend > 0)
	{
		modulo = (dividend - 1) % map.size();
		out = map[modulo] + out;
		dividend = (int)((dividend - modulo) / map.size());
	}

	return out;
}

static litehtml::tstring to_mapped_alpha(int num, const std::vector<std::wstring>& map)
{
	int dividend = num;
	litehtml::tstring out;
	int modulo;

	while (dividend > 0)
	{
		modulo = (dividend - 1) % map.size();
		out = litehtml_from_wchar(map[modulo]).c_str() + out;
		dividend = (int)((dividend - modulo) / map.size());
	}

	return out;
}

litehtml::tstring litehtml::num_cvt::to_latin_lower(int val)
{
	return to_mapped_alpha(val, latin_lower);
}

litehtml::tstring litehtml::num_cvt::to_latin_upper(int val)
{
	return to_mapped_alpha(val, latin_upper);
}

litehtml::tstring litehtml::num_cvt::to_greek_lower(int val)
{
	return to_mapped_alpha(val, greek_lower);
}

litehtml::tstring litehtml::num_cvt::to_roman_lower(int value)
{
	struct romandata_t { int value; const litehtml::tchar_t* numeral; };
	const struct romandata_t romandata[] =
	{
		{ 1000, _t("m") }, { 900, _t("cm" )},
		{ 500, _t("d") }, { 400, _t("cd") },
		{ 100, _t("c") }, { 90, _t("xc") },
		{ 50, _t("l") }, { 40, _t("xl") },
		{ 10, _t("x") }, { 9, _t("ix") },
		{ 5, _t("v") }, { 4, _t("iv") },
		{ 1, _t("i") },
		{ 0, nullptr } // end marker
	};

	litehtml::tstring result;
	for (const romandata_t* current = romandata; current->value > 0; ++current)
	{
		while (value >= current->value)
		{
			result += current->numeral;
			value -= current->value;
		}
	}
	return result;
}

litehtml::tstring litehtml::num_cvt::to_roman_upper(int value)
{
	struct romandata_t { int value; const litehtml::tchar_t* numeral; };
	const struct romandata_t romandata[] =
	{
		{ 1000, _t("M") }, { 900, _t("CM") },
		{ 500, _t("D") }, { 400, _t("CD") },
		{ 100, _t("C") }, { 90, _t("XC") },
		{ 50, _t("L") }, { 40, _t("XL") },
		{ 10, _t("X") }, { 9, _t("IX") },
		{ 5, _t("V") }, { 4, _t("IV") },
		{ 1, _t("I") },
		{ 0, nullptr } // end marker
	};

	litehtml::tstring result;
	for (const romandata_t* current = romandata; current->value > 0; ++current)
	{
		while (value >= current->value)
		{
			result += current->numeral;
			value -= current->value;
		}
	}
	return result;
}
