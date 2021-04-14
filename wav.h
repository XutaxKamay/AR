struct sRIFFHeader
{
    char sRIFF[4];       // Signature du fichier. Contient le mot RIFF.
    uint32_t sizeOfFile; // Taille du fichier RIFF en octets - 8.
    char sFormatType[4]; // Type du format RIFF, peut contenir WAVE etc...
};

extern char extRIFF[];

struct sWAVHeader
{
    sRIFFHeader riffHeader;      // En-tête du format RIFF.
    char stypeOfHeaderFormat[4]; // Le type de format des données
                                 // restantes de l'en-tête. Ici il sera
                                 // "fmt ", il décrit le format de l'audio
                                 // ci dessous.
    uint32_t sizeOfHeaderFormat; // Taille des données restantes ci
                                 // dessous.
    uint16_t typeOfFormat; // Format des données audio. Généralement PCM
                           // (1).
    uint16_t numberOfChannels; // Le nombre de canaux.
    uint32_t nFreq;            // Fréquence d'échantillonnage.
    uint32_t
      rateOfBytesSec; // Nombre d'octets à lire par seconde.
                      // (nFreq*numberOfChannels*numberOfBitsPerSample/8)
    uint16_t sizeOfBlock; // Nombres d'octets pour un échantillonnage pour
                          // tout les canaux.
                          // (numberOfChannels*numberOfBitsPerSample/8)
    uint16_t numberOfBitsPerSample; // Nombre de bits pour un
                                    // échantillonnage.
    char sdata[4]; // Signature du fichier. Contient le mot "data" pour
                   // dire ou les données audios commence.
    uint32_t sizeOfData; // Taille des données audio en octets.
};

constexpr auto sizeOfWAVHeader = sizeof(sWAVHeader);
typedef sWAVHeader* psWAVHeader;

class CWAVFile
{
  public:
    // Constructeur.
    CWAVFile(std::string sFileName);

    // Déconstructeur.
    ~CWAVFile();

    inline auto IsValid() -> bool
    {
        return m_pHeader != nullptr;
    }

    // Taille en octets de toutes les données PCM.
    auto SizeOfData() -> size_t;

    auto Dump() -> void;

    // 16 ou 8 bits par échantillon pour le moment supporté. Les fichiers
    // wav sont en Little Endian par défaut.
    auto GetPulseAudioBitsSample() -> pa_sample_format;

    auto Play() -> void;

    auto GetTypeOfFormat() -> const char*;

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
        return GetFileFromOffs<T>(sizeOfWAVHeader);
    }

  private:
    // Chemin absolu du fichier audio.
    std::string m_sFileName;
    // Pointeur du fichier audio.
    Pointer m_pFile;
    // Taille du fichier audio.
    size_t m_Size;
    // Pointeur vers la structure du fichier audio.
    psWAVHeader m_pHeader;
};
