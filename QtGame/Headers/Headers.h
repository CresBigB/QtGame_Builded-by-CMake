#pragma once

#ifndef Headers_H
#define Headers_H

#include <QApplication>
#include <QMainWindow>
#include <QGraphicsItem>
#include <QGraphicsView>
#include <QMovie>
#include <QKeyEvent>
#include <QPropertyAnimation>
#include <QEvent>
#include <QShortcut>
#include <QTimer>
#include <QOpenGLWidget>
#include <QThread>
#include <QMessageBox>
#include <QPushButton>
#include <QGraphicsProxyWidget>
#include <QSettings>
#include <QHash>
#include <QRandomGenerator>
#include <QPropertyAnimation>

//#define SHOW_BoundingRect

#define DebugInfo qDebug().noquote()

namespace Tool
{
	inline qreal currentTime()
	{
		auto now = std::chrono::system_clock::now();
		auto currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
			now.time_since_epoch()
		).count();

		return currentTime;
	}
}


#endif
