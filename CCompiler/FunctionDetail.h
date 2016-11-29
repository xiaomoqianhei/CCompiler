#pragma once

#include "Variable.h"

namespace Yradex
{
	namespace CCompiler
	{
		class FunctionDetail
		{
		private:
			Symbol _return_type;
			bool _is_leaf = true;
			std::vector<std::shared_ptr<Variable>> _parameter_list;
			size_t _max_used_register = 0;

		public:
			static const std::shared_ptr<FunctionDetail> null;

		public:
			FunctionDetail(Symbol type)
				: _return_type(type)
			{
			}

			bool insert_parameter(const std::shared_ptr<Variable> &v);
			std::shared_ptr<Variable> get_parameter(const typename Variable::string_type &name);
			std::vector<std::shared_ptr<Variable>>& get_parameter_list();

			Symbol get_return_type();

			void set_leaf(bool b)
			{
				_is_leaf = b;
			}

			bool is_leaf() const
			{ 
				return _is_leaf;
			}

			void set_max_used_register(size_t n)
			{
				_max_used_register = n;
			}

			size_t get_max_used_register() const
			{
				return _max_used_register;
			}

		};
	}
}
