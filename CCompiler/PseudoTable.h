#pragma once

#include "Utility.h"
#include "PseudoInstruction.h"
#include "BasicErrorHandler.h"
#include "SymbolTable.h"

#include <list>
#include <unordered_map>

namespace Yradex
{
	namespace CCompiler
	{

		class PseudoTable
		{
		public:
			class PseudoTableFunctionSwitcher
			{
			private:
				PseudoTable &_table;
				FunctionIdentifier _f;
			public:
				PseudoTableFunctionSwitcher(PseudoTable &table, const FunctionIdentifier &f)
					: _table(table), _f(_table.get_current_function_identifier())
				{
					_table.switch_function(f);
				}

				~PseudoTableFunctionSwitcher()
				{
					_table.switch_function(_f);
				}
			};

		private:
			using string_type = std::string;
			using char_type = typename string_type::value_type;
			using error_handler_type = BasicErrorHandler;

		private:
			SymbolTable &_symbol_table;

			std::unordered_map<const FunctionIdentifier, std::list<PseudoInstruction>, FunctionHasher> _instruction_map;
			FunctionIdentifier _current_function;

		private:
			template <typename T>
			void _debug(const T &info)
			{
				if (Config::pseudo_table_debug)
				{
					std::basic_ostringstream<char_type> stream;
					stream << "PseudoTable: " << info << std::flush;
					error_handler_type::instance().debug(stream.str());
				}
			}

		public:
			PseudoTable(SymbolTable &table)
				: _symbol_table(table), _current_function(FunctionIdentifier::global)
			{
			}

			void switch_function(const FunctionIdentifier &f);

			Error insert_instruction(const PseudoInstruction &i);

			std::pair<std::shared_ptr<Variable>, bool> declear_variable(const string_type &name, Symbol type)
			{
				std::shared_ptr<Variable> v = Variable::new_variale(name, type, _current_function, false);
				return std::make_pair(v, _symbol_table.insert_variable(v));
			}
			std::pair<std::shared_ptr<Variable>, bool> declear_const_variable(const string_type &name, Symbol type, int value)
			{
				std::shared_ptr<Variable> v = Variable::new_const_variable(name, type, value, _current_function, false);
				return std::make_pair(v, _symbol_table.insert_variable(v));
			}
			std::pair<std::shared_ptr<FunctionDetail>, bool> declear_function(const FunctionIdentifier &f, Symbol type)
			{
				return _symbol_table.insert_function(f, type);
			}
			std::pair<std::shared_ptr<Variable>, bool> declear_parameter(const string_type &name, Symbol type)
			{
				auto v = Variable::new_variale(name, type, _current_function, false);
				return std::make_pair(v, _symbol_table.insert_parameter(_current_function, v));
			}
			std::shared_ptr<Variable> declear_temp_variable(Symbol type)
			{
				auto v = Variable::get_temp_variable(type, _current_function);
				_symbol_table.insert_variable(v);
				return v;
			}
			std::pair<std::shared_ptr<Variable>, bool> declear_array(const string_type &name, Symbol type, size_t length)
			{
				auto v = Variable::new_array_variable(name, type, length, _current_function);
				return std::make_pair(v, _symbol_table.insert_variable(v));
			}

			std::shared_ptr<Variable> get_label()
			{
				auto label = Variable::get_label();
				return label;
			}
			std::shared_ptr<Variable> get_function_label(const string_type &name)
			{
				return Variable::get_label(name);
			}

			std::shared_ptr<Variable> get_literal(int value)
			{
				return _symbol_table.get_literal(value);
			}
			std::shared_ptr<Variable> get_variable(const string_type &name) const
			{
				return _symbol_table.get(name, _current_function);
			}
			std::shared_ptr<Variable> get_string_variable(const string_type &str) const
			{
				return _symbol_table.get_string(str);
			}
			FunctionIdentifier get_current_function_identifier() const
			{
				return _current_function;
			}
			std::shared_ptr<FunctionDetail> get_current_function_detail() const
			{
				return _symbol_table.get_function_detail(_current_function);
			}
			std::shared_ptr<FunctionDetail> get_function_detail(const FunctionIdentifier &f) const
			{
				return _symbol_table.get_function_detail(f);
			}

			PseudoInstruction get_last_instruction() const
			{
				if (_instruction_map.find(_current_function) == _instruction_map.cend())
				{
					return PseudoInstruction(PseudoOperator::nop, Variable::null, Variable::null, Variable::null);
				}
				return _instruction_map.at(_current_function).back();
			}

			std::vector<FunctionIdentifier> get_function_list() const
			{
				return _symbol_table.get_function_list();
			}

			std::list<PseudoInstruction> get_current_instruction_list() const
			{
				return _instruction_map.at(_current_function);
			}
			void set_instruction_list(const std::list<PseudoInstruction> &list)
			{
				_instruction_map.at(_current_function) = list;
			}

			const std::unordered_set<std::shared_ptr<Variable>, VariableHasher>& get_variable_set()
			{
				return _symbol_table.get_variable_set(_current_function);
			}

			void shrink_variable_set()
			{
				_symbol_table.shrink_variable_set(_current_function);
			}

			const std::unordered_set<std::shared_ptr<Variable>, VariableHasher>&
				get_global_variable_set() const
			{
				return _symbol_table.get_global_variable_set();
			}
			const std::unordered_map<std::shared_ptr<Variable>, const string_type, VariableHasher>&
				get_string_map() const
			{
				return _symbol_table.get_string_map();
			}

			void set_max_used_register(size_t n)
			{
				return _symbol_table.set_max_used_register(_current_function, n);
			}

			size_t get_max_used_register()
			{
				return _symbol_table.get_max_used_register(_current_function);
			}
		};
	}
}

