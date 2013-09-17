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

#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <QObject>
#include <QByteArray>
#include <QStringList>
#include <QLocale>
#include <QTranslator>

#include <private/qtranslator_p.h>

class QIODevice;
class TranslatorPrivate;
template <typename T> class QList;

class TranslatorMessage
{
public:
    TranslatorMessage();
    TranslatorMessage(const char * context, const char * sourceText,
                       const char * comment,
                       const QString &fileName,
                       int lineNumber,                       
                       const QStringList& translations = QStringList());
    TranslatorMessage(const TranslatorMessage & m);

    TranslatorMessage & operator=(const TranslatorMessage & m);

    uint hash() const { return m_hash; }
    const char *context() const { return m_context.isNull() ? 0 : m_context.constData(); }
    const char *sourceText() const { return m_sourcetext.isNull() ? 0 : m_sourcetext.constData(); }
    const char *comment() const { return m_comment.isNull() ? 0 : m_comment.constData(); }

    inline void setTranslations(const QStringList &translations);
    QStringList translations() const { return m_translations; }
    void setTranslation(const QString &translation) { m_translations = QStringList(translation); }
    QString translation() const { return m_translations.value(0); }
    bool isTranslated() const { return m_translations.count() > 1 || !m_translations.value(0).isEmpty(); }

    enum Prefix { NoPrefix, Hash, HashContext, HashContextSourceText,
                  HashContextSourceTextComment };
    void write(QDataStream & s, bool strip = false,
                Prefix prefix = HashContextSourceTextComment) const;
    Prefix commonPrefix(const TranslatorMessage&) const;

    bool operator==(const TranslatorMessage& m) const;
    bool operator!=(const TranslatorMessage& m) const
    { return !operator==(m); }
    bool operator<(const TranslatorMessage& m) const;
    bool operator<=(const TranslatorMessage& m) const
    { return !m.operator<(*this); }
    bool operator>(const TranslatorMessage& m) const
    { return m.operator<(*this); }
    bool operator>=(const TranslatorMessage& m) const
    { return !operator<(m); }

    QString fileName(void) const { return m_fileName; }
    void setFileName(const QString &fileName) { m_fileName = fileName; }
    int lineNumber(void) const { return m_lineNumber; }
    void setLineNumber(int lineNumber) { m_lineNumber = lineNumber; }
    bool isNull() const { return m_sourcetext.isNull() && m_lineNumber == -1 && m_translations.isEmpty(); }

private:
    uint        m_hash;
    QByteArray  m_context;
    QByteArray  m_sourcetext;
    QByteArray  m_comment;
    QStringList m_translations;
    QString     m_fileName;
    int         m_lineNumber;

    enum Tag { Tag_End = 1, Tag_SourceText16, Tag_Translation, Tag_Context16,
               Tag_Obsolete1, Tag_SourceText, Tag_Context, Tag_Comment,
               Tag_Obsolete2 };
};
Q_DECLARE_TYPEINFO(TranslatorMessage, Q_MOVABLE_TYPE);

inline void TranslatorMessage::setTranslations(const QStringList &translations)
{ m_translations = translations; }

class Translator : public QObject
{
    Q_OBJECT

public:
    explicit Translator(QObject *parent = 0);
    ~Translator();

    virtual TranslatorMessage findMessage(const char *context, const char *sourceText,
                                          const char *comment, 
                                          const QString &fileName = QLatin1String(""), int lineNumber = -1) const;

    void clear();

    enum SaveMode { Everything, Stripped };

    bool save(const QString & filename, SaveMode mode = Everything);
    bool save(QIODevice *iod, SaveMode mode = Everything);

    void insert(const TranslatorMessage&);
    inline void insert(const char *context, const char *sourceText, const QString &fileName, int lineNo, const QStringList &translations) {
        insert(TranslatorMessage(context, sourceText, "", fileName, lineNo, translations ));
    }
    void remove(const TranslatorMessage&);
    inline void remove(const char *context, const char *sourceText) {
        remove(TranslatorMessage(context, sourceText, "", QLatin1String(""), -1));
    }
    bool contains(const char *context, const char *sourceText, const char * comment = 0) const;

    bool contains(const char *context, const char *comment, const QString &fileName, int lineNumber) const;

    void squeeze(SaveMode = Everything);
    void unsqueeze();

    QList<TranslatorMessage> messages() const;

    bool isEmpty() const;

    void setNumerusRules(const QByteArray &rules);

private:
    Q_DISABLE_COPY(Translator)
    TranslatorPrivate *d;
};

static const uchar englishStyleRules[] =
    { EQ, 1 };
static const uchar frenchStyleRules[] =
    { LEQ, 1 };
static const uchar latvianRules[] =
    { MOD_10 | EQ, 1, AND, MOD_100 | NEQ, 11, NEWRULE,
      NEQ, 0 };
static const uchar irishStyleRules[] =
    { EQ, 1, NEWRULE,
      EQ, 2 };
static const uchar czechRules[] =
    { MOD_100 | EQ, 1, NEWRULE,
      MOD_100 | BETWEEN, 2, 4 };
static const uchar slovakRules[] =
    { EQ, 1, NEWRULE,
      BETWEEN, 2, 4 };
static const uchar macedonianRules[] =
    { MOD_10 | EQ, 1, NEWRULE,
      MOD_10 | EQ, 2 };
static const uchar lithuanianRules[] =
    { MOD_10 | EQ, 1, AND, MOD_100 | NEQ, 11, NEWRULE,
      MOD_10 | EQ, 2, AND, MOD_100 | NOT_BETWEEN, 10, 19 };
static const uchar russianStyleRules[] =
    { MOD_10 | EQ, 1, AND, MOD_100 | NEQ, 11, NEWRULE,
      MOD_10 | BETWEEN, 2, 4, AND, MOD_100 | NOT_BETWEEN, 10, 19 };
static const uchar polishRules[] =
    { EQ, 1, NEWRULE,
      MOD_10 | BETWEEN, 2, 4, AND, MOD_100 | NOT_BETWEEN, 10, 19 };
static const uchar romanianRules[] =
    { EQ, 1, NEWRULE,
      EQ, 0, OR, MOD_100 | BETWEEN, 1, 19 };
static const uchar slovenianRules[] =
    { MOD_100 | EQ, 1, NEWRULE,
      MOD_100 | EQ, 2, NEWRULE,
      MOD_100 | BETWEEN, 3, 4 };
static const uchar malteseRules[] =
    { EQ, 1, NEWRULE,
      EQ, 0, OR, MOD_100 | BETWEEN, 1, 10, NEWRULE,
      MOD_100 | BETWEEN, 11, 19 };
static const uchar welshRules[] =
    { EQ, 0, NEWRULE,
      EQ, 1, NEWRULE,
      BETWEEN, 2, 5, NEWRULE,
      EQ, 6 };
static const uchar arabicRules[] =
    { EQ, 0, NEWRULE,
      EQ, 1, NEWRULE,
      EQ, 2, NEWRULE,
      MOD_100 | BETWEEN, 3, 10, NEWRULE,
      MOD_100 | NEQ, 0 };

static const char * const japaneseStyleForms[] = { "Unique Form", 0 };
static const char * const englishStyleForms[] = { "Singular", "Plural", 0 };
static const char * const frenchStyleForms[] = { "Singular", "Plural", 0 };
static const char * const latvianForms[] = { "Singular", "Plural", "Nullar", 0 };
static const char * const irishStyleForms[] = { "Singular", "Dual", "Plural", 0 };
static const char * const czechForms[] = { "Singular", "Dual", "Plural", 0 };
static const char * const slovakForms[] = { "Singular", "Dual", "Plural", 0 };
static const char * const macedonianForms[] = { "Singular", "Dual", "Plural", 0 };
static const char * const lithuanianForms[] = { "Singular", "Dual", "Plural", 0 };
static const char * const russianStyleForms[] = { "Singular", "Dual", "Plural", 0 };
static const char * const polishForms[] = { "Singular", "Paucal", "Plural", 0 };
static const char * const romanianForms[] =
    { "Singular", "Plural Form for 2 to 19", "Plural", 0 };
static const char * const slovenianForms[] = { "Singular", "Dual", "Trial", "Plural", 0 };
static const char * const malteseForms[] =
    { "Singular", "Plural Form for 2 to 10", "Plural Form for 11 to 19", "Plural", 0 };
static const char * const welshForms[] =
    { "Nullar", "Singular", "Dual", "Sexal", "Plural", 0 };
static const char * const arabicForms[] =
    { "Nullar", "Singular", "Dual", "Minority Plural", "Plural", "Plural Form for 100, 200, ...", 0 };

#define EOL QLocale::C

static const QLocale::Language japaneseStyleLanguages[] = {
    QLocale::Afan,
    QLocale::Armenian,
    QLocale::Bhutani,
    QLocale::Bislama,
    QLocale::Burmese,
    QLocale::Chinese,
    QLocale::FijiLanguage,
    QLocale::Guarani,
    QLocale::Hungarian,
    QLocale::Indonesian,
    QLocale::Japanese,
    QLocale::Javanese,
    QLocale::Korean,
    QLocale::Malay,
    QLocale::NauruLanguage,
    QLocale::Persian,
    QLocale::Sundanese,
    QLocale::Thai,
    QLocale::Tibetan,
    QLocale::Vietnamese,
    QLocale::Yoruba,
    QLocale::Zhuang,
    EOL
};

static const QLocale::Language englishStyleLanguages[] = {
    QLocale::Abkhazian,
    QLocale::Afar,
    QLocale::Afrikaans,
    QLocale::Albanian,
    QLocale::Amharic,
    QLocale::Assamese,
    QLocale::Aymara,
    QLocale::Azerbaijani,
    QLocale::Bashkir,
    QLocale::Basque,
    QLocale::Bengali,
    QLocale::Bihari,
    // Missing: Bokmal,
    QLocale::Bulgarian,
    QLocale::Cambodian,
    QLocale::Catalan,
    QLocale::Cornish,
    QLocale::Corsican,
    QLocale::Danish,
    QLocale::Dutch,
    QLocale::English,
    QLocale::Esperanto,
    QLocale::Estonian,
    QLocale::Faroese,
    QLocale::Finnish,
    // Missing: Friulian,
    QLocale::Frisian,
    QLocale::Galician,
    QLocale::Georgian,
    QLocale::German,
    QLocale::Greek,
    QLocale::Greenlandic,
    QLocale::Gujarati,
    QLocale::Hausa,
    QLocale::Hebrew,
    QLocale::Hindi,
    QLocale::Icelandic,
    QLocale::Interlingua,
    QLocale::Interlingue,
    QLocale::Italian,
    QLocale::Kannada,
    QLocale::Kashmiri,
    QLocale::Kazakh,
    QLocale::Kinyarwanda,
    QLocale::Kirghiz,
    QLocale::Kurdish,
    QLocale::Kurundi,
    QLocale::Laothian,
    QLocale::Latin,
    // Missing: Letzeburgesch,
    QLocale::Lingala,
    QLocale::Malagasy,
    QLocale::Malayalam,
    QLocale::Marathi,
    QLocale::Mongolian,
    // Missing: Nahuatl,
    QLocale::Nepali,
    // Missing: Northern Sotho,
    QLocale::Norwegian,
    QLocale::Nynorsk,
    QLocale::Occitan,
    QLocale::Oriya,
    QLocale::Pashto,
    QLocale::Portuguese,
    QLocale::Punjabi,
    QLocale::Quechua,
    QLocale::RhaetoRomance,
    QLocale::Sesotho,
    QLocale::Setswana,
    QLocale::Shona,
    QLocale::Sindhi,
    QLocale::Singhalese,
    QLocale::Siswati,
    QLocale::Somali,
    QLocale::Spanish,
    QLocale::Swahili,
    QLocale::Swedish,
    QLocale::Tagalog,
    QLocale::Tajik,
    QLocale::Tamil,
    QLocale::Tatar,
    QLocale::Telugu,
    QLocale::TongaLanguage,
    QLocale::Tsonga,
    QLocale::Turkish,
    QLocale::Turkmen,
    QLocale::Twi,
    QLocale::Uigur,
    QLocale::Uzbek,
    QLocale::Volapuk,
    QLocale::Wolof,
    QLocale::Xhosa,
    QLocale::Yiddish,
    QLocale::Zulu,
    EOL
};
static const QLocale::Language frenchStyleLanguages[] = {
    // keep synchronized with frenchStyleCountries
    QLocale::Breton,
    QLocale::French,
    QLocale::Portuguese,
    // Missing: Filipino,
    QLocale::Tigrinya,
    // Missing: Walloon
    EOL
};
static const QLocale::Language latvianLanguage[] = { QLocale::Latvian, EOL };
static const QLocale::Language irishStyleLanguages[] = {
    QLocale::Divehi,
    QLocale::Gaelic,
    QLocale::Inuktitut,
    QLocale::Inupiak,
    QLocale::Irish,
    QLocale::Manx,
    QLocale::Maori,
    // Missing: Sami,
    QLocale::Samoan,
    QLocale::Sanskrit,
    EOL
};
static const QLocale::Language czechLanguage[] = { QLocale::Czech, EOL };
static const QLocale::Language slovakLanguage[] = { QLocale::Slovak, EOL };
static const QLocale::Language macedonianLanguage[] = { QLocale::Macedonian, EOL };
static const QLocale::Language lithuanianLanguage[] = { QLocale::Lithuanian, EOL };
static const QLocale::Language russianStyleLanguages[] = {
    QLocale::Bosnian,
    QLocale::Byelorussian,
    QLocale::Croatian,
    QLocale::Russian,
    QLocale::Serbian,
    QLocale::SerboCroatian,
    QLocale::Ukrainian,
    EOL
};
static const QLocale::Language polishLanguage[] = { QLocale::Polish, EOL };
static const QLocale::Language romanianLanguages[] = {
    QLocale::Moldavian,
    QLocale::Romanian,
    EOL
};
static const QLocale::Language slovenianLanguage[] = { QLocale::Slovenian, EOL };
static const QLocale::Language malteseLanguage[] = { QLocale::Maltese, EOL };
static const QLocale::Language welshLanguage[] = { QLocale::Welsh, EOL };
static const QLocale::Language arabicLanguage[] = { QLocale::Arabic, EOL };

static const QLocale::Country frenchStyleCountries[] = {
    // keep synchronized with frenchStyleLanguages
    QLocale::AnyCountry,
    QLocale::AnyCountry,
    QLocale::Brazil,
    QLocale::AnyCountry
};

struct NumerusTableEntry {
    const uchar *rules;
    int rulesSize;
    const char * const *forms;
    const QLocale::Language *languages;
    const QLocale::Country *countries;
};

static const NumerusTableEntry numerusTable[] = {
    { 0, 0, japaneseStyleForms, japaneseStyleLanguages, 0 },
    { englishStyleRules, sizeof(englishStyleRules), englishStyleForms, englishStyleLanguages, 0 },
    { frenchStyleRules, sizeof(frenchStyleRules), frenchStyleForms, frenchStyleLanguages,
      frenchStyleCountries },
    { latvianRules, sizeof(latvianRules), latvianForms, latvianLanguage, 0 },
    { irishStyleRules, sizeof(irishStyleRules), irishStyleForms, irishStyleLanguages, 0 },
    { czechRules, sizeof(czechRules), czechForms, czechLanguage, 0 },
    { slovakRules, sizeof(slovakRules), slovakForms, slovakLanguage, 0 },
    { macedonianRules, sizeof(macedonianRules), macedonianForms, macedonianLanguage, 0 },
    { lithuanianRules, sizeof(lithuanianRules), lithuanianForms, lithuanianLanguage, 0 },
    { russianStyleRules, sizeof(russianStyleRules), russianStyleForms, russianStyleLanguages, 0 },
    { polishRules, sizeof(polishRules), polishForms, polishLanguage, 0 },
    { romanianRules, sizeof(romanianRules), romanianForms, romanianLanguages, 0 },
    { slovenianRules, sizeof(slovenianRules), slovenianForms, slovenianLanguage, 0 },
    { malteseRules, sizeof(malteseRules), malteseForms, malteseLanguage, 0 },
    { welshRules, sizeof(welshRules), welshForms, welshLanguage, 0 },
    { arabicRules, sizeof(arabicRules), arabicForms, arabicLanguage, 0 }
};

static const int NumerusTableSize = sizeof(numerusTable) / sizeof(numerusTable[0]);

bool getNumerusInfo(QLocale::Language language, QLocale::Country country,
                           QByteArray *rules, QStringList *forms);

#endif // TRANSLATOR_H
