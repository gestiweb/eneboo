/****************************************************************************
**
** The script engine jump table
**
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the kernel module of the Qt GUI Toolkit.
**
** This file may be used under the terms of the GNU General
** Public License versions 2.0 or 3.0 as published by the Free
** Software Foundation and appearing in the files LICENSE.GPL2
** and LICENSE.GPL3 included in the packaging of this file.
** Alternatively you may (at your option) use any later version
** of the GNU General Public License if such license has been
** publicly approved by Trolltech ASA (or its successors, if any)
** and the KDE Free Qt Foundation.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/.
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with
** the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

const q_scriptEngine scriptEngines[] = {
	// Latin,
    { basic_shape, 0 },
	// Greek,
    { basic_shape, 0 },
	// Cyrillic,
    { basic_shape, 0 },
	// Armenian,
    { basic_shape, 0 },
	// Georgian,
    { basic_shape, 0 },
	// Runic,
    { basic_shape, 0 },
	// Ogham,
    { basic_shape, 0 },
	// SpacingModifiers,
    { basic_shape, 0 },
	// CombiningMarks,
    { basic_shape, 0 },

	// // Middle Eastern Scripts
	// Hebrew,
    { hebrew_shape, 0 },
	// Arabic,
    { arabic_shape, 0 },
	// Syriac,
    { basic_shape, 0 },
	// Thaana,
    { basic_shape, 0 },

	// // South and Southeast Asian Scripts
	// Devanagari,
    { basic_shape, 0 },
	// Bengali,
    { basic_shape, 0 },
	// Gurmukhi,
    { basic_shape, 0 },
	// Gujarati,
    { basic_shape, 0 },
	// Oriya,
    { basic_shape, 0 },
	// Tamil,
    { basic_shape, 0 },
	// Telugu,
    { basic_shape, 0 },
	// Kannada,
    { basic_shape, 0 },
	// Malayalam,
    { basic_shape, 0 },
	// Sinhala,
    { basic_shape, 0 },
	// Thai,
    { basic_shape, 0 },
	// Lao,
    { basic_shape, 0 },
	// Tibetan,
    { basic_shape, 0 },
	// Myanmar,
    { basic_shape, 0 },
	// Khmer,
    { basic_shape, 0 },

	// // East Asian Scripts
	// Han,
    { basic_shape, 0 },
	// Hiragana,
    { basic_shape, 0 },
	// Katakana,
    { basic_shape, 0 },
	// Hangul,
    { basic_shape, 0 },
	// Bopomofo,
    { basic_shape, 0 },
	// Yi,
    { basic_shape, 0 },

	// // Additional Scripts
	// Ethiopic,
    { basic_shape, 0 },
	// Cherokee,
    { basic_shape, 0 },
	// CanadianAboriginal,
    { basic_shape, 0 },
	// Mongolian,
    { basic_shape, 0 },

	// // Symbols
	// CurrencySymbols,
    { basic_shape, 0 },
	// LetterlikeSymbols,
    { basic_shape, 0 },
	// NumberForms,
    { basic_shape, 0 },
	// MathematicalOperators,
    { basic_shape, 0 },
	// TechnicalSymbols,
    { basic_shape, 0 },
	// GeometricSymbols,
    { basic_shape, 0 },
	// MiscellaneousSymbols,
    { basic_shape, 0 },
	// EnclosedAndSquare,
    { basic_shape, 0 },
	// Braille,
    { basic_shape, 0 },

	// Unicode,
    { basic_shape, 0 },
    //Tagalog,
    { basic_shape, 0 },
    //Hanunoo,
    { basic_shape, 0 },
    //Buhid,
    { basic_shape, 0 },
    //Tagbanwa,
    { basic_shape, 0 },
    // KatakanaHalfWidth
    { basic_shape, 0 },
    // Limbu
    { basic_shape, 0 },
    // TaiLe
    { basic_shape, 0 }
};
