
#define Sprite GsSPRITE
#define ROT_ONE 4096
#define TYPE_LINE 0
#define TYPE_BOX 1

typedef struct {
	LINE_F2 line;
	int type;
} Line;

typedef struct {
	Line line[4];
	int type;
} Box;

typedef struct {
	RECT rect;
	RECT crect;
	GsIMAGE tim_data;
	GsSPRITE sprite;
} Image;

void sprite_create(unsigned char imageData[], int x, int y, GsSPRITE **sprite, int bitMode) {

	// Initialize image
	GsIMAGE* tim_data;
	RECT* rect;
	RECT* crect;
	tim_data = malloc3(sizeof(GsIMAGE));
	GsGetTimInfo ((u_long *)(imageData+4),tim_data);
	rect = malloc3(sizeof(RECT));
	crect = malloc3(sizeof(RECT));

	// Load the image into the GPU memory (frame buffer)
	rect->x = tim_data->px; // x position of image in frame buffer
	rect->y = tim_data->py; // y position of image in frame buffer
	rect->w = tim_data->pw; // width in frame buffer
	rect->h = tim_data->ph; // height in frame buffer
	printf("Framebuffer info {x=%d, y=%d, w=%d, h=%d}\n", rect->x, rect->y, rect->w, rect->h);
	LoadImage(rect, tim_data->pixel);

	// Load the color lookup table (CLUT) into the GPU memory (frame buffer)
	crect->x = tim_data->cx; // x position of CLUT in frame buffer
	crect->y = tim_data->cy; // y position of CLUT in frame buffer
	crect->w = tim_data->cw; // width of CLUT in frame buffer
	crect->h = tim_data->ch; // height of CLUT in frame buffer
	printf("CLUT info {x=%d, y=%d, w=%d, h=%d}\n", crect->x, crect->y, crect->w, crect->h);
	LoadImage(crect, tim_data->clut);

	// Initialize sprite (see GSSprite at PSYQ/DOCS/LIBREF.PDF)
	*sprite = malloc3(sizeof(GsSPRITE));
	(*sprite) -> attribute = 0x0000000;
	(*sprite) -> x = x;
	(*sprite) -> y = y;
	(*sprite) -> w = tim_data->pw * 4;
	(*sprite) -> h = tim_data->ph;
	(*sprite) -> tpage = GetTPage(
			bitMode, 		 // 0=4-bit, 1=8-bit, 2=16-bit
			1,       // semitransparency rate
			rect->x, // framebuffer x position of image
			rect->y  // framebuffer y position of image
	);
	(*sprite)->r = 128;  						// Color red blend
	(*sprite)->g = 128;  						// Color green blend
	(*sprite)->b = 128;  						// Color blue blend
	(*sprite)->u = (tim_data->px * 4) % 256;	// Position within tpage for sprite (0-256)
	(*sprite)->v = (tim_data->py % 256);		// Position within tpage for sprite (0-256)
	(*sprite)->cx = tim_data->cx;               // CLUT location x
	(*sprite)->cy = tim_data->cy;               // CLUT location y
	(*sprite)->rotate = ROT_ONE * 0;            // Rotation, ROT_ONE * (0 to 360)
	(*sprite)->mx = tim_data->ph/2;                          // Rotation x coord
	(*sprite)->my = tim_data->ph/2;                          // Rotation y coord
	(*sprite)->scalex = ONE * 1;				// Sprite scale (width)
	(*sprite)->scaley = ONE * 1;				// Sprite scale (height)

	// Clean up
	free3(rect);
	free3(crect);
	free3(tim_data);
}

void sprite_set_rotation(Sprite* sprite, int rotation) {
	sprite -> rotate = ROT_ONE * rotation;
}

void sprite_set_middle(Sprite* sprite, int relative_x, int relative_y) {
	sprite->mx = relative_x;
	sprite->my = relative_y;
}

void sprite_set_position(Sprite* sprite, int x, int y) {
	sprite->x = x;
	sprite->y = y;
}

void sprite_set_blend_color(Sprite* sprite, Color* color) {
	sprite -> r = color->r;
	sprite -> g = color->g;
	sprite -> b = color->b;
}

void sprite_set_blend_rgb(Sprite* sprite, int r, int g, int b) {
	sprite -> r = r;
	sprite -> g = g;
	sprite -> b = b;
}

void draw_sprite(Sprite *sprite) {
	myActiveBuff = GsGetActiveBuff();
	GsSortSprite(sprite, &orderingTable[myActiveBuff], 0);
}

void line_create(Color* color, int x1, int y1, int x2, int y2, Line* line) {
	line = malloc(sizeof(Line));
	line -> type = TYPE_LINE;
	SetLineF2(&line->line);
	setRGB0(&line->line, color->r, color->g, color->b);
	setXY2(&line->line, x1, y1, x2, y2);
}

void box_create(Color *color, int x1, int y1, int x2, int y2, Box* box) {
	box = malloc(sizeof(Box));
	line_create(color, x1, y1, x2, y1, &box -> line[0]);
	line_create(color, x1, y2, x2, y2, &box -> line[1]);
	line_create(color, x1, y1, x1, y2, &box -> line[2]);
	line_create(color, x2, y1, x2, y2, &box -> line[3]);
	box -> type = TYPE_BOX;
}

void line_move(Line* line, int x1, int y1, int x2, int y2) {
	line->line.x0 = x1;
	line->line.y0 = y1;
	line->line.x1 = x2;
	line->line.y1 = y2;
}

void box_move(Box* box, int x1, int y1) {

	// Figure out where to move the box to
	int* current_width;
	int* current_height;
	int* x2;
	int* y2;
	current_width = malloc(sizeof(int));
	current_height = malloc(sizeof(int));
	x2 = malloc(sizeof(int));
	y2 = malloc(sizeof(int));


	*current_width = box->line[0].line.x1 - box->line[0].line.x0;
	*current_height = box->line[2].line.y1 - box->line[2].line.y1;
	*x2 = x1 + *current_width;
	*y2 = y1 + *current_height;

	// Move the lines of the box
	line_move(&box->line[0], x1, y1, *x2, y1);
	line_move(&box->line[1], x1, *y2, *x2, *y2);
	line_move(&box->line[2], x1, y1, x1, *y2);
	line_move(&box->line[3], *x2, y1, *x2, *y2);

	// Clean up
	free3(current_width);
	free3(current_height);
	free3(x2);
	free3(y2);
}


void draw_line(Line *line) {
	DrawPrim(&line->line);
}

void draw_box(Box *box) {
	int i;
	for(i = 0; i < 4; i++) {
		DrawPrim(&box->line[i].line);
	}
}
