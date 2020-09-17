/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

/**
	@file FCDEmitterInstance.h
	This file contains the FCDEmitterInstance class.

	@ingroup FCDEmitters
*/

#ifndef _FCD_EMITTER_INSTANCE_H_
#define _FCD_EMITTER_INSTANCE_H_

#ifndef _FCD_ENTITY_INSTANCE_H_
#include "FCDocument/FCDEntityInstance.h"
#endif // _FCD_ENTITY_INSTANCE_H_

class FCDEmitterParticle;
class FCDMaterial;
class FCDMaterialInstance;
class FCDForceField;

/**
	A COLLADA emitter instance.

	The types of particles emitted are instance dependent.
	Instances also contain the FCDForceInstances pointers to forces applied to the particles.
*/
class FCOLLADA_EXPORT FCDEmitterInstance : public FCDEntityInstance
{
private:
	DeclareObjectType(FCDEntityInstance);
	DeclareParameterContainer(FCDMaterialInstance, materials, FC("Materials Bound"));

	DeclareParameter(uint32, FUParameterQualifiers::SIMPLE, seed, FC("Seed"));
	DeclareParameter(bool, FUParameterQualifiers::SIMPLE, alphaSort, FC("Alpha Sort"));
	DeclareParameter(uint32, FUParameterQualifiers::SIMPLE, particleType, FC("Particle Type"));
	DeclareParameter(fstring, FUParameterQualifiers::SIMPLE, url, FC("URL"));

	// Material for this set of polygons
	DeclareParameter(fstring, FUParameterQualifiers::SIMPLE, materialSemantic, FC("Material Semantic"));

	friend class FCDEntityInstanceFactory;

protected:
	/** Constructor: do not use directly.
		Instead, use the FCDSceneNode::AddInstance function.
		@param document The COLLADA document that owns the emitter instance.
		@param parent The parent visual scene node.
		@param entityType The type of the entity to instantiate. Unless this class
			is overwritten, FCDEntity::EMITTER should be given. */
	FCDEmitterInstance(FCDocument* document, FCDSceneNode* parent, FCDEntity::Type entityType = FCDEntity::EMITTER);

public:
	inline void SetSeed(uint32 _seed) { seed = _seed; SetDirtyFlag(); }
	inline void SetAlphaSort(bool _alphaSort) { alphaSort = _alphaSort; SetDirtyFlag(); }
	inline void SetParticleType(uint32 _particleType) { particleType = _particleType; SetDirtyFlag(); }
	inline void SetURL(fstring _url) { url = _url; SetDirtyFlag(); }

	inline fstring GetURL() const { return url; }

public:
	/** Destructor. */
	virtual ~FCDEmitterInstance();

	/** Retrieves the entity instance class type.
		This is used to determine the up-class for the entity instance object.
		@deprecated Instead use: FCDEntityInstance::HasType(FCDGeometryInstance::GetClassType())
		@return The class type: EMITTER. */
	virtual Type GetType() const { return EMITTER; }

	inline size_t GetMaterialInstanceCount() const { return materials.size(); }

	inline FCDMaterialInstance* GetMaterialInstance() { return materials[0]; }
	inline const FCDMaterialInstance* GetMaterialInstance() const { return materials[0]; } /**< See above. */

	FCDMaterialInstance* AddMaterialInstance();

	FCDMaterialInstance* AddMaterialInstance(FCDMaterial* material, FCDEmitterParticle* particle);

	inline const fstring& GetMaterialSemantic() const { return materialSemantic; }
};

#endif // _FCD_EMITTER_INSTANCE_H_
