#include "Quadtree.h"
#include <algorithm>

#include "Character.h"

// QuadtreeNode实现
QuadtreeNode::QuadtreeNode(const QRectF& bounds, const int depth)
	: bounds_(bounds), depth_(depth)
{}

QuadtreeNode::~QuadtreeNode()
{
	clear();
}

std::vector<QRectF> QuadtreeNode::getAllBounds() const
{
	std::vector<QRectF> bounds;
	bounds.push_back(bounds_); // 添加当前节点的边界框

	if (!isLeaf())
	{
		for (const auto& child : children_)
		{
			auto childBounds = child->getAllBounds();
			bounds.insert(bounds.end(), childBounds.begin(), childBounds.end());
		}
	}
	return bounds;
}

void QuadtreeNode::insert(QGraphicsItem* item)
{
	if (!item) return;

	QRectF itemBounds = item->boundingRect().translated(item->pos());

	// 如果是叶节点
	if (isLeaf())
	{
		// 未达到最大深度且物体数量超过限制
		if (objects_.size() >= MAX_OBJECTS && depth_ < MAX_DEPTH)
		{
			subdivide();
			// 将已有物体重新插入子节点
			objects_.push_back(item);
			for (QGraphicsItem* obj : objects_)
			{
				auto quadrants = getQuadrant(obj->boundingRect().translated(obj->pos()));
				if (!quadrants.empty())
				{
					for (int quadrant : quadrants)
					{
						children_[quadrant]->insert(obj);
					}
				}
				else
				{
					objects_.push_back(obj); // 仍存储在当前节点
				}
			}
			objects_.clear();
		}
		else
		{
			objects_.push_back(item);
			return;
		}
	}

	// 插入到子节点
	auto quadrants = getQuadrant(itemBounds);
	if (!quadrants.empty())
	{
		for (int quadrant : quadrants)
		{
			children_[quadrant]->insert(item);
		}
	}
	else
	{
		objects_.push_back(item); // 跨越整个节点，存储在当前节点
	}
}

std::vector<QGraphicsItem*> QuadtreeNode::query(const QRectF& range) const
{
	std::vector<QGraphicsItem*> result;

	// 扩展查询范围以处理浮点数精度问题
	QRectF expandedRange = range.adjusted(-0.01, -0.01, 0.01, 0.01);
	if (!bounds_.intersects(expandedRange)) return result;

	// 添加本节点中的物体
	result.insert(result.end(), objects_.begin(), objects_.end());

	// 查询子节点
	if (!isLeaf())
	{
		for (const auto& i : children_)
		{
			auto childResult = i->query(range);
			result.insert(result.end(), childResult.begin(), childResult.end());
		}
	}

	return result;
}

QRectF QuadtreeNode::getBound() const
{
	return this->bounds_;
}

void QuadtreeNode::clear()
{
	if (!this->objects_.empty())
		objects_.clear();
	if (!isLeaf())
	{
		for (auto& i : children_)
			i.reset();
	}
}

void QuadtreeNode::subdivide()
{
	qreal x = bounds_.x();
	qreal y = bounds_.y();
	qreal w = bounds_.width() / 2.0;
	qreal h = bounds_.height() / 2.0;

	children_[0] = std::make_unique<QuadtreeNode>(QRectF(x, y, w, h), depth_ + 1);
	children_[1] = std::make_unique<QuadtreeNode>(QRectF(x + w, y, w, h), depth_ + 1);
	children_[2] = std::make_unique<QuadtreeNode>(QRectF(x, y + h, w, h), depth_ + 1);
	children_[3] = std::make_unique<QuadtreeNode>(QRectF(x + w, y + h, w, h), depth_ + 1);
}

std::vector<int> QuadtreeNode::getQuadrant(const QRectF& itemBounds) const
{
	qreal midX = bounds_.x() + bounds_.width() / 2.0;
	qreal midY = bounds_.y() + bounds_.height() / 2.0;

	std::vector<int> quadrants;
	// 检查物体是否与每个象限的边界框相交
	if (itemBounds.intersects(QRectF(bounds_.x(), bounds_.y(), bounds_.width() / 2.0, bounds_.height() / 2.0)))
		quadrants.push_back(0); // 左上
	if (itemBounds.intersects(QRectF(midX, bounds_.y(), bounds_.width() / 2.0, bounds_.height() / 2.0)))
		quadrants.push_back(1); // 右上
	if (itemBounds.intersects(QRectF(bounds_.x(), midY, bounds_.width() / 2.0, bounds_.height() / 2.0)))
		quadrants.push_back(2); // 左下
	if (itemBounds.intersects(QRectF(midX, midY, bounds_.width() / 2.0, bounds_.height() / 2.0)))
		quadrants.push_back(3); // 右下

	return quadrants;
}

Quadtree::~Quadtree()
{
	clear();
}

void Quadtree::visualize(QGraphicsScene* scene) const
{
	if (!root_ || !scene) return;

	// 清空之前的边界框（可选，取决于你的需求）
	for (auto item : scene->items())
	{
		if (item->data(0).toString() == "QuadtreeBound")
		{
			scene->removeItem(item);
			delete item;
		}
	}

	// 获取所有节点的边界框
	auto bounds = root_->getAllBounds();

	// 为每个边界框创建 QGraphicsRectItem
	for (const auto& bound : bounds)
	{
		QGraphicsRectItem* rectItem = new QGraphicsRectItem(bound);
		rectItem->setPen(QPen(Qt::blue, 1, Qt::DashLine)); // 蓝色虚线边框
		rectItem->setBrush(Qt::NoBrush); // 无填充
		rectItem->setZValue(-1); // 置于底层，避免遮挡游戏物体
		rectItem->setData(0, "QuadtreeBound"); // 标记为四叉树边界框
		scene->addItem(rectItem);
	}
}

void Quadtree::initialize(const QRectF& rect)
{
	clear();
	root_ = std::make_unique<QuadtreeNode>(rect);
}

void Quadtree::addItem(QGraphicsItem* item) const
{
	if (item)
		root_->insert(item);
}

std::vector<QGraphicsItem*> Quadtree::findItemSectional(const QRectF& rect)
{
	if (root_)
		return root_->query(rect);
	return {};
}

std::vector<QGraphicsItem*> Quadtree::findCollisions(const QGraphicsItem* item) const
{
	std::vector<QGraphicsItem*> collisions;

	if (!item) return collisions;

	QRectF itemBounds = item->boundingRect().translated(item->pos());

	// 查询可能碰撞的物体
	auto potentialCollisions = root_->query(itemBounds);

	// 检查实际碰撞
	for (QGraphicsItem* other : potentialCollisions)
	{
		if (other != item)
		{ // 排除自身
			QRectF otherBounds = other->boundingRect().translated(other->pos());
			// 添加容差以处理浮点数精度
			if (itemBounds.adjusted(-0.01, -0.01, 0.01, 0.01).intersects(otherBounds))
				collisions.push_back(other);
		}
	}

	//if (!collisions.empty())
	//	DebugInfo << __FUNCTION__;

	return collisions;
}

std::vector<QGraphicsItem*> Quadtree::findItemsAroundPoint(const QPointF& point) const
{
	if (!root_) return {};

	auto radius = Character::MaxRemoteRadius;
	// 创建一个以point为中心，边长为800的正方形区域（半径400像素）
	QRectF queryRect(point.x() - radius, point.y() - radius, radius * 2, radius * 2);
	return root_->query(queryRect);
}

void Quadtree::clear() const
{
	if (root_)
		root_->clear();
}
