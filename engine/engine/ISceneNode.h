#pragma once

#include "CTransform.h"
#include "vector3d.h"
#include "quaternion.h"
#include "aabbox3d.h"
#include <list>
#include <functional>

class IRenderer;
class CCamera;
struct SRenderUnit;
using TRAVERSE_SCENENODE_FUNC = std::function<void(ISceneNode* node)>;

class ISceneNode
{
public:
	ISceneNode()
		: m_Transform(this), m_Active(true), m_ToDelete(false), m_layer(LAYER_DEFAULT)
	{
		m_Renderer = nullptr;
	}

protected:
	virtual ~ISceneNode() = default;

public:
	virtual void tick(uint32_t tickTime, const CCamera* cam) {}
	virtual std::list<SRenderUnit*> render(const IRenderer* renderer, const CCamera* cam) = 0;

public:
	void setParent(ISceneNode* parent)
	{
		getTransform()->setParent(parent ? parent->getTransform() : nullptr);
	}
	void traverse(TRAVERSE_SCENENODE_FUNC func);
	CTransform* getTransform() { return &m_Transform; }
	const CTransform* getTransform() const { return &m_Transform; }
	const IRenderer* getRenderer() const { return m_Renderer; }
	IRenderer* getRenderer() { return m_Renderer; }

	void destroy();
	bool isToDelete() const { return m_ToDelete; }
	void destroyImmediate();
	static void checkDestroy(ISceneNode* node);

	void setActive(bool active) { m_Active = active; }
	bool activeSelf() const { return m_Active; }

	void setLayer(int layer) { m_layer = layer; }
	int getLayer() const { return m_layer; }

protected:
	CTransform		m_Transform;
	IRenderer*	m_Renderer;
	int m_layer;
	bool m_ToDelete;
	bool m_Active;
};

inline void ISceneNode::traverse(TRAVERSE_SCENENODE_FUNC func)
{
	func(this);

	for (const auto& trans : m_Transform.getChildList())
	{
		ISceneNode* node = trans->getSceneNode();
		node->traverse(func);
	}
}

inline void ISceneNode::destroy()
{
	m_ToDelete = true;
	for (const auto& trans : getTransform()->getChildList())
	{
		ISceneNode* node = trans->getSceneNode();
		node->destroy();
	}
}

inline void ISceneNode::destroyImmediate()
{
	destroy();
	checkDestroy(this);
}

inline void ISceneNode::checkDestroy(ISceneNode* node)
{
	if (node->isToDelete())			
		node->destroy();		//删除所有子结点

	for (const auto& trans : node->getTransform()->getChildList())
	{
		ISceneNode* node = trans->getSceneNode();
		checkDestroy(node);
	}

	if (node->isToDelete())
		delete node;
}