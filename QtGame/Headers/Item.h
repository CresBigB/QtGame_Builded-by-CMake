#pragma once

#ifndef Item_H
#define Item_h

#include "Headers.h"

class Character;

class Item :public QGraphicsItem
{
public:
	enum class Type :qint8 { None, Knife, Acceleration, Invincible, Heart };
	inline static double ItemWidth = 60;
	inline static double ItemHeight = 60;
	static Item* createItem(const Type itemType, QGraphicsItem* parent = nullptr, const QPixmap& image = {});

	Item(const Item& other) = delete;
	Item& operator=(const Item& other) = delete;

	[[nodiscard]] Type getType() const;

protected:
	[[nodiscard]] QRectF boundingRect() const override;
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
	void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

	void setType(Type itemType);


protected:
	Item(QGraphicsItem* parent = nullptr, QPixmap image = {});
	QPixmap image;
	Type itemType = Type::None;
};

inline const static QEvent::Type CharacterPickUpItemEventType = static_cast<QEvent::Type>(QEvent::User + 6);
class CharacterPickUpItemEvent :public QEvent
{
public:
	using Type = Item::Type;
	explicit CharacterPickUpItemEvent(Character* character, Item* item)
		:QEvent(CharacterPickUpItemEventType), character_(character), pickedItem(item)
	{}

	[[nodiscard]] Character* character() const
	{
		return this->character_;
	}
	[[nodiscard]] Item* item() const
	{
		return this->pickedItem;
	}

protected:
	Character* character_;
	Item* pickedItem;
};

#endif
