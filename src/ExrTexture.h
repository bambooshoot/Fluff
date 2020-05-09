#pragma once

#include <FluffDef.h>
#include <tinyexr.h>

#define ARMULTIEXR_LAYER_NUM		9

struct Pixel {
	float r, g, b;

	Pixel operator + (const Pixel a) {
		return {
			r + a.r,
			g + a.g,
			b + a.b
		};
	}
	Pixel operator * (cfloat a) {
		return {
			r * a,
			g * a,
			b * a
		};
	}
};

class PixelPicker
{
public:
	PixelPicker() : image(0) {};
	PixelPicker(const EXRImage* _image) : image(_image) {}
	virtual ~PixelPicker() {}
	Pixel getPixel(cfloat u, cfloat v) const;
	Pixel getPixel_Interp(cfloat u, cfloat v) const;
	Pixel getGradient(cfloat u, cfloat v) const;

	static PixelPicker* create(const EXRImage* _image, const int pixelType);

protected:
	struct InterpIdxAndWeight {
		UVIdx idx[2][2];
		float weight[2];
		InterpIdxAndWeight() {
			memset(idx, 0, sizeof(UVIdx) * 4);
			weight[0] = 0.0f;
			weight[1] = 0.0f;
		}
	};
	UVIdx getIdx(cfloat u, cfloat v) const;
	InterpIdxAndWeight getIdxAndWeight(cfloat u, cfloat v) const;
	Pixel getPixel(cint uIdx, cint vIdx) const;
	virtual float getFloatValue(const int channelId, const int idx) const = 0;

	const EXRImage* image;
};

class PixelPicker_Uint : public PixelPicker {
public:
	PixelPicker_Uint(const EXRImage* _image) : PixelPicker(_image) {}
	~PixelPicker_Uint() override {}

protected:
	float getFloatValue(const int channelId, const int idx) const override;
};

class PixelPicker_Half : public PixelPicker {
public:
	PixelPicker_Half(const EXRImage* _image) : PixelPicker(_image) {}
	~PixelPicker_Half() override {}

protected:
	float getFloatValue(const int channelId, const int idx) const;
};

class PixelPicker_Float : public PixelPicker {
public:
	PixelPicker_Float(const EXRImage* _image) : PixelPicker(_image) {}
	~PixelPicker_Float() override {}

protected:
	float getFloatValue(const int channelId, const int idx) const override;
};

class PixelPicker_Default : public PixelPicker {
public:
	PixelPicker_Default() : PixelPicker(){}
	~PixelPicker_Default() override {}

protected:
	float getFloatValue(const int channelId, const int idx) const override
	{
		return 1;
	}
};

class ArMultiExr
{
public:
	ArMultiExr(const char* fileName);
	~ArMultiExr();
	bool isValid() const {
		return num_headers;
	}
	const EXRImage* getImage(const int layerId) const;
	int getPixelType(const int layerId) const;

private:
	int validLayerNum;
	int num_headers;
	EXRHeader** headers;
	std::array<EXRImage, ARMULTIEXR_LAYER_NUM> imagePool;
	std::array<int, ARMULTIEXR_LAYER_NUM> imageId;
};