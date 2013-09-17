/** @file */

/** @class_declaration interna */
////////////////////////////////////////////////////////////////////////////
//// DECLARACION ///////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
//// INTERNA /////////////////////////////////////////////////////
class interna {
    var ctx:Object;
    function interna( context ) { this.ctx = context; }
    function init() { this.ctx.interna_init(); }
}
//// INTERNA /////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

/** @class_declaration oficial */
//////////////////////////////////////////////////////////////////
//// OFICIAL /////////////////////////////////////////////////////
class oficial extends interna {
	function oficial( context ) { interna( context ); }
	function init() { this.ctx.oficial_init(); }
	function prueba():String { return this.ctx.oficial_prueba(); }
	function fibo( n:Number ):String { return this.ctx.oficial_fibo( n ); }
}
//// OFICIAL /////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

/** @class_declaration head */
/////////////////////////////////////////////////////////////////
//// DESARROLLO /////////////////////////////////////////////////
class head extends oficial {
    function head( context ) { oficial ( context ); }
}
//// DESARROLLO /////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

/** @class_declaration ifaceCtx */
/////////////////////////////////////////////////////////////////
//// INTERFACE  /////////////////////////////////////////////////
class ifaceCtx extends head {
    function ifaceCtx( context ) { head( context ); }
}

const iface = new ifaceCtx( this );
//// INTERFACE  /////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

/** @class_definition interna */
////////////////////////////////////////////////////////////////////////////
//// DEFINICION ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
//// INTERNA /////////////////////////////////////////////////////
/** \C Si en la tabla de direcciones de clientes no hay todavía ninguna dirección asociada al cliente, la primera dirección introducida se tomará como dirección de facturación y dirección de envío
\end */
function interna_init()
{
	MessageBox.warning( "Init interna", MessageBox.Ok, MessageBox.NoButton );
}

//// INTERNA /////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

/** @class_definition oficial */
//////////////////////////////////////////////////////////////////
//// OFICIAL /////////////////////////////////////////////////////
function oficial_init()
{
	this.iface.__init();
	var d1 = new Date();

 	MessageBox.warning( "Init oficial " + AQUtil.getOS() + " " + AQUtil.getLang(), MessageBox.Ok, MessageBox.NoButton );
	
	AQUtil.createProgressDialog( "Probando", 10000);
	for( i=0; i<10000; ++i ) {
		AQUtil.setLabelText( "Paso " + i );
		sys.processEvents();
		AQUtil.setProgress( i );
	}
	AQUtil.destroyProgressDialog();
	var d2 = new Date();
        var e = d2.getTime() - d1.getTime();
        print( 'Total time : ' + e + 'ms' );
}

function fibonacci(n) {
    if (n == undefined)
        throw "Try calling the main function...";
    if (n == 0)
        return 0;
    else if (n == 1)
        return 1;
    return fibonacci(n-1) + fibonacci(n-2);
}

function oficial_fibo( n:Number ):String
{
	var d1 = new Date();
	var fib;
	for (var x = 0; x <= n; ++x) {
		fib = fibonacci(x);
		print('Fibonacci number: ' + x + ', is: ' + fib);
	}
	var d2 = new Date();
	var e = d2.getTime() - d1.getTime();
	print( 'Total time : ' + e + 'ms' );
}

function oficial_prueba():String
{
// 	var a = Input.getText( "Opcion" );
// 
// 	switch ( a ) {
// 		case "1":
// 			print ( "1 " + a );
// 		break;
// 		case "4":
// 		case "2":
// 			print ( "2 " + a );
// 		break;
// 		case "6":
// 		case "3":
// 			print ( "3 " + a );
// 		break;
// 		case "123":
// 			print ( "123 " + a );
// 		break;
// 		default:
// 			print ( "default " + a );
// 	}
	
	var d1 = new Date();
	var res = "Version " + this.version();
	var cur = new AQSqlCursor( "divisas" );

	cur.select();
	while( cur.next() )
		res += "\n<br>" + cur.valueBuffer( "descripcion" );

	var cur2 = cur;

	print( cur2.table );

	cur2.setModeAccess( AQSql.Insert );
	cur2.refreshBuffer();
	cur2.setValueBuffer( "coddivisa", "PRG" );
	cur2.setValueBuffer( "descripcion", "Perra gorda" );
	cur2.commitBuffer();
	
	var qry = new AQSqlQuery;

	qry.tablesList = "clientes";
	qry.select = "nombre";
	qry.from = "clientes";

	if ( qry.exec() ) {
		while( qry.next() )
			res += "\n<br>" + qry.value( 0 );
	 }


	 var bancos = [["0030", "BANESTO"],["0112", "BANCO URQUIJO"],
                ["2085", "IBERCAJA"],["0093", "BANCO DE VALENCIA"],
                ["2059", "CAIXA SABADELL"],["2073", "CAIXA TARRAGONA"],
                ["2038", "CAJA MADRID"],["2091", "CAIXA GALICIA"],
                ["0019", "DEUTSCHE BANK"],["0081", "BANCO DE SABADELL"],
                ["0049", "BANCO SANTANDER CENTRAL HISPANO"],["0072", "BANCO PASTOR"],
                ["0075", "BANCO POPULAR"],["0182","BANCO BILBAO VIZCAYA ARGENTARIA"],
                ["0128", "BANKINTER"],["2090", "C.A.M."],["2100", "LA CAIXA"],
                ["2077", "BANCAJA"],["0008", "BANCO ATLANTICO"],
                ["0061", "BANCA MARCH"],["0065", "BARCLAYS BANK"],
                ["0073", "PATAGON INTERNET BANK"],["0103", "BANCO ZARAGOZANO"],
                ["2013", "CAIXA CATALUNYA"],["2043","CAJA MURCIA"],
                ["2103", "UNICAJA"],["2105", "CAJA DE CASTILLA LA MANCHA"],
                ["0042", "BANCO GUIPUZCOANO"],["0138", "BANKOA"],
                ["3056", "CAJA RURAL DE ALBACETE"]];

        for (var i = 0; i < bancos.length; i++)
		res += "<br>" + bancos[i][0] + " " + bancos[i][1];

	var d2 = new Date();
        var e = d2.getTime() - d1.getTime();
        print( 'Total time : ' + e + 'ms' );

	return res;
}
//// OFICIAL /////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

/** @class_definition head */
/////////////////////////////////////////////////////////////////
//// DESARROLLO /////////////////////////////////////////////////

//// DESARROLLO /////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

