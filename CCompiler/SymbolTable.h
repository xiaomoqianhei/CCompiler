#pragma once

#include "Symbol.h"
#include "Variable.h"
#include "FunctionDetail.h"

#include <unordered_set>
#include <algorithm>
#include <sstream>

namespace Yradex
{
	namespace CCompiler
	{
		class SymbolTable
		{

		public:
			using string_type = std::string;
			using char_type = typename string_type::value_type;

		private:
			std::unordered_map<std::shared_ptr<Variable>, const string_type, VariableHasher> _string_map;
			std::unordered_set<std::shared_ptr<Variable>, VariableHasher> _global_set;
			std::unordered_map<FunctionIdentifier, 
				std::unordered_set<std::shared_ptr<Variable>, VariableHasher>, 
				FunctionHasher> _variable_map;
			std::unordered_map<FunctionIdentifier, std::shared_ptr<FunctionDetail>, FunctionHasher> _function_map;

		public:
			bool insert_variable(const std::shared_ptr<Variable>& v);
			std::pair<std::shared_ptr<FunctionDetail>, bool> insert_function(const FunctionIdentifier& f, Symbol type);
			bool insert_parameter(const FunctionIdentifier& f, const std::shared_ptr<Variable> &v);

			void set_leaf_function(const FunctionIdentifier& f,bool b)
			{
				_function_map.at(f)->set_leaf(b);
			}  

			std::shared_ptr<Variable> get_string(const string_type &str);
			std::shared_ptr<Variable> get(const string_type &name, const FunctionIdentifier &function);
			std::shared_ptr<Variable> get_literal(int value);
			std::shared_ptr<FunctionDetail> get_function_detail(const FunctionIdentifier &function);

			std::vector<FunctionIdentifier> get_function_list() const;

			const std::unordered_set<std::shared_ptr<Variable>, VariableHasher>& 
				get_variable_set(const FunctionIdentifier &function);

			void shrink_variable_set(const FunctionIdentifier &function);

			const std::unordered_set<std::shared_ptr<Variable>, VariableHasher>&
				get_global_variable_set() const;
			const std::unordered_map<std::shared_ptr<Variable>, const string_type, VariableHasher>&
				get_string_map() const;

			void set_max_used_register(const FunctionIdentifier &function, size_t n)
			{
				_function_map.at(function)->set_max_used_register(n);
			}

			size_t get_max_used_register(const FunctionIdentifier &function)
			{
				return _function_map.at(function)->get_max_used_register();
			}
		};


	}
}
