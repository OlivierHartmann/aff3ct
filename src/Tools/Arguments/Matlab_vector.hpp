#ifndef CLI_MATLAB_VECTOR_HPP
#define CLI_MATLAB_VECTOR_HPP

#include <iostream>
#include <sstream>
#include <vector>

namespace CLI
{
	template <typename T>
	class Matlab_vector
	{
	private:
		using Container = std::vector<T>;
		Container vec_;

		using pointer                = typename std::vector<T>::pointer;
		using const_pointer          = typename std::vector<T>::const_pointer;
		using reference              = typename std::vector<T>::reference;
		using const_reference        = typename std::vector<T>::const_reference;
		using iterator               = typename std::vector<T>::iterator;
		using const_iterator         = typename std::vector<T>::const_iterator;
		using reverse_iterator       = typename std::vector<T>::reverse_iterator;
		using const_reverse_iterator = typename std::vector<T>::const_reverse_iterator;
		using size_type              = typename std::vector<T>::size_type;

	public:
		Matlab_vector() = default;
		Matlab_vector(const std::string& desc) { (*this) = desc; }
		Matlab_vector(const std::vector<std::string>& desc) { (*this) = desc; }
		Matlab_vector(const Container& v) : vec_(v) {}
		Matlab_vector(Container&& v) : vec_(std::move(v)) {}


		Matlab_vector<T>& operator=(const std::string& description);
		Matlab_vector<T>& operator=(const std::vector<std::string>& description);

		Matlab_vector<T>& operator=(const Container& v)
		{
			vec_ = v;
			return *this;
		}

		Matlab_vector<T>& operator=(Container&& v)
		{
			vec_ = std::move(v);
			return *this;
		}


		iterator erase(iterator                              pos) { return vec_.erase(pos        ); }
		iterator erase(const_iterator                        pos) { return vec_.erase(pos        ); }
		iterator erase(iterator first,             iterator last) { return vec_.erase(first, last); }
		iterator erase(const_iterator first, const_iterator last) { return vec_.erase(first, last); }

		size_t           size()       { return vec_. size(); }
		size_t           size() const { return vec_. size(); }
		void            clear()       { return vec_.clear(); }
		void            clear() const { return vec_.clear(); }
		bool            empty()       { return vec_.empty(); }
		bool            empty() const { return vec_.empty(); }
		reference       front()       { return vec_.front(); }
		const_reference front() const { return vec_.front(); }
		reference       back ()       { return vec_. back(); }
		const_reference back () const { return vec_. back(); }

		iterator                begin()       { return vec_. begin(); }
		const_iterator          begin() const { return vec_. begin(); }
		const_iterator         cbegin()       { return vec_.cbegin(); }
		const_iterator         cbegin() const { return vec_.cbegin(); }
		iterator                  end()       { return vec_.   end(); }
		const_iterator            end() const { return vec_.   end(); }
		const_iterator           cend()       { return vec_.  cend(); }
		const_iterator           cend() const { return vec_.  cend(); }
		reverse_iterator       rbegin()       { return vec_.rbegin(); }
		const_reverse_iterator rbegin() const { return vec_.rbegin(); }
		reverse_iterator         rend()       { return vec_.  rend(); }
		const_reverse_iterator   rend() const { return vec_.  rend(); }

		reference       operator[](size_type pos)       { return vec_.operator[](pos); }
		const_reference operator[](size_type pos) const { return vec_.operator[](pos); }
	};

	template <typename T>
	std::istringstream &operator>>(std::istringstream &in, Matlab_vector<T> &val)
	{
		val = in.str();
		in.str("");
		return in;
	}

}

#endif // CLI_MATLAB_VECTOR_HPP