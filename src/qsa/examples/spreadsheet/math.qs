function sumVertically() {    
    var startCol = 1;
    var startRow = 1;
    var endRow = 1;
    var endCol = 1;
    if ( Application.sheet1.numSelections > 0 ) {
	var sel = Application.sheet1.selection( 0 );
	startCol = sel.x + 1;
	endCol = startCol + sel.width;
	startRow = sel.y + 1;
	endRow = startRow + sel.height;
    }
    
    var outputRow = endRow;

    for( var x = startCol; x<endCol; x++ ) {
	var sum = 0;
	for( var y = startRow; y<endRow; y++ ) {
	    sum += Number( Application.sheet1.text( y-1, x-1 ) );
	}
	Application.sheet1.setText( outputRow-1, x-1, sum );
    }
}