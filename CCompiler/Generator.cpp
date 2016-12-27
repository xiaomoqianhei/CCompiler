#include "Generator.h"


namespace Yradex
{
	namespace CCompiler
	{
		void Generator::_generate_data()
		{
			_stream << _separator << ".data" << std::endl;
			
			int position = 0x10010000;
			// global variables
			auto set = _table.get_global_variable_set();
			for (auto &v : set)
			{
				if (v->is_const())
				{
					continue;
				}
				v->set_address(VariableAddress(false, position));
				int size = 4 * std::max(1u, v->get_length());
				position += size;

				_stream << v->get_name() << ":" << _separator
					<< ".space" << _separator 
					<< size << std::endl;
			}

			// strings
			auto map = _table.get_string_map();
			for (auto &v : map)
			{
				_stream << v.first->get_name() << ":" << _separator
					<< ".asciiz" << _separator
					<< "\"" << v.second << "\"" << std::endl;
			}
		}
		void Generator::_generate_text()
		{
			_stream << _separator << ".text" << std::endl;

			// global init
			_print_instruction(MipsOperator::add, "$fp", "$sp", "$zero");
			_print_instruction(MipsOperator::la, "$ra", "finish");

			// main func
			{
				PseudoTable::PseudoTableFunctionSwitcher switcher(_table, string_type("main"));
				_generate_function();
			}

			// other func
			auto function_list = _table.get_function_list();
			for (auto &f : function_list)
			{
				if (f == string_type("main"))
				{
					continue;
				}
				PseudoTable::PseudoTableFunctionSwitcher switcher(_table, f);
				_generate_function();
			}

			// ending
			_stream << "finish:" << std::endl;
		}
		void Generator::_generate_function()
		{
			// function name
			_stream << std::endl << std::endl << 
				_table.get_current_function_identifier().get_name() << ":" << std::endl;

			int relative_sp = -8 - _table.get_current_function_detail()->get_parameter_list().size() * 4;

			// save $s0 - $s7 registers
			size_t i = 16;
			for (; i <= _table.get_max_used_register(); ++i)
			{
				relative_sp -= 4;
				std::ostringstream s;
				s << relative_sp << "($fp)";
				_print_instruction(MipsOperator::sw, VariableAddress(true, i), s.str());
			}
			_lower_address_of_saved_s_register_to_fp = relative_sp;

			// modify addresses of variables
			int lowest_sp = relative_sp;
			auto variable_set = _table.get_variable_set();
			for (auto &v : variable_set)
			{
				if (v->get_variable_type() != Variable::Type::array && v->get_variable_type() != Variable::Type::variable)
				{
					continue;
				}

				if (!v->in_register())
				{
					v->set_address(VariableAddress(false, v->position() - 4 * (i - 16)));
					lowest_sp = std::min(lowest_sp, v->position());
				}
			}

			// move sp
			_print_instruction(MipsOperator::add, "$sp", "$sp", lowest_sp - 4);

			// generate ins
			auto list = _table.get_current_instruction_list();
			for (auto &ins : list)
			{
				_generate_instruction(ins);
			}

		}
		void Generator::_generate_instruction(const PseudoInstruction & ins)
		{
			// print pseudo code
			if (Config::generator_debug && error_handler_type::instance().should_raise(error_handler_type::Level::debug))
			{
				_stream << " " << _separator << "# " << ins << std::endl;
			}

			switch (ins.get_operator())
			{
			case PseudoOperator::add:
			{
				PseudoInstruction new_ins(ins);
				if (new_ins.get_left_argument()->is_const())
				{
					new_ins.swap_arguments();
				}

				string_type a1 = _get_argument_1(new_ins);
				string_type a2 = _get_argument_2(new_ins);

				// res
				auto res = new_ins.get_result();
				if (res->in_register())
				{
					_print_instruction(MipsOperator::add, res->address_as_string(), a1, a2);
				}
				else
				{
					VariableAddress res_new = VariableAddress(true, 2);
					_print_instruction(MipsOperator::add, res_new, a1, a2);
					_print_instruction(MipsOperator::sw, res_new, res->address_as_string());
				}
				break;
			}
			case PseudoOperator::sub:
			{
				string_type a1 = _get_argument_1(ins);

				if (ins.get_left_argument()->is_const())
				{
					_print_instruction(MipsOperator::add, "$v0", "$zero", a1);
					a1 = "$v0";
				}

				string_type a2 = _get_argument_2(ins);

				auto res = ins.get_result();
				if (res->in_register())
				{
					_print_instruction(MipsOperator::sub, res->address_as_string(), a1, a2);
				}
				else
				{
					VariableAddress res_new = VariableAddress(true, 2);
					_print_instruction(MipsOperator::sub, res_new, a1, a2);
					_print_instruction(MipsOperator::sw, res_new, res->address_as_string());
				}
				break;
			}
			case PseudoOperator::mul:
			{
				PseudoInstruction new_ins(ins);
				if (new_ins.get_left_argument()->is_const())
				{
					new_ins.swap_arguments();
				}

				string_type a1 = _get_argument_1(new_ins);
				string_type a2 = _get_argument_2(new_ins);

				auto res = new_ins.get_result();
				if (res->in_register())
				{
					_print_instruction(MipsOperator::mul, res->address_as_string(), a1, a2);
				}
				else
				{
					VariableAddress res_new = VariableAddress(true, 2);
					_print_instruction(MipsOperator::mul, res_new, a1, a2);
					_print_instruction(MipsOperator::sw, res_new, res->address_as_string());
				}
				break;
			}
			case PseudoOperator::div:
			{
				// arg1
				string_type a1 = _get_argument_1(ins);

				if (ins.get_left_argument()->is_const())
				{
					_print_instruction(MipsOperator::add, "$v0", "$zero", a1);
					a1 = "$v0";
				}

				// arg2
				string_type a2 = _get_argument_2(ins);

				// res
				auto res = ins.get_result();
				if (res->in_register())
				{
					_print_instruction(MipsOperator::div, res->address_as_string(), a1, a2);
				}
				else
				{
					VariableAddress res_new = VariableAddress(true, 2);
					_print_instruction(MipsOperator::div, res_new, a1, a2);
					_print_instruction(MipsOperator::sw, res_new, res->address_as_string());
				}
				break;
			}
			case PseudoOperator::b:
				_print_instruction(MipsOperator::b, ins.get_result());
				break;
			case PseudoOperator::beq:
			{
				PseudoInstruction new_ins(ins);
				if (new_ins.get_left_argument()->is_const())
				{
					new_ins.swap_arguments();
				}

				string_type a1 = _get_argument_1(new_ins);
				string_type a2 = _get_argument_2(new_ins);
				_print_instruction(MipsOperator::beq, a1, a2, new_ins.get_result());
				break;
			}
			case PseudoOperator::bne:
			{
				PseudoInstruction new_ins(ins);
				if (new_ins.get_left_argument()->is_const())
				{
					new_ins.swap_arguments();
				}

				string_type a1 = _get_argument_1(new_ins);
				string_type a2 = _get_argument_2(new_ins);
				_print_instruction(MipsOperator::bne, a1, a2, new_ins.get_result());
				break;
			}
			case PseudoOperator::bltz:
			{
				string_type a1 = _get_argument_1(ins);
				_print_instruction(MipsOperator::bltz, a1, ins.get_result());
				break;
			}
			case PseudoOperator::blez:
			{
				string_type a1 = _get_argument_1(ins);
				_print_instruction(MipsOperator::blez, a1, ins.get_result());
				break;
			}
			case PseudoOperator::bgtz:
			{
				string_type a1 = _get_argument_1(ins);
				_print_instruction(MipsOperator::bgtz, a1, ins.get_result());
				break;
			}
			case PseudoOperator::bgez:
			{
				string_type a1 = _get_argument_1(ins);
				_print_instruction(MipsOperator::bgez, a1, ins.get_result());
				break;
			}
			case PseudoOperator::read:
			{
				if (ins.get_result()->get_type() == Symbol::int_symbol)
				{
					_print_instruction(MipsOperator::add, "$v0", "$zero", "5");
				}
				else
				{
					_print_instruction(MipsOperator::add, "$v0", "$zero", "12");
				}

				_print_instruction(MipsOperator::syscall);
				// res
				auto res = ins.get_result();
				if (res->in_register())
				{
					_print_instruction(MipsOperator::add, res->address_as_string(), "$zero", "$v0");
				}
				else
				{
					_print_instruction(MipsOperator::sw, "$v0", res->address_as_string());
				}
				break;
			}
			case PseudoOperator::print:
				// string
				if (ins.get_left_argument() != Variable::null())
				{
					if (!_table.get_current_function_detail()->get_parameter_list().empty())
					{
						_print_instruction(MipsOperator::sw, "$a0", "($sp)");			// store a0
						_print_instruction(MipsOperator::la, "$a0", ins.get_left_argument());	// load a0
						_print_instruction(MipsOperator::add, "$v0", "$zero", "4");		// syscall
						_print_instruction(MipsOperator::syscall);
						_print_instruction(MipsOperator::lw, "$a0", "($sp)");			// load a0
					}
					else
					{
						_print_instruction(MipsOperator::la, "$a0", ins.get_left_argument());	// load a0
						_print_instruction(MipsOperator::add, "$v0", "$zero", "4");		// syscall
						_print_instruction(MipsOperator::syscall);
					}
				}
				// variable
				if (ins.get_right_argument() != Variable::null())
				{
					auto a2 = _get_argument_2(ins);
					if (!_table.get_current_function_detail()->get_parameter_list().empty())
					{
						_print_instruction(MipsOperator::sw, "$a0", "($sp)");			// store a0
						_print_instruction(MipsOperator::add, "$a0", "$zero", a2);	// load a0
						if (ins.get_result()->get_value() == 0)		// var is int
						{
							_print_instruction(MipsOperator::add, "$v0", "$zero", "1");		// syscall
						}
						else
						{
							_print_instruction(MipsOperator::add, "$v0", "$zero", "11");		// syscall
						}
						_print_instruction(MipsOperator::syscall);
						_print_instruction(MipsOperator::lw, "$a0", "($sp)");			// load a0
					}
					else
					{
						_print_instruction(MipsOperator::add, "$a0", "$zero", a2);	// load a0
						if (ins.get_result()->get_value() == 0)		// var is int
						{
							_print_instruction(MipsOperator::add, "$v0", "$zero", "1");		// syscall
						}
						else
						{
							_print_instruction(MipsOperator::add, "$v0", "$zero", "11");		// syscall
						}
						_print_instruction(MipsOperator::syscall);
					}
				}
				break;
			case PseudoOperator::call:
			{
				// save a0 - a3
				_try_save_argument_register();

				// save fp
				_print_instruction(MipsOperator::sw, "$fp", "-4($sp)");
				_print_instruction(MipsOperator::add, "$fp", "$sp", "$zero");

				// save ra
				_print_instruction(MipsOperator::sw, "$ra", "-8($sp)");

				// CALL
				_print_instruction(MipsOperator::jal, ins.get_left_argument());

				// recover fp
				_print_instruction(MipsOperator::lw, "$fp", "-4($sp)");

				// recover ra
				_print_instruction(MipsOperator::lw, "$ra", "-8($sp)");

				// return value
				if (ins.get_result() != Variable::null())
				{
					auto res = ins.get_result();
					if (res->in_register())
					{
						_print_instruction(MipsOperator::add, res->address_as_string(), "$zero", "$v0");
					}
					else
					{
						_print_instruction(MipsOperator::sw, "$v0", res->address_as_string());
					}
				}

				// recover a0 - a3
				auto this_arg_count = _table.get_current_function_detail()->get_parameter_list().size();
				if (this_arg_count)
				{
					for (size_t i = 0; i < this_arg_count; ++i)
					{
						std::ostringstream s;
						s << i * 4 << "($sp)";
						_print_instruction(MipsOperator::lw, VariableAddress(true, i + 4), s.str());
					}
					_print_instruction(MipsOperator::add, "$sp", "$sp", 4 * (this_arg_count - 1));
				}

				_arg_count = 0;
				break;
			}
			case PseudoOperator::ret:
			{
				auto va = _get_argument_1(ins);
				_print_instruction(MipsOperator::add, "$v0", "$zero", va);

				// recover registers
				int relative_sp = _lower_address_of_saved_s_register_to_fp;
				size_t i = _table.get_max_used_register();
				for (; i >= 16; --i)
				{
					std::ostringstream s;
					s << relative_sp << "($fp)";
					relative_sp += 4;
					_print_instruction(MipsOperator::lw, VariableAddress(true, i), s.str());
				}

				// move sp
				_print_instruction(MipsOperator::add, "$sp", "$fp", "$zero");

				// return
				_print_instruction(MipsOperator::jr, "$ra");
				break;
			}
			case PseudoOperator::label:
				_stream << ins.get_left_argument() << ":" << std::endl;
				break;
			case PseudoOperator::assign:
			{
				// arg1
				string_type a1 = _get_argument_1(ins);

				// res
				auto res = ins.get_result();
				if (res->in_register())
				{
					_print_instruction(MipsOperator::add, res->address_as_string(), "$zero", a1);
				}
				else
				{
					VariableAddress res_new = VariableAddress(true, 2);
					_print_instruction(MipsOperator::add, res_new, "$zero", a1);
					_print_instruction(MipsOperator::sw, res_new, res->address_as_string());
				}
				break;
			}
			case PseudoOperator::load:
			{
				// FIX
				int addr = ins.get_left_argument()->position();
				if (ins.get_right_argument()->is_const())
				{
					addr += ins.get_right_argument()->get_value() * 4;
					_print_instruction(MipsOperator::add, "$v0", "$zero", addr);
				}
				else
				{
					string_type a2 = _get_argument_2(ins);
					_print_instruction(MipsOperator::mul, "$v0", a2, 4);
					_print_instruction(MipsOperator::add, "$v0", "$v0", addr);
				}

				if (ins.get_left_argument()->get_function() != FunctionIdentifier::global)
				{
					_print_instruction(MipsOperator::add, "$v0", "$v0", "$fp");
				}

				// res
				auto res = ins.get_result();
				if (res->in_register())
				{
					_print_instruction(MipsOperator::lw, res->address_as_string(), "($v0)");
				}
				else
				{
					VariableAddress res_new = VariableAddress(true, 2);
					_print_instruction(MipsOperator::lw, res_new, "($v0)");
					_print_instruction(MipsOperator::sw, res_new, res->address_as_string());
				}
				break;
			}
			case PseudoOperator::store:
			{
				// FIX
				string_type index = _get_argument_1(ins);
				string_type value = _get_argument_2(ins);

				if (ins.get_left_argument()->is_const())
				{
					_print_instruction(MipsOperator::add, "$v0", "$zero", ins.get_result()->position() + ins.get_left_argument()->get_value() * 4);
				}
				else
				{
					_print_instruction(MipsOperator::mul, "$v0", index, 4);
					_print_instruction(MipsOperator::add, "$v0", "$v0", ins.get_result()->position());
				}

				if (!(ins.get_result()->get_function() == FunctionIdentifier::global))
				{
					_print_instruction(MipsOperator::add, "$v0", "$v0", "$fp");
				}				

				if (ins.get_right_argument()->is_const())
				{
					_print_instruction(MipsOperator::add, VariableAddress(true, 3), "$zero", value);
					_print_instruction(MipsOperator::sw, VariableAddress(true, 3), "($v0)");
				}
				else
				{
					_print_instruction(MipsOperator::sw, value, "($v0)");
				}

				break;
			}
			case PseudoOperator::arg:
			{
				_try_save_argument_register();
				string_type value = _get_argument_1(ins);

				// if a? was wrote earlier then load it from stack
				if (ins.get_left_argument()->in_register())
				{
					int pos = ins.get_left_argument()->position();
					if (pos >= 4 && pos < 8)
					{
						value = "$v0";
						std::ostringstream s;
						s << pos * 4 - 16 << "($sp)";
						_print_instruction(MipsOperator::lw, value, s.str());
					}
				}

				if (_arg_count < 4)
				{
					_print_instruction(MipsOperator::add, VariableAddress(true, _arg_count + 4), "$zero", value);
				}
				else
				{
					std::ostringstream stream;
					stream << static_cast<int>(_arg_count - 4) * -4 - 12 << "($sp)";
					
					if (ins.get_left_argument()->get_variable_type() == Variable::Type::const_variable)
					{
						VariableAddress res_new = VariableAddress(true, 2);
						_print_instruction(MipsOperator::add, res_new, "$zero", value);
						_print_instruction(MipsOperator::sw, res_new, stream.str());
					}
					else
					{
						_print_instruction(MipsOperator::sw, value, stream.str());
					}
				}
				++_arg_count;
				break;
			}
			case PseudoOperator::nop:
				break;
			default:
				break;
			}
		}
		void Generator::_try_save_argument_register()
		{
			if (_arg_count == 0)
			{
				auto this_arg_count = _table.get_current_function_detail()->get_parameter_list().size();
				if (this_arg_count)
				{
					_print_instruction(MipsOperator::sub, "$sp", "$sp", 4 * (this_arg_count - 1));
					for (size_t i = 0; i < this_arg_count; ++i)
					{
						std::ostringstream s;
						s << i * 4 << "($sp)";
						_print_instruction(MipsOperator::sw, VariableAddress(true, i + 4), s.str());
					}
				}
			}
		}
		Generator::string_type Generator::_get_argument_1(const PseudoInstruction & ins)
		{
			if (ins.get_left_argument()->is_const())
			{
				std::ostringstream stream;
				stream << ins.get_left_argument()->get_value();
				return stream.str();
			}
			else
			{
				auto ta = ins.get_left_argument();
				if (!ta->in_register())
				{
					_print_instruction(MipsOperator::lw, "$v0", ta->address_as_string());
					return "$v0";
				}
				return ta->address_as_string();
			}
		}
		Generator::string_type Generator::_get_argument_2(const PseudoInstruction & ins)
		{
			if (ins.get_right_argument()->is_const())
			{
				std::ostringstream stream;
				stream << ins.get_right_argument()->get_value();
				return stream.str();
			}
			else
			{
				auto ta = ins.get_right_argument();
				if (!ta->in_register())
				{
					_print_instruction(MipsOperator::lw, "$v1", ta->address_as_string());
					return "$v1";
				}
				return ta->address_as_string();
			}
		}
		void Generator::generate()
		{
			_generate_data();
			_generate_text();
		}

	}
}