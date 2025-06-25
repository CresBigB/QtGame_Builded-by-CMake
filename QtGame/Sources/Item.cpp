#include "Item.h"

Item* Item::createItem(const Type itemType, QGraphicsItem* parent, const QPixmap& image)
{
	Item* item = nullptr;

	if (itemType != Type::None)
	{
		QPixmap defaultImage;
		switch (itemType)
		{
		case Type::None:
			break;
		case Type::Knife:
			defaultImage = QPixmap(":/QtGame/Images/knife_item.png");
			break;
		case Type::Acceleration:
			defaultImage = QPixmap(":/QtGame/Images/Acceleration.png");
			break;
		case Type::Invincible:
			defaultImage = QPixmap(":/QtGame/Images/infinite.png");
			break;
		case Type::Heart:
			defaultImage = QPixmap(":/QtGame/Images/Heart.png");
			break;
		}
		if (defaultImage.isNull())
			defaultImage.fill("#c80fd9");
		defaultImage = defaultImage.scaled(ItemHeight, ItemWidth, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		//if (defaultImage == nullptr)
		//{
		//	DebugInfo << "Item image load failed";
		//	return nullptr;
		//}
		item = new Item(parent, defaultImage);
	}
	else
		item = new Item(parent, image);

	item->setType(itemType);
	return item;
}

Item::Item(QGraphicsItem* parent, QPixmap image) :image(std::move(image))
{
}

Item::Type Item::getType() const
{
	return this->itemType;
}

void Item::setType(const Type itemType)
{
	this->itemType = itemType;
}

QRectF Item::boundingRect() const
{
	return { 0,0,ItemHeight,ItemWidth };
}

void Item::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{

#ifdef  SHOW_BoundingRect
	painter->setPen(Qt::red);
	painter->drawRect(this->boundingRect());
#endif

	painter->save();

	auto rect = this->boundingRect();
	painter->drawPixmap(rect, this->image, rect);

	painter->restore();
}

void Item::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	DebugInfo << "Clicked Item Pos:" << this->pos();
}
