#include <ExrTexture.h>

#define TINYEXR_IMPLEMENTATION
#include <tinyexr.h>

PixelPicker* PixelPicker::create(const EXRImage* _image, const int pixelType)
{
	if (!_image) {
		return new PixelPicker_Default();
	}

	switch (pixelType) {
	case TINYEXR_PIXELTYPE_UINT:
		return new PixelPicker_Uint(_image);
	case TINYEXR_PIXELTYPE_HALF:
		return new PixelPicker_Half(_image);
	case TINYEXR_PIXELTYPE_FLOAT:
		return new PixelPicker_Float(_image);
	}
	return new PixelPicker_Default();
}

Pixel PixelPicker::getPixel(cfloat u, cfloat v) const
{
	UVIdx idx = getIdx(u, v);
	return getPixel(idx[0], idx[1]);
}
Pixel PixelPicker::getPixel_Interp(cfloat u, cfloat v) const
{
	InterpIdxAndWeight idxAndWeight = getIdxAndWeight(u, v);
	Pixel pix4[4];
	pix4[0] = getPixel(idxAndWeight.idx[0][0][0], idxAndWeight.idx[0][0][1]);
	pix4[1] = getPixel(idxAndWeight.idx[0][1][0], idxAndWeight.idx[0][1][1]);
	pix4[2] = getPixel(idxAndWeight.idx[1][0][0], idxAndWeight.idx[1][0][1]);
	pix4[3] = getPixel(idxAndWeight.idx[1][1][0], idxAndWeight.idx[1][1][1]);

	Pixel pix2[2];
	pix2[0] = pix4[0] * (1 - idxAndWeight.weight[0]) + pix4[1] * idxAndWeight.weight[0];
	pix2[1] = pix4[2] * (1 - idxAndWeight.weight[0]) + pix4[3] * idxAndWeight.weight[0];

	return pix2[0] * (1 - idxAndWeight.weight[1]) + pix2[1] * idxAndWeight.weight[1];
}
Pixel PixelPicker::getGradient(cfloat u, cfloat v) const
{
	UVIdx idx = getIdx(u, v);
	float value[3][3];
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			value[i][j] = getPixel(idx[0] + j, idx[1] + i).r;
		}
	}

	Pixel gradPix;
	gradPix.r = (value[0][2] - value[0][0]) + (value[1][2] - value[1][0]) + (value[2][2] - value[2][0]);
	gradPix.r *= ONE_SIXTH;

	gradPix.g = (value[2][0] - value[0][0]) + (value[2][1] - value[0][1]) + (value[2][2] - value[0][2]);
	gradPix.g *= ONE_SIXTH;

	gradPix.b = value[1][1];

	return gradPix;
}

UVIdx PixelPicker::getIdx(cfloat u, cfloat v) const
{
	UVIdx idx = { 0 };
	if (!image)
		return idx;

	float fStepU = 1.0f / image->width;
	float fStepV = 1.0f / image->height;
	float fIndexU = u / fStepU;
	float fIndexV = v / fStepV;

	idx[0] = clamp<int>(int(fIndexU), 0, image->width - 1);
	idx[1] = clamp<int>(int(fIndexV), 0, image->height - 1);
	return idx;
}

PixelPicker::InterpIdxAndWeight PixelPicker::getIdxAndWeight(cfloat u, cfloat v) const
{
	UVIdx idx = { 0 };
	InterpIdxAndWeight resultIdx4;
	if (!image)
		return resultIdx4;

	float fStepU = 1.0f / image->width;
	float fStepV = 1.0f / image->height;
	float fIndexU = u / fStepU;
	float fIndexV = v / fStepV;

	resultIdx4.weight[0] = fIndexU - floorf(fIndexU);
	resultIdx4.weight[1] = fIndexV - floorf(fIndexV);

	resultIdx4.idx[0][0][0] = clamp<int>(int(fIndexU), 0, image->width - 1);
	resultIdx4.idx[0][0][1] = clamp<int>(int(fIndexV), 0, image->height - 1);

	resultIdx4.idx[0][1][0] = resultIdx4.idx[0][0][0] + 1;
	resultIdx4.idx[0][1][1] = resultIdx4.idx[0][0][1];

	resultIdx4.idx[1][0][0] = resultIdx4.idx[0][0][0];
	resultIdx4.idx[1][0][1] = resultIdx4.idx[0][0][1] + 1;

	resultIdx4.idx[1][1][0] = resultIdx4.idx[0][0][0] + 1;
	resultIdx4.idx[1][1][1] = resultIdx4.idx[0][0][1] + 1;

	return resultIdx4;
}

Pixel  PixelPicker::getPixel(cint uIdx, cint vIdx) const
{
	Pixel pix = { 1,1,1 };
	if (!image)
		return pix;

	UVIdx idx2 = { 0 };
	idx2[0] = clamp<int>(uIdx, 0, image->width - 1);
	idx2[1] = clamp<int>(vIdx, 0, image->height - 1);
	int idx = idx2[0] + image->width * idx2[1];

	pix.b = getFloatValue(0, idx);
	pix.g = getFloatValue(1, idx);
	pix.r = getFloatValue(2, idx);

	return pix;
}

ArMultiExr::ArMultiExr(const char* fileName) :
	validLayerNum(0),
	headers(0),
	num_headers(0)
{
	const char* err;
	EXRVersion version;
	ParseEXRVersionFromFile(&version, fileName);
	ParseEXRMultipartHeaderFromFile(&headers, &num_headers, &version, fileName, &err);
	const EXRHeader** c_headers = const_cast<const EXRHeader**>(headers);
	//printf("version : \nlong_name %i \nnon_image %i\nmultipart %i\ntiled %i\nversion %i\n", version.long_name, version.non_image, version.multipart, version.tiled, version.version);
	//if (!version.multipart) {
	//	printf("ERROR:EXR image file %s does not have multi layers.\n", fileName);
	//	return;
	//}

	static const char* layerNameList[] = {
		"density",
		"gradient",
		"split",
		"normal",
		"length",
		"displace",
		"width",
		"fuzzy",
		"clump"
	};

	imageId = { -1 };

	for (int i = 0; i < num_headers; ++i)
		InitEXRImage(&imagePool.at(i));

	LoadEXRMultipartImageFromFile(&imagePool.at(0), c_headers, num_headers, fileName, &err);

	for (int i = 0; i < num_headers; ++i) {
		bool breakFlag = true;
		for (int j = 0; j < ARMULTIEXR_LAYER_NUM && breakFlag; ++j) {
			for (int attrId = 0; attrId < c_headers[i]->num_custom_attributes && breakFlag; ++attrId) {
				if (strcmp(c_headers[i]->custom_attributes[attrId].name, "name") == 0) {
					if (memcmp(c_headers[i]->custom_attributes[attrId].value, layerNameList[j], c_headers[i]->custom_attributes[attrId].size) == 0) {
						imageId[j] = i;
						breakFlag = false;
					}
				}
			}
		}
	}

	//const char* pixelType[3] = { "uint","half","float" };
	//for (int i = 0; i < num_headers; ++i) {
	//	for (int j = 0; j < c_headers[i]->num_channels; ++j)
	//		printf("channel: %i pixel type: %s\n", j, pixelType[c_headers[i]->pixel_types[j]]);

	//	for (int attrId = 0; attrId < c_headers[i]->num_custom_attributes; ++attrId) {
	//		if (strcmp(c_headers[i]->custom_attributes[attrId].type, "string") == 0)
	//			printf("name: %s value: %s\n", c_headers[i]->custom_attributes[attrId].name, (char*)c_headers[i]->custom_attributes[attrId].value);
	//		else if (strcmp(c_headers[i]->custom_attributes[attrId].type, "int") == 0)
	//			printf("name: %s value: %i\n", c_headers[i]->custom_attributes[attrId].name, *(int*)c_headers[i]->custom_attributes[attrId].value);
	//	}
	//}
}

ArMultiExr::~ArMultiExr() {
	for (int i = 0; i < num_headers; ++i) {
		FreeEXRImage(&imagePool[i]);
		FreeEXRHeader(headers[i]);
		free(headers[i]);
	}
	free(headers);
}

const EXRImage* ArMultiExr::getImage(const int layerId) const
{
	int layId = imageId[layerId];
	if (layId == -1)
		return 0;

	return &imagePool[layId];
}
int ArMultiExr::getPixelType(const int layerId) const
{
	int layId = imageId[layerId];
	if (layId == -1)
		return -1;

	return headers[layId]->pixel_types[0];
}

float PixelPicker_Uint::getFloatValue(const int channelId, const int idx) const
{
	tinyexr::FP32 fp;
	fp.u = reinterpret_cast<uint**>(image->images)[channelId][idx];
	return fp.f;
}

float PixelPicker_Half::getFloatValue(const int channelId, const int idx) const
{
	tinyexr::FP16 fp;
	fp.u = reinterpret_cast<unsigned short**>(image->images)[channelId][idx];
	return tinyexr::half_to_float(fp).f;
}

float PixelPicker_Float::getFloatValue(const int channelId, const int idx) const
{
	return reinterpret_cast<float**>(image->images)[channelId][idx];
}