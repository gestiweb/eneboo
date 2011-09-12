/**********************************************************************
** Copyright (C) 2005-2007 Trolltech ASA.  All rights reserved.
**
** This file is part of Qt Designer.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifdef UIC_MWERKS_PLUGIN

/* standard headers */
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "mwerks_mac.h"
#ifdef Q_OS_MAC9
using namespace std;
/* system headers */
#include <Files.h>
#include <Strings.h>
#include <Errors.h>
#else
# include "qt_mac.h"
#endif
# include "mwerks_mac.h"

/* compiler headers */
#include "DropInCompilerLinker.h"
#include "CompilerMapping.h"
#include "CWPluginErrors.h"

//qglobal.cpp
const unsigned char * p_str(const char * c);
QCString p2qstring(const unsigned char *c);

#if CW_USE_PRAGMA_EXPORT
#pragma export on
#endif

CWPLUGIN_ENTRY(CWPlugin_GetDropInFlags)(const DropInFlags** flags, long* flagsSize)
{
	static const DropInFlags sFlags = {
		kCurrentDropInFlagsVersion,
		CWDROPINCOMPILERTYPE,
		DROPINCOMPILERLINKERAPIVERSION_7,
		kCompAlwaysReload|kCompRequiresProjectBuildStartedMsg,
		Lang_C_CPP,
		DROPINCOMPILERLINKERAPIVERSION
	};
	*flags = &sFlags;
	*flagsSize = sizeof(sFlags);
	return cwNoErr;
}



CWPLUGIN_ENTRY(CWPlugin_GetDropInName)(const char** dropinName)
{
	static const char* sDropInName = "McUic";
	*dropinName = sDropInName;
	return cwNoErr;
}

CWPLUGIN_ENTRY(CWPlugin_GetDisplayName)(const char** displayName)
{
	static const char* sDisplayName = "McUic";
	*displayName = sDisplayName;
	return cwNoErr;
}

CWPLUGIN_ENTRY(CWPlugin_GetTargetList)(const CWTargetList** targetList)
{
	static CWDataType sCPU = targetCPUAny;
	static CWDataType sOS = targetOSMacintosh;
	static CWTargetList sTargetList = {kCurrentCWTargetListVersion, 1, &sCPU, 1, &sOS};
	*targetList = &sTargetList;
	return cwNoErr;
}

CWPLUGIN_ENTRY(CWPlugin_GetDefaultMappingList)(const CWExtMapList** defaultMappingList)
{
	static CWExtensionMapping sExtension[] = { {'TEXT', ".uics", kPrecompile } };
	static CWExtMapList sExtensionMapList = {kCurrentCWExtMapListVersion, 3, sExtension};
	*defaultMappingList = &sExtensionMapList;
	return cwNoErr;
}

#if CW_USE_PRAGMA_EXPORT
#pragma export off
#endif
typedef short CWFileRef;
static int line_count = 0;
bool do_uic(const QString &fileName, const QString &headerFile, QTextStream& out, QDomDocument doc, 
	    bool decl, bool subcl, const QString &trmacro, const QString& className, bool nofwd); //main.cpp
bool internal_do_uic(CWPluginContext context, const QCString &source, bool impl)
{
    CWFileInfo fi;
    memset(&fi, 0, sizeof(fi));
    fi.fullsearch = true;
    fi.dependencyType = cwNormalDependency;
    fi.isdependentoffile = kCurrentCompiledFile;
    if(CWFindAndLoadFile( ctx, source.data(), &fi) != cwNoErr) 
	return FALSE;

    QCString path("");
    AliasHandle alias;
    Str63 str;
    AliasInfoType x = 1;
    char tmp[sizeof(Str63)+2];
    if(NewAlias( NULL, &fi.filespec, &alias) != noErr) 
	return FALSE;
    for(;;) {
	 GetAliasInfo(alias, x++, str);
	 if(!str[0])
	    break;
	 strncpy((char *)tmp, (const char *)str+1, str[0]);
	 tmp[str[0]] = '\0';
	 path.prepend(":");
	 path.prepend((char *)tmp);
    }
    path.prepend("MacOS 9:"); //FIXME

    QCString dest(source), header;
    if(dest.right(3) == ".ui")
	dest.truncate(dest.length() - 3);
    if(impl) {
	header = dest + ".h";
	dest += ".cpp";
    } else {
	dest += ".h";
    }

    CWFileSpec dspec;
    memcpy(&dspec, &fi.filespec, sizeof(fi.filespec));
    {
	const unsigned char *f = p_str(fout.data());
	memcpy(dspec.name, f, f[0]+1);
    }

    QString inpath = path + source, outpath = path + dest;
    struct stat istat, ostat;
    if(stat(inpath, &istat) == -1) 
	return FALSE;
    bool ret = TRUE, did_uic = FALSE;
    if(stat(outpath, &ostat) != 0 || istat.st_mtime > ostat.st_mtime) {
	did_uic = TRUE;
	QFile fileOut(outpath);
	if (!fileOut.open( IO_WriteOnly ) ) 
	    return FALSE;
	QTextStream out( &fileOut );
	out.setEncoding( QTextStream::UnicodeUTF8 );

	QDomDocument doc;
	QByteArray doc_data;
	doc_data.setRawData(fi.filedata, fi.filedatalength);
	QString errMsg;
	int errLine;
	if (!doc.setContent(doc_data, &errMsg, &errLine)) {
//	    qFatal( QString("uic: Failed to parse %s: ") + errMsg + QString (" in line %d\n"), fileName, errLine );
	    return FALSE;
	}
	DomTool::fixDocument(doc);
	ret = do_uic( inpath, header, out, doc, !impl, FALSE, "", "", FALSE );
	doc_data.resetRawData();
    }

    if(!did_uic || ret) {
	long whichFile;
	CWNewProjectEntryInfo ei;
	memset(&ei, '\0', sizeof(ei));
	ei.groupPath = "QtGenerated";
	err = CWAddProjectEntry(context, &dspec, true, &ei, &whichFile);
	if (!CWSUCCESS(err)) {
	    char	msg[200];
	    sprintf(msg, "\"%s\" not added", dest.data());
	    CWReportMessage(context, NULL, msg, NULL, messagetypeWarning, 0);
	}
	if(did_uic)
	    CWSetModDate(context, &dspec, NULL, true);
    }
    return ret;
}

static CWResult	uicify(CWPluginContext context, const QCString &source)
{
    CWDisplayLines(context, line_count++);

    source.stripWhiteSpace();

    CWResult err;
    QCString stem = source, ext;
    int dotpos = stem.findRev('.');
        
    //moc it
    int decl = 1;
    if(!internal_do_uic(context, source, decl) || !internal_do_uic(context, source, --decl)) {
#if 0
	QCString derr = "Implementation";
	if(!decl)
	    derr = "Declaration";
   	char	dmsg[200];
	sprintf(dmsg, "\"%s\" %s", source.data(), derr.data());
	CWReportMessage(context, NULL, dmsg, NULL, messagetypeError, 0);
#endif
    }
    return (cwNoErr);
}

pascal short main(CWPluginContext context)
{
    long request;
    if (CWGetPluginRequest(context, &request) != cwNoErr)
	return cwErrRequestFailed;
    short result = cwErrInvalidParameter;
    switch (request) { 	/* dispatch on compiler request */
    case reqInitCompiler:
    case reqTermCompiler:
	result = cwNoErr;
    	break;
		
    case reqCompile: {
	line_count = 0;
	const char *files = NULL;
	long filelen;
	CWGetMainFileText(context, &files, &filelen);
	const char *beg = files;
	for(int x = 0; x < filelen; x++) {
	    if(*(files++) == '\r') {
		char file[1024];
		memcpy(file, beg, files - beg);
		file[(files-beg)-1] = '\0';
		uicify(context, file);
		beg = files;
	    }
	}
	if(beg != files) {
	    char file[1024];
	    memcpy(file, beg, files - beg);
	    file[(files-beg)] = '\0';
	    uicify(context, file);
	}        
	result = cwNoErr;
	break; }
    }
    return (result); 	/* return result code */
}
#endif
