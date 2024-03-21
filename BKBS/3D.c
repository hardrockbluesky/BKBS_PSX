// Maximum number of objects
#define MAX_OBJECTS 100

// Object handler
GsDOBJ2	Object[MAX_OBJECTS];
int ObjectCount=0;

// Sunlight Direction and color
GsF_LIGHT sunLight[1];
VECTOR sunDirection;
Color sunColor;
Color ambientLightColor;

void SetSunColor (int r, int g, int  b) {
	sunColor = createColor(r, g, b);
}

void SetSunDirection (int x, int y, int z) {
	sunDirection.vx = x;
	sunDirection.vy = y;
	sunDirection.vz = z;
}

void SetAmbientLight (int r, int g, int b) {
	ambientLightColor = createColor(r, g, b);
	//ambient light is 0-4096 so math is required to
	//convert 0-4096 to the standard rgb range of 0-255
	//Note: ONE is the same as 4096
	GsSetAmbient(
		ambientLightColor.r * (ONE/255),
		ambientLightColor.g * (ONE/255),
		ambientLightColor.b * (ONE/255)
	);
}

void CalculateCamera() {

	// This function simply calculates the viewpoint matrix based on the camera coordinates...
	// It must be called on every frame before drawing any objects.

	VECTOR	vec;
	GsVIEW2 view;

	// Copy the camera (base) matrix for the viewpoint matrix
	view.view = Camera.coord2.coord;
	view.super = WORLD;

	//Find rotation matrix from a rotation angle.
	//input SVECTOR, output MATRIX
	RotMatrix(&Camera.rotation, &view.view);
	//Multiply a vector by a matrix.
	//Input MATRIX, input VECTOR, output VECTOR
	ApplyMatrixLV(&view.view, &Camera.position, &vec);
	//Set the amount of parallel transfer
	//Output MATRIX, input VECTOR
	TransMatrix(&view.view, &vec);

	// Set the viewpoint matrix to the GTE
	GsSetView2(&view);

	// Set the light source coordinates
	//sunLight[0].vx = sunDirection.vx;
	//sunLight[0].vy = -sunDirection.vy;
	//sunLight[0].vz = sunDirection.vz;

	//sunLight[0].r =	sunColor.r;	sunLight[0].g = sunColor.g;	sunLight[0].b = sunColor.b;
	//GsSetFlatLight(0, &sunLight[0]);

}

void RenderObject(VECTOR pos, SVECTOR rot, GsDOBJ2 *obj) {

	MATRIX lmtx,omtx;
	GsCOORDINATE2 coord;

	// Copy the camera (base) matrix for the model
	coord = Camera.coord2;

	//Flip the Y axis so a positive value
	//is up, and a negative value is down
	pos.vy *= -1;
	
	// Rotate and translate the matrix according to the specified coordinates
	RotMatrix(&rot, &omtx);
	TransMatrix(&omtx, &pos);
	CompMatrixLV(&Camera.coord2.coord, &omtx, &coord.coord);
	coord.flg = 0;

	// Apply coordinate matrix to the object
	obj->coord2 = &coord;

	// Calculate Local-World (for lighting) and Local-Screen (for projection) matrices and set both to the GTE
	GsGetLws(obj->coord2, &lmtx, &omtx);
	//GsSetLightMatrix(&lmtx); //This project doesn't use lighting, so don't waste the instructions
	GsSetLsMatrix(&omtx);

	// Sort the object!
	GsSortObject4(obj, &orderingTable[myActiveBuff], 14-OT_LENGTH, getScratchAddr(0));
}


int LoadTMD(u_long *tmd, GsDOBJ2 *obj, int enableLighting) {

	/*	This function prepares the specified TMD model for drawing and then
		links it to a GsDOBJ2 structure so it can be drawn using GsSortObject4().

		By default, light source calculation is disabled but can be re-enabled by
		simply setting the attribute variable in your GsDOBJ2 structure to 0.

		Parameters:
			*tmd - Pointer to a TMD model file loaded in memory.
			*obj - Pointer to an empty GsDOBJ2 structure.

		Returns:
			Number of objects found inside the TMD file.

	*/

	u_long *dop;
	int i,NumObj;

	// Copy pointer to TMD file so that the original pointer won't get destroyed
	dop = tmd;

	// Skip header and then remap the addresses inside the TMD file
	dop++;
	GsMapModelingData(dop);

	// Get object count
	dop++;
	NumObj = *dop;

	// Link object handler with the specified TMD
	dop++;
	for(i=0; i<NumObj; i++) {
		GsLinkObject4((u_long)dop, &obj[i], i);
		//connect the WORLD coordinate directly
		//GsInitCoordinate2(WORLD,obj[i].coord2);
		if (enableLighting == 0) {
			obj[i].attribute = (1<<6);	// Disables light source calculation
		}
	}

	// Return the object count found inside the TMD
	return(NumObj);

}

void loadTexture(unsigned char imageData[]) {

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

	// Clean up
	free3(rect);
	free3(crect);
	free3(tim_data);
}

VECTOR pos;
MATRIX tempMatrixPos;
VECTOR returnPos;
SVECTOR tempRot;
VECTOR Translate (SVECTOR rot, int x, int y, int z) {

	pos.vx = x;
	pos.vy = y;
	pos.vz = z;

    RotMatrix(&rot, &tempMatrixPos);

	ApplyMatrixLV(&tempMatrixPos, &pos, &returnPos);

	return returnPos;

}

// Cam behaves a little differently
VECTOR TranslateCam (SVECTOR rot, int x, int y, int z) {

	pos.vx = x;
	pos.vy = y;
	pos.vz = z;
	
	tempRot.vz = rot.vz;
	tempRot.vy = -rot.vy;
	tempRot.vx = 0;
	
    RotMatrix(&tempRot, &tempMatrixPos);

	ApplyMatrixLV(&tempMatrixPos, &pos, &returnPos);

	return returnPos;

}
