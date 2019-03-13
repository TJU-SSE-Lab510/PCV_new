#define _SCL_SECURE_NO_WARNINGS    //https://blog.csdn.net/cyh706510441/article/details/44757553
#include "cloudviewer.h"
#pragma comment( lib, "ws2_32.lib")

CloudViewer::CloudViewer(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	/***** Slots connection of QMenuBar and QToolBar *****/
	// File (connect)
	QObject::connect(ui.openAction, &QAction::triggered, this, &CloudViewer::open);
	QObject::connect(ui.addAction, &QAction::triggered, this, &CloudViewer::add);
	QObject::connect(ui.clearAction, &QAction::triggered, this, &CloudViewer::clear);
	QObject::connect(ui.saveAction, &QAction::triggered, this, &CloudViewer::save);
	QObject::connect(ui.saveBinaryAction, &QAction::triggered, this, &CloudViewer::saveBinary);
	QObject::connect(ui.changeAction, &QAction::triggered, this, &CloudViewer::change);
	QObject::connect(ui.exitAction, &QAction::triggered, this, &CloudViewer::exit);
	// Display (connect)
	QObject::connect(ui.pointcolorAction, &QAction::triggered, this, &CloudViewer::pointcolorChanged);
	QObject::connect(ui.bgcolorAction, &QAction::triggered, this, &CloudViewer::bgcolorChanged);
	QObject::connect(ui.mainviewAction, &QAction::triggered, this, &CloudViewer::mainview);
	QObject::connect(ui.leftviewAction, &QAction::triggered, this, &CloudViewer::leftview);
	QObject::connect(ui.topviewAction, &QAction::triggered, this, &CloudViewer::topview);
	QObject::connect(ui.actionX, &QAction::triggered, this, &CloudViewer::Xchange);
	QObject::connect(ui.actionY, &QAction::triggered, this, &CloudViewer::Ychange);
	QObject::connect(ui.actionZ, &QAction::triggered, this, &CloudViewer::Zchange);
	// View (connect)
	QObject::connect(ui.dataAction, &QAction::triggered, this, &CloudViewer::data);
	QObject::connect(ui.propertyAction, &QAction::triggered, this, &CloudViewer::properties);
	QObject::connect(ui.consoleAction, &QAction::triggered, this, &CloudViewer::console);
	QObject::connect(ui.RGBAction, &QAction::triggered, this, &CloudViewer::rgbDock);
	// Generate (connect)
	QObject::connect(ui.cubeAction, &QAction::triggered, this, &CloudViewer::cube);
	QObject::connect(ui.sphereAction, &QAction::triggered, this, &CloudViewer::createSphere);
	QObject::connect(ui.cylinderAction, &QAction::triggered, this, &CloudViewer::createCylinder);
	// Process (connect)
	QObject::connect(ui.meshsurfaceAction, &QAction::triggered, this, &CloudViewer::convertSurface);
	QObject::connect(ui.wireframeAction, &QAction::triggered, this, &CloudViewer::convertWireframe);
	// Translation
	QObject::connect(ui.actiondsmAction, &QAction::triggered, this, &CloudViewer::todsm);
	QObject::connect(ui.actionactiondem, &QAction::triggered, this, &CloudViewer::todem);
	//Filter (connect)
	QObject::connect(ui.statisticalAction, &QAction::triggered, this, &CloudViewer::statisticalFilter);
	QObject::connect(ui.radiusAction, &QAction::triggered, this, &CloudViewer::radiusFilter);
	// Option (connect)
	QObject::connect(ui.windowsThemeAction, &QAction::triggered, this, &CloudViewer::windowsTheme);
	QObject::connect(ui.darculaThemeAction, &QAction::triggered, this, &CloudViewer::darculaTheme);
	QObject::connect(ui.englishAction, &QAction::triggered, this, &CloudViewer::langEnglish);
	QObject::connect(ui.chineseAction, &QAction::triggered, this, &CloudViewer::langChinese);
	//Operations (connect)
//	QObject::connect(ui.registerAction, &QAction::triggered, this, &CloudViewer::registering);
	// About (connect)
	QObject::connect(ui.aboutAction, &QAction::triggered, this, &CloudViewer::about);
	QObject::connect(ui.helpAction, &QAction::triggered, this, &CloudViewer::help);

	/***** Slots connection of RGB widget *****/
	// Random color (connect)
	connect(ui.colorBtn, SIGNAL(clicked()), this, SLOT(colorBtnPressed()));
	// Connection between RGB slider and RGB value (connect)
	connect(ui.rSlider, SIGNAL(valueChanged(int)), this, SLOT(rSliderChanged(int)));
	connect(ui.gSlider, SIGNAL(valueChanged(int)), this, SLOT(gSliderChanged(int)));
	connect(ui.bSlider, SIGNAL(valueChanged(int)), this, SLOT(bSliderChanged(int)));
	// RGB slider released (connect)
	connect(ui.rSlider, SIGNAL(sliderReleased()), this, SLOT(RGBsliderReleased()));
	connect(ui.gSlider, SIGNAL(sliderReleased()), this, SLOT(RGBsliderReleased()));
	connect(ui.bSlider, SIGNAL(sliderReleased()), this, SLOT(RGBsliderReleased()));
	// Change size of cloud (connect)
	connect(ui.pSlider, SIGNAL(valueChanged(int)), this, SLOT(pSliderChanged(int)));
	connect(ui.pSlider, SIGNAL(sliderReleased()), this, SLOT(psliderReleased()));
	// Checkbox for coordinate and background color (connect)
	connect(ui.cooCbx, SIGNAL(stateChanged(int)), this, SLOT(cooCbxChecked(int)));
	connect(ui.bgcCbx, SIGNAL(stateChanged(int)), this, SLOT(bgcCbxChecked(int)));

	/***** Slots connection of dataTree(QTreeWidget) widget *****/
	// Item in dataTree is left-clicked (connect)
	connect(ui.dataTree, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(itemSelected(QTreeWidgetItem*, int)));
	// Item in dataTree is right-clicked
	connect(ui.dataTree, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(popMenu(const QPoint&)));

	connect(ui.consoleTable, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(popMenuInConsole(const QPoint&)));
	// Initialization
	initial();
}

CloudViewer::~CloudViewer()
{

}

//global v -by rowlynn
string inputfile;
string inputroutine;

int makedir() //v2
{
	string folderPath = ".\\result";

	if (0 != access(folderPath.c_str(), 0))
	{
		// if this folder not exist, create a new one.
		mkdir(folderPath.c_str());   // ���� 0 ��ʾ�����ɹ���-1 ��ʾʧ��

	}
	return 0;
}

void CloudViewer::todem()
{
	//QMessageBox::information(this, "DEM", charfile);
	QMessageBox::information(this, "DEM", "The DEM file is in the result folder");
}

void CloudViewer::todsm()
{

	using namespace std;
	using namespace pdal;

	//get the filename and the routine
	char charfile[100] = {}; //the filename of the input
	string purename = inputfile.substr(0, inputfile.rfind(".")); //without the format:purename
	int isfile = 0;
	//create the result folder
	makedir();

	if (purename.empty())
	{
		isfile = 0;
		QMessageBox::information(this, "DSM", "You should open a file!");
	}
	else
	{
		isfile = 1;
		//translate the in-memory pcd into las and save it
		char strOutLasName[] = "temp.las";

		std::ofstream ofs(strOutLasName, ios::out | ios::binary);

		liblas::Header header;
		header.SetVersionMajor(1);
		header.SetVersionMinor(2);
		header.SetDataFormatId(liblas::PointFormatName::ePointFormat3);
		header.SetScale(0.01, 0.01, 0.01);  //slove the long double problem

											//дliblas,
		liblas::Writer writer(ofs, header);
		liblas::Point point(&header);

		for (size_t i = 0; i < mycloud.cloud->points.size(); i++)
		{
			long double x = mycloud.cloud->points[i].x;
			long double y = mycloud.cloud->points[i].y;
			long double z = mycloud.cloud->points[i].z;
			point.SetCoordinates(x, y, z);

			uint32_t red = (uint32_t)mycloud.cloud->points[i].r;
			uint32_t green = (uint32_t)mycloud.cloud->points[i].g;
			uint32_t blue = (uint32_t)mycloud.cloud->points[i].b;

			liblas::Color pointColor(red, green, blue);
			point.SetColor(pointColor);
			writer.WritePoint(point);
		}
		long double minPt[3] = { 9999999, 9999999, 9999999 };
		long double maxPt[3] = { 0, 0, 0 };
		header.SetPointRecordsCount(mycloud.cloud->points.size());
		header.SetPointRecordsByReturnCount(0, mycloud.cloud->points.size());
		header.SetMax(maxPt[0], maxPt[1], maxPt[2]);
		header.SetMin(minPt[0], minPt[1], minPt[2]);
		writer.SetHeader(header);

	}

	if (isfile == 1) //the las file will be broken if we don' t use the 'isfile' judge number
	{
		//CopyFile(L"temp.las", L"temp2.las", FALSE);
		PipelineManager mgr;
		std::stringstream ss2;
		//the DSM will be blank if resolution is too big (eg. > 0.2)
		ss2 << R"({"pipeline":["temp.las",{"resolution": 0.19,"radius": 0.6,"filename":".\\result\\)" << purename << R"(-DSM.tif"}]})";
		mgr.readPipeline(ss2);
		mgr.execute();
		remove("temp.las");
		QMessageBox::information(this, "DSM", "The DSM file is in the result folder");
	}
}

void CloudViewer::Xchange() {
	float highest_height = INT_MIN;
	float lowest_height = INT_MAX;
	for (int i = 0; i != mycloud.cloud->points.size(); i++) {
		if (mycloud.cloud->points[i].x > highest_height) {
			highest_height = mycloud.cloud->points[i].x;
		}
		if (mycloud.cloud->points[i].x < lowest_height) {
			lowest_height = mycloud.cloud->points[i].x;
		}
	}
	for (int i = 0; i != mycloud.cloud->points.size(); i++) {
		mycloud.cloud->points[i].r = ceil((mycloud.cloud->points[i].x - lowest_height) / (highest_height - lowest_height)*(float)256);
		mycloud.cloud->points[i].g = 0;
		mycloud.cloud->points[i].b = ceil((1 - (mycloud.cloud->points[i].x - lowest_height) / (highest_height - lowest_height))*(float)256);
	}
	showPointcloud();
}
void CloudViewer::Ychange() {

	float highest_height = INT_MIN;
	float lowest_height = INT_MAX;
	for (int i = 0; i != mycloud.cloud->points.size(); i++) {
		if (mycloud.cloud->points[i].y > highest_height) {
			highest_height = mycloud.cloud->points[i].y;
		}
		if (mycloud.cloud->points[i].y < lowest_height) {
			lowest_height = mycloud.cloud->points[i].y;
		}
	}
	for (int i = 0; i != mycloud.cloud->points.size(); i++) {
		mycloud.cloud->points[i].r = ceil((mycloud.cloud->points[i].y - lowest_height) / (highest_height - lowest_height)*(float)256);
		mycloud.cloud->points[i].g = 0;
		mycloud.cloud->points[i].b = ceil((1 - (mycloud.cloud->points[i].y - lowest_height) / (highest_height - lowest_height))*(float)256);
	}

	showPointcloud();
}
void CloudViewer::Zchange() {

	float highest_height = INT_MIN;
	float lowest_height = INT_MAX;
	for (int i = 0; i != mycloud.cloud->points.size(); i++) {
		if (mycloud.cloud->points[i].z > highest_height) {
			highest_height = mycloud.cloud->points[i].z;
		}
		if (mycloud.cloud->points[i].z < lowest_height) {
			lowest_height = mycloud.cloud->points[i].z;
		}
	}
	for (int i = 0; i != mycloud.cloud->points.size(); i++) {
		mycloud.cloud->points[i].r = ceil((mycloud.cloud->points[i].z - lowest_height) / (highest_height - lowest_height)*(float)256);
		mycloud.cloud->points[i].g = 0;
		mycloud.cloud->points[i].b = ceil((1 - (mycloud.cloud->points[i].z - lowest_height) / (highest_height - lowest_height))*(float)256);
	}
	showPointcloud();
}

// Open point cloud
void CloudViewer::open()
{

	QStringList filenames = QFileDialog::getOpenFileNames(this, tr("Open point cloud file"), QString::fromLocal8Bit(mycloud.dirname.c_str()), tr("Point cloud data(*.pcd *.ply *.obj *.las);;All file(*.*)"));
	//Return if filenames is empty
	if (filenames.isEmpty())
		return;

	// Clear cache
	mycloud_vec.clear();
	total_points = 0;
	ui.dataTree->clear();
	viewer->removeAllPointClouds();

	// Open point cloud one by one
	for (int i = 0; i != filenames.size(); i++) {
		// time start
		timeStart();
		mycloud.cloud.reset(new PointCloudT); // Reset cloud
		QString filename = filenames[i];
		std::string file_name = filename.toStdString();
		std::string subname = getFileName(file_name);  //��ȡȫ·���е��ļ���������׺��
	    //change the global v - rowlynn
		inputfile = subname;
		inputroutine = file_name;
		//����״̬��
		ui.statusBar->showMessage(QString::fromLocal8Bit(subname.c_str()) + ": " + QString::number(i) + "/" + QString::number(filenames.size()) + " point cloud loading...");

		int status = -1;
		if (filename.endsWith(".pcd", Qt::CaseInsensitive))
		{
			status = pcl::io::loadPCDFile(file_name, *(mycloud.cloud));
			if (mycloud.cloud->points[0].r == 0 && mycloud.cloud->points[0].g == 0 && mycloud.cloud->points[0].b == 0)
			{
				setCloudColor(255, 255, 255);
			}
		}
		else if (filename.endsWith(".ply", Qt::CaseInsensitive))
		{
			status = pcl::io::loadPLYFile(file_name, *(mycloud.cloud));
			if (mycloud.cloud->points[0].r == 0 && mycloud.cloud->points[0].g == 0 && mycloud.cloud->points[0].b == 0)
			{
				setCloudColor(255, 255, 255);
			}
		}
		else if (filename.endsWith(".obj", Qt::CaseInsensitive))
		{
			status = pcl::io::loadOBJFile(file_name, *(mycloud.cloud));
			if (mycloud.cloud->points[0].r == 0 && mycloud.cloud->points[0].g == 0 && mycloud.cloud->points[0].b == 0)
			{
				setCloudColor(255, 255, 255);
			}
		}
		else if (filename.endsWith(".las", Qt::CaseInsensitive))
		{
			// Opening  the las file
			std::ifstream ifs(file_name.c_str(), std::ios::in | std::ios::binary);
			liblas::ReaderFactory f;
			liblas::Reader reader = f.CreateWithStream(ifs); // reading las file
			unsigned long int nbPoints = reader.GetHeader().GetPointRecordsCount();

			// Fill in the cloud data
			mycloud.cloud->width = nbPoints;				// This means that the point cloud is "unorganized"
			mycloud.cloud->height = 1;						// (i.e. not a depth map)
			mycloud.cloud->is_dense = false;
			mycloud.cloud->points.resize(mycloud.cloud->width * mycloud.cloud->height);

			int i = 0;				// counter
			uint16_t r1, g1, b1;	// RGB variables for .las (16-bit coded)
			int r2, g2, b2;			// RGB variables for converted values (see below)

			while (reader.ReadNextPoint())
			{
				// get XYZ information
				mycloud.cloud->points[i].x = (reader.GetPoint().GetX());
				mycloud.cloud->points[i].y = (reader.GetPoint().GetY());
				mycloud.cloud->points[i].z = (reader.GetPoint().GetZ());

				// get RGB information. Note: in liblas, the "Color" class can be accessed from within the "Point" class, thus the triple gets
				r1 = (reader.GetPoint().GetColor().GetRed());
				g1 = (reader.GetPoint().GetColor().GetGreen());
				b1 = (reader.GetPoint().GetColor().GetBlue());

				// .las stores RGB color in 16-bit (0-65535) while .pcd demands an 8-bit value (0-255). Let's convert them!
				mycloud.cloud->points[i].r = ceil(((float)r1 / 65536)*(float)256);
				mycloud.cloud->points[i].g = ceil(((float)g1 / 65536)*(float)256);
				mycloud.cloud->points[i].b = ceil(((float)b1 / 65536)*(float)256);

				i++; // ...moving on
				status = 0;
			}
		}
		else
		{
			//��ʾ���޷���ȡ����.ply .pcd .obj������ļ�
			QMessageBox::information(this, tr("File format error"),
				tr("Can't open files except .ply .pcd .obj .las"));
			return;
		}
		//��ʾ����׺û���⣬���ļ������޷���ȡ
		if (status != 0)
		{
			QMessageBox::critical(this, tr("Reading file error"), tr("We can not open the file"));
			return;
		}
		setA(255);  //���õ���Ϊ��͸��
					// �����ĵ��Ƶ���Ϣ
		mycloud.filename = file_name;
		mycloud.subname = subname;
		mycloud.dirname = file_name.substr(0, file_name.size() - subname.size());
		mycloud_vec.push_back(mycloud);  //�����Ƶ����������


										 // time off
		time_cost = timeOff();
		// �������
		consoleLog("Open", QString::fromLocal8Bit(mycloud.subname.c_str()), QString::fromLocal8Bit(mycloud.filename.c_str()), "Time cost: " + time_cost + " s, Points: " + QString::number(mycloud.cloud->points.size()));

		//������Դ������
		QTreeWidgetItem *cloudName = new QTreeWidgetItem(QStringList()
			<< QString::fromLocal8Bit(subname.c_str()));
		cloudName->setIcon(0, QIcon(":/Resources/images/icon.png"));
		ui.dataTree->addTopLevelItem(cloudName);

		//setWindowTitle(filename + " - CloudViewer"); //���±���

		total_points += mycloud.cloud->points.size();
	}
	ui.statusBar->showMessage("");
	showPointcloudAdd();  //������ͼ����
	setPropertyTable();

}

// Add Point Cloud
void CloudViewer::add()
{
	QStringList filenames = QFileDialog::getOpenFileNames(this, tr("Open point cloud file"), QString::fromLocal8Bit(mycloud.dirname.c_str()), tr("Point cloud data(*.pcd *.ply *.obj);;All file(*.*)"));
	if (filenames.isEmpty())
		return;
	for (int i = 0; i != filenames.size(); i++) {
		// time start
		timeStart();
		mycloud.cloud.reset(new PointCloudT);
		QString filename = filenames[i];
		std::string file_name = filename.toStdString();
		std::string subname = getFileName(file_name);

		// ����״̬��
		ui.statusBar->showMessage(QString::fromLocal8Bit(subname.c_str()) + ": " + QString::number(i) + "/" + QString::number(filenames.size()) + " point cloud loading...");

		int status = -1;
		if (filename.endsWith(".pcd", Qt::CaseInsensitive))
		{
			status = pcl::io::loadPCDFile(file_name, *(mycloud.cloud));
			if (mycloud.cloud->points[0].r == 0 && mycloud.cloud->points[0].g == 0 && mycloud.cloud->points[0].b == 0)
			{
				setCloudColor(255, 255, 255);
			}
		}
		else if (filename.endsWith(".ply", Qt::CaseInsensitive))
		{
			status = pcl::io::loadPLYFile(file_name, *(mycloud.cloud));
			if (mycloud.cloud->points[0].r == 0 && mycloud.cloud->points[0].g == 0 && mycloud.cloud->points[0].b == 0)
			{
				setCloudColor(255, 255, 255);
			}
		}
		else if (filename.endsWith(".obj", Qt::CaseInsensitive))
		{
			status = pcl::io::loadOBJFile(file_name, *(mycloud.cloud));
			if (mycloud.cloud->points[0].r == 0 && mycloud.cloud->points[0].g == 0 && mycloud.cloud->points[0].b == 0)
			{
				setCloudColor(255, 255, 255);
			}
		}
		else
		{
			//��ʾ���޷���ȡ����.ply .pcd .obj������ļ�
			QMessageBox::information(this, tr("File format error"), tr("Can't open files except .ply .pcd .obj"));
			return;
		}
		//��ʾ����׺û���⣬���ļ������޷���ȡ
		if (status != 0)
		{
			QMessageBox::critical(this, tr("Reading file error"), tr("We can not open the file"));
			return;
		}
		setA(255);  //���õ���Ϊ��͸��
		mycloud.filename = file_name;
		mycloud.subname = subname;
		mycloud.dirname = file_name.substr(0, file_name.size() - subname.size());
		mycloud_vec.push_back(mycloud);  //�����Ƶ����������

										 // time of
		time_cost = timeOff();
		//�������
		consoleLog("Add", QString::fromLocal8Bit(mycloud.subname.c_str()), QString::fromLocal8Bit(mycloud.filename.c_str()), "Time cost: " + time_cost + " s, Points: " + QString::number(mycloud.cloud->points.size()));

		//������Դ������
		QTreeWidgetItem *cloudName = new QTreeWidgetItem(QStringList() << QString::fromLocal8Bit(subname.c_str()));
		cloudName->setIcon(0, QIcon(":/Resources/images/icon.png"));
		ui.dataTree->addTopLevelItem(cloudName);

		//setWindowTitle("CloudViewer");
		total_points += mycloud.cloud->points.size();
	}
	ui.statusBar->showMessage("");
	showPointcloudAdd();
	setPropertyTable();

}

// Clear all point clouds
void CloudViewer::clear()
{
	mycloud_vec.clear();  //�ӵ����������Ƴ����е���
	viewer->removeAllPointClouds();  //��viewer���Ƴ����е���
	viewer->removeAllShapes(); //���remove������
	ui.dataTree->clear();  //��dataTree���

	ui.propertyTable->clear();  //������Դ���propertyTable
	QStringList header;
	header << "Property" << "Value";
	ui.propertyTable->setHorizontalHeaderLabels(header);

	//�������
	consoleLog("Clear", "All point clouds", "", "");

	setWindowTitle("CloudViewer");  //���´��ڱ���
	showPointcloud();  //������ʾ
}


// Save point cloud
void CloudViewer::save()
{
	save_filename = QFileDialog::getSaveFileName(this, tr("Save point cloud"),
		QString::fromLocal8Bit(mycloud.dirname.c_str()), tr("Point cloud data(*.pcd *.ply);;Allfile(*.*)"));
	std::string file_name = save_filename.toStdString();
	std::string subname = getFileName(file_name);
	//�ļ���Ϊ��ֱ�ӷ���
	if (save_filename.isEmpty())
		return;

	if (mycloud_vec.size() > 1)
	{
		savemulti();
		return;
	}

	int status = -1;
	if (save_filename.endsWith(".pcd", Qt::CaseInsensitive))
	{
		status = pcl::io::savePCDFile(file_name, *(mycloud.cloud));
	}
	else if (save_filename.endsWith(".ply", Qt::CaseInsensitive))
	{
		status = pcl::io::savePLYFile(file_name, *(mycloud.cloud));
	}
	else //��ʾ���޷�����Ϊ����.ply .pcd������ļ�
	{
		QMessageBox::information(this, tr("File format error"),
			tr("Can't save files except .ply .pcd"));
		return;
	}
	//��ʾ����׺û���⣬�����޷�����
	if (status != 0)
	{
		QMessageBox::critical(this, tr("Saving file error"),
			tr("We can not save the file"));
		return;
	}

	//�������
	consoleLog("Save", QString::fromLocal8Bit(subname.c_str()), save_filename, "Single save");

	setWindowTitle(save_filename + " - CloudViewer");
	QMessageBox::information(this, tr("save point cloud file"),
		QString::fromLocal8Bit(("Save " + subname + " successfully!").c_str()));
}

// Save point cloud as binary file
void CloudViewer::saveBinary()
{
	save_filename = QFileDialog::getSaveFileName(this, tr("Save point cloud as binary file"),
		QString::fromLocal8Bit(mycloud.dirname.c_str()), tr("Point cloud data(*.pcd *.ply);;Allfile(*.*)"));
	std::string file_name = save_filename.toStdString();
	std::string subname = getFileName(file_name);
	//�ļ���Ϊ��ֱ�ӷ���
	if (save_filename.isEmpty())
		return;

	if (mycloud_vec.size() > 1)
	{
		savemulti();
		return;
	}

	int status = -1;
	if (save_filename.endsWith(".pcd", Qt::CaseInsensitive))
	{
		status = pcl::io::savePCDFileBinary(file_name, *(mycloud.cloud));
	}
	else if (save_filename.endsWith(".ply", Qt::CaseInsensitive))
	{
		status = pcl::io::savePLYFileBinary(file_name, *(mycloud.cloud));
	}
	else //��ʾ���޷�����Ϊ����.ply .pcd������ļ�
	{
		QMessageBox::information(this, tr("File format error"),
			tr("Can't save files except .ply .pcd"));
		return;
	}
	//��ʾ����׺û���⣬�����޷�����
	if (status != 0)
	{
		QMessageBox::critical(this, tr("Saving file error"),
			tr("We can not save the file"));
		return;
	}

	//�������
	consoleLog("Save as binary", QString::fromLocal8Bit(subname.c_str()), save_filename, "Single save (binary)");

	setWindowTitle(save_filename + " - CloudViewer");
	QMessageBox::information(this, tr("save point cloud file"),
		QString::fromLocal8Bit(("Save " + subname + " successfully!").c_str()));
}


// Save multi point cloud
void CloudViewer::savemulti()
{
	std::string subname = getFileName(save_filename.toStdString());
	PointCloudT::Ptr multi_cloud;
	multi_cloud.reset(new PointCloudT);
	multi_cloud->height = 1;
	int sum = 0;
	for (auto c : mycloud_vec)
	{
		sum += c.cloud->points.size();
	}
	multi_cloud->width = sum;
	multi_cloud->resize(multi_cloud->height * multi_cloud->width);
	int k = 0;
	for (int i = 0; i != mycloud_vec.size(); i++)
	{
		for (int j = 0; j != mycloud_vec[i].cloud->points.size(); j++)          //ע��cloudvec[i]->points.size()��cloudvec[i]->size()������
		{
			multi_cloud->points[k].x = mycloud_vec[i].cloud->points[j].x;
			multi_cloud->points[k].y = mycloud_vec[i].cloud->points[j].y;
			multi_cloud->points[k].z = mycloud_vec[i].cloud->points[j].z;
			multi_cloud->points[k].r = mycloud_vec[i].cloud->points[j].r;
			multi_cloud->points[k].g = mycloud_vec[i].cloud->points[j].g;
			multi_cloud->points[k].b = mycloud_vec[i].cloud->points[j].b;
			k++;
		}
	}
	//����multi_cloud
	int status = -1;
	if (save_filename.endsWith(".pcd", Qt::CaseInsensitive))
	{
		if (save_as_binary) {
			status = pcl::io::savePCDFileBinary(save_filename.toStdString(), *multi_cloud);
		}
		else {
			status = pcl::io::savePCDFile(save_filename.toStdString(), *multi_cloud);
		}

	}
	else if (save_filename.endsWith(".ply", Qt::CaseInsensitive))
	{
		if (save_as_binary) {
			status = pcl::io::savePLYFileBinary(save_filename.toStdString(), *multi_cloud);
		}
		else {
			status = pcl::io::savePLYFile(save_filename.toStdString(), *multi_cloud);
		}
	}
	else //��ʾ���޷�����Ϊ����.ply .pcd������ļ�
	{
		QMessageBox::information(this, tr("File format error"), tr("Can't save files except .ply .pcd"));
		return;
	}

	//��ʾ����׺û���⣬�����޷�����
	if (status != 0)
	{
		QMessageBox::critical(this, tr("Saving file error"), tr("We can not save the file"));
		return;
	}

	// �������
	if (save_as_binary) {
		consoleLog("Save as binary", QString::fromLocal8Bit(subname.c_str()), save_filename, "Multi save (binary)");
	}
	else {
		consoleLog("Save", QString::fromLocal8Bit(subname.c_str()), save_filename, "Multi save");
	}


	save_as_binary = false;
	//�������� multi_cloud ����Ϊ��ǰ mycloud,�Ա㱣��֮��ֱ�ӽ��в���
	mycloud.cloud = multi_cloud;
	mycloud.filename = save_filename.toStdString();
	mycloud.subname = subname;

	setWindowTitle(save_filename + " - CloudViewer");
	QMessageBox::information(this, tr("save point cloud file"), QString::fromLocal8Bit(("Save " + subname + " successfully!").c_str()));
}




//��ʽת��
void CloudViewer::change()
{

}

//�˳�����
void CloudViewer::exit()
{
	this->close();
}

// Generate cube
void CloudViewer::cube()
{
	mycloud.cloud.reset(new PointCloudT);
	total_points = 0;
	ui.dataTree->clear();  //�����Դ��������item
	viewer->removeAllPointClouds();  //��viewer���Ƴ����е���
	mycloud_vec.clear();  //��յ�������

	mycloud.cloud->width = 50000;         // ���õ��ƿ�
	mycloud.cloud->height = 1;            // ���õ��Ƹߣ���Ϊ1��˵��Ϊ����֯����
	mycloud.cloud->is_dense = false;
	mycloud.cloud->resize(mycloud.cloud->width * mycloud.cloud->height);     // ���õ��ƴ�С
	for (size_t i = 0; i != mycloud.cloud->size(); ++i)
	{
		mycloud.cloud->points[i].x = 1024 * rand() / (RAND_MAX + 1.0f);
		mycloud.cloud->points[i].y = 1024 * rand() / (RAND_MAX + 1.0f);
		mycloud.cloud->points[i].z = 1024 * rand() / (RAND_MAX + 1.0f);
		mycloud.cloud->points[i].r = red;
		mycloud.cloud->points[i].g = green;
		mycloud.cloud->points[i].b = blue;
	}
	//������Դ������
	QTreeWidgetItem *cloudName = new QTreeWidgetItem(QStringList() << QString::fromLocal8Bit("cube"));
	cloudName->setIcon(0, QIcon(":/Resources/images/icon.png"));
	ui.dataTree->addTopLevelItem(cloudName);

	// �������
	consoleLog("Generate cube", "cube", "cube", "");

	mycloud_vec.push_back(mycloud);
	showPointcloudAdd();
}

//��ʼ��
void CloudViewer::initial()
{
	//�����ʼ��
	setWindowIcon(QIcon(tr(":/Resources/images/icon.png")));
	setWindowTitle(tr("CloudViewer"));

	//���Ƴ�ʼ��
	mycloud.cloud.reset(new PointCloudT);
	mycloud.cloud->resize(1);
	viewer.reset(new pcl::visualization::PCLVisualizer("viewer", false));
	//viewer->addPointCloud(cloud, "cloud");

	ui.screen->SetRenderWindow(viewer->getRenderWindow());
	viewer->setupInteractor(ui.screen->GetInteractor(), ui.screen->GetRenderWindow());
	ui.screen->update();

	ui.propertyTable->setSelectionMode(QAbstractItemView::NoSelection); // ��ֹ������Թ������� item
	ui.consoleTable->setSelectionMode(QAbstractItemView::NoSelection);  // ��ֹ���������ڵ� item
	ui.dataTree->setSelectionMode(QAbstractItemView::ExtendedSelection); // ���� dataTree ���ж�ѡ

																		 // ����Ĭ������
	QString qss = darcula_qss;
	qApp->setStyleSheet(qss);

	setPropertyTable();
	setConsoleTable();

	// �������
	consoleLog("Software start", "CloudViewer", "Welcome to use CloudViewer", "TJ510");


	// ���ñ�����ɫΪ dark
	viewer->setBackgroundColor(30 / 255.0, 30 / 255.0, 30 / 255.0);

}

//��ʾ���ƣ�����������Ƕ�
void CloudViewer::showPointcloud()
{
	for (int i = 0; i != mycloud_vec.size(); i++)
	{
		viewer->updatePointCloud(mycloud_vec[i].cloud, "cloud" + QString::number(i).toStdString());
	}
	//viewer->resetCamera();
	ui.screen->update();
}

//���ӵ��Ƶ�viewer,����ʾ����
void CloudViewer::showPointcloudAdd()
{
	for (int i = 0; i != mycloud_vec.size(); i++)
	{
		viewer->addPointCloud(mycloud_vec[i].cloud, "cloud" + QString::number(i).toStdString());
		viewer->updatePointCloud(mycloud_vec[i].cloud, "cloud" + QString::number(i).toStdString());
	}
	viewer->resetCamera();
	ui.screen->update();
}

void CloudViewer::setCloudColor(unsigned int r, unsigned int g, unsigned int b)
{
	// Set the new color
	for (size_t i = 0; i < mycloud.cloud->size(); i++)
	{
		mycloud.cloud->points[i].r = r;
		mycloud.cloud->points[i].g = g;
		mycloud.cloud->points[i].b = b;
		mycloud.cloud->points[i].a = 255;
	}
}

void CloudViewer::setA(unsigned int a)
{
	for (size_t i = 0; i < mycloud.cloud->size(); i++)
	{
		mycloud.cloud->points[i].a = a;
	}
}

//����
void CloudViewer::about()
{
	AboutWin *aboutwin = new AboutWin(this);
	aboutwin->setModal(true);
	aboutwin->show();

	// �������
	consoleLog("About", "TJ-510", "TJ510", "TJ510");
}

//����
void CloudViewer::help()
{
	QDesktopServices::openUrl(QUrl(QLatin1String("http://www.baidu.com")));

	// �������
	consoleLog("Help", "Cloudviewer help", "none", "none");

	//QMessageBox::information(this, "Help", "we are building help widget...");
}

//�����㷨
//ͳ���˲�
//void CloudViewer::statistical(){}

//����ͣ�����ڵ���ʾ������
void CloudViewer::data()
{
	if (ui.dataAction->isChecked())
	{
		ui.dataDock->setVisible(true);
	}
	else
	{
		ui.dataDock->setVisible(false);
	}
}
void CloudViewer::properties()
{
	if (ui.propertyAction->isChecked())
	{
		ui.propertyDock->setVisible(true);
	}
	else
	{
		ui.propertyDock->setVisible(false);
	}
}
void CloudViewer::console()
{
	if (ui.consoleAction->isChecked())
	{
		ui.consoleDock->setVisible(true);
	}
	else
	{
		ui.consoleDock->setVisible(false);
	}
}
void CloudViewer::rgbDock()
{
	if (ui.RGBAction->isChecked())
	{
		ui.RGBDock->setVisible(true);
	}
	else
	{
		ui.RGBDock->setVisible(false);
	}
}

//���ƻ���ͼ��
void CloudViewer::createSphere()
{
	mycloud.cloud.reset(new PointCloudT);
	ui.dataTree->clear();  //�����Դ��������item
	viewer->removeAllShapes();
	mycloud_vec.clear();  //��յ�������

	pcl::PointXYZ p;
	p.x = 0; p.y = 0; p.z = 0;
	viewer->addSphere(p, 100, "sphere1");

	viewer->resetCamera();
	ui.screen->update();

	// �������
	consoleLog("Create sphere", "Sphere", "", "Succeeded");
}
void CloudViewer::createCylinder()
{
	mycloud.cloud.reset(new PointCloudT);
	ui.dataTree->clear();  //�����Դ��������item
	viewer->removeAllShapes();
	mycloud_vec.clear();  //��յ�������

	viewer->addCylinder(*(new pcl::ModelCoefficients()), "cylinder");

	viewer->resetCamera();
	ui.screen->update();

	// �������
	consoleLog("Create cylinder", "Cylinder", "", "Failed");

}

// Change theme: Windows/Darcula
void CloudViewer::windowsTheme() {
	/*
	QFile qssFile("Resources/qss/Windows.qss"); //��Դ�ļ�":/Darcula.qss"
	qssFile.open(QFile::ReadOnly);
	if (qssFile.isOpen())
	{
	QString qss = QLatin1String(qssFile.readAll());
	//consoleLog("", "", "", qss);
	qApp->setStyleSheet(qss);
	qssFile.close();
	}
	*/
	QString qss = windows_qss;
	qApp->setStyleSheet(qss);

	//�ı� dataTree ������ɫ������Ӧ����ı��취
	QColor light_color(241, 241, 241, 255);
	QColor dark_color(0, 0, 0, 255);
	for (int i = 0; i != mycloud_vec.size(); i++) {
		if (ui.dataTree->topLevelItem(i)->textColor(0) == light_color) {
			ui.dataTree->topLevelItem(i)->setTextColor(0, dark_color);
		}
	}

	// �������
	consoleLog("Change theme", "Windows theme", "", "");

	theme_id = 0;
}
void CloudViewer::darculaTheme() {
	/*
	QFile qssFile("Resources/qss/Darcula.qss"); //��Դ�ļ�":/Darcula.qss"
	qssFile.open(QFile::ReadOnly);
	if (qssFile.isOpen())
	{
	QString qss = QLatin1String(qssFile.readAll());
	//cout << qss.toStdString();
	consoleLog("", "", "", qss);
	qApp->setStyleSheet(qss);
	qssFile.close();
	}
	*/

	QString qss = darcula_qss;
	qApp->setStyleSheet(qss);

	//�ı� dataTree ������ɫ������Ӧ����ı��취
	QColor light_color(241, 241, 241, 255);
	QColor dark_color(0, 0, 0, 255);
	for (int i = 0; i != mycloud_vec.size(); i++) {
		if (ui.dataTree->topLevelItem(i)->textColor(0) == dark_color) {
			ui.dataTree->topLevelItem(i)->setTextColor(0, light_color);
		}
	}

	// �������
	consoleLog("Change theme", "Darcula theme", "", "");

	theme_id = 1;
}
// Change language: English/Chinese
void CloudViewer::langEnglish() {
	// �������
	consoleLog("Change language", "English", "", "");
}
void CloudViewer::langChinese() {
	// �������
	consoleLog("Change language", "Chinese", "Doesn't support Chinese temporarily", "");
}



/*********************************************/
/*****************����ۺ���*****************/
/********************************************/
void CloudViewer::colorBtnPressed()
{
	QList<QTreeWidgetItem*> itemList = ui.dataTree->selectedItems();
	int selected_item_count = ui.dataTree->selectedItems().size();
	// ���δѡ���κε��ƣ������ͼ�����е����е��ƽ�����ɫ
	if (selected_item_count == 0) {
		for (int i = 0; i != mycloud_vec.size(); i++) {
			for (int j = 0; j != mycloud_vec[i].cloud->points.size(); j++) {
				mycloud_vec[i].cloud->points[j].r = 255 * (1024 * rand() / (RAND_MAX + 1.0f));
				mycloud_vec[i].cloud->points[j].g = 255 * (1024 * rand() / (RAND_MAX + 1.0f));
				mycloud_vec[i].cloud->points[j].b = 255 * (1024 * rand() / (RAND_MAX + 1.0f));
			}
		}

		// �������
		consoleLog("Random color", "All point clous", "", "");

	}
	else {
		for (int i = 0; i != selected_item_count; i++) {
			int cloud_id = ui.dataTree->indexOfTopLevelItem(itemList[i]);
			for (int j = 0; j != mycloud_vec[cloud_id].cloud->size(); j++) {
				mycloud_vec[cloud_id].cloud->points[j].r = red;
				mycloud_vec[cloud_id].cloud->points[j].g = 255 * (1024 * rand() / (RAND_MAX + 1.0f));
				mycloud_vec[cloud_id].cloud->points[j].b = 255 * (1024 * rand() / (RAND_MAX + 1.0f));
			}
		}

		// �������
		consoleLog("Random color", "Point clouds selected", "", "");
	}

	showPointcloud();
}

void CloudViewer::RGBsliderReleased()
{
	QList<QTreeWidgetItem*> itemList = ui.dataTree->selectedItems();
	int selected_item_count = ui.dataTree->selectedItems().size();
	// ���δѡ���κε��ƣ������ͼ�����е����е��ƽ�����ɫ
	if (selected_item_count == 0) {
		for (int i = 0; i != mycloud_vec.size(); i++) {
			for (int j = 0; j != mycloud_vec[i].cloud->points.size(); j++) {
				mycloud_vec[i].cloud->points[j].r = red;
				mycloud_vec[i].cloud->points[j].g = green;
				mycloud_vec[i].cloud->points[j].b = blue;
			}
		}

		// �������
		consoleLog("Change cloud color", "All point clouds", QString::number(red) + " " + QString::number(green) + " " + QString::number(blue), "");
	}
	else {
		for (int i = 0; i != selected_item_count; i++) {
			int cloud_id = ui.dataTree->indexOfTopLevelItem(itemList[i]);
			for (int j = 0; j != mycloud_vec[cloud_id].cloud->size(); j++) {
				mycloud_vec[cloud_id].cloud->points[j].r = red;
				mycloud_vec[cloud_id].cloud->points[j].g = green;
				mycloud_vec[cloud_id].cloud->points[j].b = blue;
			}
		}
		// �������
		consoleLog("Change cloud color", "Point clouds selected", QString::number(red) + " " + QString::number(green) + " " + QString::number(blue), "");
	}
	showPointcloud();
}

//�������е��Ƶĳߴ�
void CloudViewer::psliderReleased()
{
	QList<QTreeWidgetItem*> itemList = ui.dataTree->selectedItems();
	int selected_item_count = ui.dataTree->selectedItems().size();
	if (selected_item_count == 0) {
		for (int i = 0; i != mycloud_vec.size(); i++) {
			mycloud_vec[i].pointSize = p;
			viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE,
				p, "cloud" + QString::number(i).toStdString());
		}
		// �������
		consoleLog("Change cloud size", "All point clouds", "Size: " + QString::number(p), "");
	}
	else {
		for (int i = 0; i != selected_item_count; i++) {
			int cloud_id = ui.dataTree->indexOfTopLevelItem(itemList[i]);
			mycloud_vec[cloud_id].pointSize = p;
			viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE,
				p, "cloud" + QString::number(cloud_id).toStdString());
		}
		// �������
		consoleLog("Change cloud size", "Point clouds selected", "Size: " + QString::number(p), "");
	}
	ui.screen->update();
}

void CloudViewer::pSliderChanged(int value)
{
	p = value;
	ui.pSlider->setValue(value);
	ui.sizeLCD->display(value);

}
void CloudViewer::rSliderChanged(int value)
{
	red = value;
	ui.rSlider->setValue(value);
	ui.rLCD->display(value);
}
void CloudViewer::gSliderChanged(int value)
{
	green = value;
	ui.gSlider->setValue(value);
	ui.gLCD->display(value);
}
void CloudViewer::bSliderChanged(int value)
{
	blue = value;
	ui.bSlider->setValue(value);
	ui.bLCD->display(value);
}

void CloudViewer::cooCbxChecked(int value)
{
	switch (value)
	{
	case 0:
		viewer->removeCoordinateSystem();  //�Ƴ�����ϵ
										   // �������
		consoleLog("Remove coordinate system", "Remove", "", "");
		break;
	case 2:
		viewer->addCoordinateSystem();  //��������ϵ
										// �������
		consoleLog("Add coordinate system", "Add", "", "");
		break;
	}
	//viewer->updatePointCloud(cloud, "cloud");
	ui.screen->update();
}
void CloudViewer::bgcCbxChecked(int value)
{
	switch (value)
	{
	case 0:
		viewer->setBackgroundColor(30 / 255.0, 30 / 255.0, 30 / 255.0);
		// �������
		consoleLog("Change bg color", "Background", "30 30 30", "");
		break;
	case 2:
		//��ע�⣺setBackgroundColor()���յ���0-1��double�Ͳ���
		viewer->setBackgroundColor(240 / 255.0, 240 / 255.0, 240 / 255.0);
		// �������
		consoleLog("Change bg color", "Background", "240 240 240", "");
		break;
	}
	//viewer->updatePointCloud(cloud, "cloud");
	ui.screen->update();
}


//ͨ����ɫ�Ի���ı������ɫ
void CloudViewer::pointcolorChanged()
{
	QColor color = QColorDialog::getColor(Qt::white, this, "Select color for point cloud");

	if (color.isValid()) //�ж���ѡ����ɫ�Ƿ���Ч
	{
		//QAction* action = dynamic_cast<QAction*>(sender());
		//if (action != ui.pointcolorAction) //�ı���ɫ���ź������� dataTree
		QList<QTreeWidgetItem*> itemList = ui.dataTree->selectedItems();
		int selected_item_count = ui.dataTree->selectedItems().size();
		if (selected_item_count == 0) {
			for (int i = 0; i != mycloud_vec.size(); i++) {
				for (int j = 0; j != mycloud_vec[i].cloud->points.size(); j++) {
					mycloud_vec[i].cloud->points[j].r = color.red();
					mycloud_vec[i].cloud->points[j].g = color.green();
					mycloud_vec[i].cloud->points[j].b = color.blue();
				}
			}
			// �������
			consoleLog("Change cloud color", "All point clouds", QString::number(color.red()) + " " + QString::number(color.green()) + " " + QString::number(color.blue()), "");
		}
		else {
			for (int i = 0; i != selected_item_count; i++) {
				int cloud_id = ui.dataTree->indexOfTopLevelItem(itemList[i]);
				for (int j = 0; j != mycloud_vec[cloud_id].cloud->size(); j++) {
					mycloud_vec[cloud_id].cloud->points[j].r = color.red();
					mycloud_vec[cloud_id].cloud->points[j].g = color.green();
					mycloud_vec[cloud_id].cloud->points[j].b = color.blue();
				}
			}
			// �������
			consoleLog("Change cloud color", "Point clouds selected", QString::number(color.red()) + " " + QString::number(color.green()) + " " + QString::number(color.blue()), "");
		}
		//��ɫ�ĸı�ͬ����RGBͣ������
		ui.rSlider->setValue(color.red());
		ui.gSlider->setValue(color.green());
		ui.bSlider->setValue(color.blue());

		showPointcloud();
	}
}

//ͨ����ɫ�Ի���ı䱳����ɫ
void CloudViewer::bgcolorChanged()
{
	QColor color = QColorDialog::getColor(Qt::white, this,
		"Select color for point cloud");
	if (color.isValid())
	{
		viewer->setBackgroundColor(color.red() / 255.0,
			color.green() / 255.0, color.blue() / 255.0);
		// �������
		consoleLog("Change bg color", "Background", QString::number(color.red()) + " " + QString::number(color.green()) + " " + QString::number(color.blue()), "");
		showPointcloud();
	}
}

//����ͼ
void CloudViewer::mainview()
{
	viewer->setCameraPosition(0, -1, 0, 0.5, 0.5, 0.5, 0, 0, 1);
	ui.screen->update();
}
void CloudViewer::leftview()
{
	viewer->setCameraPosition(-1, 0, 0, 0, 0, 0, 0, 0, 1);
	ui.screen->update();
}
void CloudViewer::topview()
{
	viewer->setCameraPosition(0, 0, 1, 0, 0, 0, 0, 1, 0);
	ui.screen->update();
}

//�������Թ�������
void CloudViewer::setPropertyTable() {

	QStringList header;
	header << "Property" << "Value";
	ui.propertyTable->setHorizontalHeaderLabels(header);
	ui.propertyTable->setItem(0, 0, new QTableWidgetItem("Clouds"));
	ui.propertyTable->setItem(1, 0, new QTableWidgetItem("Points"));
	ui.propertyTable->setItem(2, 0, new QTableWidgetItem("Total points"));
	ui.propertyTable->setItem(3, 0, new QTableWidgetItem("RGB"));


	ui.propertyTable->setItem(0, 1, new QTableWidgetItem(QString::number(mycloud_vec.size())));
	ui.propertyTable->setItem(1, 1, new QTableWidgetItem(""));
	ui.propertyTable->setItem(2, 1, new QTableWidgetItem(QString::number(total_points)));
	ui.propertyTable->setItem(4, 1, new QTableWidgetItem(""));

}

void CloudViewer::setConsoleTable() {
	// �����������
	QStringList header2;
	header2 << "Time" << "Operation" << "Operation obeject" << "Details" << "Note";
	ui.consoleTable->setHorizontalHeaderLabels(header2);
	ui.consoleTable->setColumnWidth(0, 150);
	ui.consoleTable->setColumnWidth(1, 200);
	ui.consoleTable->setColumnWidth(2, 200);
	ui.consoleTable->setColumnWidth(3, 300);

	//ui.consoleTable->setEditTriggers(QAbstractItemView::NoEditTriggers); //���ò��ɱ༭
	ui.consoleTable->verticalHeader()->setDefaultSectionSize(22); //�����о�

	ui.consoleTable->setContextMenuPolicy(Qt::CustomContextMenu);

}

//
void CloudViewer::consoleLog(QString operation, QString subname, QString filename, QString note)
{
	if (enable_console == false) {
		return;
	}
	int rows = ui.consoleTable->rowCount();
	ui.consoleTable->setRowCount(++rows);
	QDateTime time = QDateTime::currentDateTime();//��ȡϵͳ���ڵ�ʱ��
	QString time_str = time.toString("MM-dd hh:mm:ss"); //������ʾ��ʽ
	ui.consoleTable->setItem(rows - 1, 0, new QTableWidgetItem(time_str));
	ui.consoleTable->setItem(rows - 1, 1, new QTableWidgetItem(operation));
	ui.consoleTable->setItem(rows - 1, 2, new QTableWidgetItem(subname));
	ui.consoleTable->setItem(rows - 1, 3, new QTableWidgetItem(filename));
	ui.consoleTable->setItem(rows - 1, 4, new QTableWidgetItem(note));

	ui.consoleTable->scrollToBottom(); // �����Զ�������ײ�
}


//QTreeWidget��item�ĵ����Ӧ����
void CloudViewer::itemSelected(QTreeWidgetItem* item, int count)
{
	count = ui.dataTree->indexOfTopLevelItem(item);  //��ȡitem���к�

	
	for (int i = 0; i != mycloud_vec.size(); i++)
	{
		viewer->updatePointCloud(mycloud_vec[i].cloud, "cloud" + QString::number(i).toStdString());
		viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "cloud" + QString::number(i).toStdString());
	}

	//��ȡ��ǰ���Ƶ�RGB,������������Ϣ
	int cloud_size = mycloud_vec[count].cloud->points.size();
	unsigned int cloud_r = mycloud_vec[count].cloud->points[0].r;
	unsigned int cloud_g = mycloud_vec[count].cloud->points[0].g;
	unsigned int cloud_b = mycloud_vec[count].cloud->points[0].b;
	bool multi_color = true;
	int point_size = mycloud_vec[count].pointSize;

	for (int i = 0; i != mycloud_vec[count].cloud->points.size(); i++) {    //�жϵ��Ƶ�ɫ��ɫ������
		if (mycloud_vec[count].cloud->points[0].r != mycloud_vec[count].cloud->points[i].r ||
			mycloud_vec[count].cloud->points[0].g != mycloud_vec[count].cloud->points[i].g ||
			mycloud_vec[count].cloud->points[0].b != mycloud_vec[count].cloud->points[i].b) {
			multi_color = false;
		}
	}

	ui.propertyTable->setItem(0, 1, new QTableWidgetItem(QString::number(mycloud_vec.size())));
	ui.propertyTable->setItem(1, 1, new QTableWidgetItem(QString::number(cloud_size)));
	ui.propertyTable->setItem(2, 1, new QTableWidgetItem(QString::number(total_points)));
	ui.propertyTable->setItem(3, 1, new QTableWidgetItem(multi_color ? "Multi Color" : (QString::number(cloud_r) + " " + QString::number(cloud_g) + " " + QString::number(cloud_b))));

	//ѡ��item����Ӧ�ĵ��Ƴߴ���
	QList<QTreeWidgetItem*> itemList = ui.dataTree->selectedItems();
	int selected_item_count = ui.dataTree->selectedItems().size();
	for (int i = 0; i != selected_item_count; i++) {
		int cloud_id = ui.dataTree->indexOfTopLevelItem(itemList[i]);
		viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE,
			mycloud_vec[cloud_id].pointSize, "cloud" + QString::number(cloud_id).toStdString());
	}
	//mycloud = mycloud_vec[count];

	//�޸�BUG ���л��������һ����ɫ����ʱ�޸�RGB�����
	if (multi_color == true) {
		rSliderChanged(mycloud_vec[count].cloud->points[0].r);
		gSliderChanged(mycloud_vec[count].cloud->points[0].g);
		bSliderChanged(mycloud_vec[count].cloud->points[0].b);
	}
	pSliderChanged(mycloud_vec[count].pointSize);
	ui.screen->update();
}

// consoleTable �һ���Ӧ�¼�
void CloudViewer::popMenuInConsole(const QPoint&) {
	QAction clearConsoleAction("Clear console", this);
	QAction enableConsoleAction("Enable console", this);
	QAction disableConsoleAction("Disable console", this);

	connect(&clearConsoleAction, &QAction::triggered, this, &CloudViewer::clearConsole);
	connect(&enableConsoleAction, &QAction::triggered, this, &CloudViewer::enableConsole);
	connect(&disableConsoleAction, &QAction::triggered, this, &CloudViewer::disableConsole);

	QPoint pos;
	QMenu menu(ui.dataTree);
	menu.addAction(&clearConsoleAction);
	menu.addAction(&enableConsoleAction);
	menu.addAction(&disableConsoleAction);

	if (enable_console == true) {
		menu.actions()[1]->setVisible(false);
		menu.actions()[2]->setVisible(true);
	}
	else {
		menu.actions()[1]->setVisible(true);
		menu.actions()[2]->setVisible(false);
	}

	menu.exec(QCursor::pos()); //�ڵ�ǰ���λ����ʾ
}
// ��� consoleTable
void CloudViewer::clearConsole() {
	ui.consoleTable->clearContents();
	ui.consoleTable->setRowCount(0);
}
// ����ʹ�� consoleTable
void CloudViewer::enableConsole() {
	enable_console = true;
}
// ���� consoleTable
void CloudViewer::disableConsole() {
	clearConsole();
	enable_console = false;

}

//QTreeWidget��item���һ���Ӧ����
void CloudViewer::popMenu(const QPoint&)
{
	QTreeWidgetItem* curItem = ui.dataTree->currentItem(); //��ȡ��ǰ������Ľڵ�
	if (curItem == NULL)return;           //����������Ҽ���λ�ò���treeItem�ķ�Χ�ڣ����ڿհ�λ���һ�
	QString name = curItem->text(0);
	int id = ui.dataTree->indexOfTopLevelItem(curItem);
	string cloud_id = "cloud" + QString::number(id).toStdString();

	QAction hideItemAction("Hide", this);
	QAction showItemAction("Show", this);
	QAction deleteItemAction("Delete", this);
	QAction changeColorAction("Change color", this);

	connect(&hideItemAction, &QAction::triggered, this, &CloudViewer::hideItem);
	connect(&showItemAction, &QAction::triggered, this, &CloudViewer::showItem);
	connect(&deleteItemAction, &QAction::triggered, this, &CloudViewer::deleteItem);
	connect(&changeColorAction, &QAction::triggered, this, &CloudViewer::pointcolorChanged);

	QPoint pos;
	QMenu menu(ui.dataTree);
	menu.addAction(&hideItemAction);
	menu.addAction(&showItemAction);
	menu.addAction(&deleteItemAction);
	menu.addAction(&changeColorAction);

	if (mycloud_vec[id].visible == true) {
		menu.actions()[1]->setVisible(false);
		menu.actions()[0]->setVisible(true);
	}
	else {
		menu.actions()[1]->setVisible(true);
		menu.actions()[0]->setVisible(false);
	}


	menu.exec(QCursor::pos()); //�ڵ�ǰ���λ����ʾ
}
void CloudViewer::hideItem()
{
	QList<QTreeWidgetItem*> itemList = ui.dataTree->selectedItems();
	for (int i = 0; i != ui.dataTree->selectedItems().size(); i++) {
		//TODO hide֮��item��ɻ�ɫ���ٴ��һ�itemʱ����hideItem�� ѡ���� ��showItem��
		//QTreeWidgetItem* curItem = ui.dataTree->currentItem();
		QTreeWidgetItem* curItem = itemList[i];
		QString name = curItem->text(0);
		int id = ui.dataTree->indexOfTopLevelItem(curItem);
		string cloud_id = "cloud" + QString::number(id).toStdString();
		//QMessageBox::information(this, "cloud_id", QString::fromLocal8Bit(cloud_id.c_str()));
		// ��cloud_id����Ӧ�ĵ������ó�͸��
		viewer->setPointCloudRenderingProperties(pcl::visualization::RenderingProperties::PCL_VISUALIZER_OPACITY, 0.0, cloud_id, 0);
		QColor item_color = QColor(112, 122, 132, 255);
		curItem->setTextColor(0, item_color);
		mycloud_vec[id].visible = false;
	}

	// �������
	consoleLog("Hide point clouds", "Point clouds selected", "", "");

	ui.screen->update(); //ˢ����ͼ���ڣ�����ʡ��
}

void CloudViewer::showItem()
{
	QList<QTreeWidgetItem*> itemList = ui.dataTree->selectedItems();
	for (int i = 0; i != ui.dataTree->selectedItems().size(); i++) {
		//QTreeWidgetItem* curItem = ui.dataTree->currentItem();
		QTreeWidgetItem* curItem = itemList[i];
		QString name = curItem->text(0);
		int id = ui.dataTree->indexOfTopLevelItem(curItem);
		string cloud_id = "cloud" + QString::number(id).toStdString();
		// ��cloud_id����Ӧ�ĵ������ó�͸��
		viewer->setPointCloudRenderingProperties(pcl::visualization::RenderingProperties::PCL_VISUALIZER_OPACITY, 1.0, cloud_id, 0);
		QColor item_color;
		if (theme_id == 0) {
			item_color = QColor(0, 0, 0, 255);
		}
		else {
			item_color = QColor(241, 241, 241, 255);
		}
		curItem->setTextColor(0, item_color);
		mycloud_vec[id].visible = true;
	}

	// �������
	consoleLog("Show point clouds", "Point clouds selected", "", "");

	ui.screen->update(); //ˢ����ͼ���ڣ�����ʡ��

}

void CloudViewer::deleteItem()
{
	QList<QTreeWidgetItem*> itemList = ui.dataTree->selectedItems();
	// ui.dataTree->selectedItems().size() ���ŵ����������ı䣬���ѭ������Ҫ����Ϊ�̶���С�� selected_item_count
	int selected_item_count = ui.dataTree->selectedItems().size();
	for (int i = 0; i != selected_item_count; i++) {
		//QTreeWidgetItem* curItem = ui.dataTree->currentItem();
		//QMessageBox::information(this, "itemList's size", QString::number(ui.dataTree->selectedItems().size()));
		QTreeWidgetItem* curItem = itemList[i];
		QString name = curItem->text(0);
		int id = ui.dataTree->indexOfTopLevelItem(curItem);
		//QMessageBox::information(this, "information", "curItem: " + name + " " + QString::number(id));
		auto it = mycloud_vec.begin() + ui.dataTree->indexOfTopLevelItem(curItem);
		// ɾ������֮ǰ����������Ŀ����
		int delete_points = (*it).cloud->points.size();
		it = mycloud_vec.erase(it);
		//QMessageBox::information(this, "information", QString::number(delete_points) + " " + QString::number(mycloud_vec.size()));

		total_points -= delete_points;
		setPropertyTable();

		ui.dataTree->takeTopLevelItem(ui.dataTree->indexOfTopLevelItem(curItem));
	}

	// �Ƴ�֮�������ӣ����� id ����Դ�������кŲ�һ�µ����
	viewer->removeAllPointClouds();
	for (int i = 0; i != mycloud_vec.size(); i++)
	{
		viewer->addPointCloud(mycloud_vec[i].cloud, "cloud" + QString::number(i).toStdString());
		viewer->updatePointCloud(mycloud_vec[i].cloud, "cloud" + QString::number(i).toStdString());
	}

	// �������
	consoleLog("Delete point clouds", "Point clouds selected", "", "");

	ui.screen->update();
}


//���߹��ơ������ؽ���������Ƭ��ʾ
int CloudViewer::convertSurface()
{
	/* ����
	����÷���ֻ�ܴ���PointXYZ�ĵ��ƣ���PointXZYRGBA�ĵ��Ʊ���ᱨ��
	����boost::this_thread::sleep����Ҳ��������
	*/
	pcl::PointXYZ point;
	cloud_xyz.reset(new pcl::PointCloud<pcl::PointXYZ>);
	for (size_t i = 0; i < mycloud.cloud->size(); i++)
	{
		point.x = mycloud.cloud->points[i].x;
		point.y = mycloud.cloud->points[i].y;
		point.z = mycloud.cloud->points[i].z;
		cloud_xyz->push_back(point);
	}
	if (!cloud_xyz)
	{
		return -1;
	}

	/****** �������ģ�� ******/
	//�������߹��ƶ��� n
	pcl::NormalEstimation<pcl::PointXYZ, pcl::Normal> n;
	//������������ָ�� normals
	pcl::PointCloud<pcl::Normal>::Ptr normals(new pcl::PointCloud<pcl::Normal>);
	//���� kdtree ���ڷ������ʱ��������
	pcl::search::KdTree<pcl::PointXYZ>::Ptr tree(new pcl::search::KdTree<pcl::PointXYZ>);
	tree->setInputCloud(cloud_xyz); //Ϊ kdtree �������
	n.setInputCloud(cloud_xyz); //Ϊ������ƶ����������
	n.setSearchMethod(tree);  //���÷������ʱ����ȡ��������ʽΪkdtree
	n.setKSearch(20); //���÷������ʱ��k���������ĵ���
	n.compute(*normals); //���з������

	QMessageBox::information(this, "information", "Normal estimation finished");

	/****** ���������뷨������ƴ�� ******/
	//����֮���������

	//����ͬʱ������ͷ��ߵ����ݽṹ��ָ��
	pcl::PointCloud<pcl::PointNormal>::Ptr cloud_with_normals(new pcl::PointCloud<pcl::PointNormal>);

	//���ѻ�õĵ����ݺͷ�������ƴ��
	pcl::concatenateFields(*cloud_xyz, *normals, *cloud_with_normals); //��������������cloud�������йأ��ĳ�PointXYZ�ĵ��ƾ�û�б�����

																	   //������һ��kdtree�����ؽ�
	pcl::search::KdTree<pcl::PointNormal>::Ptr tree2(new pcl::search::KdTree<pcl::PointNormal>);
	//Ϊkdtree����������ݣ��õ�����������Ϊ��ͷ���
	tree2->setInputCloud(cloud_with_normals);

	/****** �����ؽ�ģ�� ******/
	//����̰��������ͶӰ�ؽ�����
	pcl::GreedyProjectionTriangulation<pcl::PointNormal> gp3;
	//���������������������洢�ؽ����
	pcl::PolygonMesh triangles;
	//���ò���
	gp3.setSearchRadius(25); //�������ӵ�֮�������룬����ȷ��k���ڵ���뾶
	gp3.setMu(2.5); //��������ھ���ĳ��ӣ��Եõ�ÿ��������������뾶
	gp3.setMaximumNearestNeighbors(100); //��������������ڵ���������
	gp3.setMaximumSurfaceAngle(M_PI / 2); //45�� ���ƽ���
	gp3.setMinimumAngle(M_PI / 18); //10�� ÿ�����ǵ����Ƕȣ�
	gp3.setMaximumAngle(2 * M_PI / 3); //120��
	gp3.setNormalConsistency(false); //��������һ�£���Ϊtrue
									 //���õ������ݺ�������ʽ
	gp3.setInputCloud(cloud_with_normals);
	gp3.setSearchMethod(tree2);
	// ��ʼ�ؽ�
	gp3.reconstruct(triangles);
	QMessageBox::information(this, "informaiton", "Reconstruction finished");

	//���ؽ�������浽Ӳ���ļ��У��ؽ������VTK��ʽ�洢
	pcl::io::saveVTKFile("mymesh.vtk", triangles);

	/*
	//Additional vertex information
	std::vector<int> parts = gp3.getPartIDs();
	std::vector<int> states = gp3.getPointStates();
	fstream fs;
	fs.open("partsID.txt", ios::out);
	if (!fs)
	{
	return -2;
	}
	fs << "number of point clouds:" << parts.size() << "\n";
	for (int i = 0; i < parts.size(); i++)
	{
	if (parts[i] != 0)
	{
	fs << parts[i] << "\n";
	}
	}
	*/

	/****** ͼ����ʾģ�� ******/
	QMessageBox::information(this, "informaiton", "Start to show");
	viewer->addPolygonMesh(triangles, "my"); //����Ҫ��ʾ���������
											 //��������ģ����ʾģʽ
	viewer->setRepresentationToSurfaceForAllActors(); //����ģ������Ƭ��ʽ��ʾ
													  //viewer->setRepresentationToPointsForAllActors(); //����ģ���Ե���ʽ��ʾ
													  //viewer->setRepresentationToWireframeForAllActors(); //����ģ�����߿�ͼģʽ��ʾ

													  // �������
	consoleLog("Convert surface", "", "", "");

	viewer->removeAllShapes();
	while (!viewer->wasStopped())
	{
		viewer->spinOnce(100);
		//boost::this_thread::sleep(boost::posix_time::microseconds(100000));
	}
	return 0;

}

int CloudViewer::convertWireframe()
{
	/* ����
	����÷���ֻ�ܴ���PointXYZ�ĵ��ƣ���PointXZYRGBA�ĵ��Ʊ���ᱨ��
	����boost::this_thread::sleep����Ҳ��������
	*/
	pcl::PointXYZ point;
	cloud_xyz.reset(new pcl::PointCloud<pcl::PointXYZ>);
	for (size_t i = 0; i < mycloud.cloud->size(); i++)
	{
		point.x = mycloud.cloud->points[i].x;
		point.y = mycloud.cloud->points[i].y;
		point.z = mycloud.cloud->points[i].z;
		cloud_xyz->push_back(point);
	}
	if (!cloud_xyz)
	{
		return -1;
	}


	/****** �������ģ�� ******/
	//�������߹��ƶ��� n
	pcl::NormalEstimation<pcl::PointXYZ, pcl::Normal> n;
	//������������ָ�� normals
	pcl::PointCloud<pcl::Normal>::Ptr normals(new pcl::PointCloud<pcl::Normal>);
	//���� kdtree ���ڷ������ʱ��������
	pcl::search::KdTree<pcl::PointXYZ>::Ptr tree(new pcl::search::KdTree<pcl::PointXYZ>);
	tree->setInputCloud(cloud_xyz); //Ϊ kdtree �������
	n.setInputCloud(cloud_xyz); //Ϊ������ƶ����������
	n.setSearchMethod(tree);  //���÷������ʱ����ȡ��������ʽΪkdtree
	n.setKSearch(20); //���÷������ʱ��k���������ĵ���
	n.compute(*normals); //���з������

	QMessageBox::information(this, "information", "Normal estimation finished");

	/****** ���������뷨������ƴ�� ******/
	//����֮���������

	//����ͬʱ������ͷ��ߵ����ݽṹ��ָ��
	pcl::PointCloud<pcl::PointNormal>::Ptr cloud_with_normals(new pcl::PointCloud<pcl::PointNormal>);

	//���ѻ�õĵ����ݺͷ�������ƴ��
	pcl::concatenateFields(*cloud_xyz, *normals, *cloud_with_normals); //��������������cloud�������йأ��ĳ�PointXYZ�ĵ��ƾ�û�б�����

																	   //������һ��kdtree�����ؽ�
	pcl::search::KdTree<pcl::PointNormal>::Ptr tree2(new pcl::search::KdTree<pcl::PointNormal>);
	//Ϊkdtree����������ݣ��õ�����������Ϊ��ͷ���
	tree2->setInputCloud(cloud_with_normals);



	/****** �����ؽ�ģ�� ******/
	//����̰��������ͶӰ�ؽ�����
	pcl::GreedyProjectionTriangulation<pcl::PointNormal> gp3;
	//���������������������洢�ؽ����
	pcl::PolygonMesh triangles;
	//���ò���
	gp3.setSearchRadius(25); //�������ӵ�֮�������룬����ȷ��k���ڵ���뾶
	gp3.setMu(2.5); //��������ھ���ĳ��ӣ��Եõ�ÿ��������������뾶
	gp3.setMaximumNearestNeighbors(100); //��������������ڵ���������
	gp3.setMaximumSurfaceAngle(M_PI / 2); //45�� ���ƽ���
	gp3.setMinimumAngle(M_PI / 18); //10�� ÿ�����ǵ����Ƕȣ�
	gp3.setMaximumAngle(2 * M_PI / 3); //120��
	gp3.setNormalConsistency(false); //��������һ�£���Ϊtrue
									 //���õ������ݺ�������ʽ
	gp3.setInputCloud(cloud_with_normals);
	gp3.setSearchMethod(tree2);
	// ��ʼ�ؽ�
	gp3.reconstruct(triangles);
	QMessageBox::information(this, "informaiton", "Reconstruction finished");

	//���ؽ�������浽Ӳ���ļ��У��ؽ������VTK��ʽ�洢
	pcl::io::saveVTKFile("mymesh.vtk", triangles);

	/*
	//Additional vertex information
	std::vector<int> parts = gp3.getPartIDs();
	std::vector<int> states = gp3.getPointStates();
	fstream fs;
	fs.open("partsID.txt", ios::out);
	if (!fs)
	{
	return -2;
	}
	fs << "number of point clouds:" << parts.size() << "\n";
	for (int i = 0; i < parts.size(); i++)
	{
	if (parts[i] != 0)
	{
	fs << parts[i] << "\n";
	}
	}
	*/

	/****** ͼ����ʾģ�� ******/
	QMessageBox::information(this, "informaiton", "Start to show");
	viewer->addPolygonMesh(triangles, "my"); //����Ҫ��ʾ���������
											 //��������ģ����ʾģʽ
											 //viewer->setRepresentationToSurfaceForAllActors(); //����ģ������Ƭ��ʽ��ʾ
											 //viewer->setRepresentationToPointsForAllActors(); //����ģ���Ե���ʽ��ʾ
	viewer->setRepresentationToWireframeForAllActors(); //����ģ�����߿�ͼģʽ��ʾ

														// �������
	consoleLog("Convert wireframe", "", "", "");

	viewer->removeAllShapes();
	while (!viewer->wasStopped())
	{
		viewer->spinOnce(100);
		//boost::this_thread::sleep(boost::posix_time::microseconds(100000));
	}
	return 0;

}
//Statistical Outlier Removal
void CloudViewer::statisticalFilter() {

	//�û������ٽ���������׼�����
	bool isOK1, isOK2;
	int par = QInputDialog::getInt(this, "The number of neighbor points",
		"Please input the number of neighbor points",
		50, 1, 1000, 10, &isOK1);
	if (isOK1) {
		double smt = QInputDialog::getDouble(this, "The standard deviation multiplier", "Please input the standard deviation multiplier", 1.00, 0.01, 10.00, 1, &isOK2);
		if (isOK2) {
			//��ȡ.pcd�ļ�
			QString path = QFileDialog::getOpenFileName(this, tr("Open point cloud file"), QString::fromLocal8Bit(mycloud.dirname.c_str()), tr("Point cloud data(*.pcd);;All file(*.*)"));
			//Return if filenames is empty
			if (path.isEmpty())
				return;
			string str = path.toStdString();
			string pureName = str.substr(0, str.rfind("."));//ȥ����׺��

			pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
			pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_filtered(new pcl::PointCloud<pcl::PointXYZ>);

			// Fill in the cloud data
			pcl::PCDReader reader;
			//str��Ϊ.pcd�ļ�·��
			reader.read<pcl::PointXYZ>(str, *cloud);

			// Create the filtering object
			pcl::StatisticalOutlierRemoval<pcl::PointXYZ> sor;
			sor.setInputCloud(cloud);
			sor.setMeanK(par);
			sor.setStddevMulThresh(smt);
			sor.filter(*cloud_filtered);

			pcl::PCDWriter writer;
			writer.write<pcl::PointXYZ>(pureName + "_statistical.pcd", *cloud_filtered, false);
		}
	}
}

//Radius Outlier Removal
void CloudViewer::radiusFilter() {
	//�û����ð뾶���ڵ����
	bool isOK1, isOK2;
	double radiusSearch = QInputDialog::getDouble(this, "The standard deviation multiplier", "Please input the standard deviation multiplier", 1.00, 0.01, 10.00, 1, &isOK1);
	if (isOK1) {
		int MinNeighbor = QInputDialog::getInt(this, "The number of neighbor points",
			"Please input the number of neighbor points",
			5, 1, 1000, 10, &isOK2);
		if (isOK2) {
			//Get ".pcd" file.
			QString path = QFileDialog::getOpenFileName(this, tr("Open point cloud file"), QString::fromLocal8Bit(mycloud.dirname.c_str()), tr("Point cloud data(*.pcd);;All file(*.*)"));
			//Return if filenames is empty
			if (path.isEmpty())
				return;
			string str = path.toStdString();
			string pureName = str.substr(0, str.rfind("."));//ȥ����׺��
			pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
			pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_filtered(new pcl::PointCloud<pcl::PointXYZ>);

			// Fill in the cloud data
			pcl::PCDReader reader;
			//str��Ϊ.pcd�ļ�·��
			reader.read<pcl::PointXYZ>(str, *cloud);

			// Create the filtering object
			pcl::RadiusOutlierRemoval<pcl::PointXYZ> sor;
			sor.setInputCloud(cloud);
			sor.setRadiusSearch(radiusSearch);
			sor.setMinNeighborsInRadius(MinNeighbor);
			sor.filter(*cloud_filtered);

			pcl::PCDWriter writer;
			writer.write<pcl::PointXYZ>(pureName + "_radius.pcd", *cloud_filtered, false);
		}
	}
}

/*//registration
void CloudViewer::registering() {
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_in(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_out(new pcl::PointCloud<pcl::PointXYZ>);

	// Fill in the CloudIn data
	cloud_in->width = 5;
	cloud_in->height = 1;
	cloud_in->is_dense = false;
	cloud_in->points.resize(cloud_in->width * cloud_in->height);
	for (size_t i = 0; i < cloud_in->points.size(); ++i)
	{
		cloud_in->points[i].x = 1024 * rand() / (RAND_MAX + 1.0f);
		cloud_in->points[i].y = 1024 * rand() / (RAND_MAX + 1.0f);
		cloud_in->points[i].z = 1024 * rand() / (RAND_MAX + 1.0f);
	}
	for (size_t i = 0; i < cloud_in->points.size(); ++i) std::cout << "    " <<
		cloud_in->points[i].x << " " << cloud_in->points[i].y << " " <<
		cloud_in->points[i].z << std::endl;
	*cloud_out = *cloud_in;
	std::cout << "size:" << cloud_out->points.size() << std::endl;
	for (size_t i = 0; i < cloud_in->points.size(); ++i)
		cloud_out->points[i].x = cloud_in->points[i].x + 0.7f;
	for (size_t i = 0; i < cloud_out->points.size(); ++i)
		std::cout << "    " << cloud_out->points[i].x << " " <<
		cloud_out->points[i].y << " " << cloud_out->points[i].z << std::endl;
	pcl::IterativeClosestPoint<pcl::PointXYZ, pcl::PointXYZ> icp;
	icp.setInputSource(cloud_in);
	icp.setInputTarget(cloud_out);
	pcl::PointCloud<pcl::PointXYZ> Final;
	icp.align(Final);
	QMessageBox::about(NULL, tr("Result"), tr("has converged:%1 \nscore:%2\nThe result is saved in \"Transformation Matrix.txt\"").arg(icp.hasConverged()).arg(icp.getFitnessScore()));
	ofstream MatFile("Transformation Matrix.txt");
	MatFile << icp.getFinalTransformation() << std::endl;
}
*/