// MegaProjectFile.h
//

#define MEGAPROJECT_IDEN			"MegaProject"
#define MEGAPROJECT_VERSION			"1"
#define MEGAPROJECT_EXTENSION		".megaproject"

class idImage;

//
// bmMegaProjectLayer
//
class bmMegaProjectLayer {
public:
	void						Init( const char *diffuseImagePath, const char *maskImagePath );

	idImage						*maskImage;
	idImage						*diffuse;
};

//
// bmMegaProjectArea
//
class bmMegaProjectArea {
public:
	bmMegaProjectLayer				*AllocLayer( const char *diffuseImagePath, const char *maskImagePath );		

	int								NumLayers( void ) { return _layers.Num(); }		
	bmMegaProjectLayer				*GetLayer( int layerNum ) { return &_layers[layerNum]; }
private:
	idList<bmMegaProjectLayer>		_layers;
};



/*
=====================
bmMegaProjectArea::AllocLayer
=====================
*/
ID_INLINE bmMegaProjectLayer* bmMegaProjectArea::AllocLayer( const char *diffuseImagePath, const char *maskImagePath ) {
	bmMegaProjectLayer layer;

	layer.Init( diffuseImagePath, maskImagePath );

	_layers.Append( layer );

	return &_layers[_layers.Num() - 1];
}

//
// bmMegaProjectFile
//
class bmMegaProjectFile {
public:
				bmMegaProjectFile();
	virtual    ~bmMegaProjectFile();

				// Init a default project
	void		InitDefault( void );

				// Get the number of areas in the project.
	int			NumAreas( void );


				// Allocates a new area.
	bmMegaProjectArea *AllocArea( void );

				// Return the requested area.
	bmMegaProjectArea *GetArea( int areaNum ) { return &_areas[areaNum]; }
private:
	idList<bmMegaProjectArea>		_areas;		// Areas in the project
};

/*
===================
bmMegaProjectFile::AllocArea
===================
*/
ID_INLINE bmMegaProjectArea *bmMegaProjectFile::AllocArea( void ) {
	bmMegaProjectArea area;

	_areas.Append( area );

	return &_areas[_areas.Num() - 1];
}


/*
===================
bmMegaProjectFile::NumAreas
===================
*/
ID_INLINE int bmMegaProjectFile::NumAreas( void ) {
	return _areas.Num();
}