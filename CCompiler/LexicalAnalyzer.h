#pragma once

#include "symbol.h"
#include "utility.h"
#include "Error.h"
#include "BasicErrorHandler.h"

#include <unordered_map>
#include <string>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <functional>

namespace Yradex
{
	namespace CCompiler
	{
		class LexicalAnalyzer
		{
		private:
			using string_type = std::string;
			using char_type = typename string_type::value_type;
			using error_handler_type = BasicErrorHandler;

		private:
			static const std::unordered_map<string_type, Symbol> _symbol_map;

			const std::string _filename;
			std::basic_ifstream<char_type> _ifstream;

			size_t _line_number = 1;
			size_t _last_line_number = 1;

			string_type _string_to_analysis;
			typename string_type::const_iterator _iter;
			typename string_type::const_iterator _last_iter;

			std::pair<Symbol, string_type> _last = std::make_pair(Symbol::eof, "");

		private:
			LexicalAnalyzer(const LexicalAnalyzer&) = delete;
			LexicalAnalyzer& operator=(const LexicalAnalyzer&) = delete;

			void _skip_space();
			void _error(Error e, typename string_type::const_iterator iter) const;

			size_t _pos(typename string_type::const_iterator iter) const
			{
				return std::distance(_string_to_analysis.cbegin(), iter);
			}
			std::pair<Symbol, string_type> _next();

		public:
			LexicalAnalyzer(const std::string file_to_compile);

			const std::pair<Symbol, string_type>& next();
			const std::pair<Symbol, string_type>& last() const
			{
				return _last;
			}

			void skip_until(std::function<bool(Symbol)> pred)
			{
				while (!pred(_last.first))
				{
					_next();
				}
			}

			bool skip_this_statement_or_until(std::function<bool(Symbol)> pred);

			void skip_to_next_statement();

			size_t last_line_num() const
			{
				return _last_line_number;
			}
			size_t last_char_num() const
			{
				return _pos(_last_iter) + 1;
			}

			string_type get_str_of(Symbol s) const;

		};

		template <typename C>
		inline bool operator==(const std::pair<Symbol, std::basic_string<C>>& pair, Symbol symbol)
		{
			return symbol == pair.first;
		}

		template <typename C>
		inline bool operator==(Symbol symbol, const std::pair<Symbol, std::basic_string<C>>& pair)
		{
			return symbol == pair.first;
		}

		template <typename C>
		inline bool operator!=(const std::pair<Symbol, std::basic_string<C>>& pair, Symbol symbol)
		{
			return symbol != pair.first;
		}

		template <typename C>
		inline bool operator!=(Symbol symbol, const std::pair<Symbol, std::basic_string<C>>& pair)
		{
			return symbol != pair.first;
		}


	}
}
