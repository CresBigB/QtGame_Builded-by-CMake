#pragma once

#include "Headers.h"

class Character;

// 四叉树节点
class QuadtreeNode
{
public:
	QuadtreeNode(const QRectF& bounds, int depth = 0);
	~QuadtreeNode();

	[[nodiscard]] std::vector<QRectF> getAllBounds() const;
	[[nodiscard]] std::vector<QGraphicsItem*> query(const QRectF& range) const;
	[[nodiscard]] QRectF getBound() const;

	void insert(QGraphicsItem* item);
	void clear();

private:
	static const int MAX_OBJECTS = 12; // 每个节点最大物体数
	static const int MAX_DEPTH = 10;   // 最大深度

	QRectF bounds_;                   // 节点边界
	int depth_;                       // 当前深度
	std::vector<QGraphicsItem*> objects_; // 存储的物体
	std::unique_ptr<QuadtreeNode> children_[4]; // 子节点

	[[nodiscard]] bool isLeaf() const
	{
		return children_[0] == nullptr;
	}
	void subdivide(); // 分割节点
	[[nodiscard]] std::vector<int> getQuadrant(const QRectF& itemBounds) const; // 获取物体所属象限（支持多个）
};

// 四叉树管理类
class Quadtree
{
public:
	Quadtree() = default;
	~Quadtree();

	void initialize(const QRectF& rect);
	void visualize(QGraphicsScene* scene) const;
	void addItem(QGraphicsItem* item) const;
	[[nodiscard]] auto getRoot() const
	{
		return root_.get();
	}
	void clear() const;

	std::vector<QGraphicsItem*> findItemSectional(const QRectF& rect);
	std::vector<QGraphicsItem*> findCollisions(const QGraphicsItem* item) const;
	[[nodiscard]] std::vector<QGraphicsItem*> findItemsAroundPoint(const QPointF& point) const;

private:
	std::unique_ptr<QuadtreeNode> root_;
};