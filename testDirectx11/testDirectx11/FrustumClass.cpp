////////////////////////////////////////////////////////////////////////////////
// Filename: frustumclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "frustumclass.h"


FrustumClass::FrustumClass()
{
}


FrustumClass::FrustumClass(const FrustumClass& other)
{
}


FrustumClass::~FrustumClass()
{
}

void FrustumClass::ConstructFrustum(float screenDepth, XMMATRIX projectionMatrix, XMMATRIX viewMatrix)
{
	float zMinimum, r;
	XMFLOAT4X4 matrix;

	XMFLOAT4X4 pm, vm;
	XMStoreFloat4x4(&pm, projectionMatrix);
	XMStoreFloat4x4(&vm, viewMatrix);

	// Calculate the minimum Z distance in the frustum.
	zMinimum = -pm.m[4][3] / pm.m[3][3];
	r = screenDepth / (screenDepth - zMinimum);
	pm.m[3][3] = r;
	pm.m[4][3] = -r * zMinimum;

	// Create the frustum matrix from the view matrix and updated projection matrix.
	auto m = XMMatrixMultiply(viewMatrix, projectionMatrix);
	XMStoreFloat4x4(&matrix, m);

	// Calculate near plane of frustum.
	XMFLOAT4 pf;
	XMStoreFloat4(&pf, m_planes[0]);
	pf.x = matrix._14 + matrix._13;
	pf.y = matrix._24 + matrix._23;
	pf.z = matrix._34 + matrix._33;
	pf.w = matrix._44 + matrix._43;
	m_planes[0] = XMLoadFloat4(&pf);
	XMPlaneNormalize(m_planes[0]);

	// Calculate far plane of frustum.
	XMStoreFloat4(&pf, m_planes[1]);
	pf.x = matrix._14 - matrix._13;
	pf.y = matrix._24 - matrix._23;
	pf.z = matrix._34 - matrix._33;
	pf.w = matrix._44 - matrix._43;
	m_planes[1] = XMLoadFloat4(&pf);
	XMPlaneNormalize(m_planes[1]);

	// Calculate left plane of frustum.
	XMStoreFloat4(&pf, m_planes[1]);
	pf.x = matrix._14 + matrix._11;
	pf.y = matrix._24 + matrix._21;
	pf.z = matrix._34 + matrix._31;
	pf.w = matrix._44 + matrix._41;
	m_planes[2] = XMLoadFloat4(&pf);
	XMPlaneNormalize(m_planes[2]);

	// Calculate right plane of frustum.
	XMStoreFloat4(&pf, m_planes[3]);
	pf.x = matrix._14 - matrix._11;
	pf.y = matrix._24 - matrix._21;
	pf.z = matrix._34 - matrix._31;
	pf.w = matrix._44 - matrix._41;
	m_planes[3] = XMLoadFloat4(&pf);
	XMPlaneNormalize(m_planes[3]);

	// Calculate top plane of frustum.
	XMStoreFloat4(&pf, m_planes[4]);
	pf.x = matrix._14 - matrix._12;
	pf.y = matrix._24 - matrix._22;
	pf.z = matrix._34 - matrix._32;
	pf.w = matrix._44 - matrix._42;
	m_planes[4] = XMLoadFloat4(&pf);
	XMPlaneNormalize(m_planes[4]);

	// Calculate bottom plane of frustum.
	XMStoreFloat4(&pf, m_planes[5]);
	pf.x = matrix._14 + matrix._12;
	pf.y = matrix._24 + matrix._22;
	pf.z = matrix._34 + matrix._32;
	pf.w = matrix._44 + matrix._42;
	m_planes[5] = XMLoadFloat4(&pf);
	XMPlaneNormalize(m_planes[5]);

	return;
}

bool FrustumClass::CheckPoint(FLOAT x, FLOAT y, FLOAT z)
{
	// Check if the point is inside all six planes of the view frustum.
	for (INT i = 0; i < 6; i++)
	{
		XMVECTOR vector = XMPlaneDotCoord(m_planes[i], XMLoadFloat4(&XMFLOAT4(x, y, z, 1.0f)));

		if (XMVectorGetX(vector) < 0.0f)
		{
			return false;
		}
	}

	return true;
}

bool FrustumClass::CheckCube(FLOAT xCenter, FLOAT yCenter, FLOAT zCenter, FLOAT radius)
{
	// Check if any one point of the cube is in the view frustum.
	for (INT i = 0; i < 6; i++)
	{
		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMLoadFloat4(&XMFLOAT4((xCenter - radius), (yCenter - radius), (zCenter - radius), 1.0f)))) >= 0.0f)
		{
			continue;
		}

		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMLoadFloat4(&XMFLOAT4((xCenter + radius), (yCenter - radius), (zCenter - radius), 1.0f)))) >= 0.0f)
		{
			continue;
		}

		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMLoadFloat4(&XMFLOAT4((xCenter - radius), (yCenter + radius), (zCenter - radius), 1.0f)))) >= 0.0f)
		{
			continue;
		}

		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMLoadFloat4(&XMFLOAT4((xCenter + radius), (yCenter + radius), (zCenter - radius), 1.0f)))) >= 0.0f)
		{
			continue;
		}

		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMLoadFloat4(&XMFLOAT4((xCenter - radius), (yCenter - radius), (zCenter + radius), 1.0f)))) >= 0.0f)
		{
			continue;
		}

		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMLoadFloat4(&XMFLOAT4((xCenter + radius), (yCenter - radius), (zCenter + radius), 1.0f)))) >= 0.0f)
		{
			continue;
		}

		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMLoadFloat4(&XMFLOAT4((xCenter - radius), (yCenter + radius), (zCenter + radius), 1.0f)))) >= 0.0f)
		{
			continue;
		}

		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMLoadFloat4(&XMFLOAT4((xCenter + radius), (yCenter + radius), (zCenter + radius), 1.0f)))) >= 0.0f)
		{
			continue;
		}

		return false;
	}

	return true;
}

bool FrustumClass::CheckSphere(FLOAT xCenter, FLOAT yCenter, FLOAT zCenter, FLOAT radius)
{
	// Check if the radius of the sphere is inside the view frustum.
	for (INT i = 0; i < 6; i++)
	{
		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMLoadFloat4(&XMFLOAT4(xCenter, yCenter, zCenter, 1.0f)))) < -radius)
		{
			return false;
		}
	}

	return true;
}

bool FrustumClass::CheckRectangle(FLOAT xCenter, FLOAT yCenter, FLOAT zCenter, FLOAT xSize, FLOAT ySize, FLOAT zSize)
{
	// Check if any of the 6 planes of the rectangle are inside the view frustum.
	for (INT i = 0; i < 6; i++)
	{
		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMLoadFloat4(&XMFLOAT4((xCenter - xSize), (yCenter - ySize), (zCenter - zSize), 1.0f)))) >= 0.0f)
		{																																	 
			continue;																														 
		}																																	 
																																			 
		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMLoadFloat4(&XMFLOAT4((xCenter + xSize), (yCenter - ySize), (zCenter - zSize), 1.0f)))) >= 0.0f)
		{
			continue;
		}

		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMLoadFloat4(&XMFLOAT4((xCenter - xSize), (yCenter + ySize), (zCenter - zSize), 1.0f)))) >= 0.0f)
		{
			continue;
		}

		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMLoadFloat4(&XMFLOAT4((xCenter - xSize), (yCenter - ySize), (zCenter + zSize), 1.0f)))) >= 0.0f)
		{
			continue;
		}

		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMLoadFloat4(&XMFLOAT4((xCenter + xSize), (yCenter + ySize), (zCenter - zSize), 1.0f)))) >= 0.0f)
		{
			continue;
		}

		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMLoadFloat4(&XMFLOAT4((xCenter + xSize), (yCenter - ySize), (zCenter + zSize), 1.0f)))) >= 0.0f)
		{
			continue;
		}

		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMLoadFloat4(&XMFLOAT4((xCenter - xSize), (yCenter + ySize), (zCenter + zSize), 1.0f)))) >= 0.0f)
		{
			continue;
		}

		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMLoadFloat4(&XMFLOAT4((xCenter + xSize), (yCenter + ySize), (zCenter + zSize), 1.0f)))) >= 0.0f)
		{
			continue;
		}

		return false;
	}

	return true;
}