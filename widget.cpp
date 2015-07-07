#include "widget.h"
#include "ui_widget.h"

#include <QtWidgets>
#include "mission_generator.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_clicked()
{
	// base class name for class <missionName>: DefaultMission
	QString missionClassName;
	QString missionClassNameNext;

	// mission file name and real readable name
	QString missionFileName, missionRealName;

	// terrain name
	QString terrainName = ui->terrainName->currentText();

	// number of missions
	int numberOfMissions = ui->numberOfMissions->value();

	// grab the campaign name from the lineEdit box
	QString CamDir = ui->CampaignName->text();

	// replace spaces with underscores
	CamDir.replace(" ", "_");

	// create the directory
	QDir file;
	// PMCTODO please add check here if same name directory already exists, show cancel or replace dialog.
	file.mkdir (CamDir);

	// give some statusbox feedback
	ui->StatusBox->append("Created campaign directory: " + CamDir);

	file.cd(CamDir);
	ui->StatusBox->append("We are now in this dir: " + file.absolutePath());

	// create DtaExt dir
	file.mkdir("DtaExt");
	ui->StatusBox->append("Created DtaExt dir!");

	// create music dir
	file.mkpath("DtaExt//Music");
	// create sound dir
	file.mkpath("DtaExt//Sound");

	// create missions sub dir
	file.mkdir("Missions");
	ui->StatusBox->append("Created Missions dir!");

	// create scripts sub dir
	file.mkdir("Scripts");
	ui->StatusBox->append("Created Scripts dir!");

	// create description.ext file to the campaign root
	QString descriptionExt = file.absolutePath() + "/description.ext";
	QFile des(descriptionExt);
	des.open(QIODevice::WriteOnly | QIODevice::Text);

	QString weaponPool = "false";

	// check weaponPool status
	if (ui->weaponPool->isChecked())
	{
		weaponPool = "true";
	}

	// description.ext header
	QString str1 = "/*\n\n\t" + ui->CampaignName->text() + "\n\n*/\n\nexitScore = -100;\nweaponPool = " + weaponPool + ";\n\n";
	str1.append("class MissionDefault\n{\n\tlives = -1;\n\tlost = ;\n\tend1 = ;\n\tend2 = ;\n\tend3 = ;\n\tend4 = ;\n\tend5 = ;\n\tend6 = ;\n};\n\n");

	// class Campaign start
	str1.append("class Campaign\n{\n\tname = \"" + ui->CampaignName->text() + "\";\n\tfirstBattle = Beginning;\n\n\t// first chapter\n\tclass Beginning\n\t{\n");
	str1.append("\t\tname = \"Beginning\";\n\t\tcutscene = ");

	// if we have opening cutscene from checkbox
	if (ui->openingCutscene->isChecked())
	{
		missionClassName = ui->missionTagName->text();
		missionClassName.append("OC");
	}
	else
	{
		missionClassName = "";
	}
	
	// opening cutscene
	str1.append(missionClassName + ";\n\t\tfirstMission = ");
	
	// mission class name for first mission
	missionClassName = ui->missionTagName->text();
	missionClassName.append("00");
	missionClassName.append(QString::number(1));
	
	str1.append(missionClassName + ";");
	str1.append("\n\t\tend1 = ;\n\t\tend2 = ;\n\t\tend3 = ;\n\t\tend4 = ;\n\t\tend5 = ;\n\t\tend6 = ;\n\t\tlost = ;\n\n");
	
	// create mission classes, a big loop
	str1.append("\t\t// campaign missions\n");

	// opening cutscene or not
	if (ui->openingCutscene->isChecked())
	{
		missionClassName = ui->missionTagName->text();
		missionClassName.append("OC");
		
		missionClassNameNext = ui->missionTagName->text();
		missionClassNameNext.append("00");
		missionClassNameNext.append(QString::number(1));
		
		missionFileName = ui->missionTagName->text();
		missionFileName.append("OC");
		// create real name
		missionRealName = missionFileName;
		missionFileName.append("." + terrainName);
		
		// put in our stuff into string
		str1.append("\n\t\t// Opening Cutscene\n\t\tclass " + missionClassName + ": MissionDefault\n\t\t{\n\t\t\tend1 = " + missionClassNameNext + ";\n\t\t\tlost = " + missionClassName + ";\n\t\t\ttemplate = " + missionFileName + ";\n\t\t};\n");
		
		// write missions dir
		//CreateMissionDir(missionFileName, CamDir);
		
		// write cutscene dir
		CreateCutsceneDir(missionFileName, CamDir);
	}

	// write as many mission class names as we have input from the number of missions UI field
	// writes actually one mission less, then we finish up the classes with one final manual entry
	for (int i = 1; i < numberOfMissions; i++)
	{
		// mission class name
		missionClassName = ui->missionTagName->text();

		// inject the double or single zeros in front of the mission number for sorting purposes
		missionClassName = injectMissionDigit(missionClassName, i);

		// now add the actual mission number digit
		missionClassName.append(QString::number(i));

		// mission class name for succesfully complete mission, ie next mission name.
		missionClassNameNext = ui->missionTagName->text();

		// inject zeros but add one because this is for the NEXT mission
		missionClassNameNext = injectMissionDigit(missionClassNameNext, (i + 1));

		// if we have cutscenes!
		if (ui->cutsceneEveryMission->isChecked())
		{
			// ugly crap :(
			if (i == 9) missionClassNameNext.append("0");
			if (i == 99) missionClassNameNext.append("0");

			missionClassNameNext.append(QString::number(i));
			missionClassNameNext.append("Cut");
		}
		else
		{
			missionClassNameNext.append(QString::number(i + 1));
		}
		
		// mission file name plus extension of terrain name.
		missionFileName = ui->missionTagName->text();
		missionFileName = injectMissionDigit(missionFileName, i);
		// real human readable name
		missionRealName = missionFileName;
		missionRealName.append(QString::number(i));
		missionFileName.append(QString::number(i) + "." + terrainName);

		// put in our stuff into string
		str1.append("\n\t\tclass " + missionClassName + ": MissionDefault\n\t\t{\n\t\t\tend1 = " + missionClassNameNext + ";\n\t\t\tlost = " + missionClassName + ";\n\t\t\ttemplate = " + missionFileName + ";\n\t\t};\n");

		// write missions dir
		CreateMissionDir(missionFileName, CamDir, missionRealName);

		/*

		Cutscene between every mission or not?

		*/
		if (ui->cutsceneEveryMission->isChecked())
		{
			// mission class name
			missionClassName = ui->missionTagName->text();
			// inject zeros
			missionClassName = injectMissionDigit(missionClassName, i);
			missionClassName.append(QString::number(i) + "Cut");
			
			// mission class name for succesfully complete mission, ie next mission name.
			missionClassNameNext = ui->missionTagName->text();
			missionClassNameNext = injectMissionDigit(missionClassNameNext, (i + 1));
			missionClassNameNext.append(QString::number(i + 1));
			
			// mission file name plus extension of terrain name.
			missionFileName = ui->missionTagName->text();
			missionFileName = injectMissionDigit(missionFileName, i);
			// real human readable name
			missionRealName = missionFileName;
			missionRealName.append(QString::number(i));
			missionFileName.append(QString::number(i) + "Cut." + terrainName);
			
			// put in our stuff into string
			str1.append("\n\t\tclass " + missionClassName + ": MissionDefault\n\t\t{\n\t\t\tend1 = " + missionClassNameNext + ";\n\t\t\tlost = " + missionClassName + ";\n\t\t\ttemplate = " + missionFileName + ";\n\t\t};\n");
			
			// write missions dir
			//CreateMissionDir(missionFileName, CamDir);

			// write cutscene dir
			CreateCutsceneDir(missionFileName, CamDir);
		}
		
		// final mission entry, no proceeding mission class in this one, campaign ends here, finito.
		if (i + 1 == numberOfMissions)
		{
			// mission class name
			missionClassName = ui->missionTagName->text();
			missionClassName = injectMissionDigit(missionClassName, (i + 1));
			missionClassName.append(QString::number(i + 1));
			
			// ending cutscene optional checkbox
			if (ui->endingCutscene->isChecked())
			{
				// ending cutscene class name
				missionClassNameNext = ui->missionTagName->text();
				missionClassNameNext.append("EC");
			}
			else
			{
				// empty, no more missions
				missionClassNameNext = "";
			}
			
			// mission file name plus extension of terrain name.
			missionFileName = ui->missionTagName->text();
			missionFileName = injectMissionDigit(missionFileName, (i + 1));
			// real human readable name
			missionRealName = missionFileName;
			missionRealName.append(QString::number(i));
			missionFileName.append(QString::number(i + 1) + "." + terrainName);
		
			// write it down
			str1.append("\n\t\t//Final mission!\n\t\tclass " + missionClassName + ": MissionDefault\n\t\t{\n\t\t\tend1 = " + missionClassNameNext + ";\n\t\t\tlost = " + missionClassName + ";\n\t\t\ttemplate = " + missionFileName + ";\n\t\t};\n");

			// write missions dir
			CreateMissionDir(missionFileName, CamDir, missionRealName);

			// ending cutscene if checkbox is on
			if (ui->endingCutscene->isChecked())
			{
				missionClassName = ui->missionTagName->text();
				missionClassName.append("EC");
				
				// next mission, no, we end after the cutscene
				missionClassNameNext = "";
				
				missionFileName = ui->missionTagName->text();
				missionFileName.append("EC");
				// real human readable name
				missionRealName = missionFileName;
				missionFileName.append("." + terrainName);
				
				// write it down
				str1.append("\n\t\t//Ending cutscene!\n\t\tclass " + missionClassName + ": MissionDefault\n\t\t{\n\t\t\tend1 = " + missionClassNameNext + ";\n\t\t\tlost = " + missionClassName + ";\n\t\t\ttemplate = " + missionFileName + ";\n\t\t};\n");
				
				// write missions dir
				//CreateMissionDir(missionFileName, CamDir);

				// write cutscene dir
				CreateCutsceneDir(missionFileName, CamDir);
			}
		}
	}

	// remaining closing curly braces
	str1.append("\t};\n};\n");

	QTextStream fish1(&des);
	fish1 << str1;
	des.close();

	// overview.html creation
	CreateOverviewHTML(file, CamDir);
	
	// final words before exit
	ui->StatusBox->append("All done, campaign is created!");
}

/*

Create mission directory with mission.sqm, description.ext, init.sqf etc using the external function

*/
void Widget::CreateMissionDir(QString missionFileName, QString CamDir, QString missionRealName)
{
	QDir file;
	file.cd(CamDir);
	file.cd("Missions");
	file.mkdir (missionFileName);

	// another class comes from mission_generator.h / cpp !
	Mission_Generator gObj;
	gObj.setName(missionRealName);
	gObj.setDescription(missionRealName);
	gObj.Do_Mission(CamDir + "\\" + "Missions" + "\\" + missionFileName);

	// copies overview.html, paa and briefing.html for every mission dir using our resource dir as source
	QFile::copy(":/Resource/overview_mission.html", CamDir + "\\" + "Missions" + "\\" + missionFileName + "\\" + "overview.html");
	QFile::copy(":/Resource/overview_mission_ca.paa", CamDir + "\\" + "Missions" + "\\" + missionFileName + "\\" + "overview_mission_ca.paa");
	QFile::copy(":/Resource/briefing.html", CamDir + "\\" + "Missions" + "\\" + missionFileName + "\\" + "briefing.html");
	
	// remove read-only property from the copied file
	QFile foo(CamDir + "\\" + "Missions" + "\\" + missionFileName + "\\" + "overview.html");
	foo.setPermissions(QFile::WriteUser);
	foo.close();
	
	QFile foo2(CamDir + "\\" + "Missions" + "\\" + missionFileName + "\\" + "overview_mission_ca.paa");
	foo2.setPermissions(QFile::WriteUser);
	foo2.close();
	
	QFile foo3(CamDir + "\\" + "Missions" + "\\" + missionFileName + "\\" + "briefing.html");
	foo3.setPermissions(QFile::WriteUser);
	foo3.close();
}


/*

Create cutscene directory with the already made template mission.sqm and movie.sqf from resource dir

*/
void Widget::CreateCutsceneDir(QString missionFileName, QString CamDir)
{
	QDir file;
	file.cd(CamDir);
	file.cd("Missions");
	file.mkdir (missionFileName);

	// copies template files into cutscene dir using our resource dir as source
	QFile::copy(":/Resource/Cutscene/mission.sqm", CamDir + "\\" + "Missions" + "\\" + missionFileName + "\\" + "mission.sqm");
	QFile::copy(":/Resource/Cutscene/movie.sqf", CamDir + "\\" + "Missions" + "\\" + missionFileName + "\\" + "movie.sqf");
	
	// remove read-only property from the copied file
	QFile foo(CamDir + "\\" + "Missions" + "\\" + missionFileName + "\\" + "mission.sqm");
	foo.setPermissions(QFile::WriteUser);
	foo.close();
	
	QFile foo2(CamDir + "\\" + "Missions" + "\\" + missionFileName + "\\" + "movie.sqf");
	foo2.setPermissions(QFile::WriteUser);
	foo2.close();
	
	// copies overview.html, paa and briefing.html for every mission dir using our resource dir as source
	QFile::copy(":/Resource/overview_mission.html", CamDir + "\\" + "Missions" + "\\" + missionFileName + "\\" + "overview.html");
	QFile::copy(":/Resource/overview_mission_ca.paa", CamDir + "\\" + "Missions" + "\\" + missionFileName + "\\" + "overview_mission_ca.paa");
	QFile::copy(":/Resource/briefing.html", CamDir + "\\" + "Missions" + "\\" + missionFileName + "\\" + "briefing.html");

	// remove read-only property from the copied file
	QFile foo3(CamDir + "\\" + "Missions" + "\\" + missionFileName + "\\" + "overview.html");
	foo3.setPermissions(QFile::WriteUser);
	foo3.close();
	
	QFile foo4(CamDir + "\\" + "Missions" + "\\" + missionFileName + "\\" + "overview_mission_ca.paa");
	foo4.setPermissions(QFile::WriteUser);
	foo4.close();
	
	QFile foo5(CamDir + "\\" + "Missions" + "\\" + missionFileName + "\\" + "briefing.html");
	foo5.setPermissions(QFile::WriteUser);
	foo5.close();
}


/*

PMCTODO Make this as resource file. then use replace string function to just replace the PAA file name to mission specific one, or not.
but the point is, make this as resource file.

*/
void Widget::CreateOverviewHTML(QDir file, QString CamDir)
{
	// create overview.html file
	QString overview_html = file.absolutePath() + "/overview.html";
	QFile overhtml(overview_html);
	overhtml.open(QIODevice::WriteOnly | QIODevice::Text);

	// fill up overview.html contents
	QString str = "<html>\n<head>\n<title>Overview</title>\n</head>\n";
	str.append(CamDir  + "\n");
	str.append("<body>\n<p><img src=\"overview_512x512_ca.paa\" width=\"240\" height=\"238\"></p>\n\n");
	str.append("<p>" + ui->OverviewText->text() + "\n</p>\n");
	str.append("\n</body>\n</html>\n");
	ui->StatusBox->append("Overview.html is written");

	QTextStream kala(&overhtml);
	kala << str;
	overhtml.close();

	// copy the .paa image with it
	QFile::copy(":/Resource/overview_campaign_ca.paa", file.absolutePath() + "/overview_512x512_ca.paa");
	// remove read-only property from the copied file
	QFile foo(file.absolutePath() + "/overview_512x512_ca.paa");
	foo.setPermissions(QFile::WriteUser);
	foo.close();
}


/*

  Add the zeros to class and file names so they are sorted properly in-game

*/
QString Widget::injectMissionDigit(QString missionClassName, int i)
{
	// inject <TAG>00<NUMBER> type stuff so they are sorted properly in mission editor
	// inject 00
	if (i < 10)
	{
		missionClassName.append("00");
	}
	// inject 0
	if (i > 9 && i < 100)
	{
		missionClassName.append("0");
	}

	return missionClassName;
}
