#pragma once

#include "Application.h"
#include <GeometryActor.h>

using namespace Glyph3;

#include <BasicVertexDX11.h>
typedef std::shared_ptr<Glyph3::DrawExecutorDX11<Glyph3::BasicVertexDX11::Vertex>> BasicMeshPtr;

class LJMUGeometryActor : public GeometryActor
{
public:
	//------------CONSTRUCTORS------------------------------------------------
	LJMUGeometryActor();

	void UseLitTexturedMaterial(ResourcePtr texture);
	void UseSkyboxMaterial(ResourcePtr texture1, ResourcePtr texture2);

	MaterialPtr m_pLitTexturedMaterial;
	MaterialPtr m_pSkyboxMaterial;

	MaterialPtr LJMUGeometryActor::createLitTexturedMaterial();
	MaterialPtr LJMUGeometryActor::createSkyboxMaterial();

	BasicMeshPtr generateObjMesh(std::wstring pmeshname, Vector4f pmeshcolour);

	void applyTextureToMaterial(MaterialPtr material, ResourcePtr texture);
};

