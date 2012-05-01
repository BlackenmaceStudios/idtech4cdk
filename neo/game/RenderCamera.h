// RenderCamera.h
//

class idEntity;

//
// bmRenderCamera
//
class bmRenderCamera : public idEntity {
public:
	void					Spawn( void );

	virtual void			Think( void );
	virtual void			Present( void );
};