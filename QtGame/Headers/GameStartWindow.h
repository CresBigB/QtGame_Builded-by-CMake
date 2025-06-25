#pragma once

#include "Headers.h"
#include "ui_GameWindow.h"

class GameStartWindow : public QMainWindow
{
	Q_OBJECT
public:
	GameStartWindow(QWidget* parent = nullptr);
	~GameStartWindow() override;

private:
	void saveSetting() const;
	void loadSetting() const;
	void acceptSetting() const;

public slots:
	void on_btnStart_clicked();
	void on_btnSetting_clicked();
	void on_btnBack_clicked();
	void on_btnAccept_clicked();
	void on_btnRestore_clicked();
	void on_btnExit_clicked();

	void gameOver();

private:
	Ui::QtGameClass ui;
};
