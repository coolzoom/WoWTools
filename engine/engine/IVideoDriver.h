#pragma once

#include "base.h"
#include "rect.h"
#include "vector2d.h"
#include "matrix4.h"
#include "SMaterial.h"
#include "IRenderTarget.h"
#include "SColor.h"
#include "RenderStruct.h"
#include "IVertexIndexBuffer.h"
#include "S3DVertex.h"
#include <string>
#include <list>

class ITexture;
class ITextureWriter;
class CCamera;

struct SDriverSetting
{
	SDriverSetting()
	{
		vsync = true;
		aaMode = E_AA_MSAA_1;
		antialias = 1;
	}

	E_AA_MODE aaMode;
	bool vsync;
	uint8_t antialias;
};

struct SDisplayMode
{
	SDisplayMode()
	{
		width = 0;
		height = 0;
		refreshrate = 60;
	}

	uint16_t width;
	uint16_t height;
	uint32_t refreshrate;

	bool operator==(const SDisplayMode& mode) const
	{
		return width == mode.width &&
			height == mode.height &&
			refreshrate == mode.refreshrate;
	}
};

class CAdapterInfo
{
public:
	CAdapterInfo() : vendorID(0)
	{
	}

public:
	uint32_t		vendorID;
	std::string		description;
	std::string		name;
	std::string		vendorName;
};

enum EConstant : uint32_t
{
	MAX_QUADS = 128,
	MAX_VERTEXCOUNT = 1024 * 3,			//dynamic vertex count


	MAX_2DLINE_BATCH_COUNT = 512,
	MAX_3DLINE_BATCH_COUNT = 2048,
	MAX_IMAGE_BATCH_COUNT = MAX_QUADS,
	MAX_VERTEX_COUNT = MAX_VERTEXCOUNT,
	MAX_VERTEX_2D_COUNT = 64,
	MAX_INDEX_2D_COUNT = 128,
};

class IVideoDriver
{
private:
	DISALLOW_COPY_AND_ASSIGN(IVideoDriver);

public:
	explicit IVideoDriver(E_DRIVER_TYPE type)
		: DriverType(type)
	{
		CurrentRenderTarget = nullptr;

		IsSupportDepthTexture = false;
		IsMultiSampleEnabled = false;
		IsSupportA8L8 = false;

		PrimitivesDrawn = 0;
		DrawCall = 0;
	}

	virtual ~IVideoDriver() {}

public:
	E_DRIVER_TYPE getDriverType() const { return DriverType; }

	const recti& getViewPort() const { return Viewport; }
	const dimension2d& getDisplayMode() const { return ScreenSize; }
	const SDriverSetting& getDriverSetting() const { return DriverSetting; }

	void setGlobalMaterial(const SGlobalMaterial& globalMaterial) { GlobalMaterial = &globalMaterial; }
	const SGlobalMaterial* getGlobalMaterial() const { return GlobalMaterial; }

	IRenderTarget* getFrameBufferRT() const { return FrameBufferRT.get(); }
	IIndexBuffer*  getStaticIndexBufferQuadList() const { return StaticIndexBufferQuadList.get(); }
	IVertexBuffer* getDynamicVertexBuffer() const { return DynamicVertexBuffer.get(); }

	bool isFXAAEnabled() const { return DriverSetting.aaMode >= E_AA_FXAA && DriverSetting.aaMode <= E_AA_FXAA; }
	bool isMultiSampleEnabled() const { return IsMultiSampleEnabled; }
	bool isSupportDepthTexture() const { return IsSupportDepthTexture; }
	bool isSupportA8L8() const { return IsSupportA8L8; }

	void setShaderVariable(const char* name, const float* src, uint32_t size);
	void setShaderVariable(const char* name, const matrix4& mat) { setShaderVariable(name, mat.M, 16); }
	void setShaderVariable(const char* name, const vector4df& vec) { setShaderVariable(name, &vec.x, 4); }
	void clearShaderVariables() { ShaderVariableMap.clear(); }

public:
	virtual bool beginScene() = 0;
	virtual bool endScene() = 0;
	virtual bool clear(bool renderTarget, bool zBuffer, bool stencil, SColor color) = 0;

	virtual bool checkValid() = 0;
	virtual bool setRenderTarget(const IRenderTarget* texture, bool bindDepth = true) = 0;

	virtual void setViewPort(const recti& area) = 0;
	virtual void setDisplayMode(const dimension2d& size) = 0;
	virtual bool setDriverSetting(const SDriverSetting& setting) = 0;

	virtual void draw(const SMaterial* material, const IVertexBuffer* vbuffer, const IIndexBuffer* ibuffer,
		E_PRIMITIVE_TYPE primType,
		uint32_t primCount,
		const SDrawParam& drawParam) = 0;

public:
	virtual IVertexBuffer* createVertexBuffer(E_MESHBUFFER_MAPPING mapping) = 0;
	virtual IIndexBuffer* createIndexBuffer(E_MESHBUFFER_MAPPING mapping) = 0;

	//
	virtual ITextureWriter* createTextureWriter(ITexture* texture) = 0;
	virtual bool removeTextureWriter(ITexture* texture) = 0;

	//
	virtual ITexture* createEmptyTexture(const dimension2d& size, ECOLOR_FORMAT format) = 0;
	virtual ITexture* getTextureWhite() const = 0;
	
public:
	CAdapterInfo	AdapterInfo;
	uint32_t		PrimitivesDrawn;
	uint32_t		DrawCall;

	//global variable 
	std::map<std::string, std::vector<float>>	ShaderVariableMap;

protected:
	const E_DRIVER_TYPE	DriverType;

	const SGlobalMaterial*	GlobalMaterial;

	const IRenderTarget*		CurrentRenderTarget;		//��ǰrender target, ��Ϊnullptr���ʾframe buffer
	std::unique_ptr<IRenderTarget>		FrameBufferRT;

	std::unique_ptr<IVertexBuffer>			StaticVertexBufferScreenQuad;
	std::unique_ptr<IVertexBuffer>			StaticVertexBufferScreenQuadFlip;
	std::unique_ptr<IIndexBuffer>			StaticIndexBufferQuadList;
	std::unique_ptr<IIndexBuffer>			StaticIndexBufferTriangleList;

	std::unique_ptr<IVertexBuffer>			DynamicVertexBuffer;

	recti			Viewport;
	dimension2d		ScreenSize;
	SDriverSetting	DriverSetting;

	bool	IsMultiSampleEnabled;
	bool	IsSupportDepthTexture;
	bool	IsSupportA8L8;
};

inline void IVideoDriver::setShaderVariable(const char* name, const float* src, uint32_t size)
{
	auto itr = ShaderVariableMap.find(name);
	if (itr == ShaderVariableMap.end())
	{
		std::vector<float> buffer;
		buffer.resize(size);
		memcpy(buffer.data(), src, sizeof(float) * size);
		ShaderVariableMap[name] = buffer;
	}
	else
	{
		std::vector<float>& buffer = itr->second;
		ASSERT(buffer.size() >= size);
		memcpy(buffer.data(), src, sizeof(float) * size);
	}
}