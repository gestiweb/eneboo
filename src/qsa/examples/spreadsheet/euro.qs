function convertToEuro()
{
    var inputCol = 1;
    var startRow = 1;
    var endRow = 1;
    var outputCol = 1;
    if ( Application.sheet1.numSelections > 0 ) {
	var sel = Application.sheet1.selection( 0 );
	inputCol = sel.x + 1;
	outputCol = inputCol + 1;
	startRow = sel.y + 1;
	endRow = sel.bottom + 1;
    }

    d = new Dialog;
    d.caption = "Settings for Euro Converter";
    d.okButtonText = "Calculate";
    d.cancelButtonText = "Cancel";

    var g = new GroupBox;
    g.title = "Conversion Range:";
    d.add( g );

    var spinColumn = new SpinBox;
    spinColumn.label = "Column:";
    spinColumn.minimum = 1;
    spinColumn.maximum = 100;
    spinColumn.value = inputCol;
    g.add( spinColumn );

    var spinStartRow = new SpinBox;
    spinStartRow.label = "Start at Row:";
    spinStartRow.minimum = 1;
    spinStartRow.maximum = 100;
    spinStartRow.value = startRow;
    g.add( spinStartRow );

    var spinEndRow = new SpinBox;
    spinEndRow.label = "End at Row:";
    spinEndRow.minimum = 1;
    spinEndRow.maximum = 100;
    spinEndRow.value = endRow;
    g.add( spinEndRow );

    var spinOutput = new SpinBox;
    spinOutput.label = "Output Column:";
    spinOutput.minimum = 1;
    spinOutput.maximum = 100;
    spinOutput.value = outputCol;
    g.add( spinOutput );

    d.newColumn();

    g = new GroupBox;
    g.title = "Convert From:";
    d.add( g );

    var radioUSD = new RadioButton;
    radioUSD.text = "USD";
    radioUSD.checked = true;
    g.add(radioUSD);
    var radioYEN = new RadioButton;
    radioYEN.text = "YEN";
    g.add(radioYEN);
    var radioNOK = new RadioButton;
    radioNOK.text = "NOK";
    g.add(radioNOK);

    if ( !d.exec() )
	return;

    var divisor;

    if( radioUSD.checked )
        divisor = 0.930492;
    else if( radioYEN.checked )
        divisor = 0.007677;
    else if ( radioNOK.checked )
        divisor = 0.133828;

    inputCol = spinColumn.value - 1;
    outputCol = spinOutput.value - 1;
    startRow = spinStartRow.value - 1;
    endRow = spinEndRow.value - 1;

    if ( endRow < startRow ) {
	MessageBox.critical( "Cannot calculate!", "Error" );
	return;
    }

    for ( var i = startRow; i <= endRow; ++i ) {
	var input = Application.sheet1.text( i, inputCol );
	Application.sheet1.setText( i, outputCol, 
				    Math.round( input / divisor * 100 ) / 100 );
    }
}
