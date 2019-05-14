#define _SCL_SECURE_NO_WARNINGS    //https://blog.csdn.net/cyh706510441/article/details/44757553

#include "csfwin.h"
#include <iostream>
#include "cloudviewer.h"
#pragma comment(lib, "CSF.lib") //v5
#pragma comment( lib, "ws2_32.lib")
QString globaliter;
QString globalri;
QString globalts;
int finish_para;  //1-finished 0-not yet
csfwin::csfwin(QWidget *parent)
	: QWidget(parent)
{
	
	ui.setupUi(this);
	connect(ui.para_push, SIGNAL(clicked()), this, SLOT(get_para()));
	connect(ui.para_default, SIGNAL(clicked()), this, SLOT(get_default()));
}

csfwin::~csfwin()
{
}

void csfwin::get_para()
{
	using namespace std;

	globaliter = ui.i_iter->text(); //获取lineEdit输入的值赋值给全局变量 MainWindow *w = new MainWindow; w->show(); 
	globalri = ui.i_rigidness->text();
	globalts = ui.i_timestep->text();
	finish_para = 1;
	cout << "";

	if ((((atof(globaliter.toStdString().c_str())< 0)) || ((atof(globalts.toStdString().c_str()) < 0))) || ((atof(globalri.toStdString().c_str()) < 0)))
	{

		QMessageBox::warning(NULL, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("你输入的包含负数，请重新输入"));
	}

	this->close();

}

void csfwin::get_default()
{
	using namespace std;
	globaliter = "100"; //获取lineEdit输入的值赋值给全局变量 MainWindow *w = new MainWindow; w->show(); 
	globalri = "3";
	globalts = "0.65";
	finish_para = 1;
	this->close();
}