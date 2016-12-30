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
					_register_pool.insert(std::make_pair(i, Variable::null()));
				}
				_max_used = 7;
				break;
			case MemoryPool::default_:
				for (int i = 16; i < 24; ++i)
				{
					_register_pool.insert(std::make_pair(i, Variable::null()));
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
				[](const std::pair<int, std::shared_ptr<Variable>> &p) {return p.second == Variable::null(); }
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

			iter->second = Variable::null();
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
		void MemoryPool::clear_temp_variables(const std::map<std::shared_ptr<Variable>, size_t> &variable_counter)
		{
			for (auto &i : _register_pool)
			{
				if (i.second->is_temp() && i.second->get_ref() == variable_counter.at(i.second))
				{
					i.second = Variable::null();
				}
			}
		}
		void MemoryPool::assign_register(const VariableAddress &addr, const std::shared_ptr<Variable> &v)
		{
			_register_pool[addr.position] = v;
			v->set_address(addr);
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
			for (const auto &ins : table.get_current_instruction_list())
			{
				if (ins.get_left_argument() != Variable::null())
				{
					if (ins.get_left_argument()->get_variable_type() == Variable::Type::variable 
						&& ins.get_left_argument()->is_temp())
					{
						_variable_counter[ins.get_left_argument()]++;
					}
				}
				if (ins.get_right_argument() != Variable::null())
				{
					if (ins.get_right_argument()->get_variable_type() == Variable::Type::variable 
						&& ins.get_right_argument()->is_temp())
					{
						_variable_counter[ins.get_right_argument()]++;
					}
				}

				auto result = ins.get_result();
				VariableAddress resultAddress;
				if (result != Variable::null())
				{
					if (result->get_variable_type() == Variable::Type::variable && result->is_temp())
					{
						resultAddress = memory_pool.push(result);
						result->set_address(resultAddress);
						_variable_counter[result]++;
					}
				}

				// clear temps when end of basic block
				if (PseudoOperatorUtility::is_end_of_basic_block(ins.get_operator()))
				{
					memory_pool.clear_temp_variables(_variable_counter);
					if (ins.get_operator() == PseudoOperator::call && resultAddress.in_register)
					{
						memory_pool.assign_register(resultAddress, ins.get_result());
					}
				}
			}

			// allocate for others
			auto variable_set = table.get_variable_set();
			for (auto &v : variable_set)
			{
				if (v != Variable::null() && !v->is_temp())
				{
					switch (v->get_variable_type())
					{
					case Variable::Type::variable:
						v->set_address(memory_pool.push_beyond_max(v));
						break;
					case Variable::Type::array:
						v->set_address(memory_pool.push_to_stack(v->get_length()));
						break;
					default:
						break;
					}
				}
			}

			table.set_max_used_register(std::min(memory_pool.get_max_used_register(), 23u));

			_variable_counter.clear();
		}
	}
}
