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
	QObject::connect(ui.actionshowdem, &QAction::triggered, this, &CloudViewer::showdem);
	QObject::connect(ui.actioncsf, &QAction::triggered, this, &CloudViewer::showcsf);
	QObject::connect(ui.actionsetpara, &QAction::triggered, this, &CloudViewer::setpara);
	QObject::connect(ui.actiondembycsf, &QAction::triggered, this, &CloudViewer::dembycsf);
	
	// Option (connect)
	QObject::connect(ui.windowsThemeAction, &QAction::triggered, this, &CloudViewer::windowsTheme);
	QObject::connect(ui.darculaThemeAction, &QAction::triggered, this, &CloudViewer::darculaTheme);
	QObject::connect(ui.englishAction, &QAction::triggered, this, &CloudViewer::langEnglish);
	QObject::connect(ui.chineseAction, &QAction::triggered, this, &CloudViewer::langChinese);
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
vector<int> gi;
int finishcsf;
MyCloud mycloud2;
extern QString globaliter;
extern QString globalri;
extern QString globalts;
extern int finish_para;
vector<string> filelist2; //v6
int makedir() //v2
{
	string folderPath = ".\\result";

	if (0 != access(folderPath.c_str(), 0))
	{
		// if this folder not exist, create a new one.
		mkdir(folderPath.c_str());   

	}
	return 0;
}

void CloudViewer::todem()
{

	using namespace std;
	using namespace pdal;

	//get the filename and the routine
	char charfile[100] = {}; //the filename of the input
	string purename = inputfile.substr(0, inputfile.rfind(".")); //without the format:purename
	int isfile = 0;
	QList<QTreeWidgetItem*> itemList = ui.dataTree->selectedItems();
	int selected_item_count = ui.dataTree->selectedItems().size();
	//create the result folder
	makedir();
	if (filelist2.size() == 0)
	{
		isfile = 0;
		QMessageBox::warning(NULL, QString::fromLocal8Bit("DEM-I"), QString::fromLocal8Bit("尚未打开点云文件！"));
	}

	//如果没有选中任何点云文件
	if (selected_item_count == 0)
	{
		for (int a = 0; a != mycloud_vec.size(); a++)
		{
			if (filelist2.size() == 0)
			{
				isfile = 0;
			}
			else
			{
				purename = filelist2[a].substr(0, filelist2[a].rfind("."));
				isfile = 1;
				//translate the in-memory pcd into las and save it
				char strOutLasName[] = "temp2.las";

				std::ofstream ofs(strOutLasName, ios::out | ios::binary);

				liblas::Header header;
				header.SetVersionMajor(1);
				header.SetVersionMinor(2);
				header.SetDataFormatId(liblas::PointFormatName::ePointFormat3);
				header.SetScale(0.01, 0.01, 0.01);  //slove the long double problem

													//дliblas,
				liblas::Writer writer(ofs, header);
				liblas::Point point(&header);

				for (size_t i = 0; i < mycloud_vec[a].cloud->points.size(); i++)
				{
					long double x = mycloud_vec[a].cloud->points[i].x;
					long double y = mycloud_vec[a].cloud->points[i].y;
					long double z = mycloud_vec[a].cloud->points[i].z;
					point.SetCoordinates(x, y, z);

					uint32_t red = (uint32_t)mycloud_vec[a].cloud->points[i].r;
					uint32_t green = (uint32_t)mycloud_vec[a].cloud->points[i].g;
					uint32_t blue = (uint32_t)mycloud_vec[a].cloud->points[i].b;

					liblas::Color pointColor(red, green, blue);
					point.SetColor(pointColor);
					writer.WritePoint(point);
				}
				long double minPt[3] = { 9999999, 9999999, 9999999 };
				long double maxPt[3] = { 0, 0, 0 };
				header.SetPointRecordsCount(mycloud_vec[a].cloud->points.size());
				header.SetPointRecordsByReturnCount(0, mycloud_vec[a].cloud->points.size());
				header.SetMax(maxPt[0], maxPt[1], maxPt[2]);
				header.SetMin(minPt[0], minPt[1], minPt[2]);
				writer.SetHeader(header);

			}

			if (isfile == 1) //the las file will be broken if we don' t use the 'isfile' judge number
			{
				//CopyFile(L"temp.las", L"temp2.las", FALSE);
				PipelineManager mgr;
				std::stringstream ss22;
				//the DSM will be blank if resolution is too big (eg. > 0.2)
				ss22 << R"({
  "pipeline":["temp2.las",
    {
      "type":"filters.assign",
      "assignment": [ "NumberOfReturns[:]=1", "ReturnNumber[:]=1" ]
   },
    {
      "type":"filters.smrf",
      "scalar":1.2,
      "slope":0.2,
      "threshold":0.45,
      "window":16.0
    },
    {
      "type":"filters.range",
      "limits":"Classification[2:2]"
    },
    {
      "resolution": 0.3,
      "radius": 2,
      "filename":".\\result\\)" << purename << R"(-DEM.tif"}]})";
				mgr.readPipeline(ss22);
				mgr.execute();
				remove("temp2.las");

			}

			getchar();
		}
	}
	//如果选中了某个点云文件
	else
	{
		for (int b = 0; b != selected_item_count; b++)
		{
			int cloud_id = ui.dataTree->indexOfTopLevelItem(itemList[b]);

			{
				if (filelist2.size() == 0)
				{
					isfile = 0;
				}
				else
				{
					purename = filelist2[cloud_id].substr(0, filelist2[cloud_id].rfind("."));
					isfile = 1;
					//translate the in-memory pcd into las and save it
					char strOutLasName[] = "temp2.las";

					std::ofstream ofs(strOutLasName, ios::out | ios::binary);

					liblas::Header header;
					header.SetVersionMajor(1);
					header.SetVersionMinor(2);
					header.SetDataFormatId(liblas::PointFormatName::ePointFormat3);
					header.SetScale(0.01, 0.01, 0.01);  //slove the long double problem

														//дliblas,
					liblas::Writer writer(ofs, header);
					liblas::Point point(&header);

					for (size_t i = 0; i < mycloud_vec[cloud_id].cloud->points.size(); i++)
					{
						long double x = mycloud_vec[cloud_id].cloud->points[i].x;
						long double y = mycloud_vec[cloud_id].cloud->points[i].y;
						long double z = mycloud_vec[cloud_id].cloud->points[i].z;
						point.SetCoordinates(x, y, z);

						uint32_t red = (uint32_t)mycloud_vec[cloud_id].cloud->points[i].r;
						uint32_t green = (uint32_t)mycloud_vec[cloud_id].cloud->points[i].g;
						uint32_t blue = (uint32_t)mycloud_vec[cloud_id].cloud->points[i].b;

						liblas::Color pointColor(red, green, blue);
						point.SetColor(pointColor);
						writer.WritePoint(point);
					}
					long double minPt[3] = { 9999999, 9999999, 9999999 };
					long double maxPt[3] = { 0, 0, 0 };
					header.SetPointRecordsCount(mycloud_vec[cloud_id].cloud->points.size());
					header.SetPointRecordsByReturnCount(0, mycloud_vec[cloud_id].cloud->points.size());
					header.SetMax(maxPt[0], maxPt[1], maxPt[2]);
					header.SetMin(minPt[0], minPt[1], minPt[2]);
					writer.SetHeader(header);

				}

				if (isfile == 1) //the las file will be broken if we don' t use the 'isfile' judge number
				{
					//CopyFile(L"temp.las", L"temp2.las", FALSE);
					PipelineManager mgr;
					std::stringstream ss22;
					//the DSM will be blank if resolution is too big (eg. > 0.2)
					ss22 << R"({
  "pipeline":["temp2.las",
    {
      "type":"filters.assign",
      "assignment": [ "NumberOfReturns[:]=1", "ReturnNumber[:]=1" ]
   },
    {
      "type":"filters.smrf",
      "scalar":1.2,
      "slope":0.2,
      "threshold":0.45,
      "window":16.0
    },
    {
      "type":"filters.range",
      "limits":"Classification[2:2]"
    },
    {
      "resolution": 0.3,
      "radius": 2,
      "filename":".\\result\\)" << purename << R"(-DEM.tif"}]})";
					mgr.readPipeline(ss22);
					mgr.execute();
					remove("temp2.las");

				}

				getchar();
			}
		}
	}
	if (isfile == 1)QMessageBox::information(this, "DEM-I", QString::fromLocal8Bit("成功生成数字高程模型"));

}

void CloudViewer::todsm()
{

	using namespace std;
	using namespace pdal;

	//get the filename and the routine
	char charfile[100] = {}; //the filename of the input
	string purename = inputfile.substr(0, inputfile.rfind(".")); //without the format:purename
	int isfile = 0;
	QList<QTreeWidgetItem*> itemList = ui.dataTree->selectedItems();
	int selected_item_count = ui.dataTree->selectedItems().size();
	//create the result folder
	makedir();
	if (filelist2.size() == 0)
	{
		isfile = 0;
		QMessageBox::warning(NULL, QString::fromLocal8Bit("DSM"), QString::fromLocal8Bit("尚未打开点云文件！"));
	}
	if (selected_item_count == 0)
	{
		for (int a = 0; a != mycloud_vec.size(); a++)
		{
			if (filelist2.size() == 0)
			{
				isfile = 0;
			}
			else
			{
				purename = filelist2[a].substr(0, filelist2[a].rfind("."));
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

				for (size_t i = 0; i < mycloud_vec[a].cloud->points.size(); i++)
				{
					long double x = mycloud_vec[a].cloud->points[i].x;
					long double y = mycloud_vec[a].cloud->points[i].y;
					long double z = mycloud_vec[a].cloud->points[i].z;
					point.SetCoordinates(x, y, z);

					uint32_t red = (uint32_t)mycloud_vec[a].cloud->points[i].r;
					uint32_t green = (uint32_t)mycloud_vec[a].cloud->points[i].g;
					uint32_t blue = (uint32_t)mycloud_vec[a].cloud->points[i].b;

					liblas::Color pointColor(red, green, blue);
					point.SetColor(pointColor);
					writer.WritePoint(point);
				}
				long double minPt[3] = { 9999999, 9999999, 9999999 };
				long double maxPt[3] = { 0, 0, 0 };
				header.SetPointRecordsCount(mycloud_vec[a].cloud->points.size());
				header.SetPointRecordsByReturnCount(0, mycloud_vec[a].cloud->points.size());
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
				ss2 << R"({"pipeline":["temp.las",{"resolution": 0.09,"radius": 0.6,"filename":".\\result\\)" << purename << R"(-DSM.tif"}]})";
				mgr.readPipeline(ss2);
				mgr.execute();
				remove("temp.las");

			}
		}
	}
	else
	{
		for (int b = 0; b != selected_item_count; b++)
		{
			int cloud_id = ui.dataTree->indexOfTopLevelItem(itemList[b]);


			{
				if (filelist2.size() == 0)
				{
					isfile = 0;
				}
				else
				{
					purename = filelist2[cloud_id].substr(0, filelist2[cloud_id].rfind("."));
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

					for (size_t i = 0; i < mycloud_vec[cloud_id].cloud->points.size(); i++)
					{
						long double x = mycloud_vec[cloud_id].cloud->points[i].x;
						long double y = mycloud_vec[cloud_id].cloud->points[i].y;
						long double z = mycloud_vec[cloud_id].cloud->points[i].z;
						point.SetCoordinates(x, y, z);

						uint32_t red = (uint32_t)mycloud_vec[cloud_id].cloud->points[i].r;
						uint32_t green = (uint32_t)mycloud_vec[cloud_id].cloud->points[i].g;
						uint32_t blue = (uint32_t)mycloud_vec[cloud_id].cloud->points[i].b;

						liblas::Color pointColor(red, green, blue);
						point.SetColor(pointColor);
						writer.WritePoint(point);
					}
					long double minPt[3] = { 9999999, 9999999, 9999999 };
					long double maxPt[3] = { 0, 0, 0 };
					header.SetPointRecordsCount(mycloud_vec[cloud_id].cloud->points.size());
					header.SetPointRecordsByReturnCount(0, mycloud_vec[cloud_id].cloud->points.size());
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
					ss2 << R"({"pipeline":["temp.las",{"resolution": 0.09,"radius": 0.6,"filename":".\\result\\)" << purename << R"(-DSM.tif"}]})";
					mgr.readPipeline(ss2);
					mgr.execute();
					remove("temp.las");

				}
			}

		}
	}

	if (isfile == 1)QMessageBox::information(this, "DSM", QString::fromLocal8Bit("成功生成数字表面模型"));
}

void CloudViewer::showdem()
{
	using namespace std;
	using namespace pdal;

	//get the filename and the routine
	char charfile[100] = {}; //the filename of the input
	string purename = inputfile.substr(0, inputfile.rfind(".")); //without the format:purename
	int isfile = 0;

	//judge "select item"
	QList<QTreeWidgetItem*> itemList = ui.dataTree->selectedItems();
	int selected_item_count = ui.dataTree->selectedItems().size();

	//判断点云文件是否打开
	if (filelist2.size() == 0)
	{
		isfile = 0;
		QMessageBox::warning(NULL, QString::fromLocal8Bit("DEM"), QString::fromLocal8Bit("尚未打开点云文件！"));
	}

	if (selected_item_count == 0)
	{
		for (int a = 0; a != mycloud_vec.size(); a++)
		{
			if (filelist2.size() == 0)
			{
				isfile = 0;
				//QMessageBox::warning(NULL, QString::fromLocal8Bit("DEM"), QString::fromLocal8Bit("尚未打开点云文件！"));
			}
			else
			{
				isfile = 1;
				//translate the in-memory pcd into las and save it
				char strOutLasName[] = "temp2.las";

				std::ofstream ofs(strOutLasName, ios::out | ios::binary);

				liblas::Header header;
				header.SetVersionMajor(1);
				header.SetVersionMinor(2);
				header.SetDataFormatId(liblas::PointFormatName::ePointFormat3);
				header.SetScale(0.01, 0.01, 0.01);  //slove the long double problem

													//дliblas,
				liblas::Writer writer(ofs, header);
				liblas::Point point(&header);

				for (size_t i = 0; i < mycloud_vec[a].cloud->points.size(); i++)
				{
					long double x = mycloud_vec[a].cloud->points[i].x;
					long double y = mycloud_vec[a].cloud->points[i].y;
					long double z = mycloud_vec[a].cloud->points[i].z;
					point.SetCoordinates(x, y, z);

					uint32_t red = (uint32_t)mycloud_vec[a].cloud->points[i].r;
					uint32_t green = (uint32_t)mycloud_vec[a].cloud->points[i].g;
					uint32_t blue = (uint32_t)mycloud_vec[a].cloud->points[i].b;

					liblas::Color pointColor(red, green, blue);
					point.SetColor(pointColor);
					writer.WritePoint(point);
				}
				long double minPt[3] = { 9999999, 9999999, 9999999 };
				long double maxPt[3] = { 0, 0, 0 };
				header.SetPointRecordsCount(mycloud_vec[a].cloud->points.size());
				header.SetPointRecordsByReturnCount(0, mycloud_vec[a].cloud->points.size());
				header.SetMax(maxPt[0], maxPt[1], maxPt[2]);
				header.SetMin(minPt[0], minPt[1], minPt[2]);
				writer.SetHeader(header);

			}

			if (isfile == 1) //the las file will be broken if we don' t use the 'isfile' judge number
			{
				//CopyFile(L"temp.las", L"temp2.las", FALSE);
				PipelineManager mgr;
				std::stringstream ss22;
				//the DSM will be blank if resolution is too big (eg. > 0.2)
				ss22 << R"({
  "pipeline":["temp2.las",
    {
      "type":"filters.assign",
      "assignment": [ "NumberOfReturns[:]=1", "ReturnNumber[:]=1" ]
   },
    {
      "type":"filters.smrf",
      "scalar":1.2,
      "slope":0.2,
      "threshold":0.45,
      "window":16.0
    },
    {
      "type":"filters.range",
      "limits":"Classification[2:2]"
    },
    {
      "type":"writers.las",
      "filename":"temp_dem.las"
    }]})";
				mgr.readPipeline(ss22);
				mgr.execute();
				remove("temp2.las");



				// Opening  the las file
				string file_name = "temp_dem.las";
				std::ifstream ifs(file_name.c_str(), std::ios::in | std::ios::binary);
				liblas::ReaderFactory f;
				liblas::Reader reader = f.CreateWithStream(ifs); // reading las file
				unsigned long int nbPoints = reader.GetHeader().GetPointRecordsCount();

				// Fill in the cloud data
				mycloud_vec[a].cloud->width = nbPoints;				// This means that the point cloud is "unorganized"
				mycloud_vec[a].cloud->height = 1;						// (i.e. not a depth map)
				mycloud_vec[a].cloud->is_dense = false;
				mycloud_vec[a].cloud->points.resize(mycloud_vec[a].cloud->width * mycloud_vec[a].cloud->height);

				int i = 0;				// counter
				uint16_t r1, g1, b1;	// RGB variables for .las (16-bit coded)
				int r2, g2, b2;			// RGB variables for converted values (see below)

				while (reader.ReadNextPoint())
				{
					// get XYZ information
					mycloud_vec[a].cloud->points[i].x = (reader.GetPoint().GetX());
					mycloud_vec[a].cloud->points[i].y = (reader.GetPoint().GetY());
					mycloud_vec[a].cloud->points[i].z = (reader.GetPoint().GetZ());

					// get RGB information. Note: in liblas, the "Color" class can be accessed from within the "Point" class, thus the triple gets
					r1 = (reader.GetPoint().GetColor().GetRed());
					g1 = (reader.GetPoint().GetColor().GetGreen());
					b1 = (reader.GetPoint().GetColor().GetBlue());

					// .las stores RGB color in 16-bit (0-65535) while .pcd demands an 8-bit value (0-255). Let's convert them!
					mycloud_vec[a].cloud->points[i].r = ceil(((float)r1 / 65536)*(float)256);
					mycloud_vec[a].cloud->points[i].g = ceil(((float)g1 / 65536)*(float)256);
					mycloud_vec[a].cloud->points[i].b = ceil(((float)b1 / 65536)*(float)256);

					i++; // ...moving on
				}
				ifs.close();
				remove("temp_dem.las");
				showPointcloud();
			}
		}
	}
	else  //选中了某个点云
	{
		for (int b = 0; b != selected_item_count; b++)
		{
			int cloud_id = ui.dataTree->indexOfTopLevelItem(itemList[b]);
			if (filelist2.size() == 0)
			{
				isfile = 0;
			}
			else
			{
				isfile = 1;
				//translate the in-memory pcd into las and save it
				char strOutLasName[] = "temp2.las";

				std::ofstream ofs(strOutLasName, ios::out | ios::binary);

				liblas::Header header;
				header.SetVersionMajor(1);
				header.SetVersionMinor(2);
				header.SetDataFormatId(liblas::PointFormatName::ePointFormat3);
				header.SetScale(0.01, 0.01, 0.01);  //slove the long double problem

													//дliblas,
				liblas::Writer writer(ofs, header);
				liblas::Point point(&header);

				for (size_t i = 0; i < mycloud_vec[cloud_id].cloud->points.size(); i++)
				{
					long double x = mycloud_vec[cloud_id].cloud->points[i].x;
					long double y = mycloud_vec[cloud_id].cloud->points[i].y;
					long double z = mycloud_vec[cloud_id].cloud->points[i].z;
					point.SetCoordinates(x, y, z);

					uint32_t red = (uint32_t)mycloud_vec[cloud_id].cloud->points[i].r;
					uint32_t green = (uint32_t)mycloud_vec[cloud_id].cloud->points[i].g;
					uint32_t blue = (uint32_t)mycloud_vec[cloud_id].cloud->points[i].b;

					liblas::Color pointColor(red, green, blue);
					point.SetColor(pointColor);
					writer.WritePoint(point);
				}
				long double minPt[3] = { 9999999, 9999999, 9999999 };
				long double maxPt[3] = { 0, 0, 0 };
				header.SetPointRecordsCount(mycloud_vec[cloud_id].cloud->points.size());
				header.SetPointRecordsByReturnCount(0, mycloud_vec[cloud_id].cloud->points.size());
				header.SetMax(maxPt[0], maxPt[1], maxPt[2]);
				header.SetMin(minPt[0], minPt[1], minPt[2]);
				writer.SetHeader(header);

			}

			if (isfile == 1) //the las file will be broken if we don' t use the 'isfile' judge number
			{
				//CopyFile(L"temp.las", L"temp2.las", FALSE);
				PipelineManager mgr;
				std::stringstream ss22;
				//the DSM will be blank if resolution is too big (eg. > 0.2)
				ss22 << R"({
  "pipeline":["temp2.las",
    {
      "type":"filters.assign",
      "assignment": [ "NumberOfReturns[:]=1", "ReturnNumber[:]=1" ]
   },
    {
      "type":"filters.smrf",
      "scalar":1.2,
      "slope":0.2,
      "threshold":0.45,
      "window":16.0
    },
    {
      "type":"filters.range",
      "limits":"Classification[2:2]"
    },
    {
      "type":"writers.las",
      "filename":"temp_dem.las"
    }]})";
				mgr.readPipeline(ss22);
				mgr.execute();
				remove("temp2.las");



				// Opening  the las file
				string file_name = "temp_dem.las";
				std::ifstream ifs(file_name.c_str(), std::ios::in | std::ios::binary);
				liblas::ReaderFactory f;
				liblas::Reader reader = f.CreateWithStream(ifs); // reading las file
				unsigned long int nbPoints = reader.GetHeader().GetPointRecordsCount();

				// Fill in the cloud data
				mycloud_vec[cloud_id].cloud->width = nbPoints;				// This means that the point cloud is "unorganized"
				mycloud_vec[cloud_id].cloud->height = 1;						// (i.e. not a depth map)
				mycloud_vec[cloud_id].cloud->is_dense = false;
				mycloud_vec[cloud_id].cloud->points.resize(mycloud_vec[cloud_id].cloud->width * mycloud_vec[cloud_id].cloud->height);

				int i = 0;				// counter
				uint16_t r1, g1, b1;	// RGB variables for .las (16-bit coded)
				int r2, g2, b2;			// RGB variables for converted values (see below)

				while (reader.ReadNextPoint())
				{
					// get XYZ information
					mycloud_vec[cloud_id].cloud->points[i].x = (reader.GetPoint().GetX());
					mycloud_vec[cloud_id].cloud->points[i].y = (reader.GetPoint().GetY());
					mycloud_vec[cloud_id].cloud->points[i].z = (reader.GetPoint().GetZ());

					// get RGB information. Note: in liblas, the "Color" class can be accessed from within the "Point" class, thus the triple gets
					r1 = (reader.GetPoint().GetColor().GetRed());
					g1 = (reader.GetPoint().GetColor().GetGreen());
					b1 = (reader.GetPoint().GetColor().GetBlue());

					// .las stores RGB color in 16-bit (0-65535) while .pcd demands an 8-bit value (0-255). Let's convert them!
					mycloud_vec[cloud_id].cloud->points[i].r = ceil(((float)r1 / 65536)*(float)256);
					mycloud_vec[cloud_id].cloud->points[i].g = ceil(((float)g1 / 65536)*(float)256);
					mycloud_vec[cloud_id].cloud->points[i].b = ceil(((float)b1 / 65536)*(float)256);

					i++; // ...moving on
				}
				ifs.close();
				remove("temp_dem.las");
				showPointcloud();
			}
			//QMessageBox::information(this, "SHOWDSM", "SHOW DEM");
		}
	}
	//QMessageBox::information(this, "SHOWDSM", "SHOW DEM");
}

void getgroundpoint(csf::PointCloud pc)
{

	//some original value

	string sinter;
	string sri;
	string sts;
	globaliter;
	globalri;
	globalts;
	cout << "";
	if (globaliter > "0")
	{
		sinter = globaliter.toStdString();
	}
	else
	{
		globaliter = "100";
		sinter = globaliter.toStdString();
	}

	//if ((globalri == "1")|| (globalri == "2") || (globalri == "3"))
	if (globalri>"0")
	{
		sri = globalri.toStdString();
	}
	else
	{
		globalri = "3";
		sri = globalri.toStdString();
	}

	if (globalts > "0")
	{
		sts = globalts.toStdString();
	}
	else
	{
		globalts = "0.65";
		sts = globalts.toStdString();
	}

	//get the ground point
	CSF *csf = new CSF;
	(*csf).setPointCloud(pc);
	//(*csf).readPointsFromFile("samp311.txt"); //it won't work for samp311(guess:the file is too big)

	//set the parameters

	(*csf).params.bSloopSmooth = false;
	(*csf).params.cloth_resolution = 0.5; //default=0.5 when this is 0.001 the process will break
										  //=0.5 the 28kb work
										  //=5 the 41kb work
	(*csf).params.rigidness = atof(sri.c_str()); //default=3 break:0.03
	(*csf).params.time_step = atof(sts.c_str()); //default=0.65 break:1000 not break:0.001
												 //if it is too big(100),the tree will remain
												 //if it is too small(0.3),the tree will remain
	(*csf).params.class_threshold = 1; //default=0.5 break=5 when 5 , it will be useless
									   //mount.las---best :2
	(*csf).params.interations = atoi(sinter.c_str()); //default=500
													  //100 is the min value that can have the good result
													  //result remains the same for 100-500
	cout << "";


	//do the filtering&get the indexes of the ground points
	std::vector<int> groundIndexes;
	std::vector<int> offGroundIndexes;
	(*csf).do_filtering(groundIndexes, offGroundIndexes);
	//(*csf).savePoints(groundIndexes, "ground.txt");

	//create the new cloud
	//the indexes is saved in offGroundIndexes
	gi = groundIndexes;
	cout << "";
	//clear the vector
	getchar();
	delete csf;
	//groundIndexes.clear();
	//offGroundIndexes.clear();


	getchar();
	finishcsf = 1;
	ExitThread(0);
}

string doubleToString(double num)
{
	char str[256];
	sprintf(str, "%lf", num);
	string result = str;
	return result;
}

void CloudViewer::setpara()
{

	csfwin *CsfWin;
	CsfWin = new csfwin;
	CsfWin->show();
}
void CloudViewer::showcsf()
{
	//get the oripoint
	using namespace std;
	using namespace pdal;
	finish_para = 0;
	QList<QTreeWidgetItem*> itemList = ui.dataTree->selectedItems();
	int selected_item_count = ui.dataTree->selectedItems().size();

	//判断是否有打开文件
	string purename = inputfile.substr(0, inputfile.rfind(".")); //without the format:purename
	int isfile = 0;
	if (filelist2.size() == 0)
	{
		isfile = 0;
		QMessageBox::warning(NULL, QString::fromLocal8Bit("CSF"), QString::fromLocal8Bit("尚未打开点云文件！"));

	}

	//正式执行
	//没有选择文件
	if (selected_item_count == 0)
	{
		for (int a = 0; a != mycloud_vec.size(); a++)
		{
			finishcsf = 0;
			mycloud2 = mycloud_vec[a];
			csf::PointCloud oricloud;
			csf::Point oripoint;

			if (filelist2.size() == 0)
			{
				isfile = 0;
			}
			else
			{

				//执行自窗口赋值程序
				//打开窗口

				//showwin();
				isfile = 1;
				//translate the in-memory pcd into las and save it

				for (size_t i = 0; i < mycloud_vec[a].cloud->points.size(); i++)
				{
					long double x = mycloud_vec[a].cloud->points[i].x;
					long double y = mycloud_vec[a].cloud->points[i].y;
					long double z = mycloud_vec[a].cloud->points[i].z;

					string tempx = doubleToString(x);
					string tempy = doubleToString(y);
					string tempz = doubleToString(z);

					oripoint.x = atof(tempx.c_str());
					oripoint.y = atof(tempy.c_str());
					oripoint.z = atof(tempz.c_str());
					oricloud.push_back(oripoint);
				}

				cout << "";
			}

			if (isfile == 1)
			{
				MyCloud gpcloud = mycloud_vec[a];
				//get the ground point
				//times1
				std::thread t(getgroundpoint, oricloud);
				t.detach();


				Sleep(500);  //if this step is not added , the function wouldn't be excuted
							 //wait(int *status);
				while (1)
				{
					if ((finishcsf == 1)) { goto nextstep; }
					finishcsf;
					cout << "";
				}

			nextstep:
				for (int i = 0; i != gi.size(); i++)
				{
					gpcloud.cloud->points[i] = mycloud_vec[a].cloud->points[gi[i]];
				}
				//mycloud = gpcloud;
				mycloud_vec[a] = gpcloud;


			}

			showPointcloud();

		}

	}
	else  //如果选中了某个文件
	{
		for (int b = 0; b != selected_item_count; b++)
		{
			int cloud_id = ui.dataTree->indexOfTopLevelItem(itemList[b]);
			{
				finishcsf = 0;
				mycloud2 = mycloud_vec[cloud_id];
				csf::PointCloud oricloud;
				csf::Point oripoint;

				if (filelist2.size() == 0)
				{
					isfile = 0;
				}
				else
				{

					//执行自窗口赋值程序
					//打开窗口

					//showwin();
					isfile = 1;
					//translate the in-memory pcd into las and save it

					for (size_t i = 0; i < mycloud_vec[cloud_id].cloud->points.size(); i++)
					{
						long double x = mycloud_vec[cloud_id].cloud->points[i].x;
						long double y = mycloud_vec[cloud_id].cloud->points[i].y;
						long double z = mycloud_vec[cloud_id].cloud->points[i].z;

						string tempx = doubleToString(x);
						string tempy = doubleToString(y);
						string tempz = doubleToString(z);

						oripoint.x = atof(tempx.c_str());
						oripoint.y = atof(tempy.c_str());
						oripoint.z = atof(tempz.c_str());
						oricloud.push_back(oripoint);
					}

					cout << "";
				}

				if (isfile == 1)
				{
					MyCloud gpcloud = mycloud_vec[cloud_id];
					//get the ground point
					//times1
					std::thread t(getgroundpoint, oricloud);
					t.detach();


					Sleep(500);  //if this step is not added , the function wouldn't be excuted
								 //wait(int *status);
					while (1)
					{
						if ((finishcsf == 1)) { goto nextstep23; }
						finishcsf;
						cout << "";
					}

				nextstep23:
					for (int i = 0; i != gi.size(); i++)
					{
						gpcloud.cloud->points[i] = mycloud_vec[cloud_id].cloud->points[gi[i]];
					}
					//mycloud = gpcloud;
					mycloud_vec[cloud_id] = gpcloud;


				}

				showPointcloud();

			}
		}
	}


}

void CloudViewer::dembycsf()
{
	using namespace std;
	using namespace pdal;

	//get the filename and the routine
	char charfile[100] = {}; //the filename of the input
	string purename = inputfile.substr(0, inputfile.rfind(".")); //without the format:purename
	int isfile = 0;
	finish_para = 0;
	QList<QTreeWidgetItem*> itemList = ui.dataTree->selectedItems();
	int selected_item_count = ui.dataTree->selectedItems().size();
	//create the result folder
	makedir();
	if (filelist2.size() == 0)
	{
		isfile = 0;
		QMessageBox::warning(NULL, QString::fromLocal8Bit("DEM-II"), QString::fromLocal8Bit("尚未打开点云文件！"));
	}

	//如果没有选中任何点云文件
	if (selected_item_count == 0)
	{
		for (int a = 0; a != mycloud_vec.size(); a++)
		{
			if (filelist2.size() == 0)
			{
				isfile = 0;
			}
			else
			{
				//改变信号量
				isfile = 1;
				//CSF
				//定义参数
				finishcsf = 0;
				mycloud2 = mycloud_vec[a];
				csf::PointCloud oricloud;
				csf::Point oripoint;

				//生成坐标结构体
				for (size_t i = 0; i < mycloud_vec[a].cloud->points.size(); i++)
				{
					long double x = mycloud_vec[a].cloud->points[i].x;
					long double y = mycloud_vec[a].cloud->points[i].y;
					long double z = mycloud_vec[a].cloud->points[i].z;

					string tempx = doubleToString(x);
					string tempy = doubleToString(y);
					string tempz = doubleToString(z);

					oripoint.x = atof(tempx.c_str());
					oripoint.y = atof(tempy.c_str());
					oripoint.z = atof(tempz.c_str());
					oricloud.push_back(oripoint);
				}

				MyCloud gpcloud = mycloud_vec[a];
				//get the ground point
				//times1
				std::thread t(getgroundpoint, oricloud);
				t.detach();


				Sleep(500);  //if this step is not added , the function wouldn't be excuted
							 //wait(int *status);
				while (1)
				{
					if ((finishcsf == 1)) { goto nextstep; }
					finishcsf;
					cout << "";
				}

			nextstep:
				for (int i = 0; i != gi.size(); i++)
				{
					gpcloud.cloud->points[i] = mycloud_vec[a].cloud->points[gi[i]];
				}
				//mycloud = gpcloud;
				mycloud_vec[a] = gpcloud;

				//转换并生成las中间文件
				purename = filelist2[a].substr(0, filelist2[a].rfind("."));

				//translate the in-memory pcd into las and save it
				char strOutLasName[] = "temp2.las";

				std::ofstream ofs(strOutLasName, ios::out | ios::binary);

				liblas::Header header;
				header.SetVersionMajor(1);
				header.SetVersionMinor(2);
				header.SetDataFormatId(liblas::PointFormatName::ePointFormat3);
				header.SetScale(0.01, 0.01, 0.01);  //slove the long double problem

													//дliblas,
				liblas::Writer writer(ofs, header);
				liblas::Point point(&header);

				for (size_t i = 0; i < mycloud_vec[a].cloud->points.size(); i++)
				{
					long double x = mycloud_vec[a].cloud->points[i].x;
					long double y = mycloud_vec[a].cloud->points[i].y;
					long double z = mycloud_vec[a].cloud->points[i].z;
					point.SetCoordinates(x, y, z);

					uint32_t red = (uint32_t)mycloud_vec[a].cloud->points[i].r;
					uint32_t green = (uint32_t)mycloud_vec[a].cloud->points[i].g;
					uint32_t blue = (uint32_t)mycloud_vec[a].cloud->points[i].b;

					liblas::Color pointColor(red, green, blue);
					point.SetColor(pointColor);
					writer.WritePoint(point);
				}
				long double minPt[3] = { 9999999, 9999999, 9999999 };
				long double maxPt[3] = { 0, 0, 0 };
				header.SetPointRecordsCount(mycloud_vec[a].cloud->points.size());
				header.SetPointRecordsByReturnCount(0, mycloud_vec[a].cloud->points.size());
				header.SetMax(maxPt[0], maxPt[1], maxPt[2]);
				header.SetMin(minPt[0], minPt[1], minPt[2]);
				writer.SetHeader(header);

			}

			if (isfile == 1) //the las file will be broken if we don' t use the 'isfile' judge number
			{
				//处理生成tif

				PipelineManager mgr;
				std::stringstream ss22;
				//the DSM will be blank if resolution is too big (eg. > 0.2)
				ss22 << R"({
  "pipeline":["temp2.las",
    {
      "type":"filters.assign",
      "assignment": [ "NumberOfReturns[:]=1", "ReturnNumber[:]=1" ]
   },
    {
      "type":"filters.smrf",
      "scalar":1.2,
      "slope":0.2,
      "threshold":0.45,
      "window":16.0
    },
    {
      "type":"filters.range",
      "limits":"Classification[2:2]"
    },
    {
      "resolution": 0.3,
      "radius": 2,
      "filename":".\\result\\)" << purename << R"(-DEM.tif"}]})";
				mgr.readPipeline(ss22);
				mgr.execute();
				remove("temp2.las");

			}

			getchar();
		}
	}
	//如果选中了某个点云文件
	else
	{
		for (int b = 0; b != selected_item_count; b++)
		{
			int cloud_id = ui.dataTree->indexOfTopLevelItem(itemList[b]);

			{
				finishcsf = 0;
				mycloud2 = mycloud_vec[cloud_id];
				csf::PointCloud oricloud;
				csf::Point oripoint;

				if (filelist2.size() == 0)
				{
					isfile = 0;
				}
				else
				{
					purename = filelist2[cloud_id].substr(0, filelist2[cloud_id].rfind("."));
					isfile = 1;

					//CSF
					for (size_t i = 0; i < mycloud_vec[cloud_id].cloud->points.size(); i++)
					{
						long double x = mycloud_vec[cloud_id].cloud->points[i].x;
						long double y = mycloud_vec[cloud_id].cloud->points[i].y;
						long double z = mycloud_vec[cloud_id].cloud->points[i].z;

						string tempx = doubleToString(x);
						string tempy = doubleToString(y);
						string tempz = doubleToString(z);

						oripoint.x = atof(tempx.c_str());
						oripoint.y = atof(tempy.c_str());
						oripoint.z = atof(tempz.c_str());
						oricloud.push_back(oripoint);
					}

					MyCloud gpcloud = mycloud_vec[cloud_id];
					//get the ground point
					//times1
					std::thread t(getgroundpoint, oricloud);
					t.detach();


					Sleep(500);  //if this step is not added , the function wouldn't be excuted
								 //wait(int *status);
					while (1)
					{
						if ((finishcsf == 1)) { goto nextstep23; }
						finishcsf;
						cout << "";
					}

				nextstep23:
					for (int i = 0; i != gi.size(); i++)
					{
						gpcloud.cloud->points[i] = mycloud_vec[cloud_id].cloud->points[gi[i]];
					}
					//mycloud = gpcloud;
					mycloud_vec[cloud_id] = gpcloud;


					//translate the in-memory pcd into las and save it
					char strOutLasName[] = "temp2.las";

					std::ofstream ofs(strOutLasName, ios::out | ios::binary);

					liblas::Header header;
					header.SetVersionMajor(1);
					header.SetVersionMinor(2);
					header.SetDataFormatId(liblas::PointFormatName::ePointFormat3);
					header.SetScale(0.01, 0.01, 0.01);  //slove the long double problem

														//дliblas,
					liblas::Writer writer(ofs, header);
					liblas::Point point(&header);

					for (size_t i = 0; i < mycloud_vec[cloud_id].cloud->points.size(); i++)
					{
						long double x = mycloud_vec[cloud_id].cloud->points[i].x;
						long double y = mycloud_vec[cloud_id].cloud->points[i].y;
						long double z = mycloud_vec[cloud_id].cloud->points[i].z;
						point.SetCoordinates(x, y, z);

						uint32_t red = (uint32_t)mycloud_vec[cloud_id].cloud->points[i].r;
						uint32_t green = (uint32_t)mycloud_vec[cloud_id].cloud->points[i].g;
						uint32_t blue = (uint32_t)mycloud_vec[cloud_id].cloud->points[i].b;

						liblas::Color pointColor(red, green, blue);
						point.SetColor(pointColor);
						writer.WritePoint(point);
					}
					long double minPt[3] = { 9999999, 9999999, 9999999 };
					long double maxPt[3] = { 0, 0, 0 };
					header.SetPointRecordsCount(mycloud_vec[cloud_id].cloud->points.size());
					header.SetPointRecordsByReturnCount(0, mycloud_vec[cloud_id].cloud->points.size());
					header.SetMax(maxPt[0], maxPt[1], maxPt[2]);
					header.SetMin(minPt[0], minPt[1], minPt[2]);
					writer.SetHeader(header);

				}

				if (isfile == 1) //the las file will be broken if we don' t use the 'isfile' judge number
				{
					//CopyFile(L"temp.las", L"temp2.las", FALSE);
					PipelineManager mgr;
					std::stringstream ss22;
					//the DSM will be blank if resolution is too big (eg. > 0.2)
					ss22 << R"({
  "pipeline":["temp2.las",
    {
      "type":"filters.assign",
      "assignment": [ "NumberOfReturns[:]=1", "ReturnNumber[:]=1" ]
   },
    {
      "type":"filters.smrf",
      "scalar":1.2,
      "slope":0.2,
      "threshold":0.45,
      "window":16.0
    },
    {
      "type":"filters.range",
      "limits":"Classification[2:2]"
    },
    {
      "resolution": 0.3,
      "radius": 2,
      "filename":".\\result\\)" << purename << R"(-DEM.tif"}]})";
					mgr.readPipeline(ss22);
					mgr.execute();
					remove("temp2.las");

				}

				getchar();
			}
		}
	}
	if (isfile == 1)QMessageBox::information(this, "DEM-II", QString::fromLocal8Bit("成功生成数字高程模型"));
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
	QStringList filenames = QFileDialog::getOpenFileNames(this, QString::fromLocal8Bit("打开点云文件"), QString::fromLocal8Bit(mycloud.dirname.c_str()), QString::fromLocal8Bit("点云文件(*.pcd *.ply *.obj *.las);;所有文件(*.*)"));
	//Return if filenames is empty
	if (filenames.isEmpty())
		return;

	//v6 clear filelist2;
	filelist2.clear();

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
		std::string file_name = string(filename.toLocal8Bit()); //直接调用Qstring::.toStdString()会导致中文乱码
		std::string subname = getFileName(file_name);  //提取全路径中的文件名（带后缀）
													   //change the global v - rowlynn
		inputfile = subname;
		inputroutine = file_name;
		filelist2.push_back(inputfile);  //v6
		//更新状态栏
		ui.statusBar->showMessage(QString::fromLocal8Bit(subname.c_str()) + ": " + QString::number(i) + "/" + QString::number(filenames.size()) + QString::fromLocal8Bit(" 点云文件载入中..."));

		int status = -1;

		if (filename.endsWith(".pcd", Qt::CaseInsensitive))
		{
			status = pcl::io::loadPCDFile(file_name, *(mycloud.cloud));
			rSliderChanged(mycloud.cloud->points[0].r);
			gSliderChanged(mycloud.cloud->points[0].g);
			bSliderChanged(mycloud.cloud->points[0].b);
		}
		else if (filename.endsWith(".ply", Qt::CaseInsensitive))
		{
			status = pcl::io::loadPLYFile(file_name, *(mycloud.cloud));
			rSliderChanged(mycloud.cloud->points[0].r);
			gSliderChanged(mycloud.cloud->points[0].g);
			bSliderChanged(mycloud.cloud->points[0].b);
		}
		else if (filename.endsWith(".obj", Qt::CaseInsensitive))
		{
			status = pcl::io::loadOBJFile(file_name, *(mycloud.cloud));
			rSliderChanged(mycloud.cloud->points[0].r);
			gSliderChanged(mycloud.cloud->points[0].g);
			bSliderChanged(mycloud.cloud->points[0].b);
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
				//QCoreApplication::processEvents();

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
			//提示：无法读取除了.ply .pcd .obj以外的文件
			QMessageBox::information(this, QString::fromLocal8Bit("文件格式错误"),
				QString::fromLocal8Bit("不能打开除 .ply .pcd .obj .las以外格式的文件"));
			return;
		}
		//提示：后缀没问题，但文件内容无法读取
		if (status != 0)
		{
			QMessageBox::critical(this, QString::fromLocal8Bit("添加文件错误"), QString::fromLocal8Bit("不能打开选中的文件"));
			return;
		}
		setA(255);  //设置点云为不透明
					// 最后导入的点云的信息
		mycloud.filename = file_name;
		mycloud.subname = subname;
		mycloud.dirname = file_name.substr(0, file_name.size() - subname.size());
		mycloud_vec.push_back(mycloud);  //将点云导入点云容器


										 // time off
		time_cost = timeOff();
		// 输出窗口
		consoleLog(QString::fromLocal8Bit("打开点云文件"), QString::fromLocal8Bit(mycloud.subname.c_str()), QString::fromLocal8Bit(mycloud.filename.c_str())
			, QString::fromLocal8Bit("花费时间: ") + time_cost + QString::fromLocal8Bit(" 秒, 点的个数: ") + QString::number(mycloud.cloud->points.size()));

		//更新资源管理树
		QTreeWidgetItem *cloudName = new QTreeWidgetItem(QStringList()
			<< QString::fromLocal8Bit(subname.c_str()));
		cloudName->setIcon(0, QIcon(":/Resources/images/icon.png"));
		ui.dataTree->addTopLevelItem(cloudName);

		//setWindowTitle(filename + " - CloudViewer"); //更新标题

		total_points += mycloud.cloud->points.size();
	}
	ui.statusBar->showMessage("");
	showPointcloudAdd();  //更新视图窗口
	setPropertyTable();

}

// Add Point Cloud
void CloudViewer::add()
{
	QStringList filenames = QFileDialog::getOpenFileNames(this, QString::fromLocal8Bit("打开点云文件"), QString::fromLocal8Bit(mycloud.dirname.c_str()), QString::fromLocal8Bit("点云文件(*.pcd *.ply *.obj *.las);;所有文件(*.*)"));
	if (filenames.isEmpty())
		return;
	for (int i = 0; i != filenames.size(); i++) {
		// time start
		timeStart();
		mycloud.cloud.reset(new PointCloudT);
		QString filename = filenames[i];
		std::string file_name = string(filename.toLocal8Bit());
		std::string subname = getFileName(file_name);
		//change the global v - rowlynn
		inputfile = subname;  //v6
		inputroutine = file_name;    //v6
		filelist2.push_back(inputfile);  //v6

		// 更新状态栏
		ui.statusBar->showMessage(QString::fromLocal8Bit(subname.c_str()) + ": " + QString::number(i) + "/" + QString::number(filenames.size()) + QString::fromLocal8Bit(" 点云文件载入中..."));

		int status = -1;
		if (filename.endsWith(".pcd", Qt::CaseInsensitive))
		{
			status = pcl::io::loadPCDFile(file_name, *(mycloud.cloud));
			rSliderChanged(mycloud.cloud->points[0].r);
			gSliderChanged(mycloud.cloud->points[0].g);
			bSliderChanged(mycloud.cloud->points[0].b);
		}
		else if (filename.endsWith(".ply", Qt::CaseInsensitive))
		{
			status = pcl::io::loadPLYFile(file_name, *(mycloud.cloud));
			rSliderChanged(mycloud.cloud->points[0].r);
			gSliderChanged(mycloud.cloud->points[0].g);
			bSliderChanged(mycloud.cloud->points[0].b);
		}
		else if (filename.endsWith(".obj", Qt::CaseInsensitive))
		{
			status = pcl::io::loadOBJFile(file_name, *(mycloud.cloud));
			rSliderChanged(mycloud.cloud->points[0].r);
			gSliderChanged(mycloud.cloud->points[0].g);
			bSliderChanged(mycloud.cloud->points[0].b);
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
			//提示：无法读取除了.ply .pcd .obj以外的文件
			QMessageBox::information(this, QString::fromLocal8Bit("文件格式错误"),
				QString::fromLocal8Bit("不能打开除 .ply .pcd .obj .las以外格式的文件"));
			return;
		}
		//提示：后缀没问题，但文件内容无法读取
		if (status != 0)
		{
			QMessageBox::critical(this, QString::fromLocal8Bit("添加文件错误"), QString::fromLocal8Bit("不能打开选中的文件"));
			return;
		}
		setA(255);  //设置点云为不透明
		mycloud.filename = file_name;
		mycloud.subname = subname;
		mycloud.dirname = file_name.substr(0, file_name.size() - subname.size());
		mycloud_vec.push_back(mycloud);  //将点云导入点云容器

										 // time of
		time_cost = timeOff();
		//输出窗口
		consoleLog(QString::fromLocal8Bit("打开点云文件"), QString::fromLocal8Bit(mycloud.subname.c_str()), QString::fromLocal8Bit(mycloud.filename.c_str())
			, QString::fromLocal8Bit("花费时间: ") + time_cost + QString::fromLocal8Bit(" 秒, 点的个数: ") + QString::number(mycloud.cloud->points.size()));

		//设置资源管理器
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
	mycloud_vec.clear();  //从点云容器中移除所有点云
	viewer->removeAllPointClouds();  //从viewer中移除所有点云
	viewer->removeAllShapes(); //这个remove更彻底
	ui.dataTree->clear();  //将dataTree清空
	filelist2.clear();  //v6

	ui.propertyTable->clear();  //清空属性窗口propertyTable
	QStringList header;
	header << QString::fromLocal8Bit("属性") << QString::fromLocal8Bit("值");
	ui.propertyTable->setHorizontalHeaderLabels(header);

	//输出窗口
	consoleLog(QString::fromLocal8Bit("清除"), QString::fromLocal8Bit("所有点云文件"), "", "");

	setWindowTitle("CloudViewer");  //更新窗口标题
	showPointcloud();  //更新显示
}


// Save point cloud
void CloudViewer::save()
{
	save_filename = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("保存点云文件"),
		QString::fromLocal8Bit(mycloud.dirname.c_str()), QString::fromLocal8Bit("点云文件(*.pcd *.ply);;Allfile(*.*)"));
	if (save_filename.isEmpty())	//文件名为空直接返回
		return;
	std::string file_name = string(save_filename.toLocal8Bit());
	std::string subname = getFileName(file_name);

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
	else //提示：无法保存为除了.ply .pcd以外的文件
	{
		QMessageBox::information(this, QString::fromLocal8Bit("文件格式错误"),
			QString::fromLocal8Bit("不能保存除.ply .pcd以外格式的文件"));
		return;
	}
	//提示：后缀没问题，但是无法保存
	if (status != 0)
	{
		QMessageBox::critical(this, QString::fromLocal8Bit("保存文件错误"),
			QString::fromLocal8Bit("我们不能保存这个文件"));
		return;
	}

	//输出窗口
	consoleLog(QString::fromLocal8Bit("保存文件"), QString::fromLocal8Bit(subname.c_str()), save_filename, QString::fromLocal8Bit("保存"));
	setWindowTitle(save_filename + " - CloudViewer");
	QMessageBox::information(this, QString::fromLocal8Bit("成功保存点云文件"),
		QString::fromLocal8Bit(("保存 " + subname + " 文件成功!").c_str()));
}



// Save point cloud as binary file
void CloudViewer::saveBinary()
{
	save_filename = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("将点雨文件保存为二进制文件"),
		QString::fromLocal8Bit(mycloud.dirname.c_str()), QString::fromLocal8Bit("点云文件(*.pcd *.ply);;所有文件(*.*)"));
	if (save_filename.isEmpty())//文件名为空直接返回
		return;
	std::string file_name = string(save_filename.toLocal8Bit());
	std::string subname = getFileName(file_name);

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
	else //提示：无法保存为除了.ply .pcd以外的文件
	{
		QMessageBox::information(this, QString::fromLocal8Bit("File format error"),
			QString::fromLocal8Bit("无法保存为除了.ply .pcd以外的文件"));
		return;
	}
	//提示：后缀没问题，但是无法保存
	if (status != 0)
	{
		QMessageBox::critical(this, QString::fromLocal8Bit("Saving file error"),
			QString::fromLocal8Bit("不能保存这个文件"));
		return;
	}

	//输出窗口
	consoleLog(QString::fromLocal8Bit("保存为二进制文件"), QString::fromLocal8Bit(subname.c_str()), save_filename, QString::fromLocal8Bit("二进制保存"));

	setWindowTitle(save_filename + " - CloudViewer");
	QMessageBox::information(this, QString::fromLocal8Bit("保存二进制点云文件"),
		QString::fromLocal8Bit(("保存 " + subname + " 文件成功!").c_str()));
}


// Save multi point cloud
void CloudViewer::savemulti()
{
	std::string subname = getFileName(string(save_filename.toLocal8Bit()));
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
		for (int j = 0; j != mycloud_vec[i].cloud->points.size(); j++)          //注意cloudvec[i]->points.size()和cloudvec[i]->size()的区别
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
	//保存multi_cloud
	int status = -1;
	if (save_filename.endsWith(".pcd", Qt::CaseInsensitive))
	{
		if (save_as_binary) {
			status = pcl::io::savePCDFileBinary(string(save_filename.toLocal8Bit()), *multi_cloud);
		}
		else {
			status = pcl::io::savePCDFile(string(save_filename.toLocal8Bit()), *multi_cloud);
		}

	}
	else if (save_filename.endsWith(".ply", Qt::CaseInsensitive))
	{
		if (save_as_binary) {
			status = pcl::io::savePLYFileBinary(string(save_filename.toLocal8Bit()), *multi_cloud);
		}
		else {
			status = pcl::io::savePLYFile(string(save_filename.toLocal8Bit()), *multi_cloud);
		}
	}
	else //提示：无法保存为除了.ply .pcd以外的文件
	{
		QMessageBox::information(this, QString::fromLocal8Bit("文件格式错误"), QString::fromLocal8Bit("无法保存为除了.ply .pcd以外的文件"));
		return;
	}

	//提示：后缀没问题，但是无法保存
	if (status != 0)
	{
		QMessageBox::critical(this, QString::fromLocal8Bit("保存文件错误"), QString::fromLocal8Bit("不能保存这个文件"));
		return;
	}

	// 输出窗口
	if (save_as_binary) {
		consoleLog(QString::fromLocal8Bit("保存为二进制文件"), QString::fromLocal8Bit(subname.c_str()), save_filename, QString::fromLocal8Bit("保存多个二进制文件"));
	}
	else {
		consoleLog(QString::fromLocal8Bit("保存多个二进制文件"), QString::fromLocal8Bit(subname.c_str()), save_filename, QString::fromLocal8Bit("保存"));
	}

	save_as_binary = false;
	//将保存后的 multi_cloud 设置为当前 mycloud,以便保存之后直接进行操作
	mycloud.cloud = multi_cloud;
	mycloud.filename = string(save_filename.toLocal8Bit());
	mycloud.subname = subname;

	setWindowTitle(save_filename + QString::fromLocal8Bit(" - CloudViewer"));
	QMessageBox::information(this, QString::fromLocal8Bit("保存点云文件"), QString::fromLocal8Bit(("保存 " + subname + " 文件成功!").c_str()));
}




//格式转换
void CloudViewer::change()
{

}

//退出程序
void CloudViewer::exit()
{
	this->close();
}

// Generate cube
void CloudViewer::cube()
{
	mycloud.cloud.reset(new PointCloudT);
	total_points = 0;
	ui.dataTree->clear();  //清空资源管理器的item
	viewer->removeAllPointClouds();  //从viewer中移除所有点云
	mycloud_vec.clear();  //清空点云容器

	mycloud.cloud->width = 50000;         // 设置点云宽
	mycloud.cloud->height = 1;            // 设置点云高，高为1，说明为无组织点云
	mycloud.cloud->is_dense = false;
	mycloud.cloud->resize(mycloud.cloud->width * mycloud.cloud->height);     // 重置点云大小
	for (size_t i = 0; i != mycloud.cloud->size(); ++i)
	{
		mycloud.cloud->points[i].x = 1024 * rand() / (RAND_MAX + 1.0f);
		mycloud.cloud->points[i].y = 1024 * rand() / (RAND_MAX + 1.0f);
		mycloud.cloud->points[i].z = 1024 * rand() / (RAND_MAX + 1.0f);
		mycloud.cloud->points[i].r = red;
		mycloud.cloud->points[i].g = green;
		mycloud.cloud->points[i].b = blue;
	}
	//设置资源管理器
	QTreeWidgetItem *cloudName = new QTreeWidgetItem(QStringList() << QString::fromLocal8Bit("cube"));
	cloudName->setIcon(0, QIcon(":/Resources/images/icon.png"));
	ui.dataTree->addTopLevelItem(cloudName);

	// 输出窗口
	consoleLog(QString::fromLocal8Bit("生成正方体"), QString::fromLocal8Bit("正方体"), QString::fromLocal8Bit("正方体"), "");

	mycloud_vec.push_back(mycloud);
	showPointcloudAdd();
}

//初始化
void CloudViewer::initial()
{
	//界面初始化
	setWindowIcon(QIcon(tr(":/Resources/images/icon.png")));
	setWindowTitle(tr("CloudViewer"));

	//点云初始化
	mycloud.cloud.reset(new PointCloudT);
	mycloud.cloud->resize(1);
	viewer.reset(new pcl::visualization::PCLVisualizer("viewer", false));
	//viewer->addPointCloud(cloud, "cloud");

	ui.screen->SetRenderWindow(viewer->getRenderWindow());
	viewer->setupInteractor(ui.screen->GetInteractor(), ui.screen->GetRenderWindow());
	ui.screen->update();

	ui.propertyTable->setSelectionMode(QAbstractItemView::NoSelection); // 禁止点击属性管理器的 item
	ui.consoleTable->setSelectionMode(QAbstractItemView::NoSelection);  // 禁止点击输出窗口的 item
	ui.dataTree->setSelectionMode(QAbstractItemView::ExtendedSelection); // 允许 dataTree 进行多选

																		 // 设置默认主题
	QString qss = darcula_qss;
	qApp->setStyleSheet(qss);

	setPropertyTable();
	setConsoleTable();

	// 输出窗口
	consoleLog(QString::fromLocal8Bit("软件初始化"), "CloudViewer", QString::fromLocal8Bit("欢迎使用CloudViewer"), "TJ510");


	// 设置背景颜色为 dark
	viewer->setBackgroundColor(30 / 255.0, 30 / 255.0, 30 / 255.0);

}

//显示点云，不重置相机角度
void CloudViewer::showPointcloud()
{
	for (int i = 0; i != mycloud_vec.size(); i++)
	{
		viewer->updatePointCloud(mycloud_vec[i].cloud, "cloud" + string(QString::number(i).toLocal8Bit()));
	}
	//viewer->resetCamera();
	ui.screen->update();
}

//添加点云到viewer,并显示点云
void CloudViewer::showPointcloudAdd()
{
	for (int i = 0; i != mycloud_vec.size(); i++)
	{
		viewer->addPointCloud(mycloud_vec[i].cloud, "cloud" + string(QString::number(i).toLocal8Bit()));
		viewer->updatePointCloud(mycloud_vec[i].cloud, "cloud" + string(QString::number(i).toLocal8Bit()));
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

//关于
void CloudViewer::about()
{
	AboutWin *aboutwin = new AboutWin(this);
	aboutwin->setModal(true);
	aboutwin->show();

	// 输出窗口
	consoleLog(QString::fromLocal8Bit("关于"), "TJ-510", "TJ510", "TJ510");
}

//帮助
void CloudViewer::help()
{
	QDesktopServices::openUrl(QUrl(QLatin1String("http://www.baidu.com")));

	// 输出窗口
	consoleLog(QString::fromLocal8Bit("帮助"), "Cloudviewer help", "none", "none");

	//QMessageBox::information(this, "Help", "we are building help widget...");
}

//降噪算法
//统计滤波
//void CloudViewer::statistical(){}

//设置停靠窗口的显示与隐藏
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

//绘制基本图形
void CloudViewer::createSphere()
{
	mycloud.cloud.reset(new PointCloudT);
	ui.dataTree->clear();  //清空资源管理器的item
	viewer->removeAllShapes();
	mycloud_vec.clear();  //清空点云容器

	pcl::PointXYZ p;
	p.x = 0; p.y = 0; p.z = 0;
	viewer->addSphere(p, 100, "sphere1");

	viewer->resetCamera();
	ui.screen->update();

	// 输出窗口
	consoleLog(QString::fromLocal8Bit("生成球体"), QString::fromLocal8Bit("球"), "", "Succeeded");
}
void CloudViewer::createCylinder()
{
	mycloud.cloud.reset(new PointCloudT);
	ui.dataTree->clear();  //清空资源管理器的item
	viewer->removeAllShapes();
	mycloud_vec.clear();  //清空点云容器

	viewer->addCylinder(*(new pcl::ModelCoefficients()), "cylinder");

	viewer->resetCamera();
	ui.screen->update();

	// 输出窗口
	consoleLog(QString::fromLocal8Bit("生成圆柱体"), QString::fromLocal8Bit("圆柱体"), "", "Failed");

}

// Change theme: Windows/Darcula
void CloudViewer::windowsTheme() {
	/*
	QFile qssFile("Resources/qss/Windows.qss"); //资源文件":/Darcula.qss"
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

	//改变 dataTree 字体颜色，以适应主题的笨办法
	QColor light_color(241, 241, 241, 255);
	QColor dark_color(0, 0, 0, 255);
	for (int i = 0; i != mycloud_vec.size(); i++) {
		if (ui.dataTree->topLevelItem(i)->textColor(0) == light_color) {
			ui.dataTree->topLevelItem(i)->setTextColor(0, dark_color);
		}
	}

	// 输出窗口
	consoleLog(QString::fromLocal8Bit("更改主题"), QString::fromLocal8Bit("窗口主题"), "", "");

	theme_id = 0;
}
void CloudViewer::darculaTheme() {
	/*
	QFile qssFile("Resources/qss/Darcula.qss"); //资源文件":/Darcula.qss"
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

	//改变 dataTree 字体颜色，以适应主题的笨办法
	QColor light_color(241, 241, 241, 255);
	QColor dark_color(0, 0, 0, 255);
	for (int i = 0; i != mycloud_vec.size(); i++) {
		if (ui.dataTree->topLevelItem(i)->textColor(0) == dark_color) {
			ui.dataTree->topLevelItem(i)->setTextColor(0, light_color);
		}
	}

	// 输出窗口
	consoleLog(QString::fromLocal8Bit("更改主题"), QString::fromLocal8Bit("黑色背景主题"), "", "");

	theme_id = 1;
}
// Change language: English/Chinese
void CloudViewer::langEnglish() {
	// 输出窗口
	consoleLog(QString::fromLocal8Bit("更改语言"), QString::fromLocal8Bit("英语"), "", "");
}
void CloudViewer::langChinese() {
	// 输出窗口
	consoleLog(QString::fromLocal8Bit("更改语言"), QString::fromLocal8Bit("汉语"), "", "");
}



/*********************************************/
/*****************界面槽函数*****************/
/********************************************/
void CloudViewer::colorBtnPressed()
{
	QList<QTreeWidgetItem*> itemList = ui.dataTree->selectedItems();
	int selected_item_count = ui.dataTree->selectedItems().size();
	// 如果未选中任何点云，则对视图窗口中的所有点云进行着色
	if (selected_item_count == 0) {
		for (int i = 0; i != mycloud_vec.size(); i++) {
			for (int j = 0; j != mycloud_vec[i].cloud->points.size(); j++) {
				mycloud_vec[i].cloud->points[j].r = 255 * (1024 * rand() / (RAND_MAX + 1.0f));
				mycloud_vec[i].cloud->points[j].g = 255 * (1024 * rand() / (RAND_MAX + 1.0f));
				mycloud_vec[i].cloud->points[j].b = 255 * (1024 * rand() / (RAND_MAX + 1.0f));
			}
		}

		// 输出窗口
		consoleLog(QString::fromLocal8Bit("随机生成颜色"), QString::fromLocal8Bit("所有点云"), "", "");

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

		// 输出窗口
		consoleLog(QString::fromLocal8Bit("随机生成颜色"), QString::fromLocal8Bit("选中的点云"), "", "");
	}

	showPointcloud();
}

void CloudViewer::RGBsliderReleased()
{
	QList<QTreeWidgetItem*> itemList = ui.dataTree->selectedItems();
	int selected_item_count = ui.dataTree->selectedItems().size();
	// 如果未选中任何点云，则对视图窗口中的所有点云进行着色
	if (selected_item_count == 0) {
		for (int i = 0; i != mycloud_vec.size(); i++) {
			for (int j = 0; j != mycloud_vec[i].cloud->points.size(); j++) {
				mycloud_vec[i].cloud->points[j].r = red;
				mycloud_vec[i].cloud->points[j].g = green;
				mycloud_vec[i].cloud->points[j].b = blue;
			}
		}

		// 输出窗口
		consoleLog(QString::fromLocal8Bit("改变点云颜色"), QString::fromLocal8Bit("所有点云"), QString::number(red) + " " + QString::number(green) + " " + QString::number(blue), "");
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
		// 输出窗口
		consoleLog(QString::fromLocal8Bit("改变点云颜色"), QString::fromLocal8Bit("选中的点云"), QString::number(red) + " " + QString::number(green) + " " + QString::number(blue), "");
	}
	showPointcloud();
}

//设置所有点云的尺寸
void CloudViewer::psliderReleased()
{
	QList<QTreeWidgetItem*> itemList = ui.dataTree->selectedItems();
	int selected_item_count = ui.dataTree->selectedItems().size();
	if (selected_item_count == 0) {
		for (int i = 0; i != mycloud_vec.size(); i++) {
			mycloud_vec[i].pointSize = p;
			viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE,
				p, "cloud" + string(QString::number(i).toLocal8Bit()));
		}
		// 输出窗口
		consoleLog(QString::fromLocal8Bit("改变点云大小"), QString::fromLocal8Bit("所有点云"), "Size: " + QString::number(p), "");
	}
	else {
		for (int i = 0; i != selected_item_count; i++) {
			int cloud_id = ui.dataTree->indexOfTopLevelItem(itemList[i]);
			mycloud_vec[cloud_id].pointSize = p;
			viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE,
				p, "cloud" + string(QString::number(cloud_id).toLocal8Bit()));
		}
		// 输出窗口
		consoleLog(QString::fromLocal8Bit("改变点云颜色"), QString::fromLocal8Bit("选中的点云"), "Size: " + QString::number(p), "");
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
		viewer->removeCoordinateSystem();  //移除坐标系
										   // 输出窗口
		consoleLog(QString::fromLocal8Bit("移除坐标输出窗口"), "Remove", "", "");
		break;
	case 2:
		viewer->addCoordinateSystem();  //添加坐标系
										// 输出窗口
		consoleLog(QString::fromLocal8Bit("添加坐标输出窗口"), "Add", "", "");
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
		// 输出窗口
		consoleLog(QString::fromLocal8Bit("改变背景颜色"), QString::fromLocal8Bit("背景"), "30 30 30", "");
		break;
	case 2:
		//！注意：setBackgroundColor()接收的是0-1的double型参数
		viewer->setBackgroundColor(240 / 255.0, 240 / 255.0, 240 / 255.0);
		// 输出窗口
		consoleLog(QString::fromLocal8Bit("改变背景颜色"), QString::fromLocal8Bit("背景"), "240 240 240", "");
		break;
	}
	//viewer->updatePointCloud(cloud, "cloud");
	ui.screen->update();
}


//通过颜色对话框改变点云颜色
void CloudViewer::pointcolorChanged()
{
	QColor color = QColorDialog::getColor(Qt::white, this, QString::fromLocal8Bit("选择点云颜色"));

	if (color.isValid()) //判断所选的颜色是否有效
	{
		//QAction* action = dynamic_cast<QAction*>(sender());
		//if (action != ui.pointcolorAction) //改变颜色的信号来自于 dataTree
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
			// 输出窗口
			consoleLog(QString::fromLocal8Bit("改变点云颜色"), QString::fromLocal8Bit("所有点云"), QString::number(color.red()) + " " + QString::number(color.green()) + " " + QString::number(color.blue()), "");
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
			// 输出窗口
			consoleLog(QString::fromLocal8Bit("改变点云颜色"), QString::fromLocal8Bit("选中的点云"), QString::number(color.red()) + " " + QString::number(color.green()) + " " + QString::number(color.blue()), "");
		}
		//颜色的改变同步至RGB停靠窗口
		ui.rSlider->setValue(color.red());
		ui.gSlider->setValue(color.green());
		ui.bSlider->setValue(color.blue());

		showPointcloud();
	}
}

//通过颜色对话框改变背景颜色
void CloudViewer::bgcolorChanged()
{
	QColor color = QColorDialog::getColor(Qt::white, this,
		QString::fromLocal8Bit("选择点云颜色"));
	if (color.isValid())
	{
		viewer->setBackgroundColor(color.red() / 255.0,
			color.green() / 255.0, color.blue() / 255.0);
		// 输出窗口
		consoleLog(QString::fromLocal8Bit("改变背景颜色"), QString::fromLocal8Bit("背景"), QString::number(color.red()) + " " + QString::number(color.green()) + " " + QString::number(color.blue()), "");
		showPointcloud();
	}
}

//三视图
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

//设置属性管理窗口
void CloudViewer::setPropertyTable() {

	QStringList header;
	header << QString::fromLocal8Bit("属性") << QString::fromLocal8Bit("值");
	ui.propertyTable->setHorizontalHeaderLabels(header);
	ui.propertyTable->setItem(0, 0, new QTableWidgetItem(QString::fromLocal8Bit("点云")));
	ui.propertyTable->setItem(1, 0, new QTableWidgetItem(QString::fromLocal8Bit("点")));
	ui.propertyTable->setItem(2, 0, new QTableWidgetItem(QString::fromLocal8Bit("所有点")));
	ui.propertyTable->setItem(3, 0, new QTableWidgetItem("RGB"));


	ui.propertyTable->setItem(0, 1, new QTableWidgetItem(QString::number(mycloud_vec.size())));
	ui.propertyTable->setItem(1, 1, new QTableWidgetItem(""));
	ui.propertyTable->setItem(2, 1, new QTableWidgetItem(QString::number(total_points)));
	ui.propertyTable->setItem(4, 1, new QTableWidgetItem(""));

}

void CloudViewer::setConsoleTable() {
	// 设置输出窗口
	QStringList header2;
	header2 << QString::fromLocal8Bit("时间") << QString::fromLocal8Bit("操作") << QString::fromLocal8Bit("操作对象") << QString::fromLocal8Bit("操作详情") << QString::fromLocal8Bit("信息");
	ui.consoleTable->setHorizontalHeaderLabels(header2);
	ui.consoleTable->setColumnWidth(0, 150);
	ui.consoleTable->setColumnWidth(1, 200);
	ui.consoleTable->setColumnWidth(2, 200);
	ui.consoleTable->setColumnWidth(3, 300);

	//ui.consoleTable->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑
	ui.consoleTable->verticalHeader()->setDefaultSectionSize(22); //设置行距

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
	QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
	QString time_str = time.toString("MM-dd hh:mm:ss"); //设置显示格式
	ui.consoleTable->setItem(rows - 1, 0, new QTableWidgetItem(time_str));
	ui.consoleTable->setItem(rows - 1, 1, new QTableWidgetItem(operation));
	ui.consoleTable->setItem(rows - 1, 2, new QTableWidgetItem(subname));
	ui.consoleTable->setItem(rows - 1, 3, new QTableWidgetItem(filename));
	ui.consoleTable->setItem(rows - 1, 4, new QTableWidgetItem(note));

	ui.consoleTable->scrollToBottom(); // 滑动自动滚到最底部
}


//QTreeWidget的item的点击相应函数
void CloudViewer::itemSelected(QTreeWidgetItem* item, int count)
{
	count = ui.dataTree->indexOfTopLevelItem(item);  //获取item的行号


	for (int i = 0; i != mycloud_vec.size(); i++)
	{
		viewer->updatePointCloud(mycloud_vec[i].cloud, "cloud" + string(QString::number(i).toLocal8Bit()));
		viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 1, "cloud" + string(QString::number(i).toLocal8Bit()));
	}

	//提取当前点云的RGB,点云数量等信息
	int cloud_size = mycloud_vec[count].cloud->points.size();
	unsigned int cloud_r = mycloud_vec[count].cloud->points[0].r;
	unsigned int cloud_g = mycloud_vec[count].cloud->points[0].g;
	unsigned int cloud_b = mycloud_vec[count].cloud->points[0].b;
	bool multi_color = true;
	int point_size = mycloud_vec[count].pointSize;

	for (int i = 0; i != mycloud_vec[count].cloud->points.size(); i++) {    //判断点云单色多色的条件
		if (mycloud_vec[count].cloud->points[0].r != mycloud_vec[count].cloud->points[i].r ||
			mycloud_vec[count].cloud->points[0].g != mycloud_vec[count].cloud->points[i].g ||
			mycloud_vec[count].cloud->points[0].b != mycloud_vec[count].cloud->points[i].b) {
			multi_color = false;
		}
	}

	ui.propertyTable->setItem(0, 1, new QTableWidgetItem(QString::number(mycloud_vec.size())));
	ui.propertyTable->setItem(1, 1, new QTableWidgetItem(QString::number(cloud_size)));
	ui.propertyTable->setItem(2, 1, new QTableWidgetItem(QString::number(total_points)));
	ui.propertyTable->setItem(3, 1, new QTableWidgetItem(multi_color ? "multicloud" : (QString::number(cloud_r) + " " + QString::number(cloud_g) + " " + QString::number(cloud_b))));

	//选中item所对应的点云尺寸变大
	QList<QTreeWidgetItem*> itemList = ui.dataTree->selectedItems();
	int selected_item_count = ui.dataTree->selectedItems().size();
	for (int i = 0; i != selected_item_count; i++) {
		int cloud_id = ui.dataTree->indexOfTopLevelItem(itemList[i]);
		viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE,
			mycloud_vec[cloud_id].pointSize, "cloud" + string(QString::number(cloud_id).toLocal8Bit()));
	}
	//mycloud = mycloud_vec[count];

	//修改BUG 当切换到另外的一个单色点云时修改RGB侧边栏
	if (multi_color == true) {
		rSliderChanged(mycloud_vec[count].cloud->points[0].r);
		gSliderChanged(mycloud_vec[count].cloud->points[0].g);
		bSliderChanged(mycloud_vec[count].cloud->points[0].b);
	}
	pSliderChanged(mycloud_vec[count].pointSize);
	ui.screen->update();
}

// consoleTable 右击响应事件
void CloudViewer::popMenuInConsole(const QPoint&) {
	QAction clearConsoleAction(QString::fromLocal8Bit("清空控制台"), this);
	QAction enableConsoleAction(QString::fromLocal8Bit("打开控制台"), this);
	QAction disableConsoleAction(QString::fromLocal8Bit("关闭控制台"), this);

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

	menu.exec(QCursor::pos()); //在当前鼠标位置显示
}
// 清空 consoleTable
void CloudViewer::clearConsole() {
	ui.consoleTable->clearContents();
	ui.consoleTable->setRowCount(0);
}
// 允许使用 consoleTable
void CloudViewer::enableConsole() {
	enable_console = true;
}
// 禁用 consoleTable
void CloudViewer::disableConsole() {
	clearConsole();
	enable_console = false;

}

//QTreeWidget的item的右击响应函数
void CloudViewer::popMenu(const QPoint&)
{
	QTreeWidgetItem* curItem = ui.dataTree->currentItem(); //获取当前被点击的节点
	if (curItem == NULL)return;           //这种情况是右键的位置不在treeItem的范围内，即在空白位置右击
	QString name = curItem->text(0);
	int id = ui.dataTree->indexOfTopLevelItem(curItem);
	string cloud_id = "cloud" + string(QString::number(id).toLocal8Bit());

	QAction hideItemAction(QString::fromLocal8Bit("隐藏"), this);
	QAction showItemAction(QString::fromLocal8Bit("显示"), this);
	QAction deleteItemAction(QString::fromLocal8Bit("删除"), this);
	QAction changeColorAction(QString::fromLocal8Bit("改变颜色"), this);
	QMenu menuFilter(QString::fromLocal8Bit("滤波"), this);
	QAction statisticalAction(QString::fromLocal8Bit("统计滤波"), this);
	QAction radiusAction(QString::fromLocal8Bit("半径滤波"), this);
	QMenu menuRegister(QString::fromLocal8Bit("配准"), this);
	QAction ICPAction(QString::fromLocal8Bit("ICP配准"), this);
	QAction NDTAction(QString::fromLocal8Bit("NDT配准"), this);

	connect(&hideItemAction, &QAction::triggered, this, &CloudViewer::hideItem);
	connect(&showItemAction, &QAction::triggered, this, &CloudViewer::showItem);
	connect(&deleteItemAction, &QAction::triggered, this, &CloudViewer::deleteItem);
	connect(&changeColorAction, &QAction::triggered, this, &CloudViewer::pointcolorChanged);
	//Filter (connect)
	connect(&statisticalAction, &QAction::triggered, this, &CloudViewer::statisticalFilter);
	connect(&radiusAction, &QAction::triggered, this, &CloudViewer::radiusFilter);
	//Register (connect)
	connect(&ICPAction, &QAction::triggered, this, &CloudViewer::registeringICP);
	connect(&NDTAction, &QAction::triggered, this, &CloudViewer::registeringNDT);

	QPoint pos;
	QMenu menu(ui.dataTree);
	menu.addAction(&hideItemAction);
	menu.addAction(&showItemAction);
	menu.addAction(&deleteItemAction);
	menu.addAction(&changeColorAction);
	menu.addMenu(&menuFilter);
	menuFilter.addAction(&statisticalAction);
	menuFilter.addAction(&radiusAction);
	menu.addMenu(&menuRegister);
	menuRegister.addAction(&ICPAction);
	menuRegister.addAction(&NDTAction);

	if (mycloud_vec[id].visible == true) {
		menu.actions()[1]->setVisible(false);
		menu.actions()[0]->setVisible(true);
	}
	else {
		menu.actions()[1]->setVisible(true);
		menu.actions()[0]->setVisible(false);
	}


	menu.exec(QCursor::pos()); //在当前鼠标位置显示
}
void CloudViewer::hideItem()
{
	QList<QTreeWidgetItem*> itemList = ui.dataTree->selectedItems();
	for (int i = 0; i != ui.dataTree->selectedItems().size(); i++) {
		//TODO hide之后，item变成灰色，再次右击item时，“hideItem” 选项变成 “showItem”
		//QTreeWidgetItem* curItem = ui.dataTree->currentItem();
		QTreeWidgetItem* curItem = itemList[i];
		QString name = curItem->text(0);
		int id = ui.dataTree->indexOfTopLevelItem(curItem);
		string cloud_id = "cloud" + string(QString::number(id).toLocal8Bit());
		//QMessageBox::information(this, "cloud_id", QString::fromLocal8Bit(cloud_id.c_str()));
		// 将cloud_id所对应的点云设置成透明
		viewer->setPointCloudRenderingProperties(pcl::visualization::RenderingProperties::PCL_VISUALIZER_OPACITY, 0.0, cloud_id, 0);
		QColor item_color = QColor(112, 122, 132, 255);
		curItem->setTextColor(0, item_color);
		mycloud_vec[id].visible = false;
	}

	// 输出窗口
	consoleLog(QString::fromLocal8Bit("隐藏点云"), QString::fromLocal8Bit("选中的点云"), "", "");

	ui.screen->update(); //刷新视图窗口，不能省略
}

void CloudViewer::showItem()
{
	QList<QTreeWidgetItem*> itemList = ui.dataTree->selectedItems();
	for (int i = 0; i != ui.dataTree->selectedItems().size(); i++) {
		//QTreeWidgetItem* curItem = ui.dataTree->currentItem();
		QTreeWidgetItem* curItem = itemList[i];
		QString name = curItem->text(0);
		int id = ui.dataTree->indexOfTopLevelItem(curItem);
		string cloud_id = "cloud" + string(QString::number(id).toLocal8Bit());
		// 将cloud_id所对应的点云设置成透明
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

	// 输出窗口
	consoleLog(QString::fromLocal8Bit("显示点云"), QString::fromLocal8Bit("选中的点云"), "", "");

	ui.screen->update(); //刷新视图窗口，不能省略

}

void CloudViewer::deleteItem()
{
	QList<QTreeWidgetItem*> itemList = ui.dataTree->selectedItems();
	// ui.dataTree->selectedItems().size() 随着迭代次数而改变，因此循环条件要设置为固定大小的 selected_item_count
	int selected_item_count = ui.dataTree->selectedItems().size();
	for (int i = 0; i != selected_item_count; i++) {
		//QTreeWidgetItem* curItem = ui.dataTree->currentItem();
		//QMessageBox::information(this, "itemList's size", QString::number(ui.dataTree->selectedItems().size()));
		QTreeWidgetItem* curItem = itemList[i];
		QString name = curItem->text(0);
		int id = ui.dataTree->indexOfTopLevelItem(curItem);
		//v6 filelist2
		filelist2.erase(filelist2.begin() + id);
		//QMessageBox::information(this, "information", "curItem: " + name + " " + QString::number(id));
		auto it = mycloud_vec.begin() + ui.dataTree->indexOfTopLevelItem(curItem);
		// 删除点云之前，将其点的数目保存
		int delete_points = (*it).cloud->points.size();
		it = mycloud_vec.erase(it);
		//QMessageBox::information(this, "information", QString::number(delete_points) + " " + QString::number(mycloud_vec.size()));

		total_points -= delete_points;
		setPropertyTable();

		ui.dataTree->takeTopLevelItem(ui.dataTree->indexOfTopLevelItem(curItem));
	}

	// 移除之后再添加，避免 id 和资源管理树行号不一致的情况
	viewer->removeAllPointClouds();
	for (int i = 0; i != mycloud_vec.size(); i++)
	{
		viewer->addPointCloud(mycloud_vec[i].cloud, "cloud" + string(QString::number(i).toLocal8Bit()));
		viewer->updatePointCloud(mycloud_vec[i].cloud, "cloud" + string(QString::number(i).toLocal8Bit()));
	}

	// 输出窗口
	consoleLog(QString::fromLocal8Bit("删除点云"), QString::fromLocal8Bit("选中的点云"), "", "");

	ui.screen->update();
}


//法线估计、曲面重建、网格面片显示
int CloudViewer::convertSurface()
{
	/* 问题
	好像该方法只能处理PointXYZ的点云，用PointXZYRGBA的点云编译会报错
	调用boost::this_thread::sleep好像也会编译出错
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

	/****** 法向估计模块 ******/
	//创建法线估计对象 n
	pcl::NormalEstimation<pcl::PointXYZ, pcl::Normal> n;
	//创建法向数据指针 normals
	pcl::PointCloud<pcl::Normal>::Ptr normals(new pcl::PointCloud<pcl::Normal>);
	//创建 kdtree 用于法向计算时近邻搜索
	pcl::search::KdTree<pcl::PointXYZ>::Ptr tree(new pcl::search::KdTree<pcl::PointXYZ>);
	tree->setInputCloud(cloud_xyz); //为 kdtree 输入点云
	n.setInputCloud(cloud_xyz); //为法向估计对象输入点云
	n.setSearchMethod(tree);  //设置法向估计时所采取的搜索方式为kdtree
	n.setKSearch(20); //设置法向估计时，k近邻搜索的点数
	n.compute(*normals); //进行法向估计

	QMessageBox::information(this, QString::fromLocal8Bit("信息"), QString::fromLocal8Bit("法向估计完成"));

	/****** 点云数据与法向数据拼接 ******/
	//从这之后出现问题

	//创建同时包含点和法线的数据结构的指针
	pcl::PointCloud<pcl::PointNormal>::Ptr cloud_with_normals(new pcl::PointCloud<pcl::PointNormal>);

	//将已获得的点数据和法向数据拼接
	pcl::concatenateFields(*cloud_xyz, *normals, *cloud_with_normals); //这里编译出错，与cloud的类型有关？改成PointXYZ的点云就没有报错了

																	   //创建另一个kdtree用于重建
	pcl::search::KdTree<pcl::PointNormal>::Ptr tree2(new pcl::search::KdTree<pcl::PointNormal>);
	//为kdtree输入点云数据，该点云数据类型为点和法向
	tree2->setInputCloud(cloud_with_normals);

	/****** 曲面重建模块 ******/
	//创建贪婪三角形投影重建对象
	pcl::GreedyProjectionTriangulation<pcl::PointNormal> gp3;
	//创建多边形网格对象，用来存储重建结果
	pcl::PolygonMesh triangles;
	//设置参数
	gp3.setSearchRadius(25); //设置连接点之间最大距离，用于确定k近邻的球半径
	gp3.setMu(2.5); //设置最近邻距离的乘子，以得到每个点的最终搜索半径
	gp3.setMaximumNearestNeighbors(100); //设置搜索的最近邻点的最大数量
	gp3.setMaximumSurfaceAngle(M_PI / 2); //45度 最大平面角
	gp3.setMinimumAngle(M_PI / 18); //10度 每个三角的最大角度？
	gp3.setMaximumAngle(2 * M_PI / 3); //120度
	gp3.setNormalConsistency(false); //若法向量一致，设为true
									 //设置点云数据和搜索方式
	gp3.setInputCloud(cloud_with_normals);
	gp3.setSearchMethod(tree2);
	// 开始重建
	gp3.reconstruct(triangles);
	QMessageBox::information(this, QString::fromLocal8Bit("信息"), QString::fromLocal8Bit("曲面重建完成"));

	//将重建结果保存到硬盘文件中，重建结果以VTK格式存储
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

	/****** 图形显示模块 ******/
	QMessageBox::information(this, QString::fromLocal8Bit("信息"), QString::fromLocal8Bit("开始显示"));
	viewer->addPolygonMesh(triangles, "my"); //设置要显示的网格对象
											 //设置网格模型显示模式
	viewer->setRepresentationToSurfaceForAllActors(); //网格模型以面片形式显示
													  //viewer->setRepresentationToPointsForAllActors(); //网格模型以点形式显示
													  //viewer->setRepresentationToWireframeForAllActors(); //网格模型以线框图模式显示

													  // 输出窗口
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
	/* 问题
	好像该方法只能处理PointXYZ的点云，用PointXZYRGBA的点云编译会报错
	调用boost::this_thread::sleep好像也会编译出错
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


	/****** 法向估计模块 ******/
	//创建法线估计对象 n
	pcl::NormalEstimation<pcl::PointXYZ, pcl::Normal> n;
	//创建法向数据指针 normals
	pcl::PointCloud<pcl::Normal>::Ptr normals(new pcl::PointCloud<pcl::Normal>);
	//创建 kdtree 用于法向计算时近邻搜索
	pcl::search::KdTree<pcl::PointXYZ>::Ptr tree(new pcl::search::KdTree<pcl::PointXYZ>);
	tree->setInputCloud(cloud_xyz); //为 kdtree 输入点云
	n.setInputCloud(cloud_xyz); //为法向估计对象输入点云
	n.setSearchMethod(tree);  //设置法向估计时所采取的搜索方式为kdtree
	n.setKSearch(20); //设置法向估计时，k近邻搜索的点数
	n.compute(*normals); //进行法向估计

	QMessageBox::information(this, QString::fromLocal8Bit("信息"), QString::fromLocal8Bit("法向估计完成"));

	/****** 点云数据与法向数据拼接 ******/
	//从这之后出现问题

	//创建同时包含点和法线的数据结构的指针
	pcl::PointCloud<pcl::PointNormal>::Ptr cloud_with_normals(new pcl::PointCloud<pcl::PointNormal>);

	//将已获得的点数据和法向数据拼接
	pcl::concatenateFields(*cloud_xyz, *normals, *cloud_with_normals); //这里编译出错，与cloud的类型有关？改成PointXYZ的点云就没有报错了

																	   //创建另一个kdtree用于重建
	pcl::search::KdTree<pcl::PointNormal>::Ptr tree2(new pcl::search::KdTree<pcl::PointNormal>);
	//为kdtree输入点云数据，该点云数据类型为点和法向
	tree2->setInputCloud(cloud_with_normals);



	/****** 曲面重建模块 ******/
	//创建贪婪三角形投影重建对象
	pcl::GreedyProjectionTriangulation<pcl::PointNormal> gp3;
	//创建多边形网格对象，用来存储重建结果
	pcl::PolygonMesh triangles;
	//设置参数
	gp3.setSearchRadius(25); //设置连接点之间最大距离，用于确定k近邻的球半径
	gp3.setMu(2.5); //设置最近邻距离的乘子，以得到每个点的最终搜索半径
	gp3.setMaximumNearestNeighbors(100); //设置搜索的最近邻点的最大数量
	gp3.setMaximumSurfaceAngle(M_PI / 2); //45度 最大平面角
	gp3.setMinimumAngle(M_PI / 18); //10度 每个三角的最大角度？
	gp3.setMaximumAngle(2 * M_PI / 3); //120度
	gp3.setNormalConsistency(false); //若法向量一致，设为true
									 //设置点云数据和搜索方式
	gp3.setInputCloud(cloud_with_normals);
	gp3.setSearchMethod(tree2);
	// 开始重建
	gp3.reconstruct(triangles);
	QMessageBox::information(this, QString::fromLocal8Bit("信息"), QString::fromLocal8Bit("曲面重建完成"));

	//将重建结果保存到硬盘文件中，重建结果以VTK格式存储
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

	/****** 图形显示模块 ******/
	QMessageBox::information(this, QString::fromLocal8Bit("信息"), QString::fromLocal8Bit("开始显示"));
	viewer->addPolygonMesh(triangles, "my"); //设置要显示的网格对象
											 //设置网格模型显示模式
											 //viewer->setRepresentationToSurfaceForAllActors(); //网格模型以面片形式显示
											 //viewer->setRepresentationToPointsForAllActors(); //网格模型以点形式显示
	viewer->setRepresentationToWireframeForAllActors(); //网格模型以线框图模式显示

														// 输出窗口
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
	QList<QTreeWidgetItem*> itemList = ui.dataTree->selectedItems();
	for (int i = 0; i != ui.dataTree->selectedItems().size(); i++) {
		QTreeWidgetItem* curItem = itemList[i];
		int id = ui.dataTree->indexOfTopLevelItem(curItem);
		//用户设置临近点数及标准差乘数
		bool isOK1, isOK2;
		int par = QInputDialog::getInt(this, QString::fromLocal8Bit("近邻的点数"),
			QString::fromLocal8Bit(mycloud_vec[id].subname.c_str())+QString::fromLocal8Bit(":\n请输入近邻点数"),
			50, 1, 1000, 10, &isOK1);
		if (isOK1) {
			double smt = QInputDialog::getDouble(this, QString::fromLocal8Bit("标准差乘数"), QString::fromLocal8Bit("请输入标准差乘数"), 1.00, 0.01, 10.00, 1, &isOK2);
			if (isOK2) {

				// 更新状态栏
				ui.statusBar->showMessage(QString::fromLocal8Bit(mycloud_vec[id].subname.c_str()) + QString::fromLocal8Bit(":使用统计滤波过滤器删除异常值..."));

				PointCloudT::Ptr cloud(new PointCloudT);
				PointCloudT::Ptr cloud_filtered(new PointCloudT);

				// Fill in the cloud data
				cloud = mycloud_vec[id].cloud;
				total_points -= mycloud_vec[id].cloud->points.size();


				// time start
				timeStart();
				// Create the filtering object
				pcl::StatisticalOutlierRemoval<PointT> sor;
				sor.setInputCloud(cloud);
				sor.setMeanK(par);
				sor.setStddevMulThresh(smt);
				sor.filter(*cloud_filtered);
				// time off
				time_cost = timeOff();

				mycloud_vec[id].cloud = cloud_filtered;
				total_points += mycloud_vec[id].cloud->points.size();

				ui.statusBar->showMessage("");
				setPropertyTable();
				//输出窗口
				consoleLog(QString::fromLocal8Bit("统计滤波器"), QString::fromLocal8Bit(mycloud_vec[id].subname.c_str()),
					QString::fromLocal8Bit(mycloud_vec[id].filename.c_str()), QString::fromLocal8Bit("时间花费: ") + time_cost + QString::fromLocal8Bit(" s, 点的个数: ") + QString::number(mycloud_vec[id].cloud->points.size()));
				showPointcloud(); //刷新视图窗口，不能省略
			}
		}
	}
}


//Radius Outlier Removal
void CloudViewer::radiusFilter() {
	QList<QTreeWidgetItem*> itemList = ui.dataTree->selectedItems();
	for (int i = 0; i != ui.dataTree->selectedItems().size(); i++) {
		QTreeWidgetItem* curItem = itemList[i];
		int id = ui.dataTree->indexOfTopLevelItem(curItem);
		//用户设置半径及邻点个数
		bool isOK1, isOK2;
		double radiusSearch = QInputDialog::getDouble(this, QString::fromLocal8Bit("搜索半径"), QString::fromLocal8Bit(mycloud_vec[id].subname.c_str()) + 
			QString::fromLocal8Bit(":\n请输入搜索半径"), 1.00, 0.01, 10.00, 2, &isOK1);
		if (isOK1) {
			int MinNeighbor = QInputDialog::getInt(this, QString::fromLocal8Bit("近邻的点数"),
				QString::fromLocal8Bit("请输入近邻点数"),
				5, 1, 1000, 10, &isOK2);
			if (isOK2) {
				// time start
				timeStart();

				// 更新状态栏
				ui.statusBar->showMessage(QString::fromLocal8Bit(mycloud_vec[id].subname.c_str()) + QString::fromLocal8Bit("：使用半径滤波过滤器删除异常值..."));

				PointCloudT::Ptr cloud(new PointCloudT);
				PointCloudT::Ptr cloud_filtered(new PointCloudT);

				// Fill in the cloud data
				cloud = mycloud_vec[id].cloud;
				total_points -= mycloud_vec[id].cloud->points.size();

				// Create the filtering object
				pcl::RadiusOutlierRemoval<PointT> sor;
				sor.setInputCloud(cloud);
				sor.setRadiusSearch(radiusSearch);
				sor.setMinNeighborsInRadius(MinNeighbor);
				sor.filter(*cloud_filtered);

				// time off
				time_cost = timeOff();

				mycloud_vec[id].cloud = cloud_filtered;
				total_points += mycloud_vec[id].cloud->points.size();

				ui.statusBar->showMessage("");
				setPropertyTable();
				consoleLog(QString::fromLocal8Bit("半径滤波器"), QString::fromLocal8Bit(mycloud_vec[id].subname.c_str()), QString::fromLocal8Bit(mycloud_vec[id].filename.c_str())
					, QString::fromLocal8Bit("时间花费: ") + time_cost + QString::fromLocal8Bit(" s, 点的个数: ") + QString::number(mycloud_vec[id].cloud->points.size()));
				showPointcloud(); //刷新视图窗口，不能省略
			}
		}
	}
}

//registration
//ICP配准
void CloudViewer::registeringICP() {
	QList<QTreeWidgetItem*> itemList = ui.dataTree->selectedItems();
	if (itemList.size() < 2) {
		QMessageBox::warning(NULL, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("至少需要选中两个点云文件！\n按住ctrl键可多选文件！"));
	}
	else {
		bool times;
		bool distance;
		int num = QInputDialog::getInt(this, QString::fromLocal8Bit("迭代次数"), QString::fromLocal8Bit("请设置匹配迭代的最大次数"), 20, 1, 1000, 1, &times);
		if (times) {
			double dis = QInputDialog::getDouble(this, QString::fromLocal8Bit("对应点距离"), QString::fromLocal8Bit("请设置最大的对应点距离"), 0.10, 0.01, 10000.0, 2, &distance);
			if (distance) {
				//开始计时
				timeStart();
				for (int i = 1; i != ui.dataTree->selectedItems().size(); i++) {
					QTreeWidgetItem* souItem = itemList[i];
					QTreeWidgetItem* tarItem = itemList[i-1];
					int s_id = ui.dataTree->indexOfTopLevelItem(souItem);
					int t_id = ui.dataTree->indexOfTopLevelItem(tarItem);
					PointCloudT::Ptr cloud_in(new PointCloudT);  //源点云
					PointCloudT::Ptr cloud_target(new PointCloudT);   //目标点云
					cloud_in = mycloud_vec[s_id].cloud;              //源点云设为当前点云
					cloud_target = mycloud_vec[t_id].cloud;        //目标点云为总坐标系下的前一个点云
					//从点云中移除无效点
					std::vector<int> indices;
					pcl::removeNaNFromPointCloud(*cloud_in, *cloud_in, indices);
					pcl::IterativeClosestPoint<PointT, PointT> icp;
					//设置源点云的kd-tree
					pcl::search::KdTree<PointT>::Ptr tree1(new pcl::search::KdTree<PointT>);
					tree1->setInputCloud(cloud_in);
					//设置目标点云的kd-tree
					pcl::search::KdTree<PointT>::Ptr tree2(new pcl::search::KdTree<PointT>);
					tree2->setInputCloud(cloud_target);
					icp.setSearchMethodSource(tree1);	//设置源点云搜索方法
					icp.setSearchMethodTarget(tree2);	//设置目标点云搜索方法 */
					icp.setInputSource(cloud_in);      //设置源点云
					icp.setInputTarget(cloud_target);   //设置目标点云
					icp.setMaxCorrespondenceDistance(dis);   //设置最大的对应点距离
					icp.setTransformationEpsilon(1e-10);      //设置精度
					icp.setMaximumIterations(num);    //设置最大迭代次数
					icp.setEuclideanFitnessEpsilon(0.01);
					PointCloudT::Ptr Final(new PointCloudT);
					//开始配准
					icp.align(*Final);
					mycloud_vec[s_id].cloud = Final;          //变换当前点云至总坐标系，为下一个点云配准做准备
					showPointcloud();           //配准完成刷新ui窗口
				}
				//结束计时
				time_cost = timeOff();
				QMessageBox::about(NULL, tr("Result"), QString::fromLocal8Bit("花费时间:%1秒\n").arg(time_cost));
			}
		}
	}
}
//NDT配准
void CloudViewer::registeringNDT() {
	QList<QTreeWidgetItem*> itemList = ui.dataTree->selectedItems();
	if (itemList.size() < 2) {
		QMessageBox::warning(NULL, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("至少需要选中两个点云文件！\n按住ctrl键可多选文件！"));
	}
	else {
		PointCloudT::Ptr cloud_in(new PointCloudT);  //源点云
		PointCloudT::Ptr cloud_target(new PointCloudT);   //目标点云
		PointCloudT::Ptr filtered_cloud(new PointCloudT);    //过滤后的源点云
		bool step, res, times;
		//为More-Thuente线搜索设置最大步长
		double maxStep = QInputDialog::getDouble(this, QString::fromLocal8Bit("最大步长"), QString::fromLocal8Bit("请设置More-Thuente线搜索最大步长"), 0.10, 0.01, 5.00, 2, &step);
		if (step) {
			//设置NDT网格结构的分辨率（VoxelGridCovariance）
			double resolution = QInputDialog::getDouble(this, QString::fromLocal8Bit("分辨率"), QString::fromLocal8Bit("请设置NDT网格结构分辨率"), 1.00, 0.01, 10.00, 2, &res);
			if (res) {
				//设置匹配迭代的最大次数
				int num = QInputDialog::getInt(this, QString::fromLocal8Bit("迭代次数"), QString::fromLocal8Bit("请设置匹配迭代的最大次数"), 35, 1, 1000, 1, &times);
				if (times) {
					//开始计时
					timeStart();
					for (int i = 1; i != ui.dataTree->selectedItems().size(); i++) {
						QTreeWidgetItem* souItem = itemList[i];
						QTreeWidgetItem* tarItem = itemList[i - 1];
						int s_id = ui.dataTree->indexOfTopLevelItem(souItem);
						int t_id = ui.dataTree->indexOfTopLevelItem(tarItem);
						PointCloudT::Ptr cloud_in(new PointCloudT);  //源点云
						PointCloudT::Ptr cloud_target(new PointCloudT);   //目标点云
						cloud_in = mycloud_vec[s_id].cloud;              //源点云设为当前点云
						cloud_target = mycloud_vec[t_id].cloud;        //目标点云为总坐标系下的前一个点云

						//近似体素滤波器提高配准效率
						pcl::ApproximateVoxelGrid<PointT> approximate_voxel_filter;
						approximate_voxel_filter.setLeafSize(0.2, 0.2, 0.2);
						approximate_voxel_filter.setInputCloud(cloud_in);
						approximate_voxel_filter.filter(*filtered_cloud);

						//初始化正态分布变换（NDT）
						pcl::NormalDistributionsTransform<PointT, PointT> ndt;
						//设置依赖尺度NDT参数
						//为终止条件设置最小转换差异
						ndt.setTransformationEpsilon(1e-10);
						//为More-Thuente线搜索设置最大步长
						ndt.setStepSize(maxStep);
						//设置NDT网格结构的分辨率（VoxelGridCovariance）
						ndt.setResolution(resolution);
						//设置匹配迭代的最大次数
						ndt.setMaximumIterations(num);
						ndt.setInputCloud(filtered_cloud);
						ndt.setInputTarget(cloud_target);
						PointCloudT::Ptr output_cloud(new PointCloudT);
						ndt.align(*output_cloud);        //变换后源点云,此处output_cloud不能作为最终的源点云变换，因为上面对源点云进行了滤波处理
						//使用创建的变换对未过滤的输入点云进行变换
						pcl::transformPointCloud(*cloud_in, *output_cloud, ndt.getFinalTransformation());
						mycloud_vec[s_id].cloud = output_cloud;                     //变换当前点云至总坐标系，为下一个点云配准做准备
						showPointcloud();           //配准完成刷新ui窗口
						}
					//结束计时
					time_cost = timeOff();
					QMessageBox::about(NULL, tr("Result"), QString::fromLocal8Bit("花费时间:%1秒\n").arg(time_cost));
					}
				}
			}
		}
	}