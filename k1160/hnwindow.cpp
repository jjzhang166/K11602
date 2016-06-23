#include "hnwindow.h"
#include "ui_hnwindow.h"
#include "hnobjectfactory.h"
#include "hngui-qt.h"
#include "HNDefine.h"

HNWindow::HNWindow(QWidget *parent) :
    QStackedWidget(parent),
    ui(new Ui::HNWindow)
{
    ui->setupUi(this);

    HNObjectFactory::registerObject(this);

#ifdef __MIPS_LINUX__
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
#endif
    setFixedSize(1024, 768);
    moveCenter(this);

    QSettings setting;
    int bOpenRights = 1;
    bOpenRights = setting.value("UserRights", bOpenRights).toInt();
    int bSelfCheck = 1;
    bSelfCheck = setting.value("SelfCheck", bSelfCheck).toInt();

    pline() << bSelfCheck << bOpenRights;
    if(bSelfCheck)
    {
        setCurrentIndex(0);
        return;
    }
    else if(bOpenRights)
    {
        setCurrentIndex(1);
        return;
    }

    //如果不知道admin的id；设定admin id=0
    QSettings set;
    QString did = set.value("DefaultLogin", 0).toString();

    QSqlQuery query(managerDB);
    if(did == 0)
        query.exec(QString("SELECT * FROM %1 WHERE Name = 'Admin'")
                   .arg(TABLE_USERINFO));
    else
        query.exec(QString("SELECT * FROM %1 WHERE id = '%2'")
                   .arg(TABLE_USERINFO).arg(did));

    if (query.next()) {
        gUserName = query.value(Auth_Name).toString();
        gPassword = query.value(Auth_Passwd).toString();
    }

    query.finish();

    setCurrentIndex(2);
}

HNWindow::~HNWindow()
{
    delete ui;
}
