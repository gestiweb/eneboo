class GameItem {
    var x, y, w, h, id;
    var enabled;
    function enable() { 
	enableItem( id, true ); 
	enabled = true;
    }
    function disable() {
	enableItem( id, false ); 
	enabled = false;
    }
    function moveto( x1, y1 ) {
	x = x1;
	y = y1;
	moveItem( id, x, y ); 
    }
    function update() {
	moveItem( id, x, y );
    }
}

class Brick extends GameItem {
    var id1, id2, id3;
    var hitPoints = 3;
    function Brick() {
	var pm = new Pixmap( "brick1.png" );
	id1 = addItem( pm );
	id2 = addItem( new Pixmap( "brick2.png" ) );
	id3 = addItem( new Pixmap( "brick3.png" ) );
	enableItem( id1, true );
	enableItem( id2, false );
	enableItem( id3, false );
	this.x = 0;
	this.y = 0;
	this.w = pm.width;
	this.h = pm.height;
	hitPoints = 3;
	this.id = id1;
	this.enabled = true;
    }
    function damage() {
	this.disable();
	--hitPoints;
	if( hitPoints > 0 ) {
	    this.id++;
	    this.enable();
	    this.moveto( x, y );
	} else { 
	    destroyCount--;
	}	
    }
}

class Player extends GameItem {
    var speed = 0;
    function Player() {
	var pm = new Pixmap( "plate.png" );
	id = addItem( pm );
	this.enable();
	x = 0;
	this.y = 0;
	this.w = pm.width;
	this.h = pm.height;
    }
}

class Ball extends GameItem {
    var sx = 5;
    var sy = 5;
    function Ball() {
	var pm = new Pixmap( "ball.png" );
	this.id = addItem( pm );
	this.enable();
	this.x = 0;
	this.y = 0;
	this.w = pm.width;
	this.h = pm.height;
    }
}

var player;
var bricks;
var ball;
var destroyCount;

function init()
{
    var background = new Pixmap( "background.png" );
    var bgid = addItem( background );
    moveItem( bgid, 0, 0 );

    player = new Player();
    ball = new Ball();
    bricks = new Array();
    var brick;

    var startx = 250 - 15*32 / 2;
    var starty = 16;
    destroyCount = 0;
    for( var y = 0; y < 4; y+=3 ) 
	for( var x = 0; x < 15; ++x ) {
	    brick = new Brick();
	    brick.moveto( startx + 32 * x, starty + 16 * y );
	    bricks[ y*15 + x ] = brick;
	    ++destroyCount;
	}
    player.moveto( 250 - player.w / 2, 350 );
    ball.moveto( player.x + player.w / 2 - ball.w - 40, player.y - ball.h - 40  );
}

function next()
{
    movePlayer();
    moveBall();

    if( destroyCount == 0 )
	gameOver( true );
}

function moveBall()
{    
    ball.x += ball.sx;
    ball.y += ball.sy;

    if( ball.x < 0 ) {
	ball.x = 0;
	ball.sx = -ball.sx;
    }
    else if( ball.x + ball.w > width ) {
	ball.x = width - ball.w;
	ball.sx = -ball.sx;
    }

    if( ball.y < 0 ) {
	ball.y = 0;
	ball.sy = -ball.sy;
    } else if( ball.y + ball.h > height ) {
	gameOver( false );
    }

    if( checkCollide( ball, player ) ) {	
	ball.sx += player.speed;
	if( ball.sx > 5 )
	    ball.sx = 5;
	else if( ball.sx < -5 )
	    ball.sx = -5;
	ball.sy = -ball.sy;
    }
    
    var brick;
    for( var i in bricks ) {
	brick = bricks[i];
	if( !brick.enabled ) 
	    continue;
	if( checkCollide( ball, brick ) ) {
	    brick.damage();	    
	    if( hitHorizontal( ball, brick.y+brick.h, brick.x, brick.x+brick.w ) ||
		hitHorizontal( ball, brick.y, brick.x, brick.x+brick.w ) )
		ball.sy = -ball.sy;
	    else if( hitVertical( ball, brick.x+brick.w, brick.y, brick.y+brick.h ) ||
		     hitVertical( ball, brick.x, brick.y, brick.y+brick.h ) )
		ball.sx = -ball.sx;
	    break;
	}
    }
    ball.update();
}

function movePlayer() 
{
    if ( left ) {
	if( player.speed > -10 )
	    --player.speed;
	player.x += player.speed;	
    } else if ( right ) {
	if( player.speed < 10 )
	    ++player.speed;
	player.x += player.speed;
    } else if( player.speed > 0 ) {	
	player.x += --player.speed;	
    } else if( player.speed < 0 ) {
	player.x += ++player.speed;
    }

    if ( player.x < 0 ) {
	player.x = 0;
	player.speed = 0;
    } else if ( player.x > width-player.w ) {
	player.x = width-player.w;
	player.speed = 0;
    }

    player.update( player.x, player.y );
}

function checkCollide( a, b ) 
{
    if( !( a.enabled && b.enabled ) )
	return false;
    var ra = new Rect( a.x, a.y, a.w, a.h );
    var rb = new Rect( b.x, b.y, b.w, b.h );
    return ra.intersects( rb );
} 

function hitHorizontal( b, y, x1, x2 ) {        
    var beta = ( - ( ( y - b.y ) / b.sy ) * b.sx + b.x - x1 ) / ( x2 - x1 );
    return beta >= 0 && beta <= 1;
}

function hitVertical( b, x, y1, y2 ) {
    var beta = ( b.y - y1 - b.sy * ( ( b.x - x ) / b.sx ) ) / ( y2 - y1 );
    return beta >= 0 && beta <= 1;
}
