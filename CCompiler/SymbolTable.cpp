#include "SymbolTable.h"

namespace Yradex
{
	namespace CCompiler
	{
		bool SymbolTable::insert_variable(const std::shared_ptr<Variable> & v)
		{
			const FunctionIdentifier &f = v->get_function();
			if (f == FunctionIdentifier::global)
			{
				auto p = _global_set.insert(v);
				return p.second;
			}
			else
			{
				auto p = _variable_map.at(f).insert(v);
				return p.second;
			}
		}
		std::pair<std::shared_ptr<FunctionDetail>, bool> SymbolTable::insert_function(const FunctionIdentifier & f, Symbol type)
		{
			auto detail = std::make_shared<FunctionDetail>(type);
			auto p1 = _variable_map.insert(std::make_pair(f, std::unordered_set<std::shared_ptr<Variable>, VariableHasher>()));
			auto p2 = _function_map.insert(std::make_pair(f, detail));
			return std::make_pair(detail, p1.second && p2.second);
		}
		bool SymbolTable::insert_parameter(const FunctionIdentifier & f, const std::shared_ptr<Variable>& v)
		{
			return _function_map.at(f)->insert_parameter(v);
		}
		std::shared_ptr<Variable> SymbolTable::get_string(const string_type & str)
		{
			auto iter = std::find_if(_string_map.cbegin(), _string_map.cend(),
				[&str](const std::pair<std::shared_ptr<Variable>, string_type> &p) {return p.second == str; });

			if (iter == _string_map.cend())
			{
				std::shared_ptr<Variable> v = Variable::get_string_variable();
				return _string_map.insert(std::make_pair(v, str)).first->first;
			}
			else
			{
				return iter->first;
			}
		}
		std::shared_ptr<Variable> SymbolTable::get(const string_type & name, const FunctionIdentifier & f)
		{
			auto iter = std::find_if(_variable_map.at(f).cbegin(), _variable_map.at(f).cend(),
				[&name](const std::shared_ptr<Variable> &p) {return p->get_name() == name; });
			if (iter != _variable_map.at(f).cend())
			{
				return *iter;
			}

			auto iter2 = std::find_if(_function_map.at(f)->get_parameter_list().cbegin(),
				_function_map.at(f)->get_parameter_list().cend(),
				[&name](const std::shared_ptr<Variable> &p) {return p->get_name() == name; });
			if (iter2 != _function_map.at(f)->get_parameter_list().cend())
			{
				return *iter2;
			}
			
			iter = std::find_if(_global_set.cbegin(), _global_set.cend(),
				[&name](const std::shared_ptr<Variable> &p) {return p->get_name() == name; });
			if (iter != _global_set.cend())
			{
				return *iter;
			}
			return Variable::null;
		}
		std::shared_ptr<Variable> SymbolTable::get_literal(int value)
		{
			std::basic_ostringstream<char_type> stream;
			stream << "$Ltr" << value;
			string_type name = stream.str();

			std::shared_ptr<Variable> v = Variable::new_const_variable(name, Symbol::int_symbol, value, FunctionIdentifier::global, false);

			return *_global_set.insert(v).first;
		}
		std::shared_ptr<FunctionDetail> SymbolTable::get_function_detail(const FunctionIdentifier & function)
		{
			auto iter = _function_map.find(function);
			if (iter == _function_map.cend())
			{
				return FunctionDetail::null;
			}
			return iter->second;
		}
		std::vector<FunctionIdentifier> SymbolTable::get_function_list() const
		{
			std::vector<FunctionIdentifier> fi_list;
			for (auto & p : _function_map)
			{
				fi_list.push_back(p.first);
			}
			return fi_list;
		}
		const std::unordered_set<std::shared_ptr<Variable>, VariableHasher>& 
			SymbolTable::get_variable_set(const FunctionIdentifier & function)
		{
			return _variable_map.at(function);
		}
		void SymbolTable::set_variable_set(const FunctionIdentifier & function, 
			const std::unordered_set<std::shared_ptr<Variable>, VariableHasher>& set)
		{
			_variable_map.at(function) = set;
		}
		const std::unordered_set<std::shared_ptr<Variable>, VariableHasher>& SymbolTable::get_global_variable_set() const
		{
			return _global_set;
		}
		const std::unordered_map<std::shared_ptr<Variable>, const SymbolTable::string_type, VariableHasher>& SymbolTable::get_string_map() const
		{
			return _string_map;
		}
	}
}
