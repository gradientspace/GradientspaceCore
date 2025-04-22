// Copyright Gradientspace Corp. All Rights Reserved.
#include "Mesh/TriangleMeshAttributes.h"

using namespace GS;

GS::MeshAttributeName GS::MeshAttributeName::Normal(uint8_t Index)
{
	return GS::MeshAttributeName("NORMAL", Index);
}

GS::MeshAttributeName GS::MeshAttributeName::UV(uint8_t Index)
{
	return GS::MeshAttributeName("UV", Index);
}

GS::MeshAttributeName GS::MeshAttributeName::Color4f(uint8_t Index)
{
	return GS::MeshAttributeName("COLOR4F", Index);
}
GS::MeshAttributeName GS::MeshAttributeName::Color4b(uint8_t Index)
{
	return GS::MeshAttributeName("COLOR4B", Index);
}

GS::MeshAttributeName GS::MeshAttributeName::Material(uint8_t Index)
{
	return GS::MeshAttributeName("MATERIAL", Index);
}

GS::MeshAttributeName GS::MeshAttributeName::TriGroup(uint8_t Index)
{
	return GS::MeshAttributeName("TRIGROUP", Index);
}

GS::MeshAttributeHeader GS::MeshAttributeHeader::Normal3f()
{
	return MeshAttributeHeader(3, sizeof(float));
}
GS::MeshAttributeHeader GS::MeshAttributeHeader::UV2f()
{
	return MeshAttributeHeader(2, sizeof(float));
}
GS::MeshAttributeHeader GS::MeshAttributeHeader::Color4f()
{
	return MeshAttributeHeader(4, sizeof(float));
}
GS::MeshAttributeHeader GS::MeshAttributeHeader::Color4b()
{
	return MeshAttributeHeader(4, sizeof(uint8_t));
}
GS::MeshAttributeHeader GS::MeshAttributeHeader::TriGroup()
{
	return MeshAttributeHeader(1, sizeof(int32_t));
}

GS::MeshAttributeHeader GS::MeshAttributeHeader::Material()
{
	return MeshAttributeHeader(1, sizeof(uint8_t));
}
