#include "CCamera.h"

#include "function3d.h"

CCamera::CCamera(bool isOrthogonal)
	: m_bInit(false), m_IsOrthogonal(isOrthogonal)
{
	m_vFOV = 0.0f;
	m_vRatio = 0.0f;
	m_vFront = 0.0f;
	m_vBack = 0.0f;
	m_orthoLeft = m_orthoRight = m_orthoLeft = m_orthoRight = 0;

	m_vecPos = vector3df::Zero();
	m_vecDir = vector3df::UnitZ();
	m_vecUp = vector3df::UnitY();
	m_vecRight = vector3df::UnitX();
}

CCamera::~CCamera()
{

}

void CCamera::Init(float vFov, float vRatio, float vFront, float vBack)
{
	setProjectionParam(vFov, vRatio, vFront, vBack);
	m_bInit = true;
}

void CCamera::Init(float left, float right, float top, float bottom, float vFront, float vBack)
{
	setProjectionParam(left, right, top, bottom, vFront, vBack);
	m_bInit = true;
}

void CCamera::setDirAndUp(const vector3df& vecDir, const vector3df& vecUp)
{
	m_vecDir = f3d::normalize(vecDir);
	vector3df vUp = f3d::normalize(vecUp);
	m_vecRight = f3d::normalize(f3d::crossProduct(vUp, m_vecDir));
	m_vecUp = f3d::normalize(f3d::crossProduct(m_vecDir, m_vecRight));

	updateViewTM();
}

void CCamera::move(const vector3df& vecDelta)
{
	m_vecPos = m_vecPos + vecDelta;
	updateViewTM();
}

void CCamera::turnAroundAxis(const vector3df& vecPos, const vector3df& vecAxis, float vDeltaRad)
{
	matrix4 matRotate = f3d::rotateAxis(vecPos, vecAxis, vDeltaRad);
	vector3df vecOrigin = matRotate.transformVector(vector3df::Zero());
	m_vecPos = matRotate.transformVector(m_vecPos);
	m_vecDir = matRotate.transformVector(m_vecDir) - vecOrigin;
	m_vecUp = matRotate.transformVector(m_vecUp) - vecOrigin;

	setDirAndUp(m_vecDir, m_vecUp);
}

void CCamera::setViewTM(const matrix4& matView)
{
	m_matViewTM = matView;
	m_matVPTM = m_matViewTM * m_matProjectionTM;

	vector3df vecDir, vecUp, vecPos;
	vecDir = matView.getCol(2);
	vecUp = matView.getCol(1);
	vecPos = matView.getRow(3);

	matrix4 matInv = matView;
	matInv._41 = matInv._42 = matInv._43 = 0.0f;
	matInv.transpose();

	setDirAndUp(vecDir, vecUp);
	setPos(vecPos);
}

void CCamera::setProjectionTM(const matrix4& matProjection)
{
	m_matProjectionTM = matProjection;

	m_matVPTM = m_matViewTM * m_matProjectionTM;

	updateWorldFrustum();
}

void CCamera::setZFrontAndBack(float vFront, float vBack)
{
	if (vFront > 0.0f)
		m_vFront = vFront;
	if (vBack > 0.0f)
		m_vBack = vBack;

	updateProjectionTM();
}

void CCamera::setProjectionParam(float vFOV, float vRatio, float vFront, float vBack)
{
	ASSERT(!m_IsOrthogonal);

	if (vFOV > 0.0f)
		m_vFOV = vFOV;
	if (vRatio > 0.0f)
		m_vRatio = vRatio;
	setZFrontAndBack(vFront, vBack);
}

void CCamera::setProjectionParam(float left, float right, float top, float bottom, float vFront, float vBack)
{
	ASSERT(m_IsOrthogonal);

	m_orthoLeft = left;
	m_orthoRight = right;
	m_orthoTop = top;
	m_orthoBottom = bottom;
	setZFrontAndBack(vFront, vBack);
}

void CCamera::updateViewTM()
{
	m_matViewTM = f3d::makeViewMatrix(m_vecPos, m_vecDir, m_vecUp, 0.0f);
	m_matVPTM = m_matViewTM * m_matProjectionTM;

	updateWorldFrustum();
}

void CCamera::updateProjectionTM()
{
	if (!m_IsOrthogonal)
		m_matProjectionTM = f3d::makePerspectiveFovMatrixLH(m_vFOV, m_vRatio, m_vFront, m_vBack);
	else
		m_matProjectionTM = f3d::makeOrthoOffCenterMatrixLH(m_orthoLeft, m_orthoRight, m_orthoTop, m_orthoBottom, m_vFront, m_vBack);
	m_matVPTM = m_matViewTM * m_matProjectionTM;
	updateWorldFrustum();
}

void CCamera::updateWorldFrustum()
{
	if (!m_IsOrthogonal)
	{
		float c, s, fAngle = m_vFOV * 0.5f;
		c = (float)cos(fAngle);
		s = (float)sin(fAngle);

		vector3df vNormals[6];
		vNormals[frustum::VF_TOP].set(0.0f, -c, s);
		vNormals[frustum::VF_BOTTOM].set(0.0f, c, s);

		fAngle = (float)(tan(fAngle) * m_vRatio);
		c = (float)cos(atan(fAngle));
		s = (float)sin(atan(fAngle));

		vNormals[frustum::VF_LEFT].set(c, 0.0f, s);
		vNormals[frustum::VF_RIGHT].set(-c, 0.0f, s);

		vNormals[frustum::VF_NEAR].set(0.0f, 0.0f, 1.0f);
		vNormals[frustum::VF_FAR].set(0.0f, 0.0f, -1.0f);

		//left
		{
			vector3df vNormal = f3d::viewToWorld(vNormals[frustum::VF_LEFT], m_matViewTM);
			float fDist = vNormal.dotProduct(m_vecPos);
			m_worldFrustum.setPlane(frustum::VF_LEFT, plane3df(vNormal, fDist));
		}

		//right
		{
			vector3df vNormal = f3d::viewToWorld(vNormals[frustum::VF_RIGHT], m_matViewTM);
			float fDist = vNormal.dotProduct(m_vecPos);
			m_worldFrustum.setPlane(frustum::VF_RIGHT, plane3df(vNormal, fDist));
		}

		//top
		{
			vector3df vNormal = f3d::viewToWorld(vNormals[frustum::VF_TOP], m_matViewTM);
			float fDist = vNormal.dotProduct(m_vecPos);
			m_worldFrustum.setPlane(frustum::VF_TOP, plane3df(vNormal, fDist));
		}

		//bottom
		{
			vector3df vNormal = f3d::viewToWorld(vNormals[frustum::VF_BOTTOM], m_matViewTM);
			float fDist = vNormal.dotProduct(m_vecPos);
			m_worldFrustum.setPlane(frustum::VF_BOTTOM, plane3df(vNormal, fDist));
		}

		//near
		{
			vector3df vPos = m_vecPos + m_vecDir * m_vFront;
			vector3df vNormal = m_vecDir;
			float fDist = vNormal.dotProduct(vPos);
			m_worldFrustum.setPlane(frustum::VF_NEAR, plane3df(m_vecDir, fDist));
		}

		//far
		{
			vector3df vPos = m_vecPos + m_vecDir * m_vBack;
			vector3df vNormal = -m_vecDir;
			float fDist = vNormal.dotProduct(vPos);
			m_worldFrustum.setPlane(frustum::VF_FAR, plane3df(m_vecDir, fDist));
		}
	}
	else
	{
		//left
		{
			vector3df vNormal = m_vecRight;
			float fDist = vNormal.dotProduct(m_vecPos + m_vecRight * m_orthoLeft);
			m_worldFrustum.setPlane(frustum::VF_LEFT, plane3df(vNormal, fDist));
		}

		//right
		{
			vector3df vNormal = -m_vecRight;
			float fDist = vNormal.dotProduct(m_vecPos + m_vecRight * m_orthoRight);
			m_worldFrustum.setPlane(frustum::VF_RIGHT, plane3df(vNormal, fDist));
		}

		//top
		{
			vector3df vNormal = -m_vecUp;
			float fDist = vNormal.dotProduct(m_vecPos + m_vecUp * m_orthoTop);
			m_worldFrustum.setPlane(frustum::VF_TOP, plane3df(vNormal, fDist));
		}

		//bottom
		{
			vector3df vNormal = m_vecUp;
			float fDist = vNormal.dotProduct(m_vecPos + m_vecRight * m_orthoBottom);
			m_worldFrustum.setPlane(frustum::VF_BOTTOM, plane3df(vNormal, fDist));
		}

		//near
		{
			vector3df vNormal = m_vecDir;
			float fDist = vNormal.dotProduct(m_vecPos + m_vecDir * m_vFront);
			m_worldFrustum.setPlane(frustum::VF_LEFT, plane3df(vNormal, fDist));
		}

		//far
		{
			vector3df vNormal = -m_vecDir;
			float fDist = vNormal.dotProduct(m_vecPos + m_vecRight * m_vBack);
			m_worldFrustum.setPlane(frustum::VF_LEFT, plane3df(vNormal, fDist));
		}
	}
}