function bumpmap() 
{
    var src = new ImageSource();
    var bump = new BumpMapper();

    connect( src, 'output(Data)', bump.input );
    connect( bump, 'output(Data)', Renderer.input );

    src.file = 'bump.png';
    bump.smoothness = 16;
    var x=100;
    
    bump.radius = 300;
    for( var i=100; i<400; i+=5 ) {
	bump.light = new Point( x, 140 );
	x+=5;
	src.load();
    }    
}

function threshold()	
{
    var src = new ImageSource();
    var threshold = new Threshold();
  
    connect( src, 'output(Data)', threshold.input );
    connect( threshold, 'output(Data)', Renderer.input );

    src.file = 'bump.png';
    threshold.steps = 8;
    src.load();
}

function doAll() 
{
    var src = new ImageSource();
    var bump = new BumpMapper();
    var thres = new Threshold();
    var blur = new Smudger();

    connect( src,   'output(Data)', thres.input );
    connect( thres, 'output(Data)', blur.input );
    connect( blur,  'output(Data)', bump.input );
    connect( bump,  'output(Data)', Renderer.input );

    src.file = 'bump.png';
    thres.steps = 4;
    bump.smoothness = 16;
    bump.light = new Point( 80, 80 );
    bump.radius = 600;
    blur.iterations = 10;
    src.load();    
}