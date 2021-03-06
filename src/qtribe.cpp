/***************************************************************************
 *   Copyright (C) 2008 by Pete   *
 *   pete@marchingcubes.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include "qtribe.h"


#include <q3mainwindow.h>
#include <qdir.h>
//Added by qt3to4:
#include <QCloseEvent>

#include "stepsequencerwidget.h"
#include "jackIO.h"

#ifndef DEFAULT_BANK_DIR
#define DEFAULT_BANK_DIR "/usr/share/qtribe"
#endif

qTribe::qTribe()
    : Q3MainWindow( 0, appName, Qt::WDestructiveClose )
	{
        //fprintf(stderr,"stepsequencer:stepsequencer()\n");
        
        sequencerThread=new sequencerCore();
        
        sequencerThread->initSequencer();       
        QString file("qtribe.bank");
        QString homePath=QDir::homeDirPath()+QDir::separator()+".qtribe"+QDir::separator()+file;
        QString defaultPath=DEFAULT_BANK_DIR+QDir::separator()+file;

	//check if our bankfile exists here
	if (QFile::exists(homePath))
		{
		fprintf(stdout,"Loading Bankfile from path: %s\n",homePath.ascii());
        	sequencerThread->loadBank((char*)homePath.ascii());
		}
	else if (QFile::exists(defaultPath))
		{
		fprintf(stdout,"Loading Bankfile from path: %s\n",defaultPath.ascii());
        	sequencerThread->loadBank((char*)defaultPath.ascii());
		}
	//if not see if we can load it from /usr/share/qtribe/
	else
		{
		fprintf(stdout,"Creating new Bank\n");
		sequencerThread->createBank();
		}

	if (! QFile::exists(homePath))
		{
		//if our dotfile doesnt exist, we should create it.
		QDir d(QDir::homeDirPath());
		if (!d.exists(".qtribe"))
			{
			//dotdir exists, just no file saved in it.
			}
		else
			{
			if (!d.mkdir(".qtribe"))
				{
				fprintf(stdout,"Created ~/.qtribe\n");
				}
			else
				{
				fprintf(stderr,"ERROR: Couldn't create ~/.qtdir - check permissions!\n");
				sequencerThread->stopSequence();
       				sequencerThread->cleanup();
        			disconnectJACK();
				exit(0);
				}
			}
		}
	
	
		

        stepsequencerWidget* sequencerUI=new stepsequencerWidget( this );
        sequencerUI->setBankFile((char*)homePath.ascii());
        
        sequencerThread->start();
        
        setCentralWidget( (QWidget*)sequencerUI);
        resize( 800, 480 );
	//fprintf(stderr,"stepsequencer:stepsequencer - thread pointer %d\n",sequencerThread);
	}


qTribe::~qTribe()
	{
	//fprintf(stderr,"stepsequencer:~stepsequencer()\n");
        sequencerThread->cleanup();
        delete sequencerThread;   
	}


sequencerCore* qTribe::getSequencerThread()
        {
        //fprintf(stderr,"stepsequencer::getSequencerThread() - returning %d\nsequencerThread);
        return sequencerThread;
        }

void qTribe::closeEvent(QCloseEvent* e)
	{
	sequencerThread->stopSequence();
        sequencerThread->cleanup();
        //fprintf(stderr,"Got close event!\n");
        disconnectJACK();
	e->accept();
	exit(0);
	}





