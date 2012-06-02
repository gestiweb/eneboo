class Item 
{
    var x, y, w, h;
    var id;
    var enabled;
    var ext = 1;
    function Item( pix, ix, iy ) 
    {
	x = ix;
	y = iy;
	w = pix.width;
	h = pix.height;
	id = addItem( pix );
	enabled = true;
    }

    function setEnabled( en ) 
    {
	enabled = en;
	enableItem( id, en );
    }

}

var enemy1;
var enemy2;
var player;
var playerShot;
var e1shot;
var e2shot;
var killCount;
var enemies;
var enemyShots;
var enemyCount = 7;

function init()
{
    var enemypm = new Pixmap( "enemy.png" );
    var playerpm = new Pixmap( "player.png" );
    var shotpm = new Pixmap( "bullet.png" );

    player = new Item( playerpm, 236, 300 );  
    playerShot = new Item( shotpm, 0, 0 );  
    playerShot.setEnabled( false );

    killCount = 0;

    enemies = new Array( enemyCount );
    enemyShots = new Array( enemyCount );
    for( var i=0; i<enemyCount; i++ ) {
	var en = new Item( enemypm, 32+64*i, 30 );
	enemies[i] = en;
	moveItem( en.id, en.x, en.y );      
	en = new Item( shotpm, 0, 0 );
	en.setEnabled( false );
	enemyShots[i] = en;
    }
}


function next()
{
    movePlayer();
    moveEnemies();

    if( killCount>=enemyCount ) 
	gameOver( true );
}


function movePlayer() 
{
    if ( left ) {
	player.x -= 5;
	if ( player.x < 0 )
	    player.x = 0;
    }
    else if ( right ) {
	player.x += 5;
	if ( player.x > width-player.w )
	    player.x = width-player.w;
    }

    if ( up ) {
	player.y -= 5;
	if ( player.y < 100 )
	    player.y = 100;
    }
    else if ( down ) {
	player.y += 5;
	if ( player.y > height-player.h )
	    player.y = height-player.h;
    }
    moveItem( player.id, player.x, player.y );

    if ( playerShot.enabled ) {
	playerShot.y -= 10;
	for( var i in enemies ) {
	    if( checkCollide( playerShot, enemies[i] ) ) {
		handleCollide( playerShot, enemies[i] );
		killCount++;
	    }
	}	
	if( playerShot.y < 0 ) {
	    playerShot.setEnabled( false );
	}     
    } else if ( space ) {
	playerShot.setEnabled( true );
	playerShot.x = player.x + player.w/2 - playerShot.w/2;
	playerShot.y = player.y;      
    } 
    moveItem( playerShot.id, playerShot.x, playerShot.y );
}


function moveEnemies() 
{
    for( var i in enemies )
	moveEnemy( enemies[i], enemyShots[i] );
} 


function moveEnemy( enemy, eshot ) 
{
    var speed = 30 / ( enemyCount - killCount + 1);

    enemy.x += speed * enemy.ext;
    if ( enemy.ext>0 && enemy.x+enemy.w>width ) {
	enemy.x = width-enemy.w;
	enemy.ext = -1;
	enemy.y += 2*enemy.h;
    } else if ( enemy.ext<0 && enemy.x<0 ) {
	enemy.x = 0;
	enemy.ext = 1;
	enemy.y += 2*enemy.h;
    } 

    if( checkCollide( enemy, player ) )
	gameOver( false );
    else if( enemy.y + enemy.h > height )
	gameOver( false );
  
    if( eshot.enabled ) {
	eshot.y += 6;
	if( eshot.y>height ) 
	    eshot.setEnabled( false );
    } else if( enemy.enabled ) {
	eshot.setEnabled( true );
	eshot.x = enemy.x + enemy.w/2 - eshot.w/2;
	eshot.y = enemy.y + enemy.h;
    }
  
    if ( checkCollide( eshot, player ) ) 
	gameOver( false );      

    moveItem( eshot.id, eshot.x, eshot.y );
    moveItem( enemy.id, enemy.x, enemy.y );
} 


function checkCollide( a, b ) 
{
    if( !( a.enabled && b.enabled ) )
	return false;
    var ra = new Rect( a.x, a.y, a.w, a.h );
    var rb = new Rect( b.x, b.y, b.w, b.h );
    return ra.intersects( rb );
} 


function handleCollide( shot, target ) 
{
    shot.setEnabled( false );
    target.setEnabled( false );
} 

