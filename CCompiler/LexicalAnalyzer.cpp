#include "LexicalAnalyzer.h"

namespace Yradex
{
	namespace CCompiler
	{

		const std::unordered_map<typename LexicalAnalyzer::string_type, Symbol>
			LexicalAnalyzer::_symbol_map = {

				{ "const", Symbol::const_symbol },
				{ "int", Symbol::int_symbol },
				{ "char", Symbol::char_symbol },
				{ "void", Symbol::void_symbol },
				{ "if", Symbol::if_symbol },
				{ "else", Symbol::else_symbol },
				{ "do", Symbol::do_symbol },
				{ "while", Symbol::while_symbol },
				{ "for", Symbol::for_symbol },
				{ "return", Symbol::return_symbol },
				{ "scanf", Symbol::scanf_symbol },
				{ "printf", Symbol::printf_symbol },

				{ "==", Symbol::equal },
				{ "!=", Symbol::not_equal },
				{ ">", Symbol::greater },
				{ "<", Symbol::less },
				{ ">=", Symbol::greater_equal },
				{ "<=", Symbol::less_equal },

				{ "(", Symbol::left_parenthesis },
				{ ")", Symbol::right_parenthesis },
				{ "[", Symbol::left_square_brace },
				{ "]", Symbol::right_square_brace },
				{ "{", Symbol::left_curly_brace },
				{ "}", Symbol::right_curly_brace },

				{ "+", Symbol::plus },
				{ "-", Symbol::minus },
				{ "*", Symbol::asterisk },
				{ "/", Symbol::slash },
				{ ";", Symbol::semicolon },
				{ ",", Symbol::comma },
				{ "=", Symbol::assign },
		};

		const std::pair<Symbol, typename LexicalAnalyzer::string_type>& LexicalAnalyzer::next()
		{
			static unsigned long count = 0;
			_next();
			count++;

			if (Config::lexical_analyzer_debug && error_handler_type::instance().should_raise(error_handler_type::Level::debug))
			{
				std::basic_ostringstream<char_type> stream;
				stream << "LexicalAnalyzer: "
					<< std::setw(5) << std::left << count
					<< std::setw(20) << std::left << _last.first
					<< _last.second
					<< std::flush;
				error_handler_type::instance().debug(stream.str());
			}
			return _last;
		}
		bool LexicalAnalyzer::skip_this_statement_or_until(std::function<bool(Symbol)> pred)
		{
			while (!pred(_last.first))
			{
				if (SymbolUtilities::is_end_of_statement(_last.first))
				{
					_next();
					return false;
				}
				_next();
			}
			return true;
		}
		void LexicalAnalyzer::skip_to_next_statement()
		{
			while (_last != Symbol::left_curly_brace && _last != Symbol::right_curly_brace)
			{
				if (SymbolUtilities::is_end_of_statement(_last.first))
				{
					_next();
					return;
				}
				_next();
			}
		}
		LexicalAnalyzer::string_type LexicalAnalyzer::get_str_of(Symbol s) const
		{
			for (auto & pair : _symbol_map)
			{
				if (pair.second == s)
				{
					return pair.first;
				}
			}
			return string_type();
		}

		void LexicalAnalyzer::_skip_space()
		{
			_iter = std::find_if_not(_iter, _string_to_analysis.cend(), [](char_type c) { return isspace(c); });
			while (_iter == _string_to_analysis.cend() && _ifstream)
			{
				std::getline(_ifstream, _string_to_analysis);
				_iter = _string_to_analysis.cbegin();
				_line_number++;
				_iter = std::find_if_not(_iter, _string_to_analysis.cend(), [](char_type c) { return isspace(c); });
			}
		}

		void LexicalAnalyzer::_error(Error e,
			typename string_type::const_iterator iter) const
		{
			error_handler_type::instance().raise(e, _filename, _line_number, _pos(iter), error_handler_type::Level::error);
		}

		std::pair<Symbol, typename LexicalAnalyzer::string_type>
			LexicalAnalyzer::_next()
		{
			_skip_space();
			if (_iter == _string_to_analysis.cend())
			{
				if (_last == Symbol::eof)
				{
					error_handler_type::instance().raise(Error::reached_end_of_file, error_handler_type::Level::fatal);
				}
				_last_line_number = _line_number;
				_last_iter = _iter;
				_last = std::make_pair(Symbol::eof, "");
				return _last;
			}

			char_type first_char = *_iter;
			typename string_type::const_iterator iter = std::next(_iter);
			Symbol symbol;
			string_type text;

			if (isalpha(first_char) || first_char == '_')
			{
				iter = std::find_if_not(_iter, _string_to_analysis.cend(),
					[](char_type c) { return isalnum(c) || c == '_'; });
				text = string_type(_iter, iter);
				auto find_result = _symbol_map.find(text);
				if (find_result == _symbol_map.cend())
				{
					symbol = Symbol::identifier;
				}
				else
				{
					symbol = find_result->second;
				}
			}
			else if (isdigit(first_char))
			{
				if (first_char == '0' && isdigit(*iter))
				{
					_error(Error::illegal_leading_zero, iter);
					// skip this 0
					_iter = iter;
					return _next();
				}
				iter = std::find_if_not(_iter, _string_to_analysis.cend(), isdigit);
				text = string_type(_iter, iter);
				if (text.length() > Config::max_num_length)
				{
					_error(Error::num_too_long, std::next(_iter));
					text = "1";
				}
				symbol = Symbol::unsigned_integer;
			}
			else if (first_char == '\'')
			{
				char c;
				size_t start_pos = _pos(iter);
				size_t end_pos = _string_to_analysis.find('\'', start_pos);
				if (end_pos == start_pos)
				{
					_error(Error::invalid_character, iter);
					c = 'u';
				}
				else if (end_pos == string_type::npos)
				{
					_error(Error::invalid_character, iter);
					// skip this char
					_iter = iter;
					return _next();
				}
				else if (end_pos != start_pos + 1)
				{
					_error(Error::invalid_character, iter);
					// skip whole 'char'
					std::advance(iter, end_pos - start_pos);
					c = 'u';
				}
				else
				{
					c = *iter;
					std::advance(iter, 1);
				}
				switch (c)
				{
				case '+': case '-': case '*': case '/': case '_':
					break;
				default:
					if (!isalnum(c))
					{
						_error(Error::invalid_character, iter);
						c = 'u';
					}
					break;
				}
				std::advance(iter, 1);
				std::basic_ostringstream<char_type> stream;
				stream << c << std::flush;
				text = stream.str();
				symbol = Symbol::character;
			}
			else if (first_char == '"')
			{
				size_t start_pos = _pos(iter);
				size_t end_pos = _string_to_analysis.find('"', start_pos);
				if (end_pos == std::string::npos)
				{
					_error(Error::string_not_end, iter);
					_iter = _string_to_analysis.cend();
					return _next();
				}

				iter = std::next(_string_to_analysis.cbegin(), end_pos + 1);
				symbol = Symbol::string;
				text = _string_to_analysis.substr(start_pos, end_pos - start_pos);
			}
			else if (first_char == '=')
			{
				if (iter != _string_to_analysis.cend() && *iter == '=')
				{
					std::advance(iter, 1);
					text = "==";
					symbol = Symbol::equal;
				}
				else
				{
					text = first_char;
					symbol = Symbol::assign;
				}
			}
			else if (first_char == '<')
			{
				if (iter != _string_to_analysis.cend() && *iter == '=')
				{
					std::advance(iter, 1);
					text = "<=";
					symbol = Symbol::less_equal;
				}
				else
				{
					text = first_char;
					symbol = Symbol::less;
				}
			}
			else if (first_char == '>')
			{
				if (iter != _string_to_analysis.cend() && *iter == '=')
				{
					std::advance(iter, 1);
					text = ">=";
					symbol = Symbol::greater_equal;
				}
				else
				{
					text = first_char;
					symbol = Symbol::greater;
				}
			}
			else if (first_char == '!')
			{
				if (iter != _string_to_analysis.cend() && *iter == '=')
				{
					std::advance(iter, 1);
					text = "!=";
					symbol = Symbol::not_equal;
				}
				else
				{
					_error(Error::illegal_character, iter);
					_iter = iter;
					return _next();
				}
			}
			else if (first_char == '/')
			{
				if (iter != _string_to_analysis.cend() && *iter == '/')
				{
					_iter = _string_to_analysis.cend();
					return _next();
				}
				else
				{
					text = first_char;
					symbol = Symbol::slash;
				}
			}
			else
			{
				auto result = _symbol_map.find(string_type(1, first_char));
				if (result == _symbol_map.cend())
				{
					_error(Error::illegal_character, iter);
					_iter = iter;
					return _next();
				}
				text = first_char;
				symbol = result->second;
			}

			_last_line_number = _line_number;
			_last_iter = _iter;
			_iter = iter;
			_last = std::make_pair(symbol, text);
			return _last;
		}

		LexicalAnalyzer::LexicalAnalyzer(const std::string file_to_compile)
			: _filename(file_to_compile), _ifstream(file_to_compile)
		{
			if (!_ifstream)
			{
				error_handler_type::instance().raise(Error::invalid_source_file, error_handler_type::Level::fatal);
			}

			std::getline(_ifstream, _string_to_analysis);
			_iter = _string_to_analysis.cbegin();
		}
	}
}