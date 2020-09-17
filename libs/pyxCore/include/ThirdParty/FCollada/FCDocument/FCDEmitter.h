/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef _FCD_EMITTER_H_
#define _FCD_EMITTER_H_

/**
	@file FCDEmitter.h
	This file defines the entity class for emitters in FCollada.
*/

/**	
	@defgroup FCDEmitters COLLADA Particle Emitter Classes
*/

class FCDocument;
class FCDExtra;
class FCDEmitterParticle;

#include "FCDAnimationKey.h"

#define ATTRIBUTE(type,name)\
	public:\
		inline type Get##name() const { return name; }\
		inline void Set##name( const type value ) { name = value; }\
	private:\
		type name

#define ARRAY_ATTRIBUTE(type,name,num)\
	public:\
		inline const type* Get##name() const { return name; }\
		inline void Set##name( const type* value )\
		{\
			for ( unsigned int i = 0; i < num; ++i )\
			{\
				name[i] = value[i];\
			}\
		}\
	private:\
		type name[num]



class FCDColorAnimationKey
{
public:
	//operator < 

public:
	/** The key input. Typically, this will be a time value, in seconds.
		For driven curves, the dimension of this value will depend on the driver. */
	float input;

	/** The key output. */
	float output[3];

	/** The key interpolation type.
		@see FUDaeInterpolation::Interpolation */
	uint32 interpolation;
};

typedef fm::pvector<FCDAnimationKey> FCDAnimationKeyList; /**< A dynamically-sized array of curve keys. */
typedef fm::pvector<FCDColorAnimationKey> FCDColorAnimationKeyList;

#ifndef _FCD_ENTITY_H_
#include "FCDocument/FCDEntity.h"
#endif // _FCD_ENTITY_H_

/**
	A COLLADA generic emitter.

	This class does not belong to the COLLADA COMMON profile.
	It is used to define both particle and sound emitters.

	@ingroup FCDEmitters
*/

class FCOLLADA_EXPORT FCDEmitter : public FCDEntity
{
public:
	// add
	/** The types of emitter supported. */
	enum EmitterType
	{
		DIRECTIONAL,
		OMNI,
		SURFACE
	};

private:
	DeclareObjectType(FCDEntity);

	//// Contains only one of the following, in order of importance.
	DeclareParameterRef(FCDEmitterParticle, particle, FC("Particle"));
	//DeclareParameterRef(FCDGeometryMesh, mesh, FC("Mesh"));
	//DeclareParameterRef(FCDGeometrySpline, spline, FC("Spline"));

	// add
	DeclareParameter(fstring, FUParameterQualifiers::SIMPLE, url, FC("URL"));

	// Basic Emitter Attributes
	DeclareParameter(uint32, FUParameterQualifiers::SIMPLE, emitterType, FC("Emitter Type"));
	//DeclareParameter(float, FUParameterQualifiers::SIMPLE, rate, FC("Rate"));
	FCDAnimationKeyList rates;

	// Distance/Direction Attributes
	DeclareParameter(float, FUParameterQualifiers::SIMPLE, minDistance, FC("Min Distance"));
	DeclareParameter(float, FUParameterQualifiers::SIMPLE, maxDistance, FC("Max Distance"));
	DeclareParameter(float, FUParameterQualifiers::SIMPLE, directionX, FC("Direction X"));
	DeclareParameter(float, FUParameterQualifiers::SIMPLE, directionY, FC("Direction Y"));
	DeclareParameter(float, FUParameterQualifiers::SIMPLE, directionZ, FC("Direction Z"));
	DeclareParameter(float, FUParameterQualifiers::SIMPLE, spread, FC("Spread"));

	// Basic Emission Speed Attributes
	DeclareParameter(float, FUParameterQualifiers::SIMPLE, speed, FC("Speed"));
	DeclareParameter(float, FUParameterQualifiers::SIMPLE, speedRandom, FC("Speed Random"));

	// Emission Attributes
	DeclareParameter(int32, FUParameterQualifiers::SIMPLE, maxCount, FC("Max Count"));

	// Lifespan Attributes
	DeclareParameter(uint32, FUParameterQualifiers::SIMPLE, lifespanMode, FC("Lifespan Mode"));
	DeclareParameter(float, FUParameterQualifiers::SIMPLE, lifespan, FC("Lifespan"));
	DeclareParameter(float, FUParameterQualifiers::SIMPLE, lifespanRandom, FC("Lifespan Random"));

	// Render Attributes
	DeclareParameter(uint32, FUParameterQualifiers::SIMPLE, particleRenderType, FC("Particle Render Type"));
	DeclareParameter(float, FUParameterQualifiers::SIMPLE, spriteScaleX, FC("Sprite Scale X"));
	DeclareParameter(float, FUParameterQualifiers::SIMPLE, spriteScaleY, FC("Sprite Scale Y"));

	// Extra Attributes
	DeclareParameter(float, FUParameterQualifiers::SIMPLE, minScale, FC("Min Scale"));
	DeclareParameter(float, FUParameterQualifiers::SIMPLE, maxScale, FC("Max Scale"));
	//DeclareParameter(float, FUParameterQualifiers::SIMPLE, beginScale, FC("Begin Scale"));
	//DeclareParameter(float, FUParameterQualifiers::SIMPLE, endScale, FC("End Scale"));
	//DeclareParameter(float, FUParameterQualifiers::SIMPLE, beginAlpha, FC("Begin Alpha"));
	//DeclareParameter(float, FUParameterQualifiers::SIMPLE, endAlpha, FC("End Alpha"));
	DeclareParameter(float, FUParameterQualifiers::SIMPLE, gravityX, FC("Gravity X"));
	DeclareParameter(float, FUParameterQualifiers::SIMPLE, gravityY, FC("Gravity Y"));
	DeclareParameter(float, FUParameterQualifiers::SIMPLE, gravityZ, FC("Gravity Z"));
	DeclareParameter(float, FUParameterQualifiers::SIMPLE, spriteTwistSpeed, FC("Sprite Twist Speed"));
	DeclareParameter(float, FUParameterQualifiers::SIMPLE, attenuation, FC("Attenuation"));
	DeclareParameter(bool, FUParameterQualifiers::SIMPLE, depthSort, FC("Depth Sort"));

	// 2010/02/04
	DeclareParameter(bool, FUParameterQualifiers::SIMPLE, loop, FC("Loop"));
	DeclareParameter(float, FUParameterQualifiers::SIMPLE, numFrame, FC("Num Frame"));

	// 2010/02/09
	//DeclareParameter(float, FUParameterQualifiers::SIMPLE, lifeColor, FC("Life Color"));
	//FUParameterColor3 lifeColor;
	//DeclareParameter(float, FUParameterQualifiers::SIMPLE, lifeTransparency, FC("Life Transparency"));

	// 2010/02/15
	FCDColorAnimationKeyList	color;
	FCDColorAnimationKeyList	transparency;
	//FCDColorAnimationKeyList	lifeColor;
	//FCDColorAnimationKeyList	lifeTransparency;
	FCDColorAnimationKeyList	rgbRamp;
	FCDColorAnimationKeyList	opacityRamp;

	// 2010/02/18
	//DeclareParameter(bool, FUParameterQualifiers::SIMPLE, billboard, FC("Billboard"));

	FCDColorAnimationKeyList	lifeSpriteScaleXPPRamp;
	FCDColorAnimationKeyList	lifeSpriteScaleYPPRamp;

	// 2010/02/19
	//float rotOffset[3];
	//float initRandomRot[3];
	//float rotSpeed[3];
	//float randomRotSpeed[3];
	//float pivot[2];
	//float zBillboardOffset;
	ATTRIBUTE(bool,Billboard);
	ARRAY_ATTRIBUTE(float,RotOffset,3);
	ARRAY_ATTRIBUTE(float,InitRandomRot,3);
	ARRAY_ATTRIBUTE(float,RotSpeed,3);
	ARRAY_ATTRIBUTE(float,RandomRotSpeed,3);
	ARRAY_ATTRIBUTE(float,Pivot,2);
	//ATTRIBUTE(float,ZBillboardOffset);
	ATTRIBUTE(float,PolygonOffsetFactor);
	ATTRIBUTE(float,PolygonOffsetUnits);

	//
	//DeclareParameterContainer(FCDLifeColors, lifeColors, FC("Life Colors"));
	//DeclareParameterContainer(FCDLifeColors, lifeColors, FC("Life Colors"));

public:
	/** Constructor.  Do not use directly, emitters should be created using
		the AddEntity function on the FCDEmitterLibrary
		@param document The COLLADA document that owns the emitter. */
	FCDEmitter(FCDocument* document);

	/** Destructor. */
	~FCDEmitter();

	/** Retrieves the entity class type.
		@return The entity class type: EMITTER */
	virtual Type GetType() const { return EMITTER; }


	/** */
	bool IsParticle() const { return particle != NULL; }

	/** */
	FCDEmitterParticle* GetParticle() { return particle; }
	const FCDEmitterParticle* GetParticle() const { return particle; } /**< See above. */

	/** */
	FCDEmitterParticle* CreateParticle();


	// add
	inline fstring GetURL() const { return url; }
	inline void SetURL(fstring _url) { url = fstring(_url); SetDirtyFlag(); }

	// Basic Emitter Attributes
	inline EmitterType GetEmitterType() const { return (EmitterType) *emitterType; }
	inline void SetEmitterType(EmitterType type) { emitterType = type; SetDirtyFlag(); }

//	inline float GetRate() const { return rate; }
//	inline void SetRate(float _rate) { rate = _rate; SetDirtyFlag(); }

	// Distance/Direction Attributes
	inline float GetMinDistance() const { return minDistance; }
	inline void SetMinDistance(float _minDistance) { minDistance = _minDistance; SetDirtyFlag(); }

	inline float GetMaxDistance() const { return maxDistance; }
	inline void SetMaxDistance(float _maxDistance) { maxDistance = _maxDistance; SetDirtyFlag(); }

	inline float GetDirectionX() const { return directionX; }
	inline void SetDirectionX(float _directionX) { directionX = _directionX; SetDirtyFlag(); }

	inline float GetDirectionY() const { return directionY; }
	inline void SetDirectionY(float _directionY) { directionY = _directionY; SetDirtyFlag(); }

	inline float GetDirectionZ() const { return directionZ; }
	inline void SetDirectionZ(float _directionZ) { directionZ = _directionZ; SetDirtyFlag(); }

	inline float GetSpread() const { return spread; }
	inline void SetSpread(float _spread) { spread = _spread; SetDirtyFlag(); }

	// Basic Emission Speed Attributes
	inline float GetSpeed() const { return speed; }
	inline void SetSpeed(float _speed) { speed = _speed; SetDirtyFlag(); }

	inline float GetSpeedRandom() const { return speedRandom; }
	inline void SetSpeedRandom(float _speedRandom) { speedRandom = _speedRandom; SetDirtyFlag(); }

	// Emission Attributes
	inline int32 GetMaxCount() const { return maxCount; }
	inline void SetMaxCount(int32 _maxCount) { maxCount = _maxCount; SetDirtyFlag(); }

	// Lifespan Attributes
	inline uint32 GetLifespanMode() const { return lifespanMode; }
	inline void SetLifespanMode(uint32 _lifespanMode) { lifespanMode = _lifespanMode; SetDirtyFlag(); }

	inline float GetLifespan() const { return lifespan; }
	inline void SetLifespan(float _lifespan) { lifespan = _lifespan; SetDirtyFlag(); }

	inline float GetLifespanRandom() const { return lifespanRandom; }
	inline void SetLifespanRandom(float _lifespanRandom) { lifespanRandom = _lifespanRandom; SetDirtyFlag(); }

	// Render Attributes
	inline uint32 GetParticleRenderType() const { return particleRenderType; }
	inline void SetParticleRenderType(uint32 _particleRenderType) { particleRenderType = _particleRenderType; SetDirtyFlag(); }

	inline float GetSpriteScaleX() const { return spriteScaleX; }
	inline void SetSpriteScaleX(float _spriteScaleX) { spriteScaleX = _spriteScaleX; SetDirtyFlag(); }

	inline float GetSpriteScaleY() const { return spriteScaleY; }
	inline void SetSpriteScaleY(float _spriteScaleY) { spriteScaleY = _spriteScaleY; SetDirtyFlag(); }

	// Extra Attributes
	inline float GetMinScale() const { return minScale; }
	inline void SetMinScale(float _minScale) { minScale = _minScale; SetDirtyFlag(); }

	inline float GetMaxScale() const { return maxScale; }
	inline void SetMaxScale(float _maxScale) { maxScale = _maxScale; SetDirtyFlag(); }

//	inline float GetBeginScale() const { return beginScale; }
//	inline void SetBeginScale(float _beginScale) { beginScale = _beginScale; SetDirtyFlag(); }

//	inline float GetEndScale() const { return endScale; }
//	inline void SetEndScale(float _endScale) { endScale = _endScale; SetDirtyFlag(); }

//	inline float GetBeginAlpha() const { return beginAlpha; }
//	inline void SetBeginAlpha(float _beginAlpha) { beginAlpha = _beginAlpha; SetDirtyFlag(); }

//	inline float GetEndAlpha() const { return endAlpha; }
//	inline void SetEndAlpha(float _endAlpha) { endAlpha = _endAlpha; SetDirtyFlag(); }

	inline float GetGravityX() const { return gravityX; }
	inline void SetGravityX(float _gravityX) { gravityX = _gravityX; SetDirtyFlag(); }

	inline float GetGravityY() const { return gravityY; }
	inline void SetGravityY(float _gravityY) { gravityY = _gravityY; SetDirtyFlag(); }

	inline float GetGravityZ() const { return gravityZ; }
	inline void SetGravityZ(float _gravityZ) { gravityZ = _gravityZ; SetDirtyFlag(); }

	inline float GetSpriteTwistSpeed() const { return spriteTwistSpeed; }
	inline void SetSpriteTwistSpeed(float _spriteTwistSpeed) { spriteTwistSpeed = _spriteTwistSpeed; SetDirtyFlag(); }

	inline float GetAttenuation() const { return attenuation; }
	inline void SetAttenuation(float _attenuation) { attenuation = _attenuation; SetDirtyFlag(); }

	inline bool GetDepthSort() const { return depthSort; }
	inline void SetDepthSort(bool _depthSort) { depthSort = _depthSort; SetDirtyFlag(); }

	inline bool GetLoop() const { return loop; }
	inline void SetLoop(bool _loop) { loop = _loop; SetDirtyFlag(); }

	inline float GetNumFrame() const { return numFrame; }
	inline void SetNumFrame(float _numFrame) { numFrame = _numFrame; SetDirtyFlag(); }

	//inline uint32 GetLifeColorUInt() const
	//{
	//	unsigned char r = static_cast<unsigned char>(255 * GetLifeColor().x);
	//	unsigned char g = static_cast<unsigned char>(255 * GetLifeColor().y);
	//	unsigned char b = static_cast<unsigned char>(255 * GetLifeColor().z);
	//	return (r << 24) + (g << 16) + (b << 8);
	//}

	inline FCDAnimationKeyList& GetRate() { return rates; }
	inline const FCDAnimationKeyList& GetRate() const { return rates; }
	inline void SetRate( const FCDAnimationKeyList& list ) { rates = list; SetDirtyFlag(); }

	inline FCDColorAnimationKeyList& GetColor() { return color; }
	inline const FCDColorAnimationKeyList& GetColor() const { return color; }
	inline void SetColor( const FCDColorAnimationKeyList& list ) { color = list; SetDirtyFlag(); }

	inline FCDColorAnimationKeyList& GetTransparency() { return transparency; }
	inline const FCDColorAnimationKeyList& GetTransparency() const { return transparency; }
	inline void SetTransparency( const FCDColorAnimationKeyList& list ) { transparency = list; SetDirtyFlag(); }

	//inline void SetLifeColor( const FCDColorAnimationKeyList& list ) { lifeColor = list; SetDirtyFlag(); }
	//inline const FCDColorAnimationKeyList& GetLifeColor() const { return lifeColor; }

	//inline void SetLifeTransparency( const FCDColorAnimationKeyList& list ) { lifeTransparency = list; SetDirtyFlag(); }
	//inline const FCDColorAnimationKeyList& GetLifeTransparency() const { return lifeTransparency; }

	inline const FCDColorAnimationKeyList& GetRGBRamp() const { return rgbRamp; }
	inline void SetRGBRamp( const FCDColorAnimationKeyList& list ) { rgbRamp = list; SetDirtyFlag(); }

	inline const FCDColorAnimationKeyList& GetOpatcityRamp() const { return opacityRamp; }
	inline void SetOpacityRamp( const FCDColorAnimationKeyList& list ) { opacityRamp = list; SetDirtyFlag(); }


	////////////////////////////////////////////////////////////////////////////
	// 2010/02/18

	inline void SetLifeSpriteScaleXPPRamp( const FCDColorAnimationKeyList& list ) { lifeSpriteScaleXPPRamp = list; SetDirtyFlag(); }
	inline const FCDColorAnimationKeyList& GetLifeSpriteScaleXPPRamp() const { return lifeSpriteScaleXPPRamp; }

	inline void SetLifeSpriteScaleYPPRamp( const FCDColorAnimationKeyList& list ) { lifeSpriteScaleYPPRamp = list; SetDirtyFlag(); }
	inline const FCDColorAnimationKeyList& GetLifeSpriteScaleYPPRamp() const { return lifeSpriteScaleYPPRamp; }

	////////////////////////////////////////////////////////////////////////////
	// 2010/02/19

	//inline bool GetBillboard() const { return billboard; }

	//inline const float* GetRotOffset() const { return rotOffset; }
	//inline void SetRotOffset( const float* pArray ) { rotOffset[0] = pArray[0];
	//												  rotOffset[1] = pArray[1];
	//												  rotOffset[2] = pArray[2];
	//												  SetDirtyFlag(); }

	//inline const float* GetInitRandomRot() const { return initRandomRot; }
	//inline void SetInitRandomRot( const float* pArray ) { initRandomRot[0] = pArray[0];
	//													  initRandomRot[1] = pArray[1];
	//													  initRandomRot[2] = pArray[2];
	//													  SetDirtyFlag(); }

	//inline const float* GetRotSpeed() const { return rotSpeed; }
	//inline void SetRotSpeed( const float* pArray ) { rotSpeed[0] = pArray[0];
	//												 rotSpeed[1] = pArray[1];
	//												 rotSpeed[2] = pArray[2];
	//												 SetDirtyFlag(); }

	//inline const float* GetRandomRotSpeed() const { return randomRotSpeed; }
	//inline void SetRandomRotSpeed( const float* pArray ) { randomRotSpeed[0] = pArray[0];
	//													   randomRotSpeed[1] = pArray[1];
	//													   randomRotSpeed[2] = pArray[2];
	//													   SetDirtyFlag(); }

	//inline const float* GetPivot() const { return pivot; }
	//inline void SetPivot( const float* pArray ) { pivot[0] = pArray[0];
	//											  pivot[1] = pArray[1];
	//											  SetDirtyFlag(); }

	//inline float GetZBillboardOffset() const { return zBillboardOffset; }
	//inline void SetZBillboardOffset( float f ) { zBillboardOffset = f; SetDirtyFlag(); }
	/*
	float rotOffset[3];
	float initRandomRot[3];
	float rotSpeed[3];
	float randomRotSpeed[3];
	float pivot[2];
	float zBillboardOffset;
	*/


	/** Clones the emitter information.
		@param clone The cloned emitter.
			If this pointer is NULL, a new emitter is created and
			you will need to release it manually.
		@param cloneChildren Whether to recursively clone this entity's children.
		@return The clone. */
	virtual FCDEntity* Clone(FCDEntity* clone = NULL, bool cloneChildren = false) const;
};

#endif //_FCD_EMITTER_H
