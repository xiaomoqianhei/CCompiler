#include "Allocator.h"

namespace Yradex
{
	namespace CCompiler
	{
		MemoryPool::MemoryPool(Type type)
		{
			switch (type)
			{
			case MemoryPool::leaf:
				for (int i = 8; i < 28; ++i)
				{
					_register_pool.insert(std::make_pair(i, Variable::null));
				}
				_max_used = 7;
				break;
			case MemoryPool::default_:
				for (int i = 16; i < 24; ++i)
				{
					_register_pool.insert(std::make_pair(i, Variable::null));
				}
				break;
			default:
				break;
			}
		}
		VariableAddress MemoryPool::push_to_stack(size_t size)
		{
			_sp -= 4 * size;
			return VariableAddress(false, _sp);
		}
		int MemoryPool::get_sp()
		{
			return _sp;
		}
		VariableAddress MemoryPool::push(const std::shared_ptr<Variable> &v)
		{
			auto iter = std::find_if(_register_pool.begin(), _register_pool.end(),
				[](const std::pair<int, std::shared_ptr<Variable>> &p) {return p.second == Variable::null; }
			);

			if (iter == _register_pool.end())
			{
				return push_to_stack();
			}

			_max_used = std::max(_max_used, iter->first);
			iter->second = v;
			return VariableAddress(true, iter->first);
		}
		VariableAddress MemoryPool::push_beyond_max(const std::shared_ptr<Variable>& v)
		{
			auto iter = _register_pool.find(++_max_used);

			if (iter == _register_pool.end())
			{
				return push_to_stack();
			}

			iter->second = v;
			return VariableAddress(true, iter->first);
		}
		void MemoryPool::pop_register(const std::shared_ptr<Variable> &v)
		{
			auto iter = std::find_if(_register_pool.begin(), _register_pool.end(),
				[&v](const std::pair<int, std::shared_ptr<Variable>> &p) {return p.second == v; }
			);

			iter->second = Variable::null;
		}
		bool MemoryPool::is_in_register(const std::shared_ptr<Variable>& v)
		{
			auto iter = std::find_if(_register_pool.cbegin(), _register_pool.cend(),
				[&v](const std::pair<int, std::shared_ptr<Variable>> &p) {return p.second == v; }
			);

			return iter != _register_pool.cend();
		}
		size_t MemoryPool::get_max_used_register()
		{
			return _max_used;
		}
		void Allocator::allocate(PseudoTable & table)
		{
			auto function_detail = table.get_current_function_detail();

			MemoryPool::Type type = 
				function_detail->is_leaf() ? MemoryPool::Type::leaf : MemoryPool::Type::default_;
			MemoryPool memory_pool(type);

			// allocate for parameters
			int i = 4;
			for (auto &p : function_detail->get_parameter_list())
			{
				if (i > 7)
				{
					p->set_address(memory_pool.push_to_stack());
				}
				else
				{
					p->set_address(VariableAddress(true, i++));
				}
			}

			// allocate for temps
			for (const auto &ins : table.get_instruction_list())
			{
				if (ins.argument_1 != Variable::null)
				{
					if (ins.argument_1->get_variable_type() == VariableType::variable && ins.argument_1->is_temp())
					{
						memory_pool.pop_register(ins.argument_1);
					}
				}
				if (ins.argument_2 != Variable::null)
				{
					if (ins.argument_2->get_variable_type() == VariableType::variable && ins.argument_2->is_temp())
					{
						memory_pool.pop_register(ins.argument_2);
					}
				}
				if (ins.result != Variable::null)
				{
					if (ins.result->get_variable_type() == VariableType::variable && ins.result->is_temp())
					{
						ins.result->set_address(memory_pool.push(ins.result));
					}
				}
			}

			// allocate for others
			auto variable_set = table.get_variable_set();
			for (auto &v : variable_set)
			{
				if (v != Variable::null && !v->is_temp())
				{
					switch (v->get_variable_type())
					{
					case VariableType::variable:
						v->set_address(memory_pool.push_beyond_max(v));
						break;
					case VariableType::array:
						v->set_address(memory_pool.push_to_stack(v->get_length()));
						break;
					default:
						break;
					}
				}
			}

			table.set_max_used_register(std::min(memory_pool.get_max_used_register(), 23u));

		}
	}
}
