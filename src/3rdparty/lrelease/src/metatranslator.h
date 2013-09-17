/****************************************************************************
**
** Copyright (C) 1992-2007 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Linguist of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef METATRANSLATOR_H
#define METATRANSLATOR_H

#include "translator.h"
#include <QMap>
#include <QString>
#include <QList>
#include <QtCore/QDir>

class QIODevice;
class QTextCodec;

class MetaTranslatorMessage : public TranslatorMessage
{
public:
    enum Type { Unfinished, Finished, Obsolete };

    MetaTranslatorMessage();
    MetaTranslatorMessage( const char *context, const char *sourceText,
                           const char *comment, const QString &fileName,
                           int lineNumber,
                           const QStringList& translations = QStringList(),
                           bool utf8 = false, Type type = Unfinished, bool plural = false );
    MetaTranslatorMessage( const MetaTranslatorMessage& m );

    MetaTranslatorMessage& operator=( const MetaTranslatorMessage& m );

    void setType( Type nt ) { ty = nt; }
    Type type() const { return ty; }
    bool utf8() const { return utfeight; }
    bool isPlural() const { return m_plural; }
    void setPlural(bool isplural) { m_plural = isplural; }
    bool operator==( const MetaTranslatorMessage& m ) const;
    bool operator!=( const MetaTranslatorMessage& m ) const
    { return !operator==( m ); }
    bool operator<( const MetaTranslatorMessage& m ) const;
    bool operator<=( const MetaTranslatorMessage& m )
    { return !operator>( m ); }
    bool operator>( const MetaTranslatorMessage& m ) const
    { return this->operator<( m ); }
    bool operator>=( const MetaTranslatorMessage& m ) const
    { return !operator<( m ); }

private:
    bool utfeight;
    Type ty;
    bool m_plural;
};

class MetaTranslator
{
public:
    MetaTranslator();
    MetaTranslator( const MetaTranslator& tor );

    MetaTranslator& operator=( const MetaTranslator& tor );

    void clear();
    bool load( const QString& filename );
    bool save( const QString& filename ) const;
    bool release( const QString& filename, bool verbose = false,
                  bool ignoreUnfinished = false,
                  Translator::SaveMode mode = Translator::Stripped ) const;
    bool release( QIODevice *iod, bool verbose = false,
                  bool ignoreUnfinished = false,
                  Translator::SaveMode mode = Translator::Stripped ) const;

    bool contains( const char *context, const char *sourceText,
                   const char *comment ) const;
                   
    MetaTranslatorMessage find( const char *context, const char *sourceText,
                   const char *comment ) const;

    MetaTranslatorMessage find(const char *context, const char *comment, 
                    const QString &fileName, int lineNumber) const;
    
    void insert( const MetaTranslatorMessage& m );

    void stripObsoleteMessages();
    void stripEmptyContexts();

	void setCodec( const char *name ); // kill me
	void setCodecForTr( const char *name ) { setCodec(name); }
	QTextCodec *codecForTr() const { return codec; }
    QString toUnicode( const char *str, bool utf8 ) const;

    QString languageCode() const;
    static void languageAndCountry(const QString &languageCode, QLocale::Language *lang, QLocale::Country *country);
    void setLanguageCode(const QString &languageCode);
    QList<MetaTranslatorMessage> messages() const;
    QList<MetaTranslatorMessage> translatedMessages() const;
    static int grammaticalNumerus(QLocale::Language language, QLocale::Country country);
    static QStringList normalizedTranslations(const MetaTranslatorMessage& m, 
                    QLocale::Language lang, QLocale::Country country);

private:
    void makeFileNamesAbsolute(const QDir &oldPath);

    typedef QMap<MetaTranslatorMessage, int> TMM;       // int stores the sequence position.
    typedef QMap<int, MetaTranslatorMessage> TMMInv;    // Used during save operation. Seems to use the map only the get the sequence order right.

    bool saveTS( const QString& filename ) const;
    bool saveXLIFF( const QString& filename) const;

    TMM mm;
    QByteArray codecName;
    QTextCodec *codec;
    QString m_language;     // A string beginning with a 2 or 3 letter language code (ISO 639-1 or ISO-639-2),
                            // followed by the optional country variant to distinguist between country-specific variations
                            // of the language. The language code and country code are always separated by '_'
                            // Note that the language part can also be a 3-letter ISO 639-2 code.
                            // Legal examples:
                            // 'pt'         portuguese, assumes portuguese from portugal
                            // 'pt_BR'      Brazilian portuguese (ISO 639-1 language code)
                            // 'por_BR'     Brazilian portuguese (ISO 639-2 language code)
};

/*
  This is a quick hack. The proper way to handle this would be
  to extend MetaTranslator's interface.
*/
#define ContextComment "QT_LINGUIST_INTERNAL_CONTEXT_COMMENT"

bool saveXLIFF( const MetaTranslator &mt, const QString& filename);

#endif
