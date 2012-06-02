// ===================================================================
// jmarshall -- The Raven debugger used a lot of the legacy Quake 3 syntax
#define msg_t idBitMsg

/*
=========
MSG_WriteString
=========
*/
ID_INLINE void MSG_WriteString( idBitMsg *msg, const char *s, int maxLength = -1, bool make7Bit = true ) {
	msg->WriteString( s, maxLength, make7Bit );
}

/*
=========
MSG_WriteBits
=========
*/
ID_INLINE void MSG_WriteBits( idBitMsg *msg, int value, int numBits ) {
	msg->WriteBits( value, numBits );
}

/*
=========
MSG_ReadShort
=========
*/
ID_INLINE short MSG_ReadShort( idBitMsg *msg ) {
	return msg->ReadShort();
}

/*
=========
MSG_ReadLong
=========
*/
ID_INLINE long MSG_ReadLong( idBitMsg *msg ) {
	return msg->ReadLong();
}

/*
=========
MSG_ReadString
=========
*/
ID_INLINE int MSG_ReadString( idBitMsg *msg, char *buffer, int bufferSize ) {
	return msg->ReadString( buffer, bufferSize );
}

/*
=========
MSG_WriteShort
=========
*/
ID_INLINE void MSG_WriteShort( idBitMsg *msg, short s ) {
	 msg->WriteShort( s );
}

/*
=========
MSG_WriteLong
=========
*/
ID_INLINE void MSG_WriteLong( idBitMsg *msg, long s ) {
	msg->WriteLong( s );
}
/*
=========
MSG_Init
=========
*/
ID_INLINE void MSG_Init( idBitMsg *msg, byte *data, int length ) {
	msg->Init(data, length);
}

/*
=========
MSG_ReadBits
=========
*/
ID_INLINE int MSG_ReadBits ( idBitMsg *msg, int bits ) {
	return msg->ReadBits( bits );
}
// ===================================================================