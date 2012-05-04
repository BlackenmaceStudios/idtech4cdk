// outexporter.h
//

//
// bmVTModel
//
class bmVTModel {
public:
							bmVTModel() { Init(); }
	void					Init( void );
	void					AddTris( const srfTriangles_t *uTris );
	void WriteToFile( const char *file ) ;

	idList<srfTriangles_t*> tris;
};

