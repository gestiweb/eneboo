function test()
{
    var d = new Direction;
    
    print( "Setting enum via Direction.LeftToRight" );
    d.setMode( Direction.LeftToRight );    
    print( "left to right? " + d.isLeftToRight() );
    print( "right to left? " + d.isRightToLeft() );
    
    print( "<br>" );
    print( "Setting enum via d.RightToLeft" );
    d.setMode( d.RightToLeft );
    print( "left to right? " + d.isLeftToRight() );
    print( "right to left? " + d.isRightToLeft() );
    
    print( "<br>" );
    print( "Setting illegal enum value" );
    try { 
        d.setMode(10);
    } catch (e) {
        print("<b>ERROR:</b> " + e);
    }    
 }