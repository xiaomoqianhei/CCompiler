#pragma once

#include "LexicalAnalyzer.h"
#include "Error.h"
#include "PseudoTable.h"

#define __check_character_and_raise_if_false(x) if(!_check_character_and_raise_if_false(x)) return; _lexical_analyzer.next();

namespace Yradex
{
	namespace CCompiler
	{

		class Parser
		{
		public:
			using string_type = std::string;
			using char_type = typename string_type::value_type;
			using lexical_analizer_type = LexicalAnalyzer;
			using pseudo_table_type = PseudoTable;
			using ptr_of_variable = std::shared_ptr<Variable>;
			using ptr_of_function_detail = std::shared_ptr<FunctionDetail>;
			using error_handler_type = BasicErrorHandler;

		private:
			const std::string _filename;
			lexical_analizer_type _lexical_analyzer;
			pseudo_table_type &_pseudo_table;

		private:
			Parser(const Parser&) = delete;
			Parser& operator=(const Parser&) = delete;

			void _error(Error e, typename error_handler_type::Level level = error_handler_type::Level::error) const
			{
				auto line_number = _lexical_analyzer.last_line_num();
				auto position = _lexical_analyzer.last_char_num();

				error_handler_type::instance().raise(e, _filename, line_number, position, level);
			}

			void _error(Error e, string_type info, typename error_handler_type::Level level = error_handler_type::Level::error) const
			{
				auto line_number = _lexical_analyzer.last_line_num();
				auto position = _lexical_analyzer.last_char_num();

				error_handler_type::instance().raise(e, info, _filename, line_number, position, level);
			}

			void _debug(const string_type &info) const
			{
				if (!Config::parser_debug)
				{
					return;
				}
				error_handler_type::instance().debug("Parser: " + info);
			}
			template <typename T, typename... S>
			void _debug(const string_type &info, T n, S... args) const;

			bool _check_character_and_raise_if_false(Symbol s);

			void _parse_program();
			void _parse_variable_defination(const Symbol type, string_type name, std::function<bool(Symbol)> is_start_of_next);
			void _parse_function_defination(const Symbol type, string_type name, std::function<bool(Symbol)> is_start_of_next);
			bool _parse_function_defination();
			void _parse_const_defination_list(std::function<bool(Symbol)> is_start_of_next);
			int  _parse_signed_integer(std::function<bool(Symbol)> is_start_of_next);
			void _parse_parameter_table(std::function<bool(Symbol)> is_start_of_next);
			void _parse_compound_statement();
			void _parse_variable_defination_list();
			void _parse_statement_list();
			void _parse_statement();

			void _parse_while_statement();
			void _parse_statment_started_with_identifier();
			void _parse_scanf_statement();
			void _parse_return_statement();
			void _parse_if_statement();
			void _parse_for_statement();
			void _parse_printf_statement();
			void _parse_condition(const ptr_of_variable &lable_true, const ptr_of_variable &label_false);
			ptr_of_variable _parse_function_call(const string_type& name);
			void _parse_assign_statement(const string_type& name);
			ptr_of_variable _parse_expression();
			ptr_of_variable _parse_term();
			ptr_of_variable _parse_factor();

			bool _is_eof();

			ptr_of_variable _declear_variable(const string_type &name, Symbol type);
			ptr_of_variable _declear_const_variable(const string_type &name, Symbol type, int value);
			ptr_of_variable _declear_array(const string_type &name, Symbol type, size_t length);
			ptr_of_function_detail _declear_function(const FunctionIdentifier &f, Symbol type);
			ptr_of_variable _declear_parameter(const string_type &name, Symbol type);
			ptr_of_variable _declear_temp_variable(Symbol type);

			ptr_of_variable _get_label();
			ptr_of_variable _get_function_label(const string_type &name);
			void _insert_label(const ptr_of_variable & label);

			ptr_of_variable _get_literal(int value);
			ptr_of_variable _get_variable(const string_type &name) const;
			ptr_of_variable _get_string_variable(const string_type &str) const;

			void _insert_instruction(const PseudoInstruction &pi);
			PseudoInstruction _get_last_instruction() const;
			ptr_of_function_detail _get_current_function_detail() const;
			ptr_of_function_detail _get_function_detail(const FunctionIdentifier & name) const;

		public:
			Parser(PseudoTable &pseudo_table, const std::string &file_to_compile)
				: _filename(file_to_compile), _lexical_analyzer(_filename), _pseudo_table(pseudo_table)
			{
			}

			void parse_to_pseudo();

		};



		template<typename T, typename ...S>
		inline void Parser::_debug(const string_type & info, T n, S ...args) const
		{
			std::basic_ostringstream<char_type> stream;
			stream << info << n << std::flush;
			_debug(stream.str(), args...);
		}

	}
}
