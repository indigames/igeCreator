
#pragma once

#if defined(__ANDROID__) || TARGET_OS_IPHONE
#define __EFFEKSEER_RENDERER_GLES3__
#endif

#ifdef _WIN32
#define IGE_EXPORT __declspec(dllexport)
#else
#define IGE_EXPORT
#endif

#include <Effekseer.h>
#include "EffekseerRendererGL/EffekseerRendererGL.h"
#include <unordered_map>

using namespace Effekseer;
using namespace EffekseerRenderer;

enum class TextureLoaderType
{
	LOAD = 0,
	UNLOAD,
};

struct TextureLoaderCallback
{
	const char* name;
	TextureLoaderType type;

	TextureLoaderCallback(const char* _name, TextureLoaderType _type)
		: name(_name), type(_type)
	{
	}
};

struct EffectData
{
	Handle handle;
	Effect* effect;
	bool isLooping;
	bool isShown;
	Vector3D position;	
	Vector3D rotation;
	Vector3D scale;
	Vector3D target_position;
	float dynamic_inputs[4];	
};

typedef TextureData* (*TextureLoaderFunc)(TextureLoaderCallback loader);

class IGE_EXPORT pyxieEffekseer
{
public:
	pyxieEffekseer();
	~pyxieEffekseer();

	void init(bool culling_enable, int thread_number);
	void release();
	void update(float dt);
	void render(bool isClear, bool culling_override);
	void play(Handle handle);
	uint32_t play(const char* name, bool loop, const Vector3D& position, const Vector3D& rotation, const Vector3D& scale, const Vector3D& target_position, bool play);
	void stop(int handle);
	void stopAll();
	void clearScreen();
    void setup(bool isClear);
	int32_t getDrawcallCount();
	int32_t getDrawVertexCount();
	int32_t getUpdateTime();
	int32_t getDrawTime();

	float GetFramerate() const
	{
		return desiredFramerate;
	}
	void SetFramerate(float val)
	{
		desiredFramerate = val;
	}

	int32_t getInstanceCount(Handle handle);
	int32_t getTotalInstanceCount();

	void SetTargetLocation(Handle handle, float x, float y, float z);
	const Vector3D& GetLocation(Handle handle);
	void SetLocation(Handle handle, float x, float y, float z);
	void SetLocation(Handle handle, const Vector3D& location);
	void SetRotation(Handle handle, float x, float y, float z);
	void SetScale(Handle handle, float x, float y, float z);
	void SetAllColor(Handle handle, Color color);
	void SetSpeed(Handle handle, float speed);
	float GetSpeed(Handle handle);
	bool IsPlaying(Handle handle);
	bool GetPause(Handle handle);
	void SetPause(Handle handle, bool paused);
	bool GetShown(Handle handle);
	void SetShown(Handle handle, bool shown, bool reset);
	bool GetLoop(Handle handle);
	void SetLoop(Handle handle, bool loop);
	void setDynamicInput(uint32_t handle, float* buff);
	float* getDynamicInput(uint32_t handle);

	void SetRenderProjectionMatrix(float* projection);
	void SetRenderViewMatrix(float* view);
private:
	std::string PlatformOverride(const char* name);

private:
	Matrix44 projection_mat;
	Matrix44 view_mat;

	Manager* manager;
	Renderer* renderer;
	Matrix44 camera;
	float desiredFramerate;
	bool culling_enabled;

	uint32_t nextHandle;

	std::unordered_map<int32_t, EffectData> effect_map;	
public:
	static TextureLoaderFunc textureLoaderFunc;
	static void setTextureLoader(TextureLoaderFunc loader);
};
