// ToolManager.h
//

//
// bmToolManager
//
class bmToolManager {
public:
	void				LoadTools( void );
	void				InitTool( const toolFlag_t tool, const idDict *dict );
	void				Frame( void );
private:
	ID_SYS_HANDLE		toolDLLHandle;
};

extern bmToolManager	toolManager;