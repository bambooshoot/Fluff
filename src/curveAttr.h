#pragma once

#include <fluffdef.h>

template<typename T>
struct XAndValue {
	float x;
	T     value;
};

template<typename T>
using Range = std::array<const XAndValue<T>*, 2>;

template<typename T>
class CurveAttrSampler_Linear
{
public:
	T operator()(cfloat x, Range<T>& range) const {
		if (range[1]->x == range[0]->x)
			return range[0]->value;

		float ratio = (x - range[0]->x) / (range[1]->x - range[0]->x);
		return range[0]->value * (1 - ratio) + range[1]->value * ratio;
	}
};

template<typename T,typename SAMPLE>
class CurveAttr
{
public:
	T value(cfloat x) const {
		float xx = xInRange(x);
		if (xx < 0) return 0;

		Range<T> rg = range(xx);
		return sampler(xx, rg);
	}
	void push(cfloat x, const T value) {
		xValueList.push_back(XAndValue<T>({ x,value }));
		std::sort(xValueList.begin(), xValueList.end(), [](const XAndValue<T> & a, const XAndValue<T> & b) {
			return a.x < b.x;
			});
	}

private:
	Range<T> range(cfloat x) const {
		auto iter = std::find_if(xValueList.begin(), xValueList.end(), 
			[&](const XAndValue<T> & a) { return x < a.x; });

		Range<T> rg;
		if (iter == xValueList.end()) {
			--iter;
			rg[1] = rg[0] = &*iter;
		}
		else if (iter == xValueList.begin()) {
			rg[1] = rg[0] = &*iter;
		}
		else {
			rg[1] = &*iter;
			--iter;
			rg[0] = &*iter;
		}
		return rg;
	}
	float xInRange(cfloat x) const {
		if (xValueList.empty())
			return -1;

		return clamp<float>(x, xValueList.front().x, xValueList.back().x);
	}
	
	SAMPLE sampler;
	std::vector<XAndValue<T>> xValueList;
};

using CurveAttrF = CurveAttr<float, CurveAttrSampler_Linear<float>>;