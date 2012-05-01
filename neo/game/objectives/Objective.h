// Objective.h
//

//
// bmObjectiveDecl
//
class bmObjectiveDecl : public idDecl {
public:
	virtual size_t				Size( void ) const;
	virtual const char *		DefaultDefinition( void ) const;
	virtual bool				Parse( const char *text, const int textLength );
	virtual void				FreeData( void );
};