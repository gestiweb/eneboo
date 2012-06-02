connect( EuroSettings, "shown()", this, "init" );
connect( EuroSettings, "destroyed()", this, "destroy" );
connect( buttonCalculate, "clicked()", this, "buttonCalculate_clicked" );
connect( buttonCancel, "clicked()", this, "buttonCancel_clicked" );

function init()
{
    if ( Application.sheet1.numSelections > 0 ) {
	var r = Application.sheet1.selection( 0 );
	spinStartRow.value = r.y + 1;
	spinEndRow.value = r.y + r.height;
	spinColumn.value = r.x + 1;
	spinOutputColumn.value = r.x + 2;
    }
}

function destroy()
{
}

function buttonCancel_clicked()
{
    reject();
}

function buttonCalculate_clicked()
{
    var divisor;
    
    switch ( comboCurrency.currentText ) {
    case "NOK":
	divisor = 7.8680;
	break;
    case "DM":
	divisor = 1.95583;
	break;
    case "ATS":
	divisor = 13.7603;
	break;
    }
    
    const inputColumn = spinColumn.value - 1;
    const outputColumn = spinOutputColumn.value - 1;
    const startRow = spinStartRow.value - 1;
    const endRow = spinEndRow.value - 1;
    
    if ( endRow < startRow ) {
	MessageBox.critical( "Cannot calculate!", "Error" );
	accept();
	return;
    }
    
    for ( var i = startRow; i <= endRow; ++i )
	Application.sheet1.setText( i, outputColumn, Application.sheet1.text( i, inputColumn ) / divisor );
    
    accept();
}