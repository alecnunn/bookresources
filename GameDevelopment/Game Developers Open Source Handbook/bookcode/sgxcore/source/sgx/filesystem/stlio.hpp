#ifndef SGX_FILESYSTEM_STLIO_HPP
#define SGX_FILESYSTEM_STLIO_HPP	1

//
// Operator overloads
//
template<typename T> 
CSGXFile& operator<<(CSGXFile& File, std::vector<T>& val)
{
    tUINT32 size = (tUINT32)val.size();

    File << size;
    for (std::vector<T>::iterator it = val.begin(); it != val.end(); ++it) {
        File << *it;
    }
    return File;
}

// Loading code
template<typename T> 
CSGXFile& operator>>(CSGXFile& File, std::vector<T>& val)
{
tUINT32 size;

    File >> size;
    val.clear();
    val.reserve(size);

    for (tUINT32 i = 0; i < size; i++) {
        std::vector<T>::iterator it = val.insert(val.end(), T());
        File >> *it;
    }
    return File;
}

template<class T>
CSGXFile& operator|(CSGXFile& File, T& var)
{
    if (IsSaving()) {
         return File << var;
    }
    else if (IsLoading()) {
        return File >> var;
    }
    return File;
}


#endif /* SGX_FILESYSTEM_STLIO_HPP */
