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

#include "translator.h"
#include <qplatformdefs.h>

#ifndef QT_NO_TRANSLATION

#include <QFileInfo>
#include <QString>
#include <QCoreApplication>
#include <QDataStream>
#include <QFile>
#include <QMap>
#include <QtAlgorithms>

#include <stdlib.h>

/*
$ mcookie
3cb86418caef9c95cd211cbf60a1bddd
$
*/

// magic number for the file
static const int MagicLength = 16;
static const uchar magic[MagicLength] = {
    0x3c, 0xb8, 0x64, 0x18, 0xca, 0xef, 0x9c, 0x95,
    0xcd, 0x21, 0x1c, 0xbf, 0x60, 0xa1, 0xbd, 0xdd
};

static uint elfHash(const char * name)
{
    const uchar *k;
    uint h = 0;
    uint g;

    if (name) {
        k = (const uchar *) name;
        while (*k) {
            h = (h << 4) + *k++;
            if ((g = (h & 0xf0000000)) != 0)
                h ^= g >> 24;
            h &= ~g;
        }
    }
    if (!h)
        h = 1;
    return h;
}

extern bool qt_detectRTLLanguage();

class TranslatorPrivate
{
public:
    struct Offset {
        Offset()
            : h(0), o(0) { }
        Offset(const TranslatorMessage& m, int offset)
            : h(m.hash()), o(offset) { }

        bool operator<(const Offset &other) const {
            return (h != other.h) ? h < other.h : o < other.o;
        }
        bool operator==(const Offset &other) const {
            return h == other.h && o == other.o;
        }
        uint h;
        uint o;
    };

    enum { Contexts = 0x2f, Hashes = 0x42, Messages = 0x69, NumerusRules = 0x88 };

    TranslatorPrivate(Translator *qq) : q(qq), unmapPointer(0), unmapLength(0) {}
    // Translator must finalize this before deallocating it

    Translator *q;
    // for mmap'ed files, this is what needs to be unmapped.
    char *unmapPointer;
    unsigned int unmapLength;

    // for squeezed but non-file data, this is what needs to be deleted
    QByteArray messageArray;
    QByteArray offsetArray;
    QByteArray contextArray;

#ifndef QT_NO_TRANSLATION_BUILDER
    QMap<TranslatorMessage, void *> messages;
#endif

    QByteArray numerusRules;

#if 0
    bool do_load(const uchar *data, int len);
#endif

};


Translator::Translator(QObject * parent)
    : QObject(parent)
{
    d = new TranslatorPrivate(this);
}

Translator::~Translator()
{
    clear();
    delete d;
}

bool Translator::save(const QString & filename, SaveMode mode)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) return false;
    return save(&file, mode);
}

bool Translator::save(QIODevice *iod, SaveMode mode)
{
    squeeze(mode);

    QDataStream s(iod);
    s.writeRawData((const char *)magic, MagicLength);
    quint8 tag;

    if (!d->offsetArray.isEmpty()) {
        tag = (quint8)TranslatorPrivate::Hashes;
        quint32 oas = (quint32)d->offsetArray.size();
        s << tag << oas;
        s.writeRawData(d->offsetArray, oas);
    }
    if (!d->messageArray.isEmpty()) {
        tag = (quint8)TranslatorPrivate::Messages;
        quint32 mas = (quint32)d->messageArray.size();
        s << tag << mas;
        s.writeRawData(d->messageArray, mas);
    }
    if (!d->contextArray.isEmpty()) {
        tag = (quint8)TranslatorPrivate::Contexts;
        quint32 cas = (quint32)d->contextArray.size();
        s << tag << cas;
        s.writeRawData(d->contextArray, cas);
    }
    if (!d->numerusRules.isEmpty()) {
        quint32 nrs = d->numerusRules.size();
        s << quint8(TranslatorPrivate::NumerusRules) << nrs;
        s.writeRawData(d->numerusRules.constData(), nrs);
    }
    return true;
}

void Translator::clear()
{
    if (d->unmapPointer && d->unmapLength) {
        delete [] d->unmapPointer;
        d->unmapPointer = 0;
        d->unmapLength = 0;
    }

    d->messageArray.clear();
    d->offsetArray.clear();
    d->contextArray.clear();
    d->messages.clear();

    QEvent ev(QEvent::LanguageChange);
    QCoreApplication::sendEvent(QCoreApplication::instance(), &ev);
}

void Translator::squeeze(SaveMode mode)
{
    if (d->messages.isEmpty()) {
        if (mode == Stripped)
            unsqueeze();
        else
            return;
    }

    QMap<TranslatorMessage, void *> messages = d->messages;
    clear();

    QMap<TranslatorPrivate::Offset, void *> offsets;

    QDataStream ms(&d->messageArray, QIODevice::WriteOnly);
    QMap<TranslatorMessage, void *>::const_iterator it, next;
    int cpPrev = 0, cpNext = 0;
    for (it = messages.constBegin(); it != messages.constEnd(); ++it) {
        cpPrev = cpNext;
        next = it;
        ++next;
        if (next == messages.constEnd())
            cpNext = 0;
        else
            cpNext = (int) it.key().commonPrefix(next.key());
        offsets.insert(TranslatorPrivate::Offset(it.key(), ms.device()->pos()), (void *)0);
        it.key().write(ms, mode == Stripped, (TranslatorMessage::Prefix)qMax(cpPrev, cpNext + 1));
    }

    QMap<TranslatorPrivate::Offset, void *>::Iterator offset;
    offset = offsets.begin();
    QDataStream ds(&d->offsetArray, QIODevice::WriteOnly);
    while (offset != offsets.end()) {
        TranslatorPrivate::Offset k = offset.key();
        ++offset;
        ds << (quint32)k.h << (quint32)k.o;
    }

    if (mode == Stripped) {
        QMap<QByteArray, int> contextSet;
        for (it = messages.constBegin(); it != messages.constEnd(); ++it)
            ++contextSet[it.key().context()];

        quint16 hTableSize;
        if (contextSet.size() < 200)
            hTableSize = (contextSet.size() < 60) ? 151 : 503;
        else if (contextSet.size() < 2500)
            hTableSize = (contextSet.size() < 750) ? 1511 : 5003;
        else
            hTableSize = (contextSet.size() < 10000) ? 15013 : 3 * contextSet.size() / 2;

        QMultiMap<int, const char *> hashMap;
        QMap<QByteArray, int>::const_iterator c;
        for (c = contextSet.constBegin(); c != contextSet.constEnd(); ++c)
            hashMap.insert(elfHash(c.key()) % hTableSize, c.key());

        /*
          The contexts found in this translator are stored in a hash
          table to provide fast lookup. The context array has the
          following format:

              quint16 hTableSize;
              quint16 hTable[hTableSize];
              quint8  contextPool[...];

          The context pool stores the contexts as Pascal strings:

              quint8  len;
              quint8  data[len];

          Let's consider the look-up of context "FunnyDialog".  A
          hash value between 0 and hTableSize - 1 is computed, say h.
          If hTable[h] is 0, "FunnyDialog" is not covered by this
          translator. Else, we check in the contextPool at offset
          2 * hTable[h] to see if "FunnyDialog" is one of the
          contexts stored there, until we find it or we meet the
          empty string.
        */
        d->contextArray.resize(2 + (hTableSize << 1));
        QDataStream t(&d->contextArray, QIODevice::WriteOnly);

        quint16 *hTable = new quint16[hTableSize];
        memset(hTable, 0, hTableSize * sizeof(quint16));

        t << hTableSize;
        t.device()->seek(2 + (hTableSize << 1));
        t << (quint16)0; // the entry at offset 0 cannot be used
        uint upto = 2;

        QMap<int, const char *>::const_iterator entry = hashMap.constBegin();
        while (entry != hashMap.constEnd()) {
            int i = entry.key();
            hTable[i] = (quint16)(upto >> 1);

            do {
	            const char *con = entry.value();
                uint len = (uint)qstrlen(con);
                len = qMin(len, 255u);
                t << (quint8)len;
                t.writeRawData(con, len);
                upto += 1 + len;
                ++entry;
            } while (entry != hashMap.constEnd() && entry.key() == i);
            do {
                t << (quint8) 0; // empty string
                ++upto;
            } while ((upto & 0x1) != 0); // offsets have to be even
        }
        t.device()->seek(2);
        for (int j = 0; j < hTableSize; j++)
            t << hTable[j];
        delete [] hTable;

        if (upto > 131072) {
            qWarning("Translator::squeeze: Too many contexts");
            d->contextArray.clear();
        }
    }
}


void Translator::unsqueeze()
{
    if (!d->messages.isEmpty() || d->messageArray.isEmpty())
        return;

    qFatal("Cannot unsqueeze (bug in Linguist?)");
}


bool Translator::contains(const char* context, const char* sourceText,
                            const char* comment) const
{
    return !findMessage(context, sourceText, comment).translation().isNull();
}

bool Translator::contains(const char *context,
                            const char *comment, const QString &fileName, int lineNumber) const
{
    return !findMessage(context, 0, comment, fileName, lineNumber).isNull();
}

void Translator::insert(const TranslatorMessage& message)
{
    unsqueeze();
    d->messages.remove(message); // safer
    d->messages.insert(message, (void *) 0);
}

void Translator::remove(const TranslatorMessage& message)
{
    unsqueeze();
    d->messages.remove(message);
}


TranslatorMessage Translator::findMessage(const char *context, const char *sourceText,
                                          const char *comment,
                                          const QString &fileName, int lineNumber) const
{
    if (context == 0)
        context = "";
    if (sourceText == 0)
        sourceText = "";
    if (comment == 0)
        comment = "";
    
    QString myFilename = fileName;
    int myLineNumber = lineNumber;

    if (!d->messages.isEmpty()) {
        QMap<TranslatorMessage, void *>::const_iterator it;

        // Either we want to find an item that matches context, sourcetext (and optionally comment)
        // Or we want to find an item that matches context, filename, linenumber (and optionally comment)
        it = d->messages.constFind(TranslatorMessage(context, sourceText, comment, myFilename, myLineNumber));
        if (it != d->messages.constEnd())
            return it.key();

        if (comment[0]) {
            it = d->messages.constFind(TranslatorMessage(context, sourceText, "", myFilename, myLineNumber));
            if (it != d->messages.constEnd())
                return it.key();
        }
        
        it = d->messages.constFind(TranslatorMessage(context, "", comment, myFilename, myLineNumber));
        if (it != d->messages.constEnd())
            return it.key();
        if (comment[0]) {
            it = d->messages.constFind(TranslatorMessage(context, "", "", myFilename, myLineNumber));
            if (it != d->messages.constEnd())
                return it.key();
        }
        return TranslatorMessage();
    }

    return TranslatorMessage();
}

bool Translator::isEmpty() const
{
    return !d->unmapPointer && !d->unmapLength && d->messageArray.isEmpty() &&
           d->offsetArray.isEmpty() && d->contextArray.isEmpty() && d->messages.isEmpty();
}

void Translator::setNumerusRules(const QByteArray &rules)
{
    d->numerusRules = rules;
}

QList<TranslatorMessage> Translator::messages() const
{
    ((Translator *) this)->unsqueeze();
    return d->messages.keys();
}

TranslatorMessage::TranslatorMessage()
    : m_hash(0), m_fileName(), m_lineNumber(-1)
{
}

TranslatorMessage::TranslatorMessage(const char * context,
                                        const char * sourceText,
                                        const char * comment,
                                        const QString &fileName,
                                        int lineNumber,
                                        const QStringList& translations)
    :   m_context(context), m_sourcetext(sourceText), m_comment(comment), 
        m_translations(translations), m_fileName(fileName), m_lineNumber(lineNumber)
{
    // 0 means we don't know, "" means empty
    if (m_context == (const char*)0)
        m_context = "";
    if (m_sourcetext == (const char*)0)
        m_sourcetext = "";
    if (m_comment == (const char*)0)
        m_comment = "";
    m_hash = elfHash(m_sourcetext + m_comment);
}


TranslatorMessage::TranslatorMessage(const TranslatorMessage & m)
    :   m_context(m.m_context), m_sourcetext(m.m_sourcetext), m_comment(m.m_comment), 
        m_translations(m.m_translations), m_fileName(m.m_fileName), m_lineNumber(m.m_lineNumber)
{
    m_hash = m.m_hash;
}

TranslatorMessage & TranslatorMessage::operator=(
        const TranslatorMessage & m)
{
    m_hash = m.m_hash;
    m_context = m.m_context;
    m_sourcetext = m.m_sourcetext;
    m_comment = m.m_comment;
    m_translations = m.m_translations;
    m_fileName = m.m_fileName;
    m_lineNumber = m.m_lineNumber;    
    return *this;
}

void TranslatorMessage::write(QDataStream & stream, bool strip, Prefix prefix) const
{
    for (int i = 0; i < m_translations.count(); ++i)
        stream << quint8(Tag_Translation) << m_translations.at(i);

    if (!strip)
        prefix = HashContextSourceTextComment;

    switch (prefix) {
    case HashContextSourceTextComment:
        stream << quint8(Tag_Comment) << m_comment;
        // fall through
    case HashContextSourceText:
        stream << quint8(Tag_SourceText) << m_sourcetext;
        // fall through
    case HashContext:
        stream << quint8(Tag_Context) << m_context;
    default:
        ;
    }

    stream << quint8(Tag_End);
}


TranslatorMessage::Prefix TranslatorMessage::commonPrefix(const TranslatorMessage& m) const
{
    if (m_hash != m.m_hash)
        return NoPrefix;
    if (m_context != m.m_context)
        return Hash;
    if (m_sourcetext != m.m_sourcetext)
        return HashContext;
    if (m_comment != m.m_comment)
        return HashContextSourceText;
    return HashContextSourceTextComment;
}

bool TranslatorMessage::operator==(const TranslatorMessage& m) const
{
    bool isHashEq = (m_hash == m.m_hash ? true : false);
    bool isContextEq = (m_context == m.m_context ? true : false);
    bool isSourceEq = (m_sourcetext == m.m_sourcetext ? true : false);
    bool isCommentEq = (m_comment == m.m_comment ? true : false);
    bool isLocationEq = m_lineNumber == m.m_lineNumber && m_fileName == m.m_fileName;
    
    return (isHashEq && isContextEq && isSourceEq && isCommentEq) || // translation can be different, but treat the equal
            (m_sourcetext.isEmpty() && isContextEq && isCommentEq && isLocationEq);
}

bool TranslatorMessage::operator<(const TranslatorMessage& m) const
{
    return m_hash != m.m_hash ? m_hash < m.m_hash
           : (m_context != m.m_context ? m_context < m.m_context
             : (m_sourcetext != m.m_sourcetext ? m_sourcetext < m.m_sourcetext : m_comment < m.m_comment));
}

bool getNumerusInfo(QLocale::Language language, QLocale::Country country,
                           QByteArray *rules, QStringList *forms)
{
    forever {
        for (int i = 0; i < NumerusTableSize; ++i) {
            const NumerusTableEntry &entry = numerusTable[i];
            for (int j = 0; entry.languages[j] != EOL; ++j) {
                if (entry.languages[j] == language
                        && ((!entry.countries && country == QLocale::AnyCountry)
                            || (entry.countries && entry.countries[j] == country))) {
                    if (rules) {
                        *rules = QByteArray::fromRawData(reinterpret_cast<const char *>(entry.rules),
                                                    entry.rulesSize);
                    }
                    if (forms) {
                        forms->clear();
                        for (int k = 0; entry.forms[k]; ++k)
                            forms->append(QLatin1String(entry.forms[k]));
                    }
                    return true;
                }
            }
        }

        if (country == QLocale::AnyCountry)
            break;
        country = QLocale::AnyCountry;
    }
    return false;
}

#endif // QT_NO_TRANSLATION
