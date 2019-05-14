#pragma once

#include <QWidget>
#include "ui_csfwin.h"

class csfwin : public QWidget
{
	Q_OBJECT

public:
	csfwin(QWidget *parent = Q_NULLPTR);
	~csfwin();

private:
	Ui::csfwin ui;

private slots:
	void get_para();
	void get_default();
};
