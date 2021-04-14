struct sAUHeader
{
    char snd[4];           // Signature du fichier. ".snd"
    uint32_t offsetOfData; // Position des données dans le fichier audio.
    uint32_t sizeOfData;   // Taille des données audio.
    uint32_t typeOfFormat; // Type des données audio.
    uint32_t nFreq;        // Fréquence d'échantillonnage.
    uint32_t numberOfChannels; // Nombre de canaux.
};

constexpr auto sizeOfAUHeader = sizeof(sAUHeader);
extern char extAU[];

typedef sAUHeader* psAUHeader;

class CAUFile
{
  public:
    // Constructeur.
    CAUFile(std::string sFileName);

    // Déconstructeur.
    ~CAUFile();

    inline auto IsValid() -> bool
    {
        return m_pHeader != nullptr;
    }

    // Taille en octets de toutes les données PCM.
    auto SizeOfData() -> size_t;

    auto Dump() -> void;

    // Les fichiers AU peuvent avoir certains formats d'échantillonnage,
    // en 8, 16, 24, 32 bits... Codés en Big Endian généralement.
    auto GetPulseAudioBitsSample() -> pa_sample_format;

    auto GetTypeOfFormat() -> const char*;

    auto Play() -> void;

    // Obtenir le pointeur du fichier depuis un offset
    template <typename T = Pointer>
    auto GetFileFromOffs(size_t f_Offs) -> T
    {
        return reinterpret_cast<T>(reinterpret_cast<uintptr_t>(m_pFile)
                                   + f_Offs);
    }

    // Obtenir le pointeur au début des données PCM.
    template <typename T = Pointer>
    auto GetData() -> T
    {
        return GetFileFromOffs<T>(m_pHeader->offsetOfData);
    }

  private:
    // Chemin absolu du fichier audio.
    std::string m_sFileName;
    // Pointeur du fichier audio.
    Pointer m_pFile;
    // Taille du fichier audio.
    size_t m_Size;
    // Pointeur vers la structure du fichier audio.
    psAUHeader m_pHeader;
};