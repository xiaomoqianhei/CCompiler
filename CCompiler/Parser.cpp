#include "Parser.h"

namespace Yradex
{
	namespace CCompiler
	{
		bool Parser::_check_character_and_raise_if_false(Symbol s)
		{
			if (_lexical_analyzer.last() != s)
			{
				_error(Error::character_required, "'" + _lexical_analyzer.get_str_of(s) + "'");
				_lexical_analyzer.skip_to_next_statement();
				return false;
			}
			return true;
		}

		// ［＜常量说明＞］［＜变量说明＞］{＜有返回值函数定义＞|＜无返回值函数定义＞}＜主函数＞
		void Parser::_parse_program()
		{
			// 常量定义
			_lexical_analyzer.next();
			_parse_const_defination_list(SymbolUtilities::is_type_identifier);

			// 检查下一个定义是变量定义还是函数定义
		check_type:
			while (true)
			{
				while (!SymbolUtilities::is_type_identifier(_lexical_analyzer.last().first))
				{
					_error(Error::invalid_type_identifier);
					_lexical_analyzer.skip_until([](Symbol s) {return SymbolUtilities::is_type_identifier(s); });
				}
				if (!SymbolUtilities::is_variable_type_identifier(_lexical_analyzer.last().first))
				{
					break;		// 定义是函数定义，交给后面处理
				}

				Symbol type = _lexical_analyzer.last().first;
				string_type identifier_name = "";

				while (_lexical_analyzer.next() != Symbol::identifier)
				{
					_error(Error::invalid_identifier);
					_lexical_analyzer.skip_this_statement_or_until([](Symbol s) {
						return SymbolUtilities::is_type_identifier(s) || s == Symbol::comma;
					});
					if (_lexical_analyzer.last() != Symbol::comma)
					{
						goto check_type;
					}
				}
				identifier_name = _lexical_analyzer.last().second;

				if (_lexical_analyzer.next() == Symbol::left_parenthesis)
				{
					_parse_function_defination(type, identifier_name, SymbolUtilities::is_type_identifier);
					break;
				}
				else
				{
					_parse_variable_defination(type, identifier_name, SymbolUtilities::is_type_identifier);
				}
			}

			while (!_is_eof())
			{
				while (!SymbolUtilities::is_type_identifier(_lexical_analyzer.last().first))
				{
					_error(Error::invalid_type_identifier);
					_lexical_analyzer.skip_until([](Symbol s) {return SymbolUtilities::is_type_identifier(s); });
				}
				if (_parse_function_defination())
				{
					_debug("Parser finished successfully");
					return;
				}
			}
			_error(Error::function_main_not_found, error_handler_type::Level::fatal);
		}

		// [ '['＜无符号整数＞']' ] {,＜标识符＞[ '['＜无符号整数＞']' ] }
		// 前置符号: 任意, 会主动报错
		// 后置符号: 下一语句的首字符
		void Parser::_parse_variable_defination(const Symbol type, string_type name, std::function<bool(Symbol)> is_start_of_next)
		{
			string_type identifier_name = name;

			while (true)
			{
				int length = -1;
				if (_lexical_analyzer.last() == Symbol::left_square_brace)
				{
					if (_lexical_analyzer.next() != Symbol::unsigned_integer)
					{
						_error(Error::invalid_array_length);
						length = 1;
					}
					else
					{
						length = std::atoi(_lexical_analyzer.last().second.c_str());
						if (length == 0)
						{
							_error(Error::invalid_array_length);
							length = 1;
						}
						_lexical_analyzer.next();
					}
					if (_lexical_analyzer.last() != Symbol::right_square_brace)
					{
						_error(Error::character_required, "']'");
					}
					else
					{
						_lexical_analyzer.next();
					}
				}
				if (length == -1)
				{
					_declear_variable(identifier_name, type);
					_debug("Variable defination: " + identifier_name + " of type ", type);
				}
				else
				{
					_declear_array(identifier_name, type, length);
					_debug("Array defination: " + identifier_name + " of length ", length, " type ", type);
				}

				if (_lexical_analyzer.last() == Symbol::semicolon)
				{
					_lexical_analyzer.next();
					return;
				}
				else if (_lexical_analyzer.last() != Symbol::comma)
				{
					_error(Error::character_required, "';'");
					_lexical_analyzer.skip_this_statement_or_until([is_start_of_next](Symbol s) {
						return is_start_of_next(s) || s == Symbol::comma;
					});
					if (_lexical_analyzer.last() != Symbol::comma)
					{
						return;
					}
				}
				while (_lexical_analyzer.next() != Symbol::identifier)
				{
					_error(Error::invalid_identifier);
					_lexical_analyzer.skip_this_statement_or_until([is_start_of_next](Symbol s) {
						return is_start_of_next(s) || s == Symbol::comma;
					});
					if (_lexical_analyzer.last() != Symbol::comma)
					{
						return;
					}
				}
				identifier_name = _lexical_analyzer.last().second;
				_lexical_analyzer.next();
			}
		}

		// '('＜参数＞')' '{'＜复合语句＞'}'
		// 前置符号: '('
		// 后置符号: 函数定义后的首符号
		void Parser::_parse_function_defination
		(const Symbol type, string_type name, std::function<bool(Symbol)> is_start_of_next)
		{
			assert(type != Symbol::eof);
			assert(_lexical_analyzer.last() == Symbol::left_parenthesis);

			_declear_function(name, type);
			typename PseudoTable::PseudoTableFunctionSwitcher switcher(_pseudo_table, name);

			_parse_parameter_table([](Symbol s) {return s == Symbol::left_curly_brace; });

			if (_lexical_analyzer.last() != Symbol::left_curly_brace)
			{
				_error(Error::character_required, "'{'");
				_lexical_analyzer.skip_until(
					[](Symbol s) {return SymbolUtilities::is_type_identifier(s) || s == Symbol::left_curly_brace; }
				);
				if (SymbolUtilities::is_type_identifier(_lexical_analyzer.last().first))
				{
					return;
				}
			}
			// generate function object
			_parse_compound_statement();

			if (_get_last_instruction().operator_ != PseudoOperator::ret)
			{
				// FIX retrun when if-else
				_insert_instruction(PseudoInstruction(PseudoOperator::ret, _get_literal(0), Variable::null, Variable::null));
				//_error(Error::function_not_return);
			}

			_debug("Generated function " + name + " of type ", type);
		}

		// ＜类型＞＜标识符＞'('＜参数＞')' '{'＜复合语句＞'}'
		// 前置符号: 类型之一
		// 后置符号: 函数定义后的首符号
		bool Parser::_parse_function_defination()
		{
			Symbol type = _lexical_analyzer.last().first;

			string_type name = "";

			while (_lexical_analyzer.next() != Symbol::identifier)
			{
				_error(Error::invalid_identifier);
				_lexical_analyzer.skip_this_statement_or_until([](Symbol s) {
					return SymbolUtilities::is_type_identifier(s);
				});
				return false;
			}
			name = _lexical_analyzer.last().second;

			_declear_function(name, type);
			typename PseudoTable::PseudoTableFunctionSwitcher switcher(_pseudo_table, name);

			if (_lexical_analyzer.next() != Symbol::left_parenthesis)
			{
				_error(Error::character_required, "'('");
				_lexical_analyzer.skip_this_statement_or_until([](Symbol s) {
					return SymbolUtilities::is_type_identifier(s);
				});
				return false;
			}

			_parse_parameter_table([](Symbol s) {return s == Symbol::left_curly_brace; });

			if (_lexical_analyzer.last() != Symbol::left_curly_brace)
			{
				_error(Error::character_required, "'{'");
				_lexical_analyzer.skip_until(
					[](Symbol s) {return SymbolUtilities::is_type_identifier(s) || s == Symbol::left_curly_brace; }
				);
				if (SymbolUtilities::is_type_identifier(_lexical_analyzer.last().first))
				{
					return false;
				}
			}
			_parse_compound_statement();

			if (_get_last_instruction().operator_ != PseudoOperator::ret)
			{
				if (_get_current_function_detail()->get_return_type() == Symbol::void_symbol)
				{
					_insert_instruction(PseudoInstruction(PseudoOperator::ret, Variable::null, Variable::null, Variable::null));
				}
				else
				{
					// FIX return when if-else
					_insert_instruction(PseudoInstruction(PseudoOperator::ret, _get_literal(0), Variable::null, Variable::null));
					//_error(Error::function_not_return);
				}
			}

			_debug("Generated function " + name + " of type ", type);

			return name == "main";
		}

		// { const＜常量定义＞; }
		// 前置符号: 任意, 会主动判断
		// 后置符号: 下一语句的首符号
		void Parser::_parse_const_defination_list(std::function<bool(Symbol)> is_start_of_next)
		{
		start:
			while (_lexical_analyzer.last() == Symbol::const_symbol)
			{
				Symbol type;
				string_type identifier_name;
				int value;

				if (!SymbolUtilities::is_variable_type_identifier(_lexical_analyzer.next().first))
				{
					_error(Error::invalid_type_identifier);
					_lexical_analyzer.skip_this_statement_or_until(
						[is_start_of_next](Symbol s) {return is_start_of_next(s) || s == Symbol::const_symbol; }
					);
					continue;
				}
				type = _lexical_analyzer.last().first;

				do
				{
					if (_lexical_analyzer.next() != Symbol::identifier)
					{
						_error(Error::invalid_identifier);
						_lexical_analyzer.skip_this_statement_or_until(
							[is_start_of_next](Symbol s) {return is_start_of_next(s) || s == Symbol::const_symbol || s == Symbol::comma; }
						);

						if (_lexical_analyzer.last() == Symbol::comma)
						{
							continue;
						}
						goto start;
					}
					identifier_name = _lexical_analyzer.last().second;

					if (_lexical_analyzer.next() != Symbol::assign)
					{
						_error(Error::character_required, "'='");
					}
					else
					{
						_lexical_analyzer.next();
					}

					auto tmp_is_start_of_next = [is_start_of_next](Symbol s) {
						return is_start_of_next(s) || s == Symbol::const_symbol
							|| s == Symbol::comma || s == Symbol::semicolon;
					};
					if (type == Symbol::int_symbol)
					{
						value = _parse_signed_integer(tmp_is_start_of_next);

						_declear_const_variable(identifier_name, type, value);
						_debug("Const int defination: ", identifier_name, " = ", value);
					}
					else if (type == Symbol::char_symbol)
					{
						if (_lexical_analyzer.last() != Symbol::character)
						{
							_error(Error::invalid_character);
							value = 'u';
							_lexical_analyzer.skip_this_statement_or_until(tmp_is_start_of_next);
						}
						else
						{
							value = _lexical_analyzer.last().second.front();
							_lexical_analyzer.next();
						}

						_declear_const_variable(identifier_name, type, value);
						_debug("Const char defination: ", identifier_name, " = ", static_cast<char>(value));
					}
				} while (_lexical_analyzer.last() == Symbol::comma);

				if (_lexical_analyzer.last() != Symbol::semicolon)
				{
					_error(Error::character_required, "';'");
					_lexical_analyzer.skip_this_statement_or_until(
						[is_start_of_next](Symbol s) {return is_start_of_next(s) || s == Symbol::const_symbol; }
					);
				}
				else
				{
					_lexical_analyzer.next();
				}
			}
		}

		// ［＋｜－］＜无符号整数＞
		// 前置符号: 任意
		// 后置符号: 数字之后的下一个符号
		int Parser::_parse_signed_integer(std::function<bool(Symbol)> is_start_of_next)
		{
			int value = 1;
			if (_lexical_analyzer.last() == Symbol::plus)
			{
				_lexical_analyzer.next();
			}
			else if (_lexical_analyzer.last() == Symbol::minus)
			{
				value = -1;
				_lexical_analyzer.next();
			}

			if (_lexical_analyzer.last() != Symbol::unsigned_integer)
			{
				_error(Error::invalid_integer);
				_lexical_analyzer.skip_this_statement_or_until(is_start_of_next);
				return 1;
			}
			value *= std::atoi(_lexical_analyzer.last().second.c_str());
			_lexical_analyzer.next();

			return value;
		}

		// '(' (＜类型标识符＞＜标识符＞{,＜类型标识符＞＜标识符＞} | ＜空＞) ')'
		// 前置符号: '('
		// 后置符号: '）'之后的未知符号
		void Parser::_parse_parameter_table
		(std::function<bool(Symbol)> is_start_of_next)
		{
			assert(_lexical_analyzer.last() == Symbol::left_parenthesis);

			bool should_be_comma = false;

			_lexical_analyzer.next();
			while (true)
			{
				if (_lexical_analyzer.last() == Symbol::right_parenthesis)
				{
					_lexical_analyzer.next();
					return;
				}
				if (is_start_of_next(_lexical_analyzer.last().first))
				{
					_error(Error::character_required, "')'");
					return;
				}

				Symbol type;

				if (should_be_comma)
				{
					if (_lexical_analyzer.last() != Symbol::comma)
					{
						_error(Error::character_required, "','");
						goto error_skip;
					}
					_lexical_analyzer.next();
				}
				should_be_comma = true;

				if (!SymbolUtilities::is_variable_type_identifier(_lexical_analyzer.last().first))
				{
					_error(Error::invalid_type_identifier);
					goto error_skip;
				}
				type = _lexical_analyzer.last().first;

				if (_lexical_analyzer.next() != Symbol::identifier)
				{
					_error(Error::invalid_identifier);
					goto error_skip;
				}
				else
				{
					auto name = _lexical_analyzer.last().second;

					_declear_parameter(name, type);
					_debug("Registered parameter " + name + " of type ", type);
					_lexical_analyzer.next();
					continue;
				}

			error_skip:
				_lexical_analyzer.skip_until(
					[is_start_of_next](Symbol s) {return SymbolUtilities::is_variable_type_identifier(s)
					|| is_start_of_next(s) || s == Symbol::comma || s == Symbol::right_parenthesis; }
				);
				if (SymbolUtilities::is_variable_type_identifier(_lexical_analyzer.last().first))
				{
					should_be_comma = false;
				}
				continue;
			}
		}

		// '{' ［＜常量说明＞］［＜变量说明＞］＜语句列＞ '}'
		// 前置符号: '{'
		// 后置符号: '}'后的首符号
		void Parser::_parse_compound_statement()
		{
			assert(_lexical_analyzer.last() == Symbol::left_curly_brace);

			_lexical_analyzer.next();
			_parse_const_defination_list(SymbolUtilities::is_variable_type_identifier);

			_parse_variable_defination_list();

			_parse_statement_list();

			_lexical_analyzer.next();
		}

		// ＜类型标识符＞＜标识符＞{,＜类型标识符＞＜标识符＞}| ＜空＞
		// 前置符号: 任意, 会主动判断
		// 后置符号: 下一语句的首字符
		void Parser::_parse_variable_defination_list()
		{
		start:
			while (SymbolUtilities::is_variable_type_identifier(_lexical_analyzer.last().first))
			{
				Symbol type = _lexical_analyzer.last().first;
				string_type name = "";

				while (_lexical_analyzer.next() != Symbol::identifier)
				{
					_error(Error::invalid_identifier);
					_lexical_analyzer.skip_this_statement_or_until([](Symbol s) {
						return SymbolUtilities::is_variable_type_identifier(s) || s == Symbol::comma;
					});
					if (_lexical_analyzer.last() != Symbol::comma)
					{
						continue;
					}
					else if (SymbolUtilities::is_variable_type_identifier(_lexical_analyzer.last().first))
					{
						goto start;
					}
					return;
				}
				name = _lexical_analyzer.last().second;

				_lexical_analyzer.next();
				_parse_variable_defination(type, name, SymbolUtilities::is_type_identifier);
			}
		}

		// ｛＜语句＞｝
		// 前置符号: 任何合法的语句首符号
		// 后置符号: '}'
		void Parser::_parse_statement_list()
		{
			while (_lexical_analyzer.last() != Symbol::right_curly_brace)
			{
				_parse_statement();
			}
		}

		// ＜条件语句＞｜＜循环语句＞｜'{'＜语句列＞'}'｜＜有返回值函数调用语句＞;
		//     |＜无返回值函数调用语句＞;｜＜赋值语句＞;｜＜读语句＞;｜＜写语句＞;
		//     |＜空＞;｜＜返回语句＞;
		// 前置符号: 任何合法的语句首符号
		// 后置符号: 下一个语句的首符号
		void Parser::_parse_statement()
		{
			switch (_lexical_analyzer.last().first)
			{
			case Symbol::if_symbol:
				_parse_if_statement();
				break;
			case Symbol::for_symbol:
				_parse_for_statement();
				break;
			case Symbol::do_symbol:
				_parse_while_statement();
				break;
			case Symbol::left_curly_brace:
				_lexical_analyzer.next();
				_parse_statement_list();
				_lexical_analyzer.next();
				break;
			case Symbol::identifier:
				_parse_statment_started_with_identifier();
				break;
			case Symbol::scanf_symbol:
				_parse_scanf_statement();
				break;
			case Symbol::printf_symbol:
				_parse_printf_statement();
				break;
			case Symbol::semicolon:
				_lexical_analyzer.next();
				break;
			case Symbol::return_symbol:
				_parse_return_statement();
				break;
			default:
				_error(Error::invalid_identifier);
				_lexical_analyzer.skip_to_next_statement();
				break;
			}
		}

		// do＜语句＞while '('＜条件＞')'
		// 前置符号：do
		// 后置符号：下一个语句的首符号
		void Parser::_parse_while_statement()
		{
			assert(_lexical_analyzer.last() == Symbol::do_symbol);

			auto do_label = _get_label();
			_insert_label(do_label);

			_lexical_analyzer.next();
			_parse_statement();

			if (_lexical_analyzer.last() != Symbol::while_symbol)
			{
				_error(Error::while_identifier_required);
				_lexical_analyzer.skip_to_next_statement();
				return;
			}

			if (_lexical_analyzer.next() != Symbol::left_parenthesis)
			{
				_error(Error::condition_required);
				_lexical_analyzer.skip_to_next_statement();
				return;
			}

			_lexical_analyzer.next();
			_parse_condition(do_label, Variable::null);

			__check_character_and_raise_if_false(Symbol::right_parenthesis);
			_debug("Generated do-while statement");
		}

		// ＜有返回值函数调用语句＞; |＜无返回值函数调用语句＞;｜＜赋值语句＞;
		// 前置符号：identifier
		// 后置符号：下一个语句的首符号
		void Parser::_parse_statment_started_with_identifier()
		{
			assert(_lexical_analyzer.last() == Symbol::identifier);

			auto name = _lexical_analyzer.last().second;

			if (_lexical_analyzer.next() == Symbol::left_parenthesis)
			{
				_parse_function_call(name);

				__check_character_and_raise_if_false(Symbol::semicolon);

				_debug("Generated function call statment");
			}
			else if (_lexical_analyzer.last() == Symbol::assign || _lexical_analyzer.last() == Symbol::left_square_brace)
			{
				_parse_assign_statement(name);
			}
			else
			{
				_error(Error::invalid_identifier);
				_lexical_analyzer.skip_to_next_statement();
			}
		}

		// scanf '('＜标识符＞{,＜标识符＞}')'
		// 前置符号：scanf
		// 后置符号：下一个语句的首符号
		void Parser::_parse_scanf_statement()
		{
			assert(_lexical_analyzer.last() == Symbol::scanf_symbol);

			bool need_comma = false;

			_lexical_analyzer.next();
			__check_character_and_raise_if_false(Symbol::left_parenthesis);

			while (_lexical_analyzer.last() != Symbol::right_parenthesis)
			{
				if (need_comma)
				{
					__check_character_and_raise_if_false(Symbol::comma);
				}
				need_comma = true;

				if (_lexical_analyzer.last() != Symbol::identifier)
				{
					_error(Error::invalid_identifier);
					_lexical_analyzer.skip_to_next_statement();
					return;
				}
				auto name = _lexical_analyzer.last().second;

				PseudoInstruction pi(PseudoOperator::read, Variable::null, Variable::null, _get_variable(name));
				_insert_instruction(pi);
				_debug("Data read to identifier: " + name);

				_lexical_analyzer.next();
			}

			_lexical_analyzer.next();
			__check_character_and_raise_if_false(Symbol::semicolon);
		}

		// return['('＜表达式＞')']
		// 前置符号：reutrn
		// 后置符号：下一个语句的首符号
		void Parser::_parse_return_statement()
		{
			assert(_lexical_analyzer.last() == Symbol::return_symbol);

			auto res = Variable::null;

			if (_lexical_analyzer.next() == Symbol::left_parenthesis)
			{
				_lexical_analyzer.next();
				res = _parse_expression();
				__check_character_and_raise_if_false(Symbol::right_parenthesis);
			}

			__check_character_and_raise_if_false(Symbol::semicolon);

			if (res == Variable::null)
			{
				if (_get_current_function_detail()->get_return_type() != Symbol::void_symbol)
				{
					_error(Error::illegal_return_type);
					return;
				}
				_insert_instruction(PseudoInstruction(PseudoOperator::ret, Variable::null, Variable::null, Variable::null));
			}
			else
			{
				if (_get_current_function_detail()->get_return_type() == Symbol::void_symbol)
				{
					_error(Error::illegal_return_type);
					return;
				}
				_insert_instruction(PseudoInstruction(PseudoOperator::ret, res, Variable::null, Variable::null));
			}
			_debug("Generated return statement");
		}

		//  if '('＜条件＞')'＜语句＞［else＜语句＞］
		// 前置符号: if
		// 后置符号: 下一个语句的首符号
		void Parser::_parse_if_statement()
		{
			assert(_lexical_analyzer.last() == Symbol::if_symbol);

			if (_lexical_analyzer.next() != Symbol::left_parenthesis)
			{
				_error(Error::condition_required);
				_lexical_analyzer.skip_to_next_statement();
				return;
			}

			_lexical_analyzer.next();

			auto else_label = _get_label();

			_parse_condition(Variable::null, else_label);

			__check_character_and_raise_if_false(Symbol::right_parenthesis);

			_parse_statement();

			if (_lexical_analyzer.last() != Symbol::else_symbol)
			{
				_insert_label(else_label);
				_debug("Generated if statement");
				return;
			}

			auto finally_label = _get_label();
			_insert_instruction(PseudoInstruction(PseudoOperator::b, finally_label, Variable::null, Variable::null));
			_insert_label(else_label);

			_lexical_analyzer.next();
			_parse_statement();

			_insert_label(finally_label);
			_debug("Generated if-else statement");
		}

		// for'('＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)＜步长＞')'＜语句＞
		// 前置字符: for
		// 后置字符: 下一个语句的首符号
		void Parser::_parse_for_statement()
		{
			assert(_lexical_analyzer.last() == Symbol::for_symbol);

			_lexical_analyzer.next();
			__check_character_and_raise_if_false(Symbol::left_parenthesis);

			if (_lexical_analyzer.last() != Symbol::identifier)
			{
				_error(Error::invalid_identifier);
				_lexical_analyzer.skip_to_next_statement();
				return;
			}
			auto loop_variable = _get_variable(_lexical_analyzer.last().second);

			_lexical_analyzer.next();
			__check_character_and_raise_if_false(Symbol::assign);

			auto init_value = _parse_expression();

			_insert_instruction(PseudoInstruction(PseudoOperator::assign, init_value, Variable::null, loop_variable));
			auto cond_label = _get_label();
			_insert_label(cond_label);

			__check_character_and_raise_if_false(Symbol::semicolon);

			auto finally_label = _get_label();
			_parse_condition(Variable::null, finally_label);

			__check_character_and_raise_if_false(Symbol::semicolon);

			if (_lexical_analyzer.last() != Symbol::identifier)
			{
				_error(Error::invalid_identifier);
				_lexical_analyzer.skip_to_next_statement();
				return;
			}
			auto left_identifier = _get_variable(_lexical_analyzer.last().second);

			_lexical_analyzer.next();
			__check_character_and_raise_if_false(Symbol::assign);

			if (_lexical_analyzer.last() != Symbol::identifier)
			{
				_error(Error::invalid_identifier);
				_lexical_analyzer.skip_to_next_statement();
				return;
			}
			auto right_identifier = _get_variable(_lexical_analyzer.last().second);

			int step_sign = 1;
			if (_lexical_analyzer.next() == Symbol::minus)
			{
				step_sign = -1;
			}
			else if (_lexical_analyzer.last() != Symbol::plus)
			{
				_error(Error::invalid_operator);
				_lexical_analyzer.skip_to_next_statement();
				return;
			}

			if (_lexical_analyzer.next() != Symbol::unsigned_integer)
			{
				_error(Error::invalid_integer);
				_lexical_analyzer.skip_to_next_statement();
				return;
			}
			int step_abs = std::atoi(_lexical_analyzer.last().second.c_str());
			if (step_abs <= 0)
			{
				_error(Error::invalid_integer);
				step_abs = 1;
			}
			int step = step_sign * step_abs;

			PseudoInstruction loop_ins(PseudoOperator::add, right_identifier, _get_literal(step), left_identifier);

			_lexical_analyzer.next();
			__check_character_and_raise_if_false(Symbol::right_parenthesis);

			_parse_statement();

			_insert_instruction(loop_ins);
			_insert_instruction(PseudoInstruction(PseudoOperator::b, cond_label, Variable::null, Variable::null));
			_insert_label(finally_label);

			_debug("Generated for statement");
		}

		// printf'('＜字符串＞,＜表达式＞')' | printf '('＜字符串＞')' | printf '('＜表达式＞')'
		// 前置符号：printf
		// 后置符号：下一个语句的首符号
		void Parser::_parse_printf_statement()
		{
			assert(_lexical_analyzer.last() == Symbol::printf_symbol);

			_lexical_analyzer.next();
			__check_character_and_raise_if_false(Symbol::left_parenthesis);

			if (_lexical_analyzer.last() == Symbol::right_parenthesis)
			{
				_error(Error::invalid_argument);
				_lexical_analyzer.skip_to_next_statement();
				return;
			}
			if (_lexical_analyzer.last() == Symbol::string)
			{
				string_type string = _lexical_analyzer.last().second;
				_debug("Registered string: \"" + string + "\"");
				auto str_variable = _get_string_variable(string);
				_insert_instruction(PseudoInstruction(PseudoOperator::print, str_variable, Variable::null, Variable::null));

				if (_lexical_analyzer.next() == Symbol::right_parenthesis)
				{
					_lexical_analyzer.next();
					__check_character_and_raise_if_false(Symbol::semicolon);

					_debug("Generated printf statement");
					return;
				}
				if (_lexical_analyzer.last() != Symbol::comma)
				{
					_error(Error::character_required, "')'");
					_lexical_analyzer.skip_to_next_statement();
					return;
				}
				_lexical_analyzer.next();
			}

			auto v = _parse_expression();
			_insert_instruction(PseudoInstruction(PseudoOperator::print, Variable::null, v, Variable::null));
			_debug("Generated printf statement");

			__check_character_and_raise_if_false(Symbol::right_parenthesis);

			__check_character_and_raise_if_false(Symbol::semicolon);
		}

		// ＜表达式＞＜关系运算符＞＜表达式＞｜＜表达式＞
		// 前置符号：任意，会主动报错
		// 后置符号：其后的首符号
		void Parser::_parse_condition(const ptr_of_variable &lable_true, const ptr_of_variable &label_false)
		{
			auto la = _parse_expression();

			if (!SymbolUtilities::is_compare_operator(_lexical_analyzer.last().first))
			{
				if (lable_true != Variable::null)
				{
					_insert_instruction(PseudoInstruction(PseudoOperator::bne, la, _get_literal(0), lable_true));
				}
				if (label_false != Variable::null)
				{
					_insert_instruction(PseudoInstruction(PseudoOperator::beq, la, _get_literal(0), label_false));
				}
				_debug("Generated condition");
				return;
			}
			Symbol compare_operator = _lexical_analyzer.last().first;

			_lexical_analyzer.next();
			auto ra = _parse_expression();

			ptr_of_variable res;
			switch (compare_operator)
			{
			case Symbol::equal:
				if (lable_true != Variable::null)
				{
					_insert_instruction(PseudoInstruction(PseudoOperator::beq, la, ra, lable_true));
				}
				if (label_false != Variable::null)
				{
					_insert_instruction(PseudoInstruction(PseudoOperator::bne, la, ra, label_false));
				}
				break;
			case Symbol::not_equal:
				if (lable_true != Variable::null)
				{
					_insert_instruction(PseudoInstruction(PseudoOperator::bne, la, ra, lable_true));
				}
				if (label_false != Variable::null)
				{
					_insert_instruction(PseudoInstruction(PseudoOperator::beq, la, ra, label_false));
				}
				break;
			case Symbol::greater:
			{
				res = _declear_temp_variable(Symbol::int_symbol);
				_insert_instruction(PseudoInstruction(PseudoOperator::sub, la, ra, res));
				if (lable_true != Variable::null)
				{
					_insert_instruction(PseudoInstruction(PseudoOperator::bgtz, res, _get_literal(0), lable_true));
				}
				if (label_false != Variable::null)
				{
					_insert_instruction(PseudoInstruction(PseudoOperator::blez, res, _get_literal(0), label_false));
				}
				break;
			}
			case Symbol::less:
			{
				res = _declear_temp_variable(Symbol::int_symbol);
				_insert_instruction(PseudoInstruction(PseudoOperator::sub, ra, la, res));
				if (lable_true != Variable::null)
				{
					_insert_instruction(PseudoInstruction(PseudoOperator::bgtz, res, _get_literal(0), lable_true));
				}
				if (label_false != Variable::null)
				{
					_insert_instruction(PseudoInstruction(PseudoOperator::blez, res, _get_literal(0), label_false));
				}
				break;
			}
			case Symbol::greater_equal:
			{
				res = _declear_temp_variable(Symbol::int_symbol);
				_insert_instruction(PseudoInstruction(PseudoOperator::sub, la, ra, res));
				if (lable_true != Variable::null)
				{
					_insert_instruction(PseudoInstruction(PseudoOperator::bgez, res, _get_literal(0), lable_true));
				}
				if (label_false != Variable::null)
				{
					_insert_instruction(PseudoInstruction(PseudoOperator::bltz, res, _get_literal(0), label_false));
				}
				break;
			}
			case Symbol::less_equal:
			{
				res = _declear_temp_variable(Symbol::int_symbol);
				_insert_instruction(PseudoInstruction(PseudoOperator::sub, ra, la, res));
				if (lable_true != Variable::null)
				{
					_insert_instruction(PseudoInstruction(PseudoOperator::bgez, res, _get_literal(0), lable_true));
				}
				if (label_false != Variable::null)
				{
					_insert_instruction(PseudoInstruction(PseudoOperator::bltz, res, _get_literal(0), label_false));
				}
				break;
			}
			default:
				assert(false);
				break;
			}

			_debug("Generated condition");
			return;
		}

		// '('＜值参数表＞')'
		// 前置符号: '('
		// 后置符号: ')'之后的首符号
		typename Parser::ptr_of_variable
			Parser::_parse_function_call(const string_type & name)
		{
			assert(_lexical_analyzer.last() == Symbol::left_parenthesis);

			bool need_comma = false;

			_lexical_analyzer.next();

			std::list<ptr_of_variable> arg_list;
			while (_lexical_analyzer.last() != Symbol::right_parenthesis)
			{
				if (need_comma)
				{
					if (!_check_character_and_raise_if_false(Symbol::comma))
					{
						return Variable::null;
					}
					_lexical_analyzer.next();
				}
				need_comma = true;

				arg_list.push_back(_parse_expression());
			}

			if (_get_function_detail(name) == FunctionDetail::null)
			{
				_error(Error::identifier_not_defined);
				_lexical_analyzer.next();
				return Variable::null;
			}

			if (_get_function_detail(name)->get_parameter_list().size() != arg_list.size())
			{
				_error(Error::illegal_argument_list);
				_lexical_analyzer.next();
				return Variable::null;
			}

			_lexical_analyzer.next();

			for (const ptr_of_variable &v : arg_list)
			{
				_insert_instruction(PseudoInstruction(PseudoOperator::arg, v, Variable::null, Variable::null));
			}

			if (_get_function_detail(name)->get_return_type() == Symbol::void_symbol)
			{
				_insert_instruction(PseudoInstruction(PseudoOperator::call, _get_function_label(name), Variable::null, Variable::null));
				_debug("Generated function call");
				return Variable::null;
			}

			auto res = _declear_temp_variable(Symbol::int_symbol);
			_insert_instruction(PseudoInstruction(PseudoOperator::call, _get_function_label(name), Variable::null, res));
			_debug("Generated function call");
			return res;
		}

		// ＜标识符＞＝＜表达式＞|＜标识符＞'['＜表达式＞']'=＜表达式＞
		// 前置符号: '=' '['
		// 后置符号: 下一个语句的首符号
		void Parser::_parse_assign_statement(const string_type & name)
		{
			assert(_lexical_analyzer.last() == Symbol::assign || _lexical_analyzer.last() == Symbol::left_square_brace);

			auto left_identifier = _get_variable(name);

			auto index = Variable::null;
			if (_lexical_analyzer.last() == Symbol::left_square_brace)
			{
				_lexical_analyzer.next();
				index = _parse_expression();

				if (left_identifier->get_length() == 0)
				{
					_error(Error::identifier_is_not_array);
				}

				__check_character_and_raise_if_false(Symbol::right_square_brace);
			}

			__check_character_and_raise_if_false(Symbol::assign);

			auto value = _parse_expression();

			if (index == Variable::null)
			{
				PseudoInstruction pi(PseudoOperator::assign, value, Variable::null, left_identifier);
				_insert_instruction(pi);
			}
			else
			{
				PseudoInstruction pi(PseudoOperator::store, index, value, left_identifier);
				_insert_instruction(pi);
			}
			_debug("Generated assign statement");
			__check_character_and_raise_if_false(Symbol::semicolon);
		}

		// ［＋｜－］＜项＞{＜加法运算符＞＜项＞}
		// 前置符号: 任意，会主动报错
		// 后置符号: 其后的首符号
		typename Parser::ptr_of_variable Parser::_parse_expression()
		{
			bool first_negate = false;
			if (_lexical_analyzer.last() == Symbol::plus)
			{
				_lexical_analyzer.next();
			}
			else if (_lexical_analyzer.last() == Symbol::minus)
			{
				first_negate = true;
				_lexical_analyzer.next();
			}

			auto res = _parse_term();
			if (first_negate)
			{
				auto ra = res;
				res = _declear_temp_variable(ra->get_type());
				PseudoInstruction pi(PseudoOperator::sub, _get_literal(0), ra, res);
				_insert_instruction(pi);
			}

			while (_lexical_analyzer.last() == Symbol::plus || _lexical_analyzer.last() == Symbol::minus)
			{
				Symbol operator_ = _lexical_analyzer.last().first;

				_lexical_analyzer.next();

				auto la = res;
				auto ra = _parse_term();
				res = _declear_temp_variable(SymbolUtilities::type_after_calculate(la->get_type(), ra->get_type()));
				PseudoOperator op = operator_ == Symbol::plus ? PseudoOperator::add : PseudoOperator::sub;
				PseudoInstruction pi(op, la, ra, res);
				_insert_instruction(pi);
			}

			_debug("Generated expression");
			return res;
		}

		// ＜因子＞{＜乘法运算符＞＜因子＞}
		// 前置符号: 任意，会主动报错
		// 后置符号: 其后的首符号
		typename Parser::ptr_of_variable Parser::_parse_term()
		{
			ptr_of_variable res = Variable::null;

			res = _parse_factor();

			while (_lexical_analyzer.last() == Symbol::asterisk || _lexical_analyzer.last() == Symbol::slash)
			{
				Symbol operator_ = _lexical_analyzer.last().first;

				_lexical_analyzer.next();

				auto la = res;
				auto ra = _parse_factor();
				res = _declear_temp_variable(SymbolUtilities::type_after_calculate(la->get_type(), ra->get_type()));
				PseudoOperator op = operator_ == Symbol::asterisk ? PseudoOperator::mul : PseudoOperator::div;
				PseudoInstruction pi(op, la, ra, res);
				_insert_instruction(pi);
			}

			_debug("Generated term");
			return res;
		}

		// ＜标识符＞｜＜标识符＞'['＜表达式＞']'｜＜整数＞|＜字符＞｜＜有返回值函数调用语句＞|'('＜表达式＞')'
		// 前置符号: 任意，会主动报错
		// 后置符号: 其后的首符号
		typename Parser::ptr_of_variable Parser::_parse_factor()
		{
			ptr_of_variable res = Variable::null;

			switch (_lexical_analyzer.last().first)
			{
			case Symbol::identifier:
			{
				string_type name = _lexical_analyzer.last().second;

				// 取数组值
				if (_lexical_analyzer.next() == Symbol::left_square_brace)
				{
					_lexical_analyzer.next();
					auto offset = _parse_expression();

					if (!_check_character_and_raise_if_false(Symbol::right_square_brace))
					{
						return Variable::null;
					}
					_lexical_analyzer.next();

					auto la = _get_variable(name);
					res = _declear_temp_variable(la->get_type());
					PseudoInstruction ins(PseudoOperator::load, la, offset, res);
					_insert_instruction(ins);
				}
				// 函数调用
				else if (_lexical_analyzer.last() == Symbol::left_parenthesis)
				{
					res = _parse_function_call(name);
					if (res == Variable::null)
					{
						_error(Error::use_return_value_of_void_function);
						res = _get_literal(1);
					}
				}
				// 取值
				else
				{
					res = _get_variable(name);
				}
			}
			break;
			case Symbol::unsigned_integer:
			case Symbol::plus:
			case Symbol::minus:
				res = _get_literal(_parse_signed_integer(
					[](Symbol s) {return s == Symbol::left_curly_brace || s == Symbol::right_curly_brace; }
				));
				break;
			case Symbol::character:
				res = _get_literal(_lexical_analyzer.last().second.front());
				_lexical_analyzer.next();
				break;
			case Symbol::left_parenthesis:
				_lexical_analyzer.next();
				res = _parse_expression();

				if (!_check_character_and_raise_if_false(Symbol::right_parenthesis))
				{
					return Variable::null;
				}
				_lexical_analyzer.next();
				break;
			default:
				_error(Error::invalid_factor);
				_lexical_analyzer.skip_to_next_statement();
				return res;
			}

			_debug("Generated factor");
			return res;
		}


		bool Parser::_is_eof()
		{
			return _lexical_analyzer.last() == Symbol::eof;
		}


		typename Parser::ptr_of_variable
			Parser::_declear_variable(const string_type & name, Symbol type)
		{
			auto p = _pseudo_table.declear_variable(name, type);
			if (!p.second)
			{
				_error(Error::identifier_already_defined, name);
			}
			return p.first;
		}


		typename Parser::ptr_of_variable
			Parser::_declear_const_variable(const string_type & name, Symbol type, int value)
		{
			auto p = _pseudo_table.declear_const_variable(name, type, value);
			if (!p.second)
			{
				_error(Error::identifier_already_defined, name);
			}
			return p.first;
		}


		typename Parser::ptr_of_variable
			Parser::_declear_array(const string_type & name, Symbol type, size_t length)
		{
			auto p = _pseudo_table.declear_array(name, type, length);
			if (!p.second)
			{
				_error(Error::identifier_already_defined, p.first->get_name());
			}
			return p.first;
		}


		typename Parser::ptr_of_function_detail Parser::_declear_function(const FunctionIdentifier & f, Symbol type)
		{
			auto p = _pseudo_table.declear_function(f, type);
			if (!p.second)
			{
				_error(Error::identifier_already_defined, f.get_name());
			}
			return p.first;
		}


		typename Parser::ptr_of_variable Parser::_declear_parameter(const string_type & name, Symbol type)
		{
			auto p = _pseudo_table.declear_parameter(name, type);
			if (!p.second)
			{
				_error(Error::identifier_already_defined, name);
			}
			return p.first;
		}


		typename Parser::ptr_of_variable Parser::_declear_temp_variable(Symbol type)
		{
			return _pseudo_table.declear_temp_variable(type);
		}


		typename Parser::ptr_of_variable Parser::_get_label()
		{
			return _pseudo_table.get_label();
		}


		typename Parser::ptr_of_variable Parser::_get_function_label(const string_type &name)
		{
			return _pseudo_table.get_function_label(name);
		}


		void Parser::_insert_label(const ptr_of_variable & label)
		{
			_insert_instruction(PseudoInstruction(PseudoOperator::label, label, Variable::null, Variable::null));
		}


		typename Parser::ptr_of_variable Parser::_get_literal(int value)
		{
			return _pseudo_table.get_literal(value);
		}


		typename Parser::ptr_of_variable Parser::_get_variable(const string_type & name) const
		{
			auto r = _pseudo_table.get_variable(name);
			if (r == Variable::null)
			{
				_error(Error::identifier_not_defined, name);
			}
			return r;
		}


		typename Parser::ptr_of_variable Parser::_get_string_variable(const string_type & str) const
		{
			return _pseudo_table.get_string_variable(str);
		}


		void Parser::_insert_instruction(const PseudoInstruction & pi)
		{
			auto error = _pseudo_table.insert_instruction(pi);
			if (error != Error::null)
			{
				_error(error);
			}
		}


		PseudoInstruction Parser::_get_last_instruction() const
		{
			return _pseudo_table.get_last_instruction();
		}


		Parser::ptr_of_function_detail Parser::_get_current_function_detail() const
		{
			return _pseudo_table.get_current_function_detail();
		}

		Parser::ptr_of_function_detail Parser::_get_function_detail(const FunctionIdentifier & name) const
		{
			return _pseudo_table.get_function_detail(name);
		}


		void Parser::parse_to_pseudo()
		{
			_parse_program();
			_is_eof();
		}
	}
}
