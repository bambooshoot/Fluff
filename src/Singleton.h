#pragma once

template<typename T>
class Singleton
{
public:
	static Singleton& instance() {
		if (!_pInstance)
			_pInstance = new Singleton<T>(); 

		return *_pInstance;
	}
	T& getData() {
		return data;
	}

private:
	Singleton() = delete;
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton &) = delete;
	~Singleton() {
		delete _pInstance;
	}

	static Singleton<T> * _pInstance;
	T data;
};