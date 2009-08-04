/*

Conversation widget for kdm greeter

Copyright (C) 1997, 1998, 2000 Steffen Hansen <hansen@kde.org>
Copyright (C) 2000-2003 Oswald Buddenhagen <ossi@kde.org>
Copyright (C) 2009 Jaroslav Barton <djaara@djaara.net>

 Adapted to PAM Face Authentication - during Google Summer of Code 2009
Copyright (C) 2009 Rohan Anil <rohan.anil@gmail.com>


This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/


#include "kgreet_faceauthenticate.h"
#include <qapplication.h>
#include <kglobal.h>
#include <klocale.h>
#include <klineedit.h>
#include <kuser.h>
#include <assert.h>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QImage>
#include <QRegExp>
#include <QLayout>
#include <QLabel>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <QX11EmbedContainer>

#define IPC_KEY_IMAGE 567814
#define IPC_KEY_STATUS 567813
#define IMAGE_SIZE 230400
#define IMAGE_WIDTH 320
#define IMAGE_HEIGHT 240

KFaceAuthenticateGreeter::KFaceAuthenticateGreeter( KGreeterPluginHandler *_handler,
        QWidget *parent,
        const QString &_fixedEntity,
        Function _func, Context _ctx ) :
        QObject(),
        KGreeterPlugin( _handler ),
        fixedUser( _fixedEntity ),
        func( _func ),
        ctx( _ctx ),
        running( false ),
        authStarted( false )
{
    QGridLayout *grid = 0;
    int line = 0;


        parent = new QWidget( parent );
        parent->setObjectName( "talker" );
        widgetList << parent;
        grid = new QGridLayout( parent );
        grid->setMargin( 0 );


    loginLabel = faceauthenticateStatus =  0;
    loginEdit = 0;

    if (ctx == ExUnlock)
        fixedUser = KUser().loginName();
    if (func != ChAuthTok)
    {
        if (fixedUser.isEmpty())
        {
            loginEdit = new KLineEdit( parent );
            loginEdit->setContextMenuPolicy( Qt::NoContextMenu );
            connect( loginEdit, SIGNAL(editingFinished()), SLOT(slotChanged()) );
            connect( loginEdit, SIGNAL(textChanged( const QString & )), SLOT(slotChanged()) );
            connect( loginEdit, SIGNAL(selectionChanged()), SLOT(slotChanged()) );


                loginLabel = new QLabel( i18n( "&Username:" ), parent );
                loginLabel->setBuddy( loginEdit );
                grid->addWidget( loginLabel, line, 0 );
                grid->addWidget( loginEdit, line++, 1 );

        }
        else if (ctx != Login && ctx != Shutdown && ctx != ExUnlock && grid)
        {
            loginLabel = new QLabel( i18n( "Username:" ), parent );
            grid->addWidget( loginLabel, line, 0 );
            grid->addWidget( new QLabel( fixedUser, parent ), line++, 1 );
        }

        faceauthenticateStatus = new QLabel();
        grid->addWidget( faceauthenticateStatus, line++, 0, 1, 2 );


        //ipcStart();
        frame =new QX11EmbedContainer();
        grid->addWidget( frame, line, 0, 2, 2 );

        if (loginEdit)
        {
            loginEdit->setFocus();
        }

        if (ctx == ExUnlock)
        {
            faceauthenticateStatus->setText( i18n( "Click on Unlock button." ) );
        }
    }
}


// virtual
KFaceAuthenticateGreeter::~KFaceAuthenticateGreeter()
{
    abort();
    qDeleteAll( widgetList );
}

void // virtual
KFaceAuthenticateGreeter::loadUsers( const QStringList &users )
{
    void showText();
    KCompletion *userNamesCompletion = new KCompletion;
    userNamesCompletion->setItems( users );
    loginEdit->setCompletionObject( userNamesCompletion );
    loginEdit->setAutoDeleteCompletionObject( true );
    loginEdit->setCompletionMode( KGlobalSettings::CompletionAuto );
}

void // virtual
KFaceAuthenticateGreeter::presetEntity( const QString &entity, int field )
{
    loginEdit->setText( entity );
    if (field == 0)
    {
        loginEdit->setFocus();
        loginEdit->selectAll();
    }
    curUser = entity;
}

QString // virtual
KFaceAuthenticateGreeter::getEntity() const
{
    return fixedUser.isEmpty() ? loginEdit->text() : fixedUser;
}

void // virtual
KFaceAuthenticateGreeter::setUser( const QString &user )
{
    // assert( fixedUser.isEmpty() );
    curUser = user;
    loginEdit->setText( user );
}

void // virtual
KFaceAuthenticateGreeter::setEnabled( bool enable )
{
    // assert( func == Authenticate && ctx == Shutdown );
    if (loginLabel)
    {
        loginLabel->setEnabled( enable );
        loginEdit->setEnabled( enable );
    }

    faceauthenticateStatus->setEnabled( enable );

    setActive( enable );
}

void // private
KFaceAuthenticateGreeter::returnData()
{
    handler->gplugReturnText( (loginEdit ? loginEdit->text():fixedUser).toLocal8Bit(),KGreeterPluginHandler::IsUser );
}

bool // virtual
KFaceAuthenticateGreeter::textMessage( const char *text, bool err )
{
    if (text!=0)
    {
        QString msg = QString(text);
        faceauthenticateStatus->setText( i18n(msg.toLatin1()) );
        if ( msg.indexOf("Face Verification Pluggable Authentication Module Started" ) > -1 )
        {
            QStringList arguments;
            arguments << QStringList()<< QString::number(frame->winId());
            faceAuthGUI.start(BINDIR "/xwindowFaceAuth",arguments);
            frame->resize(IMAGE_WIDTH,IMAGE_HEIGHT);
            frame->show();

        }//startTimerstartTimer(80);
        return true;
    }

    return false;

}

void // virtual
KFaceAuthenticateGreeter::textPrompt( const char *prompt, bool echo, bool nonBlocking )
{
    returnData();
}

bool // virtual
KFaceAuthenticateGreeter::binaryPrompt( const char *, bool )
{
    return true;
}

void // virtual
KFaceAuthenticateGreeter::start()
{
    running = true;
}

void // virtual
KFaceAuthenticateGreeter::suspend()
{
}

void // virtual
KFaceAuthenticateGreeter::resume()
{
}

void // virtual
KFaceAuthenticateGreeter::next()
{
    // assert( running );
    setActive(false);
    authStarted = true;
    handler->gplugStart();
    //returnData();
}

void // virtual
KFaceAuthenticateGreeter::abort()
{
    if ( authStarted )
    {
        handler->gplugReturnText( 0, 0 );
    }
    //animLabel->stop();
    faceauthenticateStatus->clear();
    authStarted = false;
    running = false;
}

void // virtual
KFaceAuthenticateGreeter::succeeded()
{
    // assert( running || timed_login );
    if (!authTok)
    {
        setActive( false );
    }
    running = false;
    authStarted = false;
    //animLabel->stop();
    faceauthenticateStatus->setText(i18n("Succeeded"));
}

void // virtual
KFaceAuthenticateGreeter::failed()
{
    // assert( running || timed_login );
    setActive( false );
    faceauthenticateStatus->clear();
    running = false;
    authStarted = false;
    //animLabel->stop();
}

void // virtual
KFaceAuthenticateGreeter::revive()
{
    // assert( !running );
    // assert( authTok );

    setActive( true );
    loginEdit->setFocus();
}

void // virtual
KFaceAuthenticateGreeter::clear()
{
    if (loginEdit)
    {
        loginEdit->clear();
        loginEdit->setFocus();
        curUser.clear();
    }
    faceauthenticateStatus->clear();
// 	QApplication::processEvents();
}


// private

void
KFaceAuthenticateGreeter::setActive( bool enable )
{
    if (loginEdit)
        loginEdit->setEnabled( enable );
    //faceauthenticateStatus->setEnabled( enable );
}

void
KFaceAuthenticateGreeter::slotChanged()
{
    if ( running )
    {
        loginEdit->setText( loginEdit->text().trimmed() );

        if (curUser != loginEdit->text())
        {
            curUser = loginEdit->text();
            handler->gplugSetUser( curUser );
        }

        handler->gplugChanged();
    }
}

// factory

static bool init( const QString &,
                  QVariant (*getConf)( void *, const char *, const QVariant & ),
                  void *ctx )
{
    Q_UNUSED(getConf);
    Q_UNUSED(ctx);
    KGlobal::locale()->insertCatalog( "kgreet_faceauthenticate" );
    return true;
}

static void done( void )
{
    KGlobal::locale()->removeCatalog( "kgreet_faceauthenticate" );
}

static KGreeterPlugin *
create( KGreeterPluginHandler *handler,
        QWidget *parent,
        const QString &fixedEntity,
        KGreeterPlugin::Function func,
        KGreeterPlugin::Context ctx )
{
    return new KFaceAuthenticateGreeter( handler, parent, fixedEntity, func, ctx );
}

KDE_EXPORT KGreeterPluginInfo kgreeterplugin_info =
{
    I18N_NOOP2("@item:inmenu authentication method", "Username + Face Verification"), "faceauthenticate",
    KGreeterPluginInfo::Local | KGreeterPluginInfo::Presettable,
    init, done, create
};

#include "kgreet_faceauthenticate.moc"
