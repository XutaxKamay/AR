#include <fstream>  // files
#include <iostream> // cout
#include <stdint.h> // types
#include <string>   // string

// Bibliothèque (PulseAudio) pour jouer du son, une autre façon serait
// d'utiliser le périphérique /dev/dsp et écrire les données PCM dedans.
// Le programme doit être démarré avec padsp dans ce cas là.
#include <pulse/error.h>
#include <pulse/simple.h>

enum e_audioFile
{
    FILE_ERROR = -1,
    FILE_RIFF,
    FILE_AU,
    FILE_MAX
};

typedef void* Pointer;
typedef Pointer* pPointer;

// Allouer de la mémoire.
template <typename T = Pointer>
inline auto Alloc(size_t size) -> T
{
    return reinterpret_cast<T>(::operator new(size));
}

// Libérer de la mémoire.
template <typename T = Pointer>
inline auto Free(T& p) -> void
{
    // On vérifie si la mémoire n'a pas déjà été "détruite".
    if (reinterpret_cast<Pointer>(p) != nullptr)
    {
        ::operator delete(p);
        p = nullptr;
    }
}

// Permet de permuter entre Little Endian vers Big Endian et vice-versa.
template <typename T>
inline auto swapEndian(T val) -> T
{
    T original = val;

    uint8_t* po   = reinterpret_cast<uint8_t*>(&original);
    uint8_t* pval = reinterpret_cast<uint8_t*>(&val);

    auto sizeOfT = sizeof(T);

    for (auto i = 0; i != sizeOfT; i++)
    {
        pval[i] = po[(sizeOfT - 1) - i];
    }

    return val;
}

// Compare si les octets sont les mêmes pour une source et une destination
// à partir d'une taille définie.
template <typename T>
auto memcmp(T pSource, T pDest, size_t sizeToCompare) -> bool
{
    auto p8Source = reinterpret_cast<uint8_t*>(pSource);
    auto p8Dest   = reinterpret_cast<uint8_t*>(pDest);

    while (sizeToCompare != 0)
    {
        if (*p8Source != *p8Dest)
            goto end;

        p8Source++;
        p8Dest++;
        sizeToCompare--;
    }

    return true;
end:
    return false;
}
