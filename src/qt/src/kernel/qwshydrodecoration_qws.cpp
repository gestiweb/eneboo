/****************************************************************************
**
** Implementation of Qt/Embedded Hydro decoration
**
** Created : 000101
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
#include <qapplication.h>
#include <qwidget.h>
#include <qpainter.h>
#include <qdrawutil.h>
#include "qwshydrodecoration_qws.h"

#ifndef QT_NO_QWS_HYDRO_WM_STYLE

#ifndef QT_NO_IMAGEIO_XPM

/* XPM */
static const char * const hydro_menu_xpm[] = {
"13 14 101 2",
"  	c None",
". 	c #2A2A2A",
"+ 	c #0F0F0F",
"@ 	c #2C2C2C",
"# 	c #606060",
"$ 	c #585858",
"% 	c #9C9C9C",
"& 	c #C5C5C5",
"* 	c #D7D7D7",
"= 	c #CACACA",
"- 	c #A0A0A0",
"; 	c #4B4B4B",
"> 	c #5B5B5B",
", 	c #A9A9A9",
"' 	c #DDDDDD",
") 	c #E8E8E8",
"! 	c #F9F9F9",
"~ 	c #ECECEC",
"{ 	c #D9D9D9",
"] 	c #AFAFAF",
"^ 	c #5D5D5D",
"/ 	c #414141",
"( 	c #7B7B7B",
"_ 	c #B9B9B9",
": 	c #D4D4D4",
"< 	c #D3D3D3",
"[ 	c #BBBBBB",
"} 	c #777777",
"| 	c #474747",
"1 	c #4E4E4E",
"2 	c #6C6C6C",
"3 	c #ABABAB",
"4 	c #A2A2A2",
"5 	c #B2B2B2",
"6 	c #B5B5B5",
"7 	c #C1C1C1",
"8 	c #BABABA",
"9 	c #A6A6A6",
"0 	c #737373",
"a 	c #454545",
"b 	c #999999",
"c 	c #AAAAAA",
"d 	c #B0B0B0",
"e 	c #C0C0C0",
"f 	c #C7C7C7",
"g 	c #C2C2C2",
"h 	c #B6B6B6",
"i 	c #8F8F8F",
"j 	c #595959",
"k 	c #C9C9C9",
"l 	c #CFCFCF",
"m 	c #D5D5D5",
"n 	c #D8D8D8",
"o 	c #B1B1B1",
"p 	c #5E5E5E",
"q 	c #676767",
"r 	c #E9E9E9",
"s 	c #E2E2E2",
"t 	c #E6E6E6",
"u 	c #E0E0E0",
"v 	c #D2D2D2",
"w 	c #D0D0D0",
"x 	c #C3C3C3",
"y 	c #626262",
"z 	c #6D6D6D",
"A 	c #D1D1D1",
"B 	c #EEEEEE",
"C 	c #F3F3F3",
"D 	c #F7F7F7",
"E 	c #F5F5F5",
"F 	c #EFEFEF",
"G 	c #DADADA",
"H 	c #767676",
"I 	c #7C7C7C",
"J 	c #E7E7E7",
"K 	c #FBFBFB",
"L 	c #FFFFFF",
"M 	c #FAFAFA",
"N 	c #EDEDED",
"O 	c #A7A7A7",
"P 	c #7A7A7A",
"Q 	c #898989",
"R 	c #CCCCCC",
"S 	c #F6F6F6",
"T 	c #FDFDFD",
"U 	c #FCFCFC",
"V 	c #878787",
"W 	c #6B6B6B",
"X 	c #E4E4E4",
"Y 	c #939393",
"Z 	c #565656",
"` 	c #747474",
" .	c #EAEAEA",
"..	c #9F9F9F",
"+.	c #727272",
"@.	c #5A5A5A",
"#.	c #4D4D4D",
"$.	c #434343",
"%.	c #424242",
"&.	c #4A4A4A",
"*.	c #656565",
"      . . . + @ # #       ",
"    $ $ % & * = - ; ;     ",
"  > > , ' ) ! ~ { ] ^ ^   ",
"  / ( _ : { { * < [ } |   ",
"1 2 3 4 5 [ 6 7 8 9 3 0 1 ",
"a b c d e e f g h h 9 i ; ",
"j d 3 k l m n { l l 5 o p ",
"q e k n { r s t u v w x y ",
"z _ A s B C D E F t G h H ",
"I , u J K L M L M B N O P ",
"  Q R S T U L S L T = V   ",
"  W i G K L L L L X Y z   ",
"    Z ` 9 A  .v ..+.@.    ",
"      y #./ $.%.&.*.      "};

/* XPM */
static const char * const hydro_minimize_xpm[] = {
"13 14 142 2",
"  	c None",
". 	c #5B5B59",
"+ 	c #252523",
"@ 	c #090A05",
"# 	c #2C2C2C",
"$ 	c #565656",
"% 	c #434144",
"& 	c #8A8A8C",
"* 	c #BFBEC4",
"= 	c #CECCD7",
"- 	c #C4B3C3",
"; 	c #908F94",
"> 	c #2C3836",
", 	c #3F4A3C",
"' 	c #8C9B86",
") 	c #CFDEC7",
"! 	c #F2FBE9",
"~ 	c #E1EED8",
"{ 	c #E0EFD7",
"] 	c #D0E0CD",
"^ 	c #8FA78D",
"/ 	c #3B5041",
"( 	c #243D1F",
"_ 	c #4A703F",
": 	c #7FA46B",
"< 	c #9CBE81",
"[ 	c #9FBF8A",
"} 	c #99B984",
"| 	c #93B37D",
"1 	c #A6C792",
"2 	c #78A169",
"3 	c #42693A",
"4 	c #253D27",
"5 	c #455441",
"6 	c #2F5C23",
"7 	c #4A7B37",
"8 	c #689948",
"9 	c #72A44F",
"0 	c #73A252",
"a 	c #78A757",
"b 	c #679A4B",
"c 	c #4B7B39",
"d 	c #376131",
"e 	c #3A4F3E",
"f 	c #1F3818",
"g 	c #40802C",
"h 	c #518D2E",
"i 	c #5C9D39",
"j 	c #6BAB4A",
"k 	c #72A74C",
"l 	c #71A74C",
"m 	c #6BA344",
"n 	c #70A947",
"o 	c #5E9D35",
"p 	c #4F8A30",
"q 	c #3C782E",
"r 	c #1B3E1D",
"s 	c #113509",
"t 	c #509B34",
"u 	c #61A430",
"v 	c #115900",
"w 	c #196000",
"x 	c #1F6200",
"y 	c #1E5F00",
"z 	c #266600",
"A 	c #195C00",
"B 	c #206700",
"C 	c #65AA3D",
"D 	c #589D42",
"E 	c #113F10",
"F 	c #2D5822",
"G 	c #5FAF3C",
"H 	c #7BC244",
"I 	c #1D6800",
"J 	c #287501",
"K 	c #3A7F12",
"L 	c #3D8112",
"M 	c #3F830C",
"N 	c #266C00",
"O 	c #277300",
"P 	c #6EB83E",
"Q 	c #62AD48",
"R 	c #1B5117",
"S 	c #2F5F23",
"T 	c #5AAE34",
"U 	c #86CD4D",
"V 	c #96E162",
"W 	c #A9F676",
"X 	c #9DE364",
"Y 	c #A5EB68",
"Z 	c #AEF470",
"` 	c #9FE862",
" .	c #8DD556",
"..	c #60AB42",
"+.	c #326A2B",
"@.	c #5C7852",
"#.	c #5C8E37",
"$.	c #99D85F",
"%.	c #B4F773",
"&.	c #BEFF77",
"*.	c #B6F76F",
"=.	c #ADF06A",
"-.	c #B5F770",
";.	c #BBFB73",
">.	c #B8FC75",
",.	c #92DB5A",
"'.	c #599440",
").	c #557754",
"!.	c #558031",
"~.	c #7DB746",
"{.	c #BBF87C",
"].	c #CBFF80",
"^.	c #CAFC88",
"/.	c #C8FC86",
"(.	c #D1FF8E",
"_.	c #C2FD82",
":.	c #83C24D",
"<.	c #568541",
"[.	c #58724F",
"}.	c #5A843C",
"|.	c #96C962",
"1.	c #C6F884",
"2.	c #D7FF96",
"3.	c #D1FA94",
"4.	c #D7FF9D",
"5.	c #C7F689",
"6.	c #9ECF5C",
"7.	c #5E8532",
"8.	c #546E47",
"9.	c #4D6448",
"0.	c #506F36",
"a.	c #739541",
"b.	c #9CBF57",
"c.	c #B9DC68",
"d.	c #99C067",
"e.	c #5D7835",
"f.	c #556541",
"g.	c #465246",
"h.	c #3F4C38",
"i.	c #3B492F",
"j.	c #455A33",
"k.	c #445533",
"      . . + @ # $ $       ",
"    % % & * = - ; > >     ",
"  , , ' ) ! ~ { ] ^ / /   ",
"  ( _ : < [ } | 1 2 3 4   ",
"5 6 7 8 9 0 0 a a b c d e ",
"f g h i j k l m n o p q r ",
"s t u v w x y z A B C D E ",
"F G H I J K L M N O P Q R ",
"S T U V W X Y Z Z `  ...+.",
"@.#.$.%.&.*.=.-.;.>.,.'.).",
"  !.~.{.].^./.(.(._.:.<.  ",
"  [.}.|.1.2.3.4.5.6.7.8.  ",
"    9.0.a.b.c.d.a.e.f.    ",
"      g.g.h.i.j.k.k.      "};

/* XPM */
static const char * const hydro_maximize_xpm[] = {
"13 14 145 2",
"  	c None",
". 	c #3D2324",
"+ 	c #2D1515",
"@ 	c #392526",
"# 	c #645655",
"$ 	c #906461",
"% 	c #6D4546",
"& 	c #A49091",
"* 	c #D2BEBD",
"= 	c #DFCDC9",
"- 	c #D0BDB6",
"; 	c #A98B89",
"> 	c #6C4548",
", 	c #8C696F",
"' 	c #906866",
") 	c #884C44",
"! 	c #C49083",
"~ 	c #F1D0C1",
"{ 	c #FADEDA",
"] 	c #FFE7DD",
"^ 	c #FEE4D7",
"/ 	c #F2CDC4",
"( 	c #B98987",
"_ 	c #7B4E55",
": 	c #90707B",
"< 	c #79322C",
"[ 	c #AD5C41",
"} 	c #DD9871",
"| 	c #F0BC8A",
"1 	c #E3AB6F",
"2 	c #E2A461",
"3 	c #DD9455",
"4 	c #F0BF8C",
"5 	c #E1A581",
"6 	c #9C5D4B",
"7 	c #6E3838",
"8 	c #8A4D52",
"9 	c #B04E35",
"0 	c #E07D42",
"a 	c #D78A3B",
"b 	c #E09E48",
"c 	c #D37833",
"d 	c #B7661A",
"e 	c #D37932",
"f 	c #DCA048",
"g 	c #DA8E37",
"h 	c #CE7936",
"i 	c #9D4D28",
"j 	c #954C3D",
"k 	c #7B3124",
"l 	c #CE602B",
"m 	c #E5811F",
"n 	c #DE8E28",
"o 	c #E59C35",
"p 	c #D06D2C",
"q 	c #A67211",
"r 	c #EAA53E",
"s 	c #DF8D20",
"t 	c #E98620",
"u 	c #8E2D0A",
"v 	c #82371A",
"w 	c #EE8033",
"x 	c #F29A1E",
"y 	c #A35800",
"z 	c #975400",
"A 	c #975F00",
"B 	c #935F00",
"C 	c #985E00",
"D 	c #A05C00",
"E 	c #A04E00",
"F 	c #F79727",
"G 	c #E47F37",
"H 	c #922F06",
"I 	c #914827",
"J 	c #F8923D",
"K 	c #E5A123",
"L 	c #9D6100",
"M 	c #956100",
"N 	c #906600",
"O 	c #966D04",
"P 	c #9E6E08",
"Q 	c #9F6200",
"R 	c #A35600",
"S 	c #D4922F",
"T 	c #E89545",
"U 	c #A14F1F",
"V 	c #9D5839",
"W 	c #F0923B",
"X 	c #CAA03A",
"Y 	c #CFAB45",
"Z 	c #FFED6D",
"` 	c #E3A34E",
" .	c #E09B4B",
"..	c #FFDD6E",
"+.	c #D2A941",
"@.	c #C89F3B",
"#.	c #DCA341",
"$.	c #9D5D1D",
"%.	c #967556",
"&.	c #C89033",
"*.	c #C89C36",
"=.	c #F4D75C",
"-.	c #DF9948",
";.	c #E1A24A",
">.	c #FFE66C",
",.	c #C19D3C",
"'.	c #CBA23E",
").	c #BB8E4A",
"!.	c #886B59",
"~.	c #A17036",
"{.	c #E4B148",
"].	c #FFE263",
"^.	c #FCF678",
"/.	c #E2A54F",
"(.	c #DB9141",
"_.	c #FFF881",
":.	c #FFE566",
"<.	c #DBAC44",
"[.	c #9E7B45",
"}.	c #7F604E",
"|.	c #9F7D3E",
"1.	c #D6C55D",
"2.	c #F6F976",
"3.	c #FBF67E",
"4.	c #FFFF8D",
"5.	c #FFFF8E",
"6.	c #F9F780",
"7.	c #E0C864",
"8.	c #9B7C39",
"9.	c #7A6749",
"0.	c #685841",
"a.	c #6D6729",
"b.	c #8D963B",
"c.	c #CFD648",
"d.	c #D5E551",
"e.	c #BDD042",
"f.	c #949D26",
"g.	c #797224",
"h.	c #6A5B3A",
"i.	c #565B3B",
"j.	c #433453",
"k.	c #494351",
"l.	c #474843",
"m.	c #4A4A42",
"n.	c #625C5C",
"      . . . + @ #         ",
"    $ % & * = - ; > ,     ",
"  ' ) ! ~ { ] ^ / ( _ :   ",
"  < [ } | 1 2 3 4 5 6 7   ",
"8 9 0 a b c d e f g h i j ",
"k l m n o p q p r s t p u ",
"v w x y z A B C D E F G H ",
"I J K L M N O P Q R S T U ",
"V W X Y Z ` p  ...+.@.#.$.",
"%.&.*.+.=.-.p ;.>.,.'.).!.",
"  ~.{.].^./.p (._.:.<.[.  ",
"  }.|.1.2.3.4.5.6.7.8.9.  ",
"    0.a.b.c.d.e.f.g.h.    ",
"        i.j.k.l.m.n.      "};

/* XPM */
static const char * const hydro_close_xpm[] = {
"13 14 149 2",
"  	c None",
". 	c #5F5F5D",
"+ 	c #242422",
"@ 	c #101010",
"# 	c #232323",
"$ 	c #575759",
"% 	c #423D41",
"& 	c #9B9599",
"* 	c #C8BDC3",
"= 	c #D7CAD1",
"- 	c #CEBFC6",
"; 	c #8F8289",
"> 	c #53484E",
", 	c #695E64",
"' 	c #64535B",
") 	c #9D8585",
"! 	c #EDCFCD",
"~ 	c #FFEFF0",
"{ 	c #FFDEDF",
"] 	c #FFEBED",
"^ 	c #EFCBCD",
"/ 	c #A98A88",
"( 	c #5F4A47",
"_ 	c #7C716D",
": 	c #4D2627",
"< 	c #6F3A40",
"[ 	c #995457",
"} 	c #BC7175",
"| 	c #F1A8AF",
"1 	c #E9A2AA",
"2 	c #ECA3AC",
"3 	c #B97077",
"4 	c #A3585C",
"5 	c #743637",
"6 	c #461E1C",
"7 	c #714F4D",
"8 	c #782C30",
"9 	c #8A3437",
"0 	c #70090A",
"a 	c #640000",
"b 	c #96363A",
"c 	c #B55860",
"d 	c #91343C",
"e 	c #5F0003",
"f 	c #71080C",
"g 	c #973739",
"h 	c #7A3331",
"i 	c #7A5052",
"j 	c #541E1C",
"k 	c #9F3738",
"l 	c #AF3D3C",
"m 	c #991813",
"n 	c #8B0C06",
"o 	c #7C100D",
"p 	c #912F2E",
"q 	c #741211",
"r 	c #811513",
"s 	c #921614",
"t 	c #B13938",
"u 	c #9D3735",
"v 	c #5D1F20",
"w 	c #51100C",
"x 	c #B53F3B",
"y 	c #C84741",
"z 	c #DE5245",
"A 	c #D85246",
"B 	c #8F2015",
"C 	c #832219",
"D 	c #801E15",
"E 	c #C3564F",
"F 	c #D9564E",
"G 	c #C94540",
"H 	c #B23531",
"I 	c #5A0E0E",
"J 	c #67201A",
"K 	c #B94035",
"L 	c #E06053",
"M 	c #F86D5A",
"N 	c #DD5B45",
"O 	c #9C3522",
"P 	c #953C2A",
"Q 	c #903425",
"R 	c #C15B4C",
"S 	c #F57667",
"T 	c #E46459",
"U 	c #CD3F3B",
"V 	c #6E1B17",
"W 	c #782F26",
"X 	c #C34B3B",
"Y 	c #E16B5D",
"Z 	c #E3614B",
"` 	c #AC3016",
" .	c #99331D",
"..	c #AD523F",
"+.	c #993E2C",
"@.	c #9B3526",
"#.	c #E36A59",
"$.	c #E56B5E",
"%.	c #D13C36",
"&.	c #832C25",
"*.	c #7C5C4D",
"=.	c #95332A",
"-.	c #CF6A5E",
";.	c #A93420",
">.	c #9D2610",
",.	c #E77D6D",
"'.	c #FFA69A",
").	c #DC776D",
"!.	c #8D2218",
"~.	c #AC382B",
"{.	c #DB6F63",
"].	c #A03837",
"^.	c #915650",
"/.	c #954641",
"(.	c #B76A60",
"_.	c #C76A59",
":.	c #E98472",
"<.	c #FFA99E",
"[.	c #FFB8B1",
"}.	c #FFADA7",
"|.	c #E07D78",
"1.	c #CB665E",
"2.	c #C36A64",
"3.	c #8E4443",
"4.	c #845754",
"5.	c #8A3C3C",
"6.	c #F07879",
"7.	c #FE958F",
"8.	c #FFAD9E",
"9.	c #FFBBA5",
"0.	c #FEBAA3",
"a.	c #F2A190",
"b.	c #D46F67",
"c.	c #AD3B3A",
"d.	c #7A5354",
"e.	c #725255",
"f.	c #573B30",
"g.	c #8B544F",
"h.	c #CD7373",
"i.	c #EF7C81",
"j.	c #D66569",
"k.	c #994845",
"l.	c #6E463C",
"m.	c #5F5242",
"n.	c #5B4D4D",
"o.	c #463F37",
"p.	c #474538",
"q.	c #4A4C3E",
"r.	c #515348",
"      . . + @ # $         ",
"    % % & * = - ; > ,     ",
"  ' ' ) ! ~ { ] ^ / ( _   ",
"  : < [ } | 1 2 3 4 5 6   ",
"7 8 9 0 a b c d e f g h i ",
"j k l m n o p q r s t u v ",
"w x y z A B C D E F G H I ",
"J K L M N O P Q R S T U V ",
"W X Y Z `  ...+.@.#.$.%.&.",
"*.=.-.;.>.,.'.).!.~.{.].^.",
"  /.(._.:.<.[.}.|.1.2.3.   ",
"  4.5.6.7.8.9.0.a.b.c.d.  ",
"    e.f.g.h.i.j.k.l.m.    ",
"        n.o.p.q.r.        "};

/* XPM */
static const char * const hydro_left_corner_xpm[] = {
"7 7 4 1",
"  	c None",
"* 	c #000000",
". 	c #FFFFFF",
"+ 	c #A0A0A0",
"    ...",
"  ...++",
" ...+  ",
" ..++  ",
"..++   ",
".+     ",
".+     "};

/* XPM */
static const char * const hydro_right_corner_xpm[] = {
"7 7 4 1",
"  	c None",
"* 	c #000000",
". 	c #FFFFFF",
"+ 	c #A0A0A0",
"..     ",
"++..   ",
"  +++  ",
"   ++* ",
"    +**",
"     **",
"     **"};

#endif // QT_NO_IMAGEIO_XPM

QWSHydroDecoration::QWSHydroDecoration()
    : QWSDefaultDecoration()
{
}

QWSHydroDecoration::~QWSHydroDecoration()
{
}

const char **QWSHydroDecoration::menuPixmap()
{
#ifndef QT_NO_IMAGEIO_XPM
    return (const char **)hydro_menu_xpm;
#else
    return 0;
#endif
}

const char **QWSHydroDecoration::closePixmap()
{
#ifndef QT_NO_IMAGEIO_XPM
    return (const char **)hydro_close_xpm;
#else
    return 0;
#endif
}

const char **QWSHydroDecoration::minimizePixmap()
{
#ifndef QT_NO_IMAGEIO_XPM
    return (const char **)hydro_minimize_xpm;
#else
    return 0;
#endif
}

const char **QWSHydroDecoration::maximizePixmap()
{
#ifndef QT_NO_IMAGEIO_XPM
    return (const char **)hydro_maximize_xpm;
#else
    return 0;
#endif
}

const char **QWSHydroDecoration::normalizePixmap()
{
#ifndef QT_NO_IMAGEIO_XPM
    return (const char **)hydro_maximize_xpm;
#else
    return 0;
#endif
}

int QWSHydroDecoration::getTitleHeight(const QWidget *)
{
    return 15;
}

/*
    If rect is empty, no frame is added. (a hack, really)
*/
QRegion QWSHydroDecoration::region(const QWidget *widget, const QRect &rect, QWSDecoration::Region type)
{
    int titleHeight = getTitleHeight(widget);
//    int titleWidth = getTitleWidth(widget);
    int bw = rect.isEmpty() ? 0 : BORDER_WIDTH;
    QRegion region;

    switch (type) {
	case Menu: {
		QRect r(rect.left(), rect.top() - titleHeight - 2,
		    titleHeight, titleHeight);
		region = r;
		break;
	    }
	case Close: {
		QRect r(rect.right() - 16, rect.top() - titleHeight - 2,
		    titleHeight, titleHeight);
		if (r.left() > rect.left() + titleHeight)
	 	    region = r;
	  	break;
	    }
	case Maximize: {
		QRect r(rect.right() - 32, rect.top() - titleHeight - 2,
		     titleHeight, titleHeight);
		if (r.left() > rect.left() + titleHeight)
		    region = r;
	        break;
	    }
	case Minimize: {
		QRect r(rect.right() - 48, rect.top() - titleHeight - 2,
		     titleHeight, titleHeight);
		if (r.left() > rect.left() + titleHeight)
		    region = r;
	 	break;
	    }
	case All: {
		    QRect r(rect.left() - bw,
			    rect.top() - titleHeight - bw,
			    rect.width() + 2 * bw,
			    rect.height() + titleHeight + 2 * bw);
		    region = r;
		    region -= rect;

		    region -= QRect( rect.left() - bw + 0, rect.top() - titleHeight - bw + 0, 8, 1);
		    region -= QRect( rect.left() - bw + 0, rect.top() - titleHeight - bw + 1, 5, 1);
		    region -= QRect( rect.left() - bw + 0, rect.top() - titleHeight - bw + 2, 3, 1);
		    region -= QRect( rect.left() - bw + 0, rect.top() - titleHeight - bw + 3, 2, 2);
		    region -= QRect( rect.left() - bw + 0, rect.top() - titleHeight - bw + 5, 1, 3);

		    region -= QRect( rect.right() + bw - 7, rect.top() - titleHeight - bw + 0, 8, 1);
		    region -= QRect( rect.right() + bw - 4, rect.top() - titleHeight - bw + 1, 5, 1);
		    region -= QRect( rect.right() + bw - 2, rect.top() - titleHeight - bw + 2, 3, 1);
		    region -= QRect( rect.right() + bw - 1, rect.top() - titleHeight - bw + 3, 2, 2);
		    region -= QRect( rect.right() + bw - 0, rect.top() - titleHeight - bw + 5, 1, 3);
	    }
	    break;
	case Title:
	case Top:
	case Left:
	case Right:
	case Bottom:
	case TopLeft:
	case TopRight:
	case BottomLeft:
	case BottomRight:
	default:
	    region = QWSDefaultDecoration::region(widget, rect, type);
	    break;
    }

    return region;
}

void QWSHydroDecoration::paint(QPainter *painter, const QWidget *widget)
{
    int titleWidth = getTitleWidth(widget);
    int titleHeight = getTitleHeight(widget);

    QRect rect(widget->rect());

    // title bar rect
    QRect tr( 0, -titleHeight,  titleWidth - 1, titleHeight - 1);

    QRegion oldClip = painter->clipRegion();
    painter->setClipRegion( oldClip - QRegion( tr ) );	// reduce flicker

#ifndef QT_NO_PALETTE
    const QColorGroup &cg = QApplication::palette().active();
//    const QColorGroup &cg = widget->palette().active();

#if !defined(QT_NO_DRAWUTIL)
    // Border rect
    QRect br( rect.left() - BORDER_WIDTH,
                rect.top() - BORDER_WIDTH - titleHeight,
                rect.width() + 2 * BORDER_WIDTH,
                rect.height() + BORDER_WIDTH + BOTTOM_BORDER_WIDTH + titleHeight );

    qDrawWinPanel(painter, br.x(), br.y(), br.width(),
		  br.height() - 4, cg, FALSE,
		  &cg.brush(QColorGroup::Background));
#endif

    painter->setClipRegion( oldClip );

    if (titleWidth > 0) {
	QBrush titleBrush;
	QPen   titlePen;
	int    titleLeft = titleHeight + 4;

	if (widget == qApp->activeWindow()) {
	    titleBrush = cg.brush(QColorGroup::Highlight);
	    titlePen   = cg.color(QColorGroup::HighlightedText);
	} else {
	    titleBrush = cg.brush(QColorGroup::Background);
	    titlePen   = cg.color(QColorGroup::Text);
	}

#define CLAMP(x, y)	    ( ((x) > (y)) ? (y) : (x) )

	    QColor c1;
	    if (widget == qApp->activeWindow()) {
		c1 = QColor( 0x90, 0xA0, 0xF0 );
	    } else {
		c1 = QColor( 0x20, 0x30, 0x50 );
		titlePen = QPen( QColor( 0x90, 0x90, 0x90 ) );
	    }
	    painter->fillRect( rect.left() - 1, rect.top() - titleHeight - 2,
			    rect.width() + 2, titleHeight + 2,  QBrush( c1 ));

#ifndef QT_NO_IMAGEIO_XPM
	    QPixmap lc((const char **)hydro_left_corner_xpm);
	    QPixmap rc((const char **)hydro_right_corner_xpm);
	    painter->drawPixmap( rect.left() - 2, rect.top() - titleHeight - 3, lc);
	    painter->drawPixmap( rect.right() - 2, rect.top() - titleHeight - 3, rc);
#endif

#ifndef QT_NO_WIDGET_TOPEXTRA
	painter->setPen(titlePen);
	painter->setFont(widget->font());
	painter->drawText( titleLeft, -titleHeight,
	 		rect.width() - titleHeight - 10, titleHeight-1,
			QPainter::AlignVCenter, widget->caption());
#endif
    }

#endif //QT_NO_PALETTE

}

void QWSHydroDecoration::paintButton(QPainter *painter, const QWidget *w,
			QWSDecoration::Region type, int state)
{
#ifndef QT_NO_PALETTE
    QRect brect(region(w, w->rect(), type).boundingRect());

    int xoff=2;
    int yoff=2;

    const QPixmap *pm=pixmapFor(w,type,state & QWSButton::On, xoff, yoff);

	QBrush titleBrush;

	if (w == qApp->activeWindow()) {
	    titleBrush = QBrush( QColor( 0x90, 0xA0, 0xF0 ) );
	} else {
	    titleBrush = QBrush( QColor( 0x20, 0x30, 0x50 ) );
	}

	xoff++;
	painter->fillRect( brect.x()+xoff, brect.y()+yoff, brect.width()+1,
		    brect.height()+1, titleBrush );
	if ((state & QWSButton::MouseOver) && (state & QWSButton::Clicked)) {
	    if (pm) painter->drawPixmap(brect.x()+1+xoff, brect.y()+1+yoff, *pm);
	} else {
	    if (pm) painter->drawPixmap(brect.x()+xoff, brect.y()+yoff, *pm);
	}

#endif

}

#endif // QT_NO_QWS_HYDRO_WM_STYLE
