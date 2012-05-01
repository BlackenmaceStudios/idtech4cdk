// image_pbo.h
//

//
// bmImagePBO
//
class bmImagePBO {
public:
							~bmImagePBO();

	void					Init(idImage *img);

	byte					*ReadPBO( bool readBack );
	void					WriteToPBO( int pbo, byte *buffer, int DestX, int DestY, int Width, int Height );

	void					UnbindPBO( void );
private:
	idImage					*_img;

	GLuint					pboReadbackHandle;
	GLuint					pboWriteHandle[2];
};