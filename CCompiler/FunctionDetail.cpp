#include "FunctionDetail.h"


#include <algorithm>

namespace Yradex
{
	namespace CCompiler
	{
		const std::shared_ptr<FunctionDetail> FunctionDetail::null = std::make_shared<FunctionDetail>(Symbol::eof);

		const FunctionIdentifier FunctionIdentifier::global("");
		bool FunctionDetail::insert_parameter(const std::shared_ptr<Variable> &v)
		{
			if (_parameter_list.cend() != std::find(_parameter_list.cbegin(), _parameter_list.cend(), v))
			{
				return false;
			}
			_parameter_list.push_back(v);
			return true;
		}
		std::shared_ptr<Variable> FunctionDetail::get_parameter(const typename Variable::string_type & name)
		{
			auto iter = std::find_if(_parameter_list.cbegin(), _parameter_list.cend(),
				[&name](const std::shared_ptr<Variable> &v) {return v->get_name() == name; });
			if (_parameter_list.cend() != iter)
			{
				return *iter;
			}
			return Variable::null();
		}
		std::vector<std::shared_ptr<Variable>>& FunctionDetail::get_parameter_list()
		{
			return _parameter_list;
		}
		Symbol FunctionDetail::get_return_type()
		{
			return _return_type;
		}
	}
}