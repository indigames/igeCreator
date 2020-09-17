/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

/**
	@file FCDEmitterParticle.h
	This file defines a base class for particle emitters in FCollada.
	The base class contains the particle emitter type, and contains a few utility functions
*/

#ifndef _FCD_EMITTER_PARTICLE_H_
#define _FCD_EMITTER_PARTICLE_H_

class FCDEmitter;

class FCOLLADA_EXPORT FCDEmitterParticle : public FCDObject
{
private:
	DeclareObjectType(FCDObject);

	FCDEmitter* parent;

	// Material for this set of polygons
	DeclareParameter(fstring, FUParameterQualifiers::SIMPLE, materialSemantic, FC("Material Semantic"));

public:
	/** Constructor. */
	FCDEmitterParticle(FCDocument* document, FCDEmitter* parent);

	/** Destructor. */
	virtual ~FCDEmitterParticle();

	/** */
	inline FCDEmitter* GetParent() { return parent; }
	inline const FCDEmitter* GetParent() const { return parent; } /**< See above. */

	/** */
	inline const fstring& GetMaterialSemantic() const { return materialSemantic; }

	/** */
	inline void SetMaterialSemantic(const fchar* semantic) { materialSemantic = semantic; SetDirtyFlag(); }
	inline void SetMaterialSemantic(const fstring& semantic) { materialSemantic = semantic; SetDirtyFlag(); } /**< See above. */

	/** */
	virtual FCDEmitterParticle* Clone(FCDEmitterParticle* clone) const;
};

#endif // _FCD_EMITTER_PARTICLE_H_
