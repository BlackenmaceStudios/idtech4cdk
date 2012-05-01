// tr_shadowmap_cache.h
//

//
// bmShadowMapCache
//
class bmShadowMapCache {
public:
	void			Init( int width, int height );
	void			BindCache( void ) { shadowMapCube->Bind(); }
	void			Bind( void ) { shadowMapCube->BindFBO(); };
	void			UnBind( void ) { shadowMapCube->UnBindFBO(); };
	void			ClearScreenMapCache( void );
	void			RenderToScreenBuffer( struct viewLight_s *vLight );
private:
	idImage *shadowMapCube;
	bool enableShadowBlend;
};