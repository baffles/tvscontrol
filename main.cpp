#include <iostream>
#include <QCoreApplication>
#include <QStringList>
#include <QTimer>
#include "tvscontrol.h"

using namespace std;

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);
  
  QStringList s = QCoreApplication::arguments();
  if(s.length() < 2) {
  	cout << "usage: tvscontrol atem_address ..." << endl
         << "       start tvscontrol" << endl;
    return 0;
  }
  
  TVSControl t(NULL, QCoreApplication::arguments().at(1));
  //QTimer::singleShot(40000, &a, SLOT(quit()));
  
  return a.exec();
}
