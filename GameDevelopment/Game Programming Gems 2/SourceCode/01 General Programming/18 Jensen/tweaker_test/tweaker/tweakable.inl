namespace eqx {

template<class T>
void
Tweakable_c<T>::ChangeValueAndDoCallback( const void* const i_pValue,
  Tweaker_c* const i_pcTweaker, const std::string& i_cID, bool i_bDoCallback ) 
{ 
  const T* const pxValue = reinterpret_cast<const T* const>( i_pValue );
  *( reinterpret_cast<T*>( m_pData ) ) = *pxValue;
} // ChangeValueAndDoCallback

template<class T>
bool 
TweakableType_c<T>::TestType( const TypeID_c* const i_pcType ) const { 
  if( i_pcType != m_pcType ) { return false; }
  return true; 
} // TestType
 
template<class T> 
void 
TweakableType_c<T>::ChangeValueAndDoCallback( const void* const i_pValue,
  Tweaker_c* const i_pcTweaker, const std::string& i_cID, bool i_bDoCallback ) { 
  const T* const pxValue = reinterpret_cast<const T* const>( i_pValue );
  *( reinterpret_cast<T*>( m_pData ) ) = *pxValue;
} // ChangeValueAndDoCallback

template<class T>
TweakError_e 
TweakableRange_c<T>::ConstrainValue( const void* const i_pValue ) const { 
  const T* const pxValue = reinterpret_cast<const T* const>( i_pValue );
  if( *pxValue < m_xMin ) { return e_MIN_EXCEEDED; }
  if( *pxValue > m_xMax ) { return e_MAX_EXCEEDED; }
  return e_OK; 
} // ConstrainValue

template<class T>
void 
TweakableRange_c<T>::ChangeValueAndDoCallback( const void* const i_pValue, 
  Tweaker_c* const i_pcTweaker, const std::string& i_cID, bool i_bDoCallback ) { 
  const T* const pxValue = reinterpret_cast<const T* const>( i_pValue );
  *( reinterpret_cast<T*>( m_pData ) ) = *pxValue;
} // ChangeValueAndDoCallback

template<class T>
TweakError_e 
TweakableRange_c<T>::GetMinMax( void* o_pMin, void* o_pMax ) const { 
  *( reinterpret_cast<T*>( o_pMin ) ) = m_xMin;
  *( reinterpret_cast<T*>( o_pMax ) ) = m_xMax;
  return e_OK; 
} // GetMinMax    

template<class T>
bool 
TweakableTypeRange_c<T>::TestType( const TypeID_c* const i_pcType ) const { 
  if( i_pcType != m_pcType ) { return false; }
  return true; 
} // TestType

template<class T>
TweakError_e 
TweakableTypeRange_c<T>::ConstrainValue( const void* const i_pValue ) const { 
  const T* const pxValue = reinterpret_cast<const T* const>( i_pValue );
  if( *pxValue < m_xMin ) { return e_MIN_EXCEEDED; }
  if( *pxValue > m_xMax ) { return e_MAX_EXCEEDED; }
  return e_OK; 
} // ConstrainValue


template<class T>
void 
TweakableTypeRange_c<T>::ChangeValueAndDoCallback( const void* const i_pValue,
  Tweaker_c* const i_pcTweaker, const std::string& i_cID, bool i_bDoCallback ) { 
  const T* const pxValue = reinterpret_cast<const T* const>( i_pValue );
  *( reinterpret_cast<T*>( m_pData ) ) = *pxValue;
} // ChangeValueAndDoCallback

template<class T>
TweakError_e 
TweakableTypeRange_c<T>::GetMinMax( void* o_pMin, void* o_pMax ) const { 
  *( reinterpret_cast<T*>( o_pMin ) ) = m_xMin;
  *( reinterpret_cast<T*>( o_pMax ) ) = m_xMax;
  return e_OK; 
} // GetMinMax    

template<class T>
void 
TweakableCallback_c<T>::ChangeValueAndDoCallback( const void* const i_pValue,
  Tweaker_c* const i_pcTweaker, const std::string& i_cID, bool i_bDoCallback ) { 
  const T* const pxValue = reinterpret_cast<const T* const>( i_pValue );
  T* pxTweakable = reinterpret_cast<T*>( m_pData );
  T xOldValue = *pxTweakable;
  *pxTweakable = *pxValue;
  if( i_bDoCallback == true ) {
    if( m_pfCallback( i_pcTweaker, i_cID, m_pCallbackData ) == false ) {
      *pxTweakable = xOldValue; // User rejected changes
    }
  }
} // ChangeValueAndDoCallback

template<class T>
TweakError_e 
TweakableCallbackRange_c<T>::ConstrainValue( const void* const i_pValue ) const { 
  const T* const pxValue = reinterpret_cast<const T* const>( i_pValue );
  if( *pxValue < m_xMin ) { return e_MIN_EXCEEDED; }
  if( *pxValue > m_xMax ) { return e_MAX_EXCEEDED; }
  return e_OK; 
} // ConstrainValue

template<class T>
void 
TweakableCallbackRange_c<T>::ChangeValueAndDoCallback( const void* const i_pValue,
  Tweaker_c* const i_pcTweaker, const std::string& i_cID, bool i_bDoCallback ) { 
  const T* const pxValue = reinterpret_cast<const T* const>( i_pValue );
  T* pxTweakable = reinterpret_cast<T*>( m_pData );
  T xOldValue = *pxTweakable;
  *pxTweakable = *pxValue;
  if( i_bDoCallback == true ) {
    if( m_pfCallback( i_pcTweaker, i_cID, m_pCallbackData ) == false ) {
      *pxTweakable = xOldValue; // User rejected changes
    }
  }
} // ChangeValueAndDoCallback

template<class T>
TweakError_e 
TweakableCallbackRange_c<T>::GetMinMax( void* o_pMin, void* o_pMax ) const { 
  *( reinterpret_cast<T*>( o_pMin ) ) = m_xMin;
  *( reinterpret_cast<T*>( o_pMax ) ) = m_xMax;
  return e_OK; 
} // GetMinMax    

template<class T>
bool 
TweakableCallbackType_c<T>::TestType( const TypeID_c* const i_pcType ) const { 
  if( i_pcType != m_pcType ) { return false; }
  return true; 
} // TestType

template<class T>
void 
TweakableCallbackType_c<T>::ChangeValueAndDoCallback( const void* const i_pValue,
    Tweaker_c* const i_pcTweaker, const std::string& i_cID, bool i_bDoCallback ) { 
  const T* const pxValue = reinterpret_cast<const T* const>( i_pValue );
  T* pxTweakable = reinterpret_cast<T*>( m_pData );
  T xOldValue = *pxTweakable;
  *pxTweakable = *pxValue;
  if( i_bDoCallback == true ) {
    if( m_pfCallback( i_pcTweaker, i_cID, m_pCallbackData ) == false ) {
      *pxTweakable = xOldValue; // User rejected changes
    }
  }
} // ChangeValueAndDoCallback

template<class T>
bool 
TweakableCallbackTypeRange_c<T>::TestType( const TypeID_c* const i_pcType ) const { 
  if( i_pcType != m_pcType ) { return false; }
  return true; 
} // TestType

template<class T>
TweakError_e 
TweakableCallbackTypeRange_c<T>::ConstrainValue( const void* const i_pValue ) const { 
  const T* const pxValue = reinterpret_cast<const T* const>( i_pValue );
  if( *pxValue < m_xMin ) { return e_MIN_EXCEEDED; }
  if( *pxValue > m_xMax ) { return e_MAX_EXCEEDED; }
  return e_OK; 
} // ConstrainValue

template<class T>
void 
TweakableCallbackTypeRange_c<T>::ChangeValueAndDoCallback( const void* const i_pValue,
    Tweaker_c* const i_pcTweaker, const std::string& i_cID, bool i_bDoCallback ) { 
  const T* const pxValue = reinterpret_cast<const T* const>( i_pValue );
  T* pxTweakable = reinterpret_cast<T*>( m_pData );
  T xOldValue = *pxTweakable;
  *pxTweakable = *pxValue;
  if( i_bDoCallback == true ) {
    if( m_pfCallback( i_pcTweaker, i_cID, m_pCallbackData ) == false ) {
      *pxTweakable = xOldValue; // User rejected changes
    }
  }
} // ChangeValueAndDoCallback

template<class T>
TweakError_e 
TweakableCallbackTypeRange_c<T>::GetMinMax( void* o_pMin, void* o_pMax ) const { 
  *( reinterpret_cast<T*>( o_pMin ) ) = m_xMin;
  *( reinterpret_cast<T*>( o_pMax ) ) = m_xMax;
  return e_OK; 
} // GetMinMax   

} // eqx