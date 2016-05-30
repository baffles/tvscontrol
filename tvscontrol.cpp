#include "tvscontrol.h"

// -1 means no corresponding led
const int TVSControl::LED[14] = { -1,  // Black
								   0,  // HDMI 1
								   1,  // HDMI 2
								   2,  // HDMI/SDI 3
								   5,  // HDMI/SDI 4
								   3,  // SDI 5
								  -1,  // SDI 6
								  -1,  // BARS
								  -1,  // Color 1
								  -1,  // Color 2
								   4,  // Media 1 Fill
								  -1,  // Media 1 Key
								  -1,  // Media 2 Fill
								  -1}; // Media 2 Key

const int TVSControl::SOURCE[6] = { 1,  // HDMI 1
									2,  // HDMI 2
									3,  // HDMI/SDI 3
									5,  // SDI 5
								   10,  // Media 1 Fill
									4}; // HDMI/SDI 4

//Constructor:
TVSControl::TVSControl(QObject* parent, QString tvsaddr)
	: QObject(parent)
{
	hasAtem = false;
	hasXKey = false;
	atemAddr = tvsaddr;

	xkeys = new QXKey24(this);
	atem  = new QAtemConnection(this);
	//f   = new QFileManager(this);
	
	connect(atem,  SIGNAL(   connected()), this, SLOT(   atemConnected()));
	connect(atem,  SIGNAL(disconnected()), this, SLOT(atemDisconnected()));
	connect(xkeys, SIGNAL(   panelConnected()), this, SLOT(   xkeyPanelConnected()));
	connect(xkeys, SIGNAL(panelDisconnected()), this, SLOT(xkeyPanelDisconnected()));

	// ATEM Status Signals:
	connect(atem, SIGNAL(fadeToBlackChanged(bool, bool)),       this, SLOT(atemFTBChanged(bool, bool)));
	connect(atem, SIGNAL(programInputChanged(quint8, quint8)),  this, SLOT(atemProgramChanged(quint8, quint8)));
	connect(atem, SIGNAL(previewInputChanged(quint8, quint8)),  this, SLOT(atemPreviewChanged(quint8, quint8)));
	connect(atem, SIGNAL(downstreamKeyOnChanged(quint8, bool)), this, SLOT(atemDSKOnChanged(quint8, bool)));
	connect(atem, SIGNAL(upstreamKeyOnChanged(quint8, bool)),   this, SLOT(atemUSKOnChanged(quint8, bool)));
	connect(atem, SIGNAL(mediaPlayerChanged(quint8, quint8, quint8, quint8)),
																this, SLOT(atemMediaSelectChanged(quint8, quint8, quint8, quint8) ));

	// XKeys Button Signals:
	connect(xkeys, SIGNAL(buttonDown(unsigned int)), this, SLOT(xkeyButtonDown(unsigned int)));
	//connect(xkeys, SIGNAL(buttonUp(unsigned int, unsigned int, unsigned int)), this, SLOT(xkeyButtonUp(unsigned int, unsigned int, unsigned int)));


	QHostAddress a;
	if(!a.setAddress(tvsaddr))
	{
		QByteArray ba = tvsaddr.toLocal8Bit();
		cerr << "TVSControl: Bad Network Address For ATEM TVS " << ba.data() << endl;
		cerr << "TVSControl: Terminating" << endl;
		QTimer::singleShot(500, parent, SLOT(quit()));
	}
	else
	{
		atem->connectToSwitcher(a);
	}
}

//Destructor
TVSControl::~TVSControl()
{
	disconnect(atem,  SIGNAL(disconnected()), this, SLOT(atemDisconnected()));
	atem->disconnectFromSwitcher();
	delete atem;

	if(hasXKey) {
		xkeys->setPanelLED(GRN_LED, BLINK);
		xkeys->setPanelLED(RED_LED, OFF);
		hasAtem = false;
		setAllLEDs(); // turns all off
	}
	delete xkeys;
	//delete filemanagerthingy;
}


//private:
void
TVSControl::readyState()
{
	if(!hasXKey || !hasAtem) {
		return;
	}

	xkeys->setPanelLED(RED_LED, OFF); // stop blink
	setAllLEDs();
}


//private slot:
void
TVSControl::xkeyPanelConnected()
{
	disconnect(xkeys, SIGNAL(panelConnected()), this, SLOT(xkeyPanelConnected()));
	connect(xkeys, SIGNAL(panelDisconnected()), this, SLOT(xkeyPanelDisconnected()));
	hasXKey = true;
	
	xkeys->setPanelLED(GRN_LED, ON);
	xkeys->setPanelLED(RED_LED, BLINK);
	setAllLEDs();

	if(hasAtem) {
		readyState();
	}
}

//private slot:
void
TVSControl::xkeyPanelDisconnected()
{
	hasXKey = false;
	disconnect(xkeys, SIGNAL(panelDisconnected()), this, SLOT(xkeyPanelDisconnected()));
	   connect(xkeys, SIGNAL(   panelConnected()), this, SLOT(   xkeyPanelConnected()));
}


//private slot:
void
TVSControl::atemConnected()
{
qDebug() << "ATEM Version " << atem->majorVersion() << "." << atem->minorVersion();
	qDebug() << "TVSConnect: Using ATEM TVS at " << atemAddr;
	hasAtem = true;
	
	// Default Atem Settings  // DJC: finish these
	// audio
	// usk ramp
	// bug
	// mix
	// mix rate
	// ftb rate
	// dsk 1 set to mp1
	// dsk 2 set to mp2
	// transition background only

	if(hasXKey) {
		readyState();
	}
}

//private slot:
void
TVSControl::atemDisconnected()
{
	qDebug() << "TVSConnect: Disconnected from ATEM TVS.";
	QHostAddress a;
	a.setAddress(atemAddr);
	atem->connectToSwitcher(a);
	
	if(hasXKey) {
		xkeys->setPanelLED(RED_LED, BLINK);
	}
	
	hasAtem = false;
}

// public slot:
void
TVSControl::atemProgramChanged(quint8 oldIndex, quint8 newIndex)
{
	int oldButton = LED[oldIndex];
	int newButton = LED[newIndex];

	// "-1" indicates no button
	if(oldButton >= 0) {
		xkeys->setButtonRedLEDState(oldButton, OFF);
	}
	
	if(newButton >= 0) {
		xkeys->setButtonRedLEDState(newButton, ON);
	}
}


// public slot:
void
TVSControl::atemPreviewChanged(quint8 oldIndex, quint8 newIndex)
{
	int oldButton = LED[oldIndex] + 8;
	int newButton = LED[newIndex] + 8;

	// "-1" indicates no button
	if(oldButton >= 0) {
		xkeys->setButtonBlueLEDState(oldButton, OFF);
	}
	
	if(newButton >= 0) {
		xkeys->setButtonBlueLEDState(newButton, ON);
	}
}


// public slot:
void
TVSControl::atemDSKOnChanged(quint8 keyer, bool enabled)
{
	xkeys->setButtonRedLEDState( 27+keyer, enabled? ON : OFF);
	xkeys->setButtonBlueLEDState(27+keyer, enabled? ON : OFF);
}


// public slot:
void
TVSControl::atemUSKOnChanged(quint8 keyer, bool enabled)
{
	xkeys->setButtonRedLEDState( 29, enabled? ON : OFF);
	xkeys->setButtonBlueLEDState(29, enabled? ON : OFF);
}


// public slot:
void
TVSControl::atemFTBChanged(bool transitioning, bool enabled)
{
	if(!transitioning) {
		xkeys->setButtonRedLEDState(26, enabled? ON : OFF);
	} else {
		xkeys->setButtonRedLEDState(26, BLINK);
	}
}


// public slot:
void
TVSControl::atemMediaSelectChanged(quint8 player, quint8 type, quint8 still, quint8 clip)
{
	if(player != 0) {
		return;
	}
	
	// clear all buttons
	for(int i = 0; i < 6; i++) {
		xkeys->setButtonBlueLEDState(16+i, OFF);
	}
	
	if(still < 6) {
		xkeys->setButtonBlueLEDState(16+still, ON);
	}
}


//private:
void
TVSControl::setAllLEDs()
{
	if(!hasAtem) {
		for(int i = 0; i < 32; i++) {
			xkeys->setButtonBlueLEDState(i, OFF);
			xkeys->setButtonRedLEDState( i, OFF);
		}
		return;
	}
	
	// Query the atem for current status
	int p = atem->programInput();
	atemProgramChanged(p,p);
	    p = atem->previewInput();
	atemPreviewChanged(p,p);
	atemDSKOnChanged(0, atem->downstreamKeyOn(0));
	atemDSKOnChanged(1, atem->downstreamKeyOn(1));
	atemUSKOnChanged(0,   atem->upstreamKeyOn(0));
	atemFTBChanged(false, false);
	atemMediaSelectChanged(0, 0, atem->mediaPlayerSelectedStill(0), 0);
}


//private slot:
void
TVSControl::xkeyButtonDown(unsigned int button)
{
	if(!hasAtem) {
		return;
	}
		
	switch(button)
	{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			atem->changePreviewInput(SOURCE[button]);
			xkeys->setButtonRedLEDState(button, BLINK);
			atem->doAuto();
			// DJC: set a timer to change it back?
			break;
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
			atem->changePreviewInput(SOURCE[button-8]);
			break;
		case 16:
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
			atem->setMediaPlayerSource(0, false, button-16);
			break;
		case 24:
			atem->doCut();
			break;
		case 25:
			if(LED[atem->previewInput()] != -1) {
				xkeys->setButtonRedLEDState(LED[atem->previewInput()], BLINK);
			}
			atem->doAuto();
			break;
		case 26:
			atem->toggleFadeToBlack();
			break;
		case 27:
		case 28:
			xkeys->setButtonRedLEDState(button, BLINK);
			atem->doDownstreamKeyAuto(button-27);
			break;
		case 29:
			atem->setUpstreamKeyOn(0, !atem->upstreamKeyOn(0));
			//xkeys->setButtonRedLEDState(29, BLINK);
			//atem->setUpstreamKeyOnNextTransition(0, true);
			//atem->setBackgroundOnNextTransition(false);
			//atem->doAuto();
			//atem->setBackgroundOnNextTransition(true);
			//atem->setUpstreamKeyOnNextTransition(0, false);
			break;
		default:
			cerr << "TVSConnect: Received bad button press. Button " << button << endl;
	}
}
