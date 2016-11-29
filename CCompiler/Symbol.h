#pragma once

#include "Utility.h"

#include <iostream>
#include <unordered_map>

namespace Yradex
{
	namespace CCompiler
	{
		enum class Symbol
		{
			eof,

			string,
			unsigned_integer,
			character,

			identifier,


			const_symbol,
			int_symbol,
			char_symbol,
			void_symbol,
			if_symbol,
			else_symbol,
			do_symbol,
			while_symbol,
			for_symbol,
			return_symbol,
			scanf_symbol,
			printf_symbol,

			equal,
			not_equal,
			greater,
			less,
			greater_equal,
			less_equal,

			left_parenthesis,
			right_parenthesis,
			left_square_brace,
			right_square_brace,
			left_curly_brace,
			right_curly_brace,

			plus,
			minus,
			asterisk,
			slash,
			semicolon,
			comma,
			assign,

		};

		template <typename Elem, typename Traits>
		std::basic_ostream<Elem, Traits>& operator<< (std::basic_ostream<Elem, Traits> &stream, Symbol symbol)
		{
			static const std::unordered_map<Symbol, std::basic_string<Elem, Traits>, EnumHasher<Symbol>> symbol_map = {
				{ Symbol::eof,					"eof" },
				{ Symbol::string,				"string" },
				{ Symbol::unsigned_integer,		"unsigned_integer" },
				{ Symbol::character,			"character" },
				{ Symbol::identifier,			"identifier" },
				{ Symbol::const_symbol,			"const_symbol" },
				{ Symbol::int_symbol,			"int_symbol" },
				{ Symbol::char_symbol,			"char_symbol" },
				{ Symbol::void_symbol,			"void_symbol" },
				{ Symbol::if_symbol,			"if_symbol" },
				{ Symbol::else_symbol,			"else_symbol" },
				{ Symbol::do_symbol,			"do_symbol" },
				{ Symbol::while_symbol,			"while_symbol" },
				{ Symbol::for_symbol,			"for_symbol" },
				{ Symbol::return_symbol,		"return_symbol" },
				{ Symbol::scanf_symbol,			"scanf_symbol" },
				{ Symbol::printf_symbol,		"printf_symbol" },
				{ Symbol::equal,				"equal" },
				{ Symbol::not_equal,			"not_equal" },
				{ Symbol::greater,				"greater" },
				{ Symbol::less,					"less" },
				{ Symbol::greater_equal,		"greater_equal" },
				{ Symbol::less_equal,			"less_equal" },
				{ Symbol::left_parenthesis,		"left_parenthesis" },
				{ Symbol::right_parenthesis,	"right_parenthesis" },
				{ Symbol::left_square_brace,	"left_square_brace" },
				{ Symbol::right_square_brace,	"right_square_brace" },
				{ Symbol::left_curly_brace,		"left_curly_brace" },
				{ Symbol::right_curly_brace,	"right_curly_brace" },
				{ Symbol::plus,					"plus" },
				{ Symbol::minus,				"minus" },
				{ Symbol::asterisk,				"asterisk" },
				{ Symbol::slash,				"slash" },
				{ Symbol::semicolon,			"semicolon" },
				{ Symbol::comma,				"comma" },
				{ Symbol::assign,				"assign" },
			};
			stream << symbol_map.at(symbol);
			return stream;
		}

		class SymbolUtilities
		{
		public:
			static bool is_variable_type_identifier(Symbol symbol)
			{
				return symbol == Symbol::int_symbol
					|| symbol == Symbol::char_symbol;
			}

			static bool is_type_identifier(Symbol symbol)
			{
				return symbol == Symbol::void_symbol
					|| is_variable_type_identifier(symbol);
			}

			static bool is_separator(Symbol symbol)
			{
				return symbol == Symbol::semicolon
					|| symbol == Symbol::right_curly_brace
					|| symbol == Symbol::comma
					|| symbol == Symbol::right_parenthesis;
			}

			static bool is_end_of_statement(Symbol symbol)
			{
				return symbol == Symbol::semicolon
					|| symbol == Symbol::right_curly_brace;
			}

			static bool is_compare_operator(Symbol symbol)
			{
				return symbol == Symbol::equal
					|| symbol == Symbol::not_equal
					|| symbol == Symbol::less
					|| symbol == Symbol::less_equal
					|| symbol == Symbol::greater
					|| symbol == Symbol::greater_equal;
			}

			static Symbol type_after_calculate(Symbol, Symbol)
			{
				//if (la == Symbol::int_symbol || ra == Symbol::int_symbol)
				//{
					return Symbol::int_symbol;
				//}
				//return Symbol::char_symbol;
			}
		};
	}
}
