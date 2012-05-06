// image_pbo.h
//

//
// bmImagePBO
//
class bmImagePBO {
public:
							bmImagePBO() { pboReadbackHandle = -1; }
							~bmImagePBO();

	void					Init(idImage *img);

	byte					*ReadPBO( bool readBack );
	void					WriteToPBO( int pbo, byte *buffer, int DestX, int DestY, int Width, int Height );

	void					UnbindPBO( void );
	bool					IsValid() { return pboReadbackHandle > 0; }
private:
	idImage					*_img;
	byte *buffer;
	GLuint					pboReadbackHandle;
	GLuint					pboWriteHandle[2];
};