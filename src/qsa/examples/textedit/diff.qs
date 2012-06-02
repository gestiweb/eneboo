class DiffSyntaxHighlighter
{
    var diffHighlighter;
    var red;
    var blue;
    var darkBlue;
    var bold;
    var defFont;
    
    function DiffSyntaxHighlighter( textedit )
    {
	red = new Color( "red" );
	blue = new Color( "blue" );
	darkBlue = new Color( "darkBlue" );
	bold = textedit.font;
	bold.bold = true;
	defFont = textedit.font;
	diffHighlighter = new SyntaxHighlighter( textedit );
	connect( diffHighlighter, "highlight( String, Number )", highlightDiff );
    }
    
    function highlightDiff( text, lastState )
    {
	var len = text.length;
	if ( text.startsWith( "+" ) )
	    diffHighlighter.setFormat( 0, len, blue );
	else if ( text.startsWith( "-" ) )
	    diffHighlighter.setFormat( 0, len, red );
	else if ( text.startsWith( "====" ) || text.startsWith( "@@" ) )
	    diffHighlighter.setFormat( 0, len, bold );
	else if ( text.startsWith( "diff" ) )
	    diffHighlighter.setFormat( 0, len, bold, darkBlue );
	else
	    diffHighlighter.setFormat( 0, len, defFont );
    }
};

// Put all global functions here

function installDiffHighlighter()
{
    new DiffSyntaxHighlighter( Application.TextEdit.currentEditor() );
}