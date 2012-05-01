// StrPool.cpp
//

#include "precompiled.h"

/*
================
idStrPool::FreeString
================
*/
void idStrPool::FreeString( const idPoolStr *poolStr ) {
	int i, hash;

	assert( poolStr->numUsers >= 1 );
// jmarshall
	//assert( poolStr->pool == this );
	if(poolStr->pool != this) {
		poolStr->pool->FreeString(poolStr);
		return;
	}
// jmarshall end

	poolStr->numUsers--;
	if ( poolStr->numUsers <= 0 ) {
		hash = poolHash.GenerateKey( poolStr->c_str(), caseSensitive );
		if ( caseSensitive ) { 
			for ( i = poolHash.First( hash ); i != -1; i = poolHash.Next( i ) ) {
				if ( pool[i]->Cmp( poolStr->c_str() ) == 0 ) {
					break;
				}
			}
		} else {
			for ( i = poolHash.First( hash ); i != -1; i = poolHash.Next( i ) ) {
				if ( pool[i]->Icmp( poolStr->c_str() ) == 0 ) {
					break;
				}
			}
		}
		assert( i != -1 );
		assert( pool[i] == poolStr );
		delete pool[i];
		pool.RemoveIndex( i );
		poolHash.RemoveIndex( hash, i );
	}
}