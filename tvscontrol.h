#ifndef TVSCONTROL_H
#define TVSCONTROL_H

#include <iostream>
#include <QTimer>
#include <QObject>
#include <QDebug>
#include <QHostAddress>
#include <QByteArray>
#include <qatemconnection.h> 
#include <qxkey24.h>

class TVSControl : public QObject
{
	Q_OBJECT

public:
	 TVSControl(QObject* parent, QString tvsaddr);
	~TVSControl();
	
private:
	QTimer  *timer;
	QXKey24 *xkeys;
	QAtemConnection *atem;
	//QFileManager f;
	
	QString atemAddr;
	bool hasAtem;
	bool hasXKey;
	
	void setAllLEDs();
	void readyState();
	
	static const int LED[14];
	static const int SOURCE[6];

private slots:
	void xkeyPanelDisconnected();
	void    xkeyPanelConnected();
	void xkeyButtonDown(unsigned int button);
	void atemDisconnected();
	void    atemConnected();
	void atemFTBChanged(bool fading, bool enabled);
	void atemProgramChanged(quint8 oldIndex, quint8 newIndex);
	void atemPreviewChanged(quint8 oldIndex, quint8 newIndex);
	void atemDSKOnChanged(quint8 keyer, bool enabled);
	void atemUSKOnChanged(quint8 keyer, bool enabled);
	void atemMediaSelectChanged(quint8 player, quint8 type, quint8 still, quint8 clip);
};

#endif //TVSCONTROL_H